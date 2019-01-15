//
//      Created by Stephens Nunnally on 1/15/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Graphics View Handling - Mouse Events
//
//

#include "project.h"
#include "project_world.h"
#include "project_world_scene.h"
#include "project_world_scene_object.h"
#include "editor_scene_item.h"

#include "settings.h"
#include "settings_component.h"
#include "settings_component_property.h"

#include "editor_scene_view.h"
#include "interface_relay.h"



//####################################################################################
//##        Mouse Events
//####################################################################################

// Mouse entered widget area event
void SceneGraphicsView::enterEvent(QEvent *event)
{
    setFocus(Qt::FocusReason::MouseFocusReason);                        // Set focus on mouse enter to allow for space bar pressing hand grab
    m_relay->setAdvisorInfo(Advisor_Info::Scene_Area);                  // Set Advisor text on mouse enter
    QGraphicsView::enterEvent(event);
}

// Mouse was pressed
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

            if (scene()->selectedItems().count() > 0) {
                // ******************* If clicked while holding Alt key
                if (event->modifiers() & Qt::KeyboardModifier::AltModifier) {
                    startRotate();
                    return;
                }

                // ******************* If clicked while in a Size Grip Handle
                if (m_over_handle != Position_Flags::No_Position) {
                    startResize();
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
                    m_view_mode = View_Mode::Translating;
                }
            } else if (event->modifiers() & Qt::KeyboardModifier::ControlModifier) {
                if (m_origin_item != nullptr) m_origin_item->setSelected(!m_origin_item->isSelected());
            }


            // ******************* If theres no item under mouse, start selection box
            if (m_origin_item == nullptr) {
                startSelect(event);
                return;
            }

        }

    } else {
        // Process mouse press event for hand drag
        QGraphicsView::mousePressEvent(event);
        m_view_mode = View_Mode::Translating;
    }

    update();
}


