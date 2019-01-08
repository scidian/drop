//
//      Created by Stephens Nunnally on 1/3/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Graphics View Definitions
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

#include "editor_scene_view.h"
#include "interface_relay.h"


// Constructor & destructor
SceneGraphicsView::SceneGraphicsView(QWidget *parent, DrProject *project, InterfaceRelay *interface) :
        QGraphicsView(parent = nullptr), m_project(project), m_interface(interface)
{
    // Initialize rubber band object used as a selection box
    m_rubber_band = new SceneViewRubberBand(QRubberBand::Rectangle, this, interface);

    // Initialize handle array
    m_handles.resize(static_cast<int>(Position_Flags::Total));
    m_over_handle = Position_Flags::No_Position;
}
SceneGraphicsView::~SceneGraphicsView() { }


// Mouse entered widget area event
void SceneGraphicsView::enterEvent(QEvent *event)
{
    setFocus(Qt::FocusReason::MouseFocusReason);                        // Set focus on mouse enter to allow for space bar pressing hand grab
    m_interface->setAdvisorInfo(Advisor_Info::Scene_Area);              // Set Advisor text on mouse enter
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
            m_origin_item = itemAt(event->pos());                           // Store item that was under mouse


            // ******************* If clicked while holding Alt key
            if (event->modifiers() & Qt::KeyboardModifier::AltModifier) {

            }


            // ******************* If clicked while in a Size Grip Handle
            if (m_over_handle != Position_Flags::No_Position) {
                if (scene()->selectedItems().count() > 0) {
                    m_is_resizing = true;
                    m_start_resize_grip = m_over_handle;

                    // Store some starting info about the selected items before we start resizing
                    m_start_resize_rect = scene()->selectedItems().first()->sceneBoundingRect();
                    for (auto item: scene()->selectedItems()) {
                        // Add on each items Rect to store total size of selection rectangle
                        m_start_resize_rect = m_start_resize_rect.united(item->sceneBoundingRect());

                        // Store starting scale of each selected item
                        QPointF my_scale(item->transform().m11(), item->transform().m22());
                        item->setData(User_Roles::Scale, my_scale);

                        // Store starting position of each selected item
                        QPointF my_pos(item->pos().x(), item->pos().y());
                        item->setData(User_Roles::Position, my_pos);
                    }
                    return;
                }
            }


            // ******************* If no keys are down, only select item under mouse
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



            // ******************* If theres no item under mouse, start selection box
            if (m_origin_item == nullptr) {
                m_is_selecting = true;                                                  // Flag that we're in selection mode
                if (event->modifiers() & Qt::KeyboardModifier::ControlModifier) {
                    m_items_start = scene()->selectedItems();                           // Store list of originally selected items
                } else {
                    m_items_start.clear();
                }

                m_rubber_band->setGeometry(QRect(m_origin, QSize()));                   // Start selection box with no size
                m_rubber_band->show();                                                  // Make selection box visible
                update();
                return;
            }

        }

    } else {
        // Process mouse press event for hand drag
        QGraphicsView::mousePressEvent(event);
    }
    update();
}


void SceneGraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    // Try and lock function, so we ony run this once at a time
    if (selection_mutex.tryLock(10) == false) return;

    // Store event mouse position
    m_last_mouse_pos = event->pos();
    m_last_mouse_pos.setX(m_last_mouse_pos.x() - 2);
    m_last_mouse_pos.setY(m_last_mouse_pos.y() - 2);

    // ******************** Check selection handles to see if mouse is over one
    if (scene()->selectedItems().count() > 0 && m_is_resizing == false) {
        m_over_handle = Position_Flags::No_Position;
        for (int i = 0; i < static_cast<int>(Position_Flags::Total); i++) {
            if (m_handles[i].contains(m_last_mouse_pos))
                m_over_handle = static_cast<Position_Flags>(i);
        }

        switch (m_over_handle)
        {
        case Position_Flags::Top_Left:      viewport()->setCursor(Qt::CursorShape::SizeFDiagCursor);    break;
        case Position_Flags::Top_Right:     viewport()->setCursor(Qt::CursorShape::SizeBDiagCursor);    break;
        case Position_Flags::Bottom_Left:   viewport()->setCursor(Qt::CursorShape::SizeBDiagCursor);    break;
        case Position_Flags::Bottom_Right:  viewport()->setCursor(Qt::CursorShape::SizeFDiagCursor);    break;
        case Position_Flags::Top:           viewport()->setCursor(Qt::CursorShape::SizeVerCursor);      break;
        case Position_Flags::Bottom:        viewport()->setCursor(Qt::CursorShape::SizeVerCursor);      break;
        case Position_Flags::Left:          viewport()->setCursor(Qt::CursorShape::SizeHorCursor);      break;
        case Position_Flags::Right:         viewport()->setCursor(Qt::CursorShape::SizeHorCursor);      break;
        default:    ;
        }
    }

    if (m_over_handle == Position_Flags::No_Position && m_is_resizing == false)
        viewport()->unsetCursor();

    m_interface->setLabelText(Label_Names::Label1, "mouse - x:" + QString::number(m_last_mouse_pos.x()) + ", y:" + QString::number(m_last_mouse_pos.y()) );


    // ******************* If we're in selection mode, process mouse movement and resize box as needed
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

    // ******************* If mouse moved while over Size Grip, resize
    if (m_is_resizing) {
        resizeSelection(mapToScene(event->pos()));
        update();
    }

    QGraphicsView::mouseMoveEvent(event);
    update();
    selection_mutex.unlock();
}


void SceneGraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
    // Process left mouse button released
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
    if (m_zoom > 500) m_zoom = 500;
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



void SceneGraphicsView::paintEvent(QPaintEvent *event)
{
    // Go ahead and paint scene
    QGraphicsView::paintEvent(event);

    // ********** Check if scene view is associated has changed, if so re-connect signals from scene
    if (scene() != m_scene) {
        if (scene() != nullptr) {
            m_scene = scene();
            connect(scene(), SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));
            connect(scene(), SIGNAL(changed(QList<QRectF>)), this, SLOT(sceneChanged(QList<QRectF>)));
            ///// Convert pointer to QString, display in label
            ///QString ptrStr = QString("0x%1").arg((quintptr)m_scene, QT_POINTER_SIZE * 2, 16, QChar('0'));
            ///m_interface->setLabelText(Label_Names::LabelObject3, "Changed: " + ptrStr);
        }
    }

    // If no selected item gets out of here
    if (scene()->selectedItems().count() < 1) return;

    // ********** Draw selection boxes around all selected items
    QPainter painter(viewport());
    QBrush pen_brush(m_interface->getColor(Window_Colors::Icon_Light));
    painter.setPen(QPen(pen_brush, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.setBrush(Qt::NoBrush);

    for (auto item: scene()->selectedItems()) {
        QRectF rect = item->boundingRect();                                 // Get item bounding box
        rect.setWidth(rect.width() * item->transform().m11());              // Transform to item scale
        rect.setHeight(rect.height() * item->transform().m22());
        rect.translate(item->pos().x(), item->pos().y());                   // Translate bounding box to proper scene location
        QPolygon to_view = mapFromScene(rect);                              // Convert scene location to view location
        painter.drawPolygon(to_view);                                       // Draw polygon
    }


    // ********** Draw box around entire seleciton, with Size Grip squares
    QRectF bigger = m_selection_rect;
    QPolygon to_view = mapFromScene(bigger);
    QPoint top_left = mapFromScene(bigger.topLeft());
    QPoint bot_right = mapFromScene(bigger.bottomRight());

    ///painter.setCompositionMode(QPainter::CompositionMode::RasterOp_NotDestination);      // Draws with pixels using NOT operation
    painter.setPen(QPen(m_interface->getColor(Window_Colors::Text_Light), 1));
    painter.drawPolygon(to_view);

    int r_half = 3;
    int r_size = r_half * 2;
    ///QLinearGradient linear_grad( QPointF(top_left.x() - r_size, top_left.y() - r_size), QPointF(top_left.x() + r_size, top_left.y() + r_size) );
    ///linear_grad.setColorAt(0, m_interface->getColor(Window_Colors::Icon_Light));
    ///linear_grad.setColorAt(1, m_interface->getColor(Window_Colors::Icon_Dark));
    ///painter.setBrush(QBrush(linear_grad));
    painter.setBrush(m_interface->getColor(Window_Colors::Icon_Light));

    m_handles[static_cast<int>(Position_Flags::Top_Left)] =     QRectF(top_left.x() - r_half,  top_left.y() - r_half,  r_size, r_size);
    m_handles[static_cast<int>(Position_Flags::Top_Right)] =    QRectF(bot_right.x() - r_half, top_left.y() - r_half,  r_size, r_size);
    m_handles[static_cast<int>(Position_Flags::Bottom_Left)] =  QRectF(top_left.x() - r_half,  bot_right.y() - r_half, r_size, r_size);
    m_handles[static_cast<int>(Position_Flags::Bottom_Right)] = QRectF(bot_right.x() - r_half, bot_right.y() - r_half, r_size, r_size);
    m_handles[static_cast<int>(Position_Flags::Top)] =    QRectF(top_left.x() + r_half,  top_left.y() - r_half,  bot_right.x() - r_size, r_size);
    m_handles[static_cast<int>(Position_Flags::Bottom)] = QRectF(top_left.x() + r_half,  bot_right.y() - r_half, bot_right.x() - r_size, r_size);
    m_handles[static_cast<int>(Position_Flags::Left)] =   QRectF(top_left.x() - r_half,  top_left.y() + r_half,  r_size, bot_right.y() - r_size);
    m_handles[static_cast<int>(Position_Flags::Right)] =  QRectF(bot_right.x() - r_half,  top_left.y() + r_half,  r_size, bot_right.y() - r_size);

    QVector<QRectF> handles;
    handles.append(m_handles[static_cast<int>(Position_Flags::Top_Left)]);
    handles.append(m_handles[static_cast<int>(Position_Flags::Top_Right)]);
    handles.append(m_handles[static_cast<int>(Position_Flags::Bottom_Left)]);
    handles.append(m_handles[static_cast<int>(Position_Flags::Bottom_Right)]);
    painter.drawRects(handles);
}


// Custom Paint override so we can draw our Rubber Band selection box with custom colors
void SceneViewRubberBand::paintEvent(QPaintEvent *)
{
    QColor bg = m_interface->getColor(Window_Colors::Icon_Light);
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



// SLOTs to keep total selection area up to date
void SceneGraphicsView::sceneChanged(QList<QRectF>) { updateSelectionRect(); }        // Connected from scene().changed
void SceneGraphicsView::selectionChanged()          { updateSelectionRect(); }        // Connected from scene().selectionChanged

// Store total size of selection rectangle
void SceneGraphicsView::updateSelectionRect()
{
    if (scene() == nullptr) return;
    if (scene()->selectedItems().count() < 1) return;

    m_selection_rect = scene()->selectedItems().first()->sceneBoundingRect();
    for (auto item: scene()->selectedItems()) {
        m_selection_rect = m_selection_rect.united(item->sceneBoundingRect());
    }

    update();
    m_interface->setLabelText(Label_Names::LabelObject1, QString::number(m_selection_rect.x()) + ", " + QString::number(m_selection_rect.y()) +
                                              ", " + QString::number(m_selection_rect.width()) + ", " + QString::number(m_selection_rect.height()) );
}



// Calculates selection resizing
void SceneGraphicsView::resizeSelection(QPointF mouse_in_scene)
{
    // Figure out what sides to use for x axis and y axis
    X_Axis do_x;
    Y_Axis do_y;
    switch (m_start_resize_grip) {
    case Position_Flags::Top_Left:      do_x = X_Axis::Left;    do_y = Y_Axis::Top;     break;
    case Position_Flags::Top:           do_x = X_Axis::None;    do_y = Y_Axis::Top;     break;
    case Position_Flags::Top_Right:     do_x = X_Axis::Right;   do_y = Y_Axis::Top;     break;
    case Position_Flags::Right:         do_x = X_Axis::Right;   do_y = Y_Axis::None;    break;
    case Position_Flags::Bottom_Right:  do_x = X_Axis::Right;   do_y = Y_Axis::Bottom;  break;
    case Position_Flags::Bottom:        do_x = X_Axis::None;    do_y = Y_Axis::Bottom;  break;
    case Position_Flags::Bottom_Left:   do_x = X_Axis::Left;    do_y = Y_Axis::Bottom;  break;
    case Position_Flags::Left:          do_x = X_Axis::Left;    do_y = Y_Axis::None;    break;
    default:                            do_x = X_Axis::None;    do_y = Y_Axis::None;
    }

    ///// To be used to calculate size while doing uniform resizing
    ///qreal dist = sqrt(pow(m_click_pos.x() - pos.x(), 2) + pow(m_click_pos.y() - pos.y(), 2));
    QPointF top_left_select =  m_start_resize_rect.topLeft();
    QPointF bot_right_select = m_start_resize_rect.bottomRight();
    qreal original_x, original_y;
    qreal new_x,      new_y;
    qreal scale_x,    scale_y;

    // Find original distance of mouse to opposite selection sides
    switch (do_x) {
    case X_Axis::Right:
        original_x = m_origin_in_scene.x() - top_left_select.x();
        new_x = mouse_in_scene.x() - top_left_select.x();
        scale_x = (new_x) / (original_x + .000001);      // New width of selection / original width
        break;
    case X_Axis::Left:
        original_x = bot_right_select.x() - m_origin_in_scene.x();
        new_x = bot_right_select.x() - mouse_in_scene.x();
        scale_x = (new_x) / (original_x + .000001);
        break;
    case X_Axis::None:
        original_x = 0;     scale_x = 1;
    }

    switch (do_y) {
    case Y_Axis::Bottom:
        original_y = m_origin_in_scene.y() - top_left_select.y();
        new_y = mouse_in_scene.y() - top_left_select.y();
        scale_y = (new_y) / (original_y + .000001);
        break;
    case Y_Axis::Top:
        original_y = bot_right_select.y() - m_origin_in_scene.y();
        new_y = bot_right_select.y() - mouse_in_scene.y();
        scale_y = (new_y) / (original_y + .000001);
        break;
    case Y_Axis::None:
        original_y = 0;     scale_y = 1;
    }


    // Apply scaling to each selected item
    for (auto item : scene()->selectedItems()) {
        // Get stored data from each item
        QVariant get_data;
        QPointF my_scale, my_pos;
        get_data = item->data(User_Roles::Scale);       my_scale = get_data.toPointF();
        get_data = item->data(User_Roles::Position);    my_pos =   get_data.toPointF();

        // Apply new scale to each item
        qreal sx = my_scale.x() * scale_x;
        qreal sy = my_scale.y() * scale_y;
        if (sx <  .001 && sx >= 0) sx =  .001;
        if (sx > -.001 && sx < 0)  sx = -.001;
        if (sy <  .001 && sy >= 0) sy =  .001;
        if (sy > -.001 && sy < 0)  sy = -.001;
        QTransform scale_transform;
        scale_transform = QTransform::fromScale(sx, sy);
        item->setTransform(scale_transform);

        // Move each item as neccessary to keep them aligned within selection box

        qreal original_x_dist, original_y_dist;
        qreal new_left, new_top;

        switch (do_x) {
        case X_Axis::Right:
            original_x_dist = (my_pos.x() - top_left_select.x());
            new_left = top_left_select.x() + (original_x_dist * scale_x);
            break;
        case X_Axis::Left:
            original_x_dist = (bot_right_select.x() - my_pos.x());
            new_left = bot_right_select.x() - (original_x_dist * scale_x);
            break;
        case X_Axis::None:      new_left = my_pos.x();
        }

        switch (do_y) {
        case Y_Axis::Bottom:
            original_y_dist = (my_pos.y() - top_left_select.y());
            new_top =  top_left_select.y() + (original_y_dist * scale_y);
            break;
        case Y_Axis::Top:
            original_y_dist = (bot_right_select.y() - my_pos.y());
            new_top =  bot_right_select.y() - (original_y_dist * scale_y);
            break;
        case Y_Axis::None:      new_top = my_pos.y();
        }

        item->setPos(new_left, new_top);
    }




    m_interface->setLabelText(Label_Names::Label2,        "  x: " + QString::number(top_left_select.x() ) +
                                                          ", y:" +  QString::number(top_left_select.y() ) );
    m_interface->setLabelText(Label_Names::LabelObject1,  "  x: " + QString::number(m_origin_in_scene.x() ) +
                                                          ", y:" +  QString::number(m_origin_in_scene.y() ) );
    m_interface->setLabelText(Label_Names::LabelObject2,  "  x: " + QString::number(mouse_in_scene.x() ) +
                                                          ", y:" +  QString::number(mouse_in_scene.y() ) );
    m_interface->setLabelText(Label_Names::LabelObject3,  "  x: " + QString::number(scale_x) +
                                                          ", y:" +  QString::number(scale_y) );

}







// Calculates selection resizing
void SceneGraphicsView::resizeSelection2(QPointF mouse_in_scene)
{
    ///// To be used to calculate size while doing uniform resizing
    ///qreal dist = sqrt(pow(m_click_pos.x() - pos.x(), 2) + pow(m_click_pos.y() - pos.y(), 2));
    QPointF top_left_select =  m_start_resize_rect.topLeft();
    QPointF bot_right_select = m_start_resize_rect.bottomRight();
    qreal original_x, original_y;
    qreal new_x,      new_y;
    qreal scale_x,    scale_y;

    // Figure out what sides to use for x axis and y axis
    X_Axis do_x;
    Y_Axis do_y;
    switch (m_start_resize_grip) {
    case Position_Flags::Top_Left:      do_x = X_Axis::Left;    do_y = Y_Axis::Top;     break;
    case Position_Flags::Top:           do_x = X_Axis::None;    do_y = Y_Axis::Top;     break;
    case Position_Flags::Top_Right:     do_x = X_Axis::Right;   do_y = Y_Axis::Top;     break;
    case Position_Flags::Right:         do_x = X_Axis::Right;   do_y = Y_Axis::None;    break;
    case Position_Flags::Bottom_Right:  do_x = X_Axis::Right;   do_y = Y_Axis::Bottom;  break;
    case Position_Flags::Bottom:        do_x = X_Axis::None;    do_y = Y_Axis::Bottom;  break;
    case Position_Flags::Bottom_Left:   do_x = X_Axis::Left;    do_y = Y_Axis::Bottom;  break;
    case Position_Flags::Left:          do_x = X_Axis::Left;    do_y = Y_Axis::None;    break;
    default:                            do_x = X_Axis::None;    do_y = Y_Axis::None;
    }


    // Find original distance of mouse to opposite selection sides
    switch (do_x) {
    case X_Axis::Right:
        original_x = m_origin_in_scene.x() - top_left_select.x();
        new_x = mouse_in_scene.x() - top_left_select.x();
        scale_x = (new_x) / (original_x + .0000001);      // New width of selection / original width
        break;
    case X_Axis::Left:
        original_x = bot_right_select.x() - m_origin_in_scene.x();
        new_x = bot_right_select.x() - mouse_in_scene.x();
        scale_x = (new_x) / (original_x + .0000001);
        break;
    case X_Axis::None:
        original_x = 0;     scale_x = 1;
    }

    switch (do_y) {
    case Y_Axis::Bottom:
        original_y = m_origin_in_scene.y() - top_left_select.y();
        new_y = mouse_in_scene.y() - top_left_select.y();
        scale_y = (new_y) / (original_y + .0000001);
        break;
    case Y_Axis::Top:
        original_y = bot_right_select.y() - m_origin_in_scene.y();
        new_y = bot_right_select.y() - mouse_in_scene.y();
        scale_y = (new_y) / (original_y + .0000001);
        break;
    case Y_Axis::None:
        original_y = 0;     scale_y = 1;
    }


    // Apply scaling to each selected item
    for (auto item : scene()->selectedItems()) {
        // Get stored data from each item
        QVariant get_data;
        QPointF my_scale, my_pos;
        get_data = item->data(User_Roles::Scale);       my_scale = get_data.toPointF();
        get_data = item->data(User_Roles::Position);    my_pos =   get_data.toPointF();

        // Apply new scale to each item
        qreal sx = my_scale.x() * scale_x;
        qreal sy = my_scale.y() * scale_y;
        if (sx <  .001 && sx >= 0) sx =  .001;
        if (sx > -.001 && sx <= 0) sx = -.001;
        if (sy <  .001 && sy >= 0) sy =  .001;
        if (sy > -.001 && sy <= 0) sy = -.001;
        QTransform scale_transform;
        scale_transform = QTransform::fromScale(sx, sy);
        item->setTransform(scale_transform);

        // Move each item as neccessary to keep them aligned within selection box

        qreal original_x_dist, original_y_dist;
        qreal new_left, new_top;

        switch (do_x) {
        case X_Axis::Right:
            original_x_dist = (my_pos.x() - top_left_select.x());
            new_left = top_left_select.x() + (original_x_dist * scale_x);
            break;
        case X_Axis::Left:
            original_x_dist = (bot_right_select.x() - my_pos.x());
            new_left = bot_right_select.x() - (original_x_dist * scale_x);
            break;
        case X_Axis::None:      new_left = my_pos.x();
        }

        switch (do_y) {
        case Y_Axis::Bottom:
            original_y_dist = (my_pos.y() - top_left_select.y());
            new_top =  top_left_select.y() + (original_y_dist * scale_y);
            break;
        case Y_Axis::Top:
            original_y_dist = (bot_right_select.y() - my_pos.y());
            new_top =  bot_right_select.y() - (original_y_dist * scale_y);
            break;
        case Y_Axis::None:      new_top = my_pos.y();
        }

        item->setPos(new_left, new_top);
    }




    m_interface->setLabelText(Label_Names::Label2,        "  x: " + QString::number(top_left_select.x() ) +
                                                          ", y:" +  QString::number(top_left_select.y() ) );
    m_interface->setLabelText(Label_Names::LabelObject1,  "  x: " + QString::number(m_origin_in_scene.x() ) +
                                                          ", y:" +  QString::number(m_origin_in_scene.y() ) );
    m_interface->setLabelText(Label_Names::LabelObject2,  "  x: " + QString::number(mouse_in_scene.x() ) +
                                                          ", y:" +  QString::number(mouse_in_scene.y() ) );
    m_interface->setLabelText(Label_Names::LabelObject3,  "  x: " + QString::number(scale_x) +
                                                          ", y:" +  QString::number(scale_y) );

}




















