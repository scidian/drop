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


//####################################################################################
//##        Constructor & destructor
//####################################################################################
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



//####################################################################################
//##        Key Events
//####################################################################################
// Key press event
void SceneGraphicsView::keyPressEvent(QKeyEvent *event)
{
    // When space bar is down, enabled mouse press and move to translate viewable area
    if (event->key() == Qt::Key::Key_Space) {   m_flag_key_down_spacebar = true;
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
    if (event->key() == Qt::Key::Key_Space) {   m_flag_key_down_spacebar = false;
        setDragMode(QGraphicsView::DragMode::NoDrag);
        setInteractive(true); }

    // Store when control / alt are released
    if (event->key() == Qt::Key::Key_Control) { m_flag_key_down_control = false; }
    if (event->key() == Qt::Key::Key_Alt) {     m_flag_key_down_alt = false; }



    QGraphicsView::keyReleaseEvent(event);
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



//####################################################################################
//##        Selection Rect Update Handling
//####################################################################################
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



//####################################################################################
//##        Mouse Events
//####################################################################################
// Mouse entered widget area event
void SceneGraphicsView::enterEvent(QEvent *event)
{
    setFocus(Qt::FocusReason::MouseFocusReason);                        // Set focus on mouse enter to allow for space bar pressing hand grab
    m_interface->setAdvisorInfo(Advisor_Info::Scene_Area);              // Set Advisor text on mouse enter
    QGraphicsView::enterEvent(event);
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
        for (int i = 0; i < static_cast<int>(Position_Flags::Total); i++) {
            if (m_handles[i].contains(adjust_mouse))
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

    // If no longer over handle and mouse is up, reset mouse cursor
    if (m_over_handle == Position_Flags::No_Position && m_view_mode == View_Mode::None && m_flag_key_down_spacebar == false)
        viewport()->unsetCursor();

    // Draw mouse coords on screen
    m_interface->setLabelText(Label_Names::LabelObject4, "Mouse X: " + QString::number(m_last_mouse_pos.x()) + ", Y: " + QString::number(m_last_mouse_pos.y()) );


    // ******************* If we're in selection mode, process mouse movement and resize box as needed
    if (m_view_mode == View_Mode::Selecting) {
        processSelection(event->pos());
        update();
    }

    // ******************* If mouse moved while over Size Grip, resize
    if (m_view_mode == View_Mode::Resizing) {
        resizeSelection(mapToScene(event->pos()));
        update();
    }

    // ******************* If mouse moved while alt pressed, rotate
    if (m_view_mode == View_Mode::Rotating) {
        rotateSelection(event->pos());
        update();
    }

    QGraphicsView::mouseMoveEvent(event);
    update();
    mouse_move_mutex.unlock();
}


void SceneGraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
    // Process left mouse button released
    if (event->button() & Qt::LeftButton)
    {
        m_rubber_band->hide();
        m_view_mode = View_Mode::None;
    }

    QGraphicsView::mouseReleaseEvent(event);
    update();
}























