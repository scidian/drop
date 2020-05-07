//
//      Created by Stephens Nunnally on 5/6/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QGraphicsItem>
#include <QMouseEvent>

#include "core/dr_math.h"
#include "core/dr_string.h"
#include "editor/helper_library.h"
#include "editor/interface_editor_relay.h"
#include "editor/world_map/world_map_view.h"
#include "project/dr_project.h"


//####################################################################################
//##
//##    Mouse Moved
//##
//####################################################################################
void WorldMapView::mouseMoveEvent(QMouseEvent *event) {
    // Test for GraphicsScene
    if (scene() == nullptr) return;

    // ***** Update keyboard modifiers in case a keyPressEvent snuck through while we didnt have focus
    if (m_flag_dont_check_keys == false) {
        m_flag_key_down_control = event->modifiers() & Qt::KeyboardModifier::ControlModifier;
        m_flag_key_down_alt     = event->modifiers() & Qt::KeyboardModifier::AltModifier;
        m_flag_key_down_shift   = event->modifiers() & Qt::KeyboardModifier::ShiftModifier;
    } else {
        // This variable is used to stop modifier flags from updating when mouseMoveEvent is forcefully called from EditorView::keyPressEvent()
        m_flag_dont_check_keys =  false;
    }

    // ***** Store event mouse position
    m_last_mouse_pos = event->pos();

    // ***** Allow movement if it has been more than x milliseconds or mouse has moved more than 2 pixels
    if (m_allow_movement == false) {
        if (m_origin_timer.elapsed() > 250) {
            m_allow_movement = true;
        } else if (QPoint(m_origin - m_last_mouse_pos).manhattanLength() > 2) {
            m_allow_movement = true;
        }
    }


    // ******************** Grab item under mouse
    QGraphicsItem *item_under_mouse = nullptr;/// = itemAt(m_last_mouse_pos);
    for (auto item : items(m_last_mouse_pos)) {
        long item_key = item->data(User_Roles::Key).toLongLong();
        DrSettings *settings = m_project->findSettingsFromKey(item_key);
        if (settings != nullptr) {
            if (settings->isLocked() == false) {
                item_under_mouse = item;
                break;
            }
        }
    }


    // ******************** Process Mouse Mode
    if (m_mouse_mode == Mouse_Mode::Pointer) {

        // Reset flag that holds which interactive mouse object we are over and has priority
        m_over_handle = Position_Flags::No_Position;

        // ***** Check selection handles to see if mouse is over one
        if (scene()->selectedItems().count() > 0 && m_view_mode == View_Mode::None && m_flag_key_down_spacebar == false) {

            if (m_over_handle == Position_Flags::No_Position && scene()->selectedItems().contains(item_under_mouse)) {
                m_over_handle = Position_Flags::Move_Item;
            }

        }

        // ***** If we are not doing anything (View_Mode::None), set cursor based on Position Flag
        if (m_view_mode == View_Mode::None && m_flag_key_down_spacebar == false) {

            if (m_over_handle == Position_Flags::Move_Item) {
                viewport()->setCursor(Qt::CursorShape::SizeAllCursor);
            } else { ///if (m_over_handle == Position_Flags::No_Position) {
                viewport()->unsetCursor();
            }

        }
    }



    // ***** Update mouse position on status bar
    m_editor_relay->setMousePosition(Dr::RoundToDecimalPlace( mapToScene(m_last_mouse_pos).x(), 1),
                                     Dr::RoundToDecimalPlace(-mapToScene(m_last_mouse_pos).y(), 1));


    // ***** If we're not doing anything, update the advisor based on item under the mouse
    if (m_view_mode == View_Mode::None) {
        if (item_under_mouse != nullptr && m_project != nullptr) {
            QString header, body;
            long item_key = item_under_mouse->data(User_Roles::Key).toLongLong();
            DrSettings *entity = m_project->findSettingsFromKey(item_key);
            if (entity != nullptr) {
                header =    QString::fromStdString(entity->getName());
                body =      item_under_mouse->data(User_Roles::Type).toString();
                m_editor_relay->setAdvisorInfo(header, body);
            }
        } else {
            m_editor_relay->setAdvisorInfo(Advisor_Info::Stage_View);
        }
    }

    // ******************* If mouse moved while in translating mode, update tooltip
    if (m_view_mode == View_Mode::Translating) {
        if (m_allow_movement) {
            QGraphicsView::mouseMoveEvent(event);
        }
    } else {
        QGraphicsView::mouseMoveEvent(event);
    }


    // ***** Update
    if (m_view_mode != View_Mode::None) {
        update();
    }

}
















