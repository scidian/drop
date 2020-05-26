//
//      Created by Stephens Nunnally on 5/1/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QKeyEvent>

#include "editor/interface_editor_relay.h"
#include "editor/view_node/node_item.h"
#include "editor/view_node/node_scene.h"
#include "editor/view_node/node_view.h"


//####################################################################################
//##    Key Events
//####################################################################################
// Key press event
void NodeView::keyPressEvent(QKeyEvent *event) {
    // When space bar is down, enabled mouse press and move to translate viewable area
    if (event->key() == Qt::Key::Key_Space)     spaceBarDown();
    if (event->key() == Qt::Key::Key_Control)   m_flag_key_down_control = true;
    if (event->key() == Qt::Key::Key_Alt)       m_flag_key_down_alt = true;
    if (event->key() == Qt::Key::Key_Shift)     m_flag_key_down_shift = true;

    QGraphicsView::keyPressEvent(event);

    // Fire mouse move event to update mouse cursor on key presses
    QMouseEvent *fire_mouse_move = new QMouseEvent(QEvent::MouseMove, m_last_mouse_pos, Qt::NoButton,
                                                   Qt::MouseButtons(), { Qt::KeyboardModifier::NoModifier });
    m_flag_dont_check_keys = true;
    mouseMoveEvent(fire_mouse_move);
    delete fire_mouse_move;
}

// Key release event
void NodeView::keyReleaseEvent(QKeyEvent *event) {
    // When space bar is released, change mode back to select / move items
    if (event->key() == Qt::Key::Key_Space)     spaceBarUp();
    if (event->key() == Qt::Key::Key_Control)   m_flag_key_down_control = false;
    if (event->key() == Qt::Key::Key_Alt)       m_flag_key_down_alt = false;
    if (event->key() == Qt::Key::Key_Shift)     m_flag_key_down_shift = false;

    QGraphicsView::keyReleaseEvent(event);

    // Fire mouse move event to update mouse cursor on key release
    QMouseEvent *fire_mouse_move = new QMouseEvent(QEvent::MouseMove, m_last_mouse_pos, Qt::NoButton,
                                                   Qt::MouseButtons(), { Qt::KeyboardModifier::NoModifier });
    mouseMoveEvent(fire_mouse_move);
    delete fire_mouse_move;
}

void NodeView::spaceBarDown() {
    m_flag_key_down_spacebar = true;
    setDragMode(QGraphicsView::DragMode::ScrollHandDrag);
    setInteractive(false);
}

void NodeView::spaceBarUp() {
    m_flag_key_down_spacebar = false;
    if (m_mouse_mode != Mouse_Mode::Hand) {
        setDragMode(QGraphicsView::DragMode::NoDrag);
        setInteractive(true);
    }
}
















