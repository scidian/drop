//
//      Created by Stephens Nunnally on 12/26/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Handles dealing with the Scene View
//
//

#include "01_project.h"
#include "02_world.h"
#include "03_scene.h"
#include "04_object.h"
#include "05_item.h"

#include "30_settings.h"
#include "31_component.h"
#include "32_property.h"

#include "form_main.h"

// Constructor
SceneGraphicsView::SceneGraphicsView(QWidget *parent, FormMain *main_window) :
        QGraphicsView(parent = nullptr), m_parent_window (main_window)
{
    // Initialize rubber band object used as a selection box
    m_rubber_band = new SceneViewRubberBand(QRubberBand::Rectangle, getMainWindow()->globals, this);
}

// Mouse entered widget area event
void SceneGraphicsView::enterEvent(QEvent *event)
{
    setFocus(Qt::FocusReason::MouseFocusReason);                        // Set focus on mouse enter to allow for space bar pressing hand grab
    getMainWindow()->setAdvisorInfo(Advisor_Info::Scene_Area);          // Set Advisor text on mouse enter
    QGraphicsView::enterEvent(event);
}

// Key press event
void SceneGraphicsView::keyPressEvent(QKeyEvent *event)
{
    // When space bar is down, enabled mouse press and move to translate viewable area
    if (event->key() == Qt::Key::Key_Space) {   m_flag_key_down_control = true;
        setDragMode(QGraphicsView::DragMode::ScrollHandDrag);
        setInteractive(false); }

    // Store when control / alt are down
    if (event->key() == Qt::Key::Key_Control) { m_flag_key_down_control = true; }
    if (event->key() == Qt::Key::Key_Alt) {     m_flag_key_down_alt = true; }

    QGraphicsView::keyPressEvent(event);
}

// Key release event
void SceneGraphicsView::keyReleaseEvent(QKeyEvent *event)
{
    // When space bar is released, change mode back to select / move items
    if (event->key() == Qt::Key::Key_Space) {   m_flag_key_down_control = false;
        setDragMode(QGraphicsView::DragMode::NoDrag);
        setInteractive(true); }

    // Store when control / alt are released
    if (event->key() == Qt::Key::Key_Control) { m_flag_key_down_control = false; }
    if (event->key() == Qt::Key::Key_Alt) {     m_flag_key_down_alt = false; }

    QGraphicsView::keyReleaseEvent(event);
}





void SceneGraphicsView::mousePressEvent(QMouseEvent *event)
{
    // On initial mouse down, store mouse origin point
    m_origin = event->pos();
    m_origin_in_scene = mapToScene(m_origin);
    m_origin_item = nullptr;

    // If space bar isnt down, process mouse down
    if (dragMode() == QGraphicsView::DragMode::NoDrag) {

        // If left mouse button down
        if (event->button() & Qt::LeftButton) {
            // If left down, store item that was under mouse
            m_origin_item = itemAt(event->pos());

            // If no keys are down, only select item under mouse
            if (event->modifiers() == Qt::KeyboardModifier::NoModifier) {
                // If no item under mouse, deselect all
                if (m_origin_item == nullptr) {
                    for (auto item: scene()->selectedItems()) { item->setSelected(false); }
                } else {
                    // In not already select, select it and deselect all others
                    if (m_origin_item->isSelected() == false) {
                        m_origin_item->setSelected(true);

                        for (auto item: scene()->selectedItems()) {
                            if (item != m_origin_item) item->setSelected(false);
                        }
                    }
                    QGraphicsView::mousePressEvent(event);                  // Process press event for movement
                }
            } else if (event->modifiers() & Qt::KeyboardModifier::ControlModifier) {
                if (m_origin_item != nullptr) m_origin_item->setSelected(!m_origin_item->isSelected());
            }

            // If clicked while holding Alt key, start resizing
            if (event->modifiers() & Qt::KeyboardModifier::AltModifier) {
                if (scene()->selectedItems().count() > 0) {
                    m_is_resizing = true;

                    // Store some starting info about the selected items before we start resizing
                    m_selection_rect = scene()->selectedItems().first()->sceneBoundingRect();
                    for (auto item: scene()->selectedItems()) {
                        // Store total size of selection rectangle
                        m_selection_rect = m_selection_rect.united(item->sceneBoundingRect());

                        // Store starting scale of each selected item
                        QSizeF my_scale(item->transform().m11(), item->transform().m22());
                        item->setData(User_Roles::Scale, my_scale);

                        // Store starting position of each selected item
                        QSizeF my_pos(item->pos().x(), item->pos().y());
                        item->setData(User_Roles::Position, my_pos);
                    }
                    return;
                }
            }

            // If theres no item under mouse, start selection box
            if (m_origin_item == nullptr) {
                m_is_selecting = true;                                                  // Flag that we're in selection mode
                if (event->modifiers() & Qt::KeyboardModifier::ControlModifier) {
                    m_items_start = scene()->selectedItems();                           // Store list of originally selected items
                } else {
                    m_items_start.clear();
                }

                m_rubber_band->setGeometry(QRect(m_origin, QSize()));                   // Start selection box with no size
                m_rubber_band->show();                                                  // Make selection box visible
                return;
            }

        }

    } else {
        // Process mouse press event for hand drag
        QGraphicsView::mousePressEvent(event);
    }
}


void SceneGraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    if (selection_mutex.tryLock() == false) return;           // Try and lock function, so we ony run this once at a time

    // If we're in selection mode, process mouse movement and resize box as needed
    if (m_is_selecting) {
        m_rubber_band->setGeometry(QRect(m_origin, event->pos()).normalized());                             // Resize selection box

        QPainterPath selectionArea;
        selectionArea.addPolygon(mapToScene(m_rubber_band->geometry()));                                    // Convert box to scene coords
        selectionArea.closeSubpath();                                                                       // Closes an open polygon

        scene()->setSelectionArea(selectionArea, Qt::ItemSelectionOperation::AddToSelection,
                                  Qt::ItemSelectionMode::IntersectsItemBoundingRect, viewportTransform());  // Pass box to scene for selection

        // Go through selected items, unselect if rubber band box covered them and has since shrunk
        for (auto item : scene()->selectedItems()) {
            QRectF item_location = item->sceneBoundingRect();                                       // Get items shape
            if (scene()->selectionArea().intersects( item_location ) == false) {
                // Check if item was in the original selection list, if so mark to keep selected
                bool has_it = false;
                for (auto check_item : m_items_start) {
                   if (check_item == item) has_it = true;                                                  // Item was in original selection list
                }
                // If it selected during this routine, but the Rubber Band has moved away, deselect it
                if (!has_it) { item->setSelected(false); }
            }
        }

    }


    // If mouse moved while Alt key is held down, resize
    if (m_is_resizing) {
        //qreal dist = sqrt(pow(m_click_pos.x() - pos.x(), 2) + pow(m_click_pos.y() - pos.y(), 2));
        QPointF mouse_in_scene =  mapToScene(event->pos());
        QPointF top_left_select = m_selection_rect.topLeft();

        // Find starting size
        qreal original_x = m_origin_in_scene.x() - top_left_select.x();             // Original width of selection
        qreal original_y = m_origin_in_scene.y() - top_left_select.y();             // Original height of selection
        if (abs(original_x) < .0001) original_x = .001;
        if (abs(original_y) < .0001) original_y = .001;

        // Calculate scale of original selection box vs new size
        qreal scale_x = (mouse_in_scene.x() - top_left_select.x()) / original_x;    // New width of selection / original width
        qreal scale_y = (mouse_in_scene.y() - top_left_select.y()) / original_y;    // New height of selection / original height

        for (auto item : scene()->selectedItems()) {
            // Get stored data from each item
            QVariant get_data;
            QSizeF my_scale, my_pos;
            get_data = item->data(User_Roles::Scale);       my_scale = get_data.toSizeF();
            get_data = item->data(User_Roles::Position);    my_pos =   get_data.toSizeF();

            // Apply new scale to each item
            QTransform scale_transform;
            scale_transform = QTransform::fromScale(my_scale.width() * scale_x, my_scale.height() * scale_y);
            item->setTransform(scale_transform);

            // Move each item as neccessary to keep them aligned within selection box
            qreal new_left = my_pos.width() * scale_x;
            qreal new_top =  my_pos.height() * scale_y;
            item->setPos(new_left, new_top);
        }

        getMainWindow()->setLabelText(Label_Names::Label2,
                                      "x: " + QString::number(top_left_select.x() ) +
                                      ", y:" + QString::number(top_left_select.y() ) );
        getMainWindow()->setLabelText(Label_Names::LabelObject1,
                                      "x: " + QString::number(m_origin_in_scene.x() ) +
                                      ", y:" + QString::number(m_origin_in_scene.y() ) );
        getMainWindow()->setLabelText(Label_Names::LabelObject2,
                                      "x: " + QString::number(mouse_in_scene.x() ) +
                                      ", y:" + QString::number(mouse_in_scene.y() ) );
        getMainWindow()->setLabelText(Label_Names::LabelObject3,
                                      "x: " + QString::number(scale_x) +
                                      ", y:" + QString::number(scale_y) );
        update();
    }



    QGraphicsView::mouseMoveEvent(event);
    update();
    selection_mutex.unlock();
}


void SceneGraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() & Qt::LeftButton)
    {
        // If we were in resizing mode, stop resizing
        if (m_is_resizing) { m_is_resizing = false; }

        // If we were in selection mode, stop it and hide selection box
        if (m_is_selecting) {
            m_is_selecting = false;
            m_rubber_band->hide();
        }

    }

    QGraphicsView::mouseReleaseEvent(event);
    update();
}



void SceneGraphicsView::paintEvent(QPaintEvent *event)
{
    // Go ahead and paint scene
    QGraphicsView::paintEvent(event);

    // Draw selection boxes around all selected items
    QPainter painter(viewport());
    QBrush pen_brush(getMainWindow()->globals->getColor(Window_Colors::Text_Light));
    painter.setPen(QPen(pen_brush, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.setBrush(Qt::NoBrush);

    for (auto item: scene()->selectedItems()) {
        QRectF rect = item->boundingRect();                                 // Get item bounding box
        rect.setWidth(rect.width() * item->transform().m11());              // Transform to item scale
        rect.setHeight(rect.height() * item->transform().m22());
        rect.translate(item->pos().x(), item->pos().y());                   // Translate bounding box to proper scene location
        QPolygon to_view = mapFromScene(rect);                              // Convert scene location to view location
        painter.drawPolygon(to_view);                                       // Draw polygon
    }
}



// Custom Paint override so we can draw our Rubber Band selection box with custom colors
void SceneViewRubberBand::paintEvent(QPaintEvent *)
{
    QColor bg = globals->getColor(Window_Colors::Icon_Light);
    QStylePainter painter(this);

    QPen pen;
    pen.setStyle(Qt::PenStyle::SolidLine); //DashLine);
    pen.setWidth(2);
    pen.setColor(bg);
    QBrush brush;
    brush.setStyle(Qt::BrushStyle::SolidPattern);
    bg.setAlpha(64);
    brush.setColor(bg);

    painter.setPen(pen);
    painter.setBrush(brush);
    painter.drawRect(this->rect());
}


// Handles zooming in / out of view with mouse wheel
#if QT_CONFIG(wheelevent)
void SceneGraphicsView::wheelEvent(QWheelEvent *event)
{
    if (event->modifiers() & Qt::KeyboardModifier::ControlModifier) {
        QGraphicsView::wheelEvent(event);
        return;
    }
    if (event->delta() > 0) {
        zoomInOut(8);  }
    else {
        zoomInOut(-8); }
    event->accept();
}
#endif


void SceneGraphicsView::zoomInOut(int level)
{
    m_zoom += level;
    applyUpdatedMatrix();
}

void SceneGraphicsView::applyUpdatedMatrix()
{
    qreal scale = qPow(qreal(2), (m_zoom - 250) / qreal(50));
    QMatrix matrix;
    matrix.scale(scale, scale);
    matrix.rotate(m_rotate);
    this->setMatrix(matrix);
}






