// Mouse was moved over View
void SceneGraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    // Try and lock function, so we ony run this once at a time
    if (mouse_move_mutex.tryLock(10) == false) return;

    m_last_mouse_pos = event->pos();                                    // Store event mouse position
    update();                                                           // Call Paint Event (which update rects) before we start

    // Adjust for Qt Arrow not having point right at tip of
    QPointF adjust_mouse = m_last_mouse_pos;
    adjust_mouse.setX(m_last_mouse_pos.x() - 2);
    adjust_mouse.setY(m_last_mouse_pos.y() - 2);


    // ******************** Check selection handles to see if mouse is over one
    if (scene()->selectedItems().count() > 0 && m_view_mode == View_Mode::None && m_flag_key_down_spacebar == false) {
        m_over_handle = Position_Flags::No_Position;
        if (m_handles[Position_Flags::Top].containsPoint(adjust_mouse, Qt::FillRule::OddEvenFill))    m_over_handle = Position_Flags::Top;
        if (m_handles[Position_Flags::Bottom].containsPoint(adjust_mouse, Qt::FillRule::OddEvenFill)) m_over_handle = Position_Flags::Bottom;
        if (m_handles[Position_Flags::Left].containsPoint(adjust_mouse, Qt::FillRule::OddEvenFill))   m_over_handle = Position_Flags::Left;
        if (m_handles[Position_Flags::Right].containsPoint(adjust_mouse, Qt::FillRule::OddEvenFill))  m_over_handle = Position_Flags::Right;

        // Check corners second as they're more important
        if (m_handles[Position_Flags::Top_Left].containsPoint(adjust_mouse, Qt::FillRule::OddEvenFill))     m_over_handle = Position_Flags::Top_Left;
        if (m_handles[Position_Flags::Top_Right].containsPoint(adjust_mouse, Qt::FillRule::OddEvenFill))    m_over_handle = Position_Flags::Top_Right;
        if (m_handles[Position_Flags::Bottom_Left].containsPoint(adjust_mouse, Qt::FillRule::OddEvenFill))  m_over_handle = Position_Flags::Bottom_Left;
        if (m_handles[Position_Flags::Bottom_Right].containsPoint(adjust_mouse, Qt::FillRule::OddEvenFill)) m_over_handle = Position_Flags::Bottom_Right;

        double a = calcRotationAngleInDegrees(m_handles_centers[m_over_handle], mapFromScene(m_selection_rect.center()));
        if (a >= 292.5 && a < 337.5) viewport()->setCursor(Qt::CursorShape::SizeFDiagCursor);              // Top Left
        if (a >= 337.5 || a <  22.5) viewport()->setCursor(Qt::CursorShape::SizeVerCursor);                // Top
        if (a >=  22.5 && a <  67.5) viewport()->setCursor(Qt::CursorShape::SizeBDiagCursor);              // Top Right
        if (a >=  67.5 && a < 112.5) viewport()->setCursor(Qt::CursorShape::SizeHorCursor);                // Right
        if (a >= 112.5 && a < 157.5) viewport()->setCursor(Qt::CursorShape::SizeFDiagCursor);              // Bottom Right
        if (a >= 157.5 && a < 202.5) viewport()->setCursor(Qt::CursorShape::SizeVerCursor);                // Bottom
        if (a >= 202.5 && a < 247.5) viewport()->setCursor(Qt::CursorShape::SizeBDiagCursor);              // Bottom Left
        if (a >= 247.5 && a < 292.5) viewport()->setCursor(Qt::CursorShape::SizeHorCursor);                // Left
    }

    // If no longer over handle and mouse is up, reset mouse cursor
    if (m_over_handle == Position_Flags::No_Position && m_view_mode == View_Mode::None && m_flag_key_down_spacebar == false)
        viewport()->unsetCursor();




    // !!!!! TEMP: Draw mouse coords on screen
    m_relay->setLabelText(Label_Names::Label_Mouse_1, "Mouse Scene X: " + QString::number(mapToScene(m_last_mouse_pos).x()) +
                                                                ", Y: " + QString::number(mapToScene(m_last_mouse_pos).y()) );
    m_relay->setLabelText(Label_Names::Label_Mouse_2, "Mouse View  X: " + QString::number(m_last_mouse_pos.x()) +
                                                                ", Y: " + QString::number(m_last_mouse_pos.y()) );
    m_relay->setLabelText(Label_Names::Label_Pos_Flag, "Position Flag: " + QString::number(static_cast<int>(m_over_handle)) );
    // !!!!! END




    // ******************* If we're in selection mode, process mouse movement and resize box as needed
    if (m_view_mode == View_Mode::Selecting)
        processSelection(event->pos());

    // ******************* If mouse moved while over Size Grip, resize
    if (m_view_mode == View_Mode::Resizing)
        resizeSelection(mapToScene(event->pos()));

    // ******************* If mouse moved while alt pressed, rotate
    if (m_view_mode == View_Mode::Rotating)
        rotateSelection(event->pos());


    // Pass on event, update and unlock mutex
    QGraphicsView::mouseMoveEvent(event);
    update();
    mouse_move_mutex.unlock();
}


// Mouse was released
void SceneGraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
    // Process left mouse button released
    if (event->button() & Qt::LeftButton)
    {
        m_rubber_band->hide();
        m_view_mode = View_Mode::None;
    }

    // Pass on event, update
    QGraphicsView::mouseReleaseEvent(event);
    update();
}




//####################################################################################
//##        Wheel Event / Zoom Functions
//####################################################################################

// Handles zooming in / out of view with mouse wheel
#if QT_CONFIG(wheelevent)
void SceneGraphicsView::wheelEvent(QWheelEvent *event)
{
    if (event->modifiers() & Qt::KeyboardModifier::ControlModifier) {
        QGraphicsView::wheelEvent(event);
        return;
    }
    if (event->delta() > 0) {
        zoomInOut(10);  }
    else {
        zoomInOut(-10); }
    event->accept();
}
#endif

void SceneGraphicsView::zoomInOut(int level)
{
    m_zoom += level;
    if (m_zoom > 500) m_zoom = 500;
    if (m_zoom < 100) m_zoom = 100;
    applyUpdatedMatrix();
}

void SceneGraphicsView::applyUpdatedMatrix()
{
    m_zoom_scale = qPow(qreal(2), (m_zoom - 250) / qreal(50));
    QMatrix matrix;
    matrix.scale(m_zoom_scale, m_zoom_scale);
    matrix.rotate(m_rotate);
    this->setMatrix(matrix);

    // !!!!! TEMP:
    m_relay->setLabelText(Label_Names::Label_2, "Zoom: " + QString::number(m_zoom) +
                                             ", Scale: " + QString::number(m_zoom_scale) );
    // !!!!! END
}














