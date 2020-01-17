//
//      Created by Stephens Nunnally on 2/16/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      DrView MouseMove event
//
//
#include <QtMath>
#include <QDebug>
#include <QMouseEvent>
#include <QTimer>

#include "core/colors/colors.h"
#include "core/dr_string.h"
#include "editor/debug.h"
#include "editor/helper_library.h"
#include "editor/interface_editor_relay.h"
#include "editor/preferences.h"
#include "editor/view/editor_item.h"
#include "editor/view/editor_scene.h"
#include "editor/view/editor_view.h"
#include "project/dr_project.h"
#include "project/entities/dr_world.h"
#include "project/entities/dr_stage.h"
#include "project/entities/dr_thing.h"
#include "project/settings/settings.h"
#include "project/settings/settings_component.h"
#include "project/settings/settings_component_property.h"


//####################################################################################
//##    Mouse cursors to use in View Mouse Move events
//####################################################################################
namespace Mouse_Cursors {
    QCursor sizeVertical()      { return QCursor(QPixmap(":/assets/cursors/size_vertical.png")); }
    QCursor size022()           { return QCursor(QPixmap(":/assets/cursors/size_022.png")); }
    QCursor size045()           { return QCursor(QPixmap(":/assets/cursors/size_045.png")); }
    QCursor size067()           { return QCursor(QPixmap(":/assets/cursors/size_067.png")); }
    QCursor sizeHorizontal()    { return QCursor(QPixmap(":/assets/cursors/size_horizontal.png")); }
    QCursor size112()           { return QCursor(QPixmap(":/assets/cursors/size_112.png")); }
    QCursor size135()           { return QCursor(QPixmap(":/assets/cursors/size_135.png")); }
    QCursor size157()           { return QCursor(QPixmap(":/assets/cursors/size_157.png")); }
    QCursor rotateAll()         { return QCursor(QPixmap(":/assets/cursors/rotate_all.png")); }
    QCursor magnify()           { return QCursor(QPixmap(":/assets/cursors/magnify.png")); }
};


//####################################################################################
//##    Mouse Moved
//####################################################################################
void DrView::leaveEvent(QEvent *event) {
    (void)event;
    m_editor_relay->setMousePosition("", "");
}


