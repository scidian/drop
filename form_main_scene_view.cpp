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
    if (event->key() == Qt::Key::Key_Space) {
        setDragMode(QGraphicsView::DragMode::ScrollHandDrag);
        setInteractive(false);
    }

    QGraphicsView::keyPressEvent(event);
}

// Key release event
void SceneGraphicsView::keyReleaseEvent(QKeyEvent *event)
{
    // When space bar is released, change mode back to select / move items
    if (event->key() == Qt::Key::Key_Space) {
        setDragMode(QGraphicsView::DragMode::NoDrag);
        setInteractive(true);
    }

    QGraphicsView::keyReleaseEvent(event);
}





void SceneGraphicsView::mousePressEvent(QMouseEvent *event)
{
    // On initial mouse down, set mouse origin point
    m_origin = event->pos();

    // If space bar isnt down, start selection box
    if (dragMode() == QGraphicsView::DragMode::NoDrag) {
        // If theres an item under mouse, don't start selection box
        if (itemAt(event->pos()) == nullptr ) {
            m_is_selecting = true;                                          // Flag that we're in selection mode
            m_items_start = scene()->selectedItems();                       // Store list of originally selected items
            m_rubber_band->setGeometry(QRect(m_origin, QSize()));           // Start selection box with no size
            m_rubber_band->show();                                          // Make selection box visible
        }
    }

    QGraphicsView::mousePressEvent(event);
    update();
}
void SceneGraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    // If we're in selection mode, process mouse movement and resize box as needed
    if (m_is_selecting) {
        m_rubber_band->setGeometry(QRect(m_origin, event->pos()).normalized());                             // Resize selection box

        QPainterPath selectionArea;
        selectionArea.addPolygon(mapToScene(m_rubber_band->geometry()));                                    // Convert box to scene coords
        selectionArea.closeSubpath();

        scene()->setSelectionArea(selectionArea, Qt::ItemSelectionOperation::AddToSelection,
                                  Qt::ItemSelectionMode::IntersectsItemShape, viewportTransform());         // Pass box to scene for selection

        // Go through selected items, unselect if rubber band box covered them and has since shrunk
        for (auto item : scene()->selectedItems()) {
            QPainterPath item_location = item->shape();                                                     // Get items shape
            item_location.translate(item->pos().x(), item->pos().y());                                      // And translate to scene coords

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

    QGraphicsView::mouseMoveEvent(event);
    update();
}
void SceneGraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
    // If we were in selection mode, stop it and hide selection box
    if (m_is_selecting) {
        m_rubber_band->hide();
        m_is_selecting = false;
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
    painter.setPen(QPen(Qt::white, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.setBrush(Qt::NoBrush);

    for (auto item: scene()->selectedItems()) {
        QRectF rect = item->boundingRect();                                 // Get item bounding box
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






