//####################################################################################
//##    Mouse Moved
//####################################################################################
void DrView::mouseMoveEvent(QMouseEvent *event) {
    // Test for scene and lock the scene
    if (scene() == nullptr) return;
    if (my_scene->scene_mutex.tryLock(10) == false) return;

    // Update keyboard modifiers in case a keyPressEvent snuck through while we didnt have focus
    if (m_flag_dont_check_keys == false) {
        m_flag_key_down_control = event->modifiers() & Qt::KeyboardModifier::ControlModifier;
        m_flag_key_down_alt     = event->modifiers() & Qt::KeyboardModifier::AltModifier;
        m_flag_key_down_shift   = event->modifiers() & Qt::KeyboardModifier::ShiftModifier;
    } else {
        // This variable is used to stop modifier flags from updating when mouseMoveEvent is forcefully called from DrView::keyPressEvent()
        m_flag_dont_check_keys =  false;
    }

    // Store event mouse position
    m_last_mouse_pos = event->pos();

    // Allow movement if it has been more than x milliseconds or mouse has moved more than 2 pixels
    if (m_allow_movement == false) {
        if (m_origin_timer.elapsed() > 250) {
            m_allow_movement = true;
        } else if (QPoint(m_origin - m_last_mouse_pos).manhattanLength() > 2) {
            m_allow_movement = true;
        }
    }

    // Updates our tool tip position
    if (m_tool_tip->isHidden() == false) {
        m_tool_tip->updateToolTipPosition(m_last_mouse_pos);
    }

    // ********** Grab item under mouse
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

    // !!!!! #DEBUG: Shows red, green, blue and alpha of pixel under mouse
    if (Dr::CheckDebugFlag(Debug_Flags::Label_Top_Item_RGBA)) {
        QColor pixel_color = dynamic_cast<DrItem*>(item_under_mouse)->getColorAtPoint(m_last_mouse_pos, this);
        Dr::SetLabelText(Label_Names::Label_1, "R: " + QString::number(pixel_color.red()) +
                                               "G: " + QString::number(pixel_color.green()) +
                                               "B: " + QString::number(pixel_color.blue()) );
        Dr::SetLabelText(Label_Names::Label_2, "Aplha: " + QString::number(pixel_color.alpha()) );
    }
    // !!!!! END


    // ******************** Process Mouse Mode
    double mouse_angle = 0;
    if (m_mouse_mode == Mouse_Mode::Hand) {
        if (m_flag_key_down_spacebar == false) {
            spaceBarDown();
        }

    } else if (m_mouse_mode == Mouse_Mode::Magnify) {
        if (m_flag_key_down_spacebar == false) {
            viewport()->setCursor(Mouse_Cursors::magnify());
        }

    } else if (m_mouse_mode == Mouse_Mode::Pointer) {

        // ********** Check selection handles to see if mouse is over one
        if (m_over_handle == Position_Flags::Move_Item) m_over_handle = Position_Flags::No_Position;

        if (my_scene->getSelectionCount() > 0 && m_view_mode == View_Mode::None && m_flag_key_down_spacebar == false) {
            m_over_handle = Position_Flags::No_Position;

            if      (m_handles[Position_Flags::Rotate       ].containsPoint(m_last_mouse_pos, Qt::FillRule::OddEvenFill)) m_over_handle = Position_Flags::Rotate;
            else if (m_handles[Position_Flags::Top_Left     ].containsPoint(m_last_mouse_pos, Qt::FillRule::OddEvenFill)) m_over_handle = Position_Flags::Top_Left;
            else if (m_handles[Position_Flags::Top_Right    ].containsPoint(m_last_mouse_pos, Qt::FillRule::OddEvenFill)) m_over_handle = Position_Flags::Top_Right;
            else if (m_handles[Position_Flags::Bottom_Left  ].containsPoint(m_last_mouse_pos, Qt::FillRule::OddEvenFill)) m_over_handle = Position_Flags::Bottom_Left;
            else if (m_handles[Position_Flags::Bottom_Right ].containsPoint(m_last_mouse_pos, Qt::FillRule::OddEvenFill)) m_over_handle = Position_Flags::Bottom_Right;
            else if (m_handles[Position_Flags::Top          ].containsPoint(m_last_mouse_pos, Qt::FillRule::OddEvenFill)) m_over_handle = Position_Flags::Top;
            else if (m_handles[Position_Flags::Bottom       ].containsPoint(m_last_mouse_pos, Qt::FillRule::OddEvenFill)) m_over_handle = Position_Flags::Bottom;
            else if (m_handles[Position_Flags::Left         ].containsPoint(m_last_mouse_pos, Qt::FillRule::OddEvenFill)) m_over_handle = Position_Flags::Left;
            else if (m_handles[Position_Flags::Right        ].containsPoint(m_last_mouse_pos, Qt::FillRule::OddEvenFill)) m_over_handle = Position_Flags::Right;

            if (m_over_handle == Position_Flags::No_Position && my_scene->getSelectionItems().contains(item_under_mouse)) {
                if (m_flag_key_down_alt == false)
                    m_over_handle = Position_Flags::Move_Item;
            }

            if (m_flag_key_down_alt == true)
                m_over_handle = Position_Flags::Rotate;
        }

        // Check if over camera when Debug Camera mode is activated
        if (Dr::GetPreference(Preferences::World_Editor_Show_Camera_Boxes).toBool() && m_view_mode == View_Mode::None  && m_flag_key_down_spacebar == false) {
            if (m_cam_selected == nullptr) {
                DrThing *before_check = m_cam_mouse_over;
                DrThing *over = nullptr;
                for (auto &cam_data_pair : m_cam_data) {
                    if (cam_data_pair.second.view_rect.contains(m_last_mouse_pos)) {
                        bool closer = true;
                        if (over != nullptr) { if (m_cam_data[over].z_order < cam_data_pair.second.z_order) closer = false; }
                        if (closer) over = cam_data_pair.first;
                    }
                }
                m_cam_mouse_over = over;
                if (m_cam_mouse_over != nullptr)
                    m_over_handle = Position_Flags::Over_Camera;
                else if (m_over_handle == Position_Flags::Over_Camera)
                    m_over_handle = Position_Flags::No_Position;
                if (before_check != m_cam_mouse_over) this->update();
            }
        }

        // ********** If we are over a handle, and not doing anything, set cursor based on precalculated angle
        if (m_over_handle != Position_Flags::No_Position && m_view_mode == View_Mode::None && m_flag_key_down_spacebar == false) {

            if (m_over_handle == Position_Flags::Move_Item) {
                viewport()->setCursor(Qt::CursorShape::SizeAllCursor);
            } else if (m_over_handle == Position_Flags::Rotate) {
                viewport()->setCursor(Mouse_Cursors::rotateAll());
            } else if (m_over_handle == Position_Flags::Over_Camera) {
                viewport()->unsetCursor();
            } else {
                mouse_angle = m_handles_angles[m_over_handle];

                ///// Custom rotated cursor
                ///QPixmap arrow = QPixmap(":/assets/cursors/size_vertical.png", nullptr, Qt::ImageConversionFlag::AutoDither);
                ///QPixmap rotated = arrow.transformed(QTransform().rotate(a));
                ///int xoffset = (rotated.width() - arrow.width()) / 2;
                ///int yoffset = (rotated.height() - arrow.height()) / 2;
                ///rotated = rotated.copy(xoffset, yoffset, arrow.width(), arrow.height());
                ///viewport()->setCursor(rotated);

                if      (mouse_angle <  11.25) viewport()->setCursor(Mouse_Cursors::sizeVertical());            // 0        Top
                else if (mouse_angle <  33.75) viewport()->setCursor(Mouse_Cursors::size022());                 // 22.5
                else if (mouse_angle <  56.25) viewport()->setCursor(Mouse_Cursors::size045());                 // 45       Top Right
                else if (mouse_angle <  78.75) viewport()->setCursor(Mouse_Cursors::size067());                 // 67.5
                else if (mouse_angle < 101.25) viewport()->setCursor(Mouse_Cursors::sizeHorizontal());          // 90       Right
                else if (mouse_angle < 123.75) viewport()->setCursor(Mouse_Cursors::size112());                 // 112.5
                else if (mouse_angle < 146.25) viewport()->setCursor(Mouse_Cursors::size135());                 // 135      Bottom Right
                else if (mouse_angle < 168.75) viewport()->setCursor(Mouse_Cursors::size157());                 // 157
                else if (mouse_angle < 191.25) viewport()->setCursor(Mouse_Cursors::sizeVertical());            // 180      Bottom
                else if (mouse_angle < 213.75) viewport()->setCursor(Mouse_Cursors::size022());                 // 202
                else if (mouse_angle < 236.25) viewport()->setCursor(Mouse_Cursors::size045());                 // 225      Bottom Left
                else if (mouse_angle < 258.75) viewport()->setCursor(Mouse_Cursors::size067());                 // 247
                else if (mouse_angle < 281.25) viewport()->setCursor(Mouse_Cursors::sizeHorizontal());          // 270      Left
                else if (mouse_angle < 303.75) viewport()->setCursor(Mouse_Cursors::size112());                 // 292
                else if (mouse_angle < 326.25) viewport()->setCursor(Mouse_Cursors::size135());                 // 315      Top Left
                else if (mouse_angle < 348.75) viewport()->setCursor(Mouse_Cursors::size157());                 // 337
                else                           viewport()->setCursor(Mouse_Cursors::sizeVertical());            // 360      Top
            }
        }

        // ********** If no longer over handle and mouse is up, reset mouse cursor
        if (m_mouse_mode == Mouse_Mode::Pointer) {
            if (m_over_handle == Position_Flags::No_Position && m_view_mode == View_Mode::None && m_flag_key_down_spacebar == false) {
                viewport()->unsetCursor();
            }
        }
    }


    // !!!!! #DEBUG:    Draw mouse coords on screen
    if (Dr::CheckDebugFlag(Debug_Flags::Label_Mouse_Coordinates)) {
        long thing_count = 0;
        if (my_scene)
            if (my_scene->getCurrentStageShown())
                thing_count = static_cast<long>(my_scene->getCurrentStageShown()->getThingMap().size());
        Dr::SetLabelText(Label_Names::Label_Mouse_1, "Mouse Scene X: " + QString::number(mapToScene(m_last_mouse_pos).x()) +
                                                               ", Y: " + QString::number(mapToScene(m_last_mouse_pos).y()) );
        Dr::SetLabelText(Label_Names::Label_Mouse_2, "Mouse View  X: " + QString::number(m_last_mouse_pos.x()) +
                                                               ", Y: " + QString::number(m_last_mouse_pos.y()) );
        Dr::SetLabelText(Label_Names::Label_Pos_Flag, "Pos Flag: " + Dr::StringFromPositionFlag(m_over_handle) + QString("\t") +
                                                     "Pos Angle: " + QString::number(mouse_angle) + QString("\t") +
                                                    "Item Count: " + QString::number(thing_count)  );
        Dr::SetLabelText(Label_Names::Label_Object_5, "Assets: " + QString::number(m_project->getAssetMap().size()) +
                                                    ", Images: " + QString::number(m_project->getImageMap().size()) +
                                                    ", Animes: " + QString::number(m_project->getAnimationMap().size()) +
                                                     ", Fonts: " + QString::number(m_project->getFontMap().size()));
    }
    // !!!!! END

    // ***** Update mouse position on status bar
    m_editor_relay->setMousePosition(Dr::RoundToDecimalPlace( mapToScene(m_last_mouse_pos).x(), 1),
                                     Dr::RoundToDecimalPlace(-mapToScene(m_last_mouse_pos).y(), 1));

    // !!!!! #DEBUG:    Showing Item Data
    if (Dr::CheckDebugFlag(Debug_Flags::Label_Selected_Item_Data) && item_under_mouse != nullptr) {
        QGraphicsItem *item = item_under_mouse;
        QPointF my_scale =  item->data(User_Roles::Scale).toPointF();
        double  my_angle =  item->data(User_Roles::Rotation).toDouble();
        QPointF my_center = item->sceneTransform().map( item->boundingRect().center() );
        Dr::SetLabelText(Label_Names::Label_Position, "Pos X: " +  QString::number(item->pos().x()) +
                                                    ", Pos Y: " +  QString::number(item->pos().y()) );
        Dr::SetLabelText(Label_Names::Label_Center, "Center X: " + QString::number(my_center.x()) +
                                                         ", Y: " + QString::number(my_center.y()) );
        Dr::SetLabelText(Label_Names::Label_Scale, "Scale X: " +   QString::number(my_scale.x()) +
                                                 ", Scale Y: " +   QString::number(my_scale.y()) );
        Dr::SetLabelText(Label_Names::Label_Rotate, "Rotation: " + QString::number(my_angle) + ", Opacity: " + QString::number(item->opacity()));
        Dr::SetLabelText(Label_Names::Label_Z_Order, "Z-Order: " + QString::number(item->zValue()) + QString("\t") +
                                                             "View Mode: " + currentViewModeAsString() );
    } else if (Dr::CheckDebugFlag(Debug_Flags::Label_Selected_Item_Data)) {
        if (m_view_mode == View_Mode::None && item_under_mouse == nullptr) {
            Dr::SetLabelText(Label_Names::Label_Position,   "Null");
            Dr::SetLabelText(Label_Names::Label_Center,     "Null");
            Dr::SetLabelText(Label_Names::Label_Scale,      "Null");
            Dr::SetLabelText(Label_Names::Label_Rotate,     "Null");
            Dr::SetLabelText(Label_Names::Label_Z_Order,    "Null");
        }
    }
    // !!!!! END


    // ***** If we're not doing anything, update the advisor based on item under the mouse
    if (m_view_mode == View_Mode::None) {
        if (m_over_handle == Position_Flags::Over_Camera) {
            m_editor_relay->setAdvisorInfo(Advisor_Info::Camera_Edit);
        } else if (item_under_mouse != nullptr && m_project != nullptr) {
            QString header, body;
            long item_key = item_under_mouse->data(User_Roles::Key).toLongLong();
            DrThing *thing = m_project->findThingFromKey(item_key);
            if (thing != nullptr) {
                header =    QString::fromStdString(thing->getName());
                body =      "<b>Asset ID Key: " + QString::number(thing->getAssetKey()) + "</b><br>" +
                            item_under_mouse->data(User_Roles::Type).toString();
                m_editor_relay->setAdvisorInfo(header, body);
            }
        } else {
            m_editor_relay->setAdvisorInfo(Advisor_Info::Stage_View);
        }
    }

    // ******************* If we're in selection mode, process mouse movement and resize box as needed
    if (m_view_mode == View_Mode::Selecting) {
        processSelection(event->pos());
    }

    // ******************* If mouse moved while over Size Grip, resize
    if (m_view_mode == View_Mode::Resizing) {
        resizeSelection(mapToScene(event->pos()));
    }

    // ******************* If mouse moved while alt pressed, rotate
    if (m_view_mode == View_Mode::Rotating) {
        viewport()->setCursor(Mouse_Cursors::rotateAll());
        rotateSelection(event->pos());
    }

    // ******************* If mouse moved while alt pressed, rotate
    if (m_view_mode == View_Mode::Moving_Camera) {
        rotateCamera(event->pos());
    }

    // ******************* If mouse moved while in translating mode, update tooltip
    if (m_view_mode == View_Mode::Translating) {
        if (m_allow_movement) {
            // Pass on event to allow movement, store starting selection center for snapping calculations
            QPointF pre_center = my_scene->getSelectionTransform().map( my_scene->getSelectionBox().center() );
            my_scene->setPreMoveSelectionCenter( pre_center );
            my_scene->setHasCalculatedAdjustment(false);
            QGraphicsView::mouseMoveEvent(event);

            // Update selection box location, disable DrItem->itemChange before we update selection box by setting View_Mode to Disable_Update
            m_view_mode = View_Mode::Disable_Update;
            my_scene->updateSelectionBox();
            m_view_mode = View_Mode::Translating;

            if (m_tool_tip->getTipType() != View_Mode::Translating)
                m_tool_tip->startToolTip(View_Mode::Translating, m_origin, my_scene->getSelectionTransform().map(my_scene->getSelectionBox().center()) );
            else
                m_tool_tip->updateToolTipData( my_scene->getSelectionTransform().map(my_scene->getSelectionBox().center()) );
        }
    } else {
        QGraphicsView::mouseMoveEvent(event);
    }


    // ***** Update
    if (m_view_mode != View_Mode::None || m_tool_tip->isHidden() == false) {
        update();
    }

    // ***** Unlock scene mutex
    my_scene->scene_mutex.unlock();

    // ***** If we're hand dragging make sure selection box is updating, must be called AFTER mutex is unlocked
    if (m_view_mode == View_Mode::Dragging) updateSelectionBoundingBox(4);
}
















