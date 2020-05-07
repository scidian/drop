//
//      Created by Stephens Nunnally on 1/15/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Graphics View Handling - Mouse Events
//
//
#include <QtMath>
#include <QDebug>
#include <QMouseEvent>
#include <QScrollBar>
#include <QTimer>

#include "core/colors/colors.h"
#include "editor/helper_library.h"
#include "editor/interface_editor_relay.h"
#include "editor/preferences.h"
#include "editor/trees/tree_inspector.h"
#include "editor/view/editor_item.h"
#include "editor/view/editor_scene.h"
#include "editor/view/editor_view.h"
#include "engine/debug_flags.h"
#include "project/dr_project.h"
#include "project/entities/dr_world.h"
#include "project/entities/dr_stage.h"
#include "project/entities/dr_thing.h"
#include "project/settings/settings.h"
#include "project/settings/settings_component.h"
#include "project/settings/settings_component_property.h"


//####################################################################################
//##
//##    Mouse Pressed
//##
//####################################################################################
// Forwards a double click as just another mouse down
void EditorView::mouseDoubleClickEvent(QMouseEvent *event) { mousePressEvent(event); }

void EditorView::mousePressEvent(QMouseEvent *event) {
    // Test for scene, convert to our custom class and lock the scene
    if (scene() == nullptr) return;
    QMutexLocker lock_scene(&my_scene->scene_mutex);

    // On initial mouse down, store mouse origin point
    m_origin =          event->pos();
    m_origin_in_scene = mapToScene(m_origin);

    // Get top most unlocked item
    long     origin_item_key = c_no_key;
    DrThing *origin_item_thing = nullptr;
    bool     origin_locked = false;
    m_origin_item =  itemAt(event->pos());
    for (auto item : items(event->pos())) {
        origin_item_key = item->data(User_Roles::Key).toLongLong();
        origin_item_thing = m_project->findThingFromKey(origin_item_key);
        if (origin_item_thing != nullptr) {
            if (origin_item_thing->isLocked() == false) {
                m_origin_item = item;
                break;
            }
        }
    }


    // ******************* Process Hand Mouse Mode
    if (m_mouse_mode == Mouse_Mode::Hand) {
        QGraphicsView::mousePressEvent(event);
        m_view_mode = View_Mode::Dragging;
    }


    // ******************* Process Magnify Mouse Mode
    if (m_mouse_mode == Mouse_Mode::Magnify) {
        if (dragMode() == QGraphicsView::DragMode::NoDrag) {
            bool control_down = event->modifiers() & Qt::KeyboardModifier::ControlModifier;
            if (event->button() & Qt::LeftButton) {
                zoomInOut(control_down ? -50 :  50);
            } else if (event->button() & Qt::RightButton) {
                zoomInOut(control_down ?  50 : -50);
            }

        } else {
            QGraphicsView::mousePressEvent(event);
            m_view_mode = View_Mode::Dragging;
        }
    }


    // ******************* Prcoess Pointer Mouse Mode: If space bar isn't down, process mouse down
    if (m_mouse_mode == Mouse_Mode::Pointer) {
        if (dragMode() == QGraphicsView::DragMode::NoDrag) {

            // If left mouse button down
            if (event->button() & Qt::LeftButton) {
                // Restart mouse select movement buffer timer
                m_origin_timer.restart();
                m_allow_movement = false;

                // ******************** If clicked on Stage resize box, need to start resizing it
                if (m_over_handle == Position_Flags::Stage_Size) {
                    m_view_mode = View_Mode::Resizing_Stage;
                    m_editor_relay->buildInspector( { my_scene->getCurrentStageShown()->getKey() } );
                    startResizeStage(m_origin);
                    return;
                }

                // ******************** If clicked on camera, need to start rotating it
                if (m_over_handle == Position_Flags::Over_Camera && m_cam_mouse_over != nullptr) {
                    // Select camera's parent graphics item in Object Inspector
                    QGraphicsItem *check_selected = setInspectorClearSelection(m_cam_mouse_over);
                    if (check_selected != nullptr) m_origin_item = check_selected;

                    // Set mode to Moving Camera, start the rotation and exit
                    if (m_cam_mouse_over->isLocked() == false) {
                        m_view_mode = View_Mode::Moving_Camera;
                        startRotateCamera(m_origin);
                        return;
                    }
                }

                if (my_scene->getSelectionCount() > 0) {
                    // ******************* If clicked while holding Alt key start rotating
                    if (event->modifiers() & Qt::KeyboardModifier::AltModifier || m_over_handle == Position_Flags::Rotate) {
                        m_view_mode = View_Mode::Rotating;
                        startRotateSelection(m_origin);
                        return;
                    }

                    // ******************* If clicked while in a Size Grip Handle start resizing
                    if (m_over_handle != Position_Flags::No_Position && m_over_handle != Position_Flags::Move_Item) {
                        m_view_mode = View_Mode::Resizing;
                        startResizeSelection(m_origin);
                        return;
                    }
                }


                // ******************* If no modifier keys are down, only select item under mouse. Also updates the Inspector
                //                           if the item clicked was already selected (but the Inspector was showing something
                //                           else like an asset or something)
                //                    #NOTE: If object was not already selected the Inspector will be updated when the
                //                           EditorScene::selectionChanged slot fires
                if (event->modifiers() == Qt::KeyboardModifier::NoModifier) {
                    if (origin_item_thing != nullptr) {

                        // ***** If we clicked clicked a new item, set selection group to that
                        if (my_scene->getSelectionItems().contains(m_origin_item) == false) {
                            my_scene->clearSelection();
                            m_origin_item->setSelected(true);
                        }

                        // ***** Process press event for item movement (Translation)
                        if (origin_item_thing->isLocked() == false) {
                            // Disable item changes before messing with Z-Order
                            EditorItem *graphics_item = dynamic_cast<EditorItem*>(m_origin_item);
                            bool flags_enabled_before = false;
                            if (graphics_item) {
                                flags_enabled_before = graphics_item->itemChangeFlagsEnabled();
                                graphics_item->disableItemChangeFlags();
                            }

                            // Make sure item is on top before firing QGraphicsView event so we start translating properly
                            double original_z = m_origin_item->zValue();
                            m_origin_item->setZValue(std::numeric_limits<double>::max());
                            QGraphicsView::mousePressEvent(event);
                            m_origin_item->setZValue(original_z);

                            // Restore item changes
                            if (graphics_item && flags_enabled_before) graphics_item->enableItemChangeFlags();

                            // Prep Translating start
                            viewport()->setCursor(Qt::CursorShape::SizeAllCursor);
                            QTimer::singleShot(500, this, SLOT(checkTranslateToolTipStarted()));

                            m_hide_bounding = true;
                            m_view_mode = View_Mode::Translating;
                            m_origin_item_start_pos = m_origin_item->pos();

                            // Pass on event to allow movement, store starting selection center for snapping calculations
                            if (m_allow_movement) {
                                QPointF pre_center = my_scene->getSelectionTransform().map( my_scene->getSelectionBox().center() );
                                my_scene->setPreMoveSelectionCenter(pre_center);
                                my_scene->setHasCalculatedAdjustment(false);
                                QGraphicsView::mousePressEvent(event);
                            }

                            // Force itemChange signals on items
                            for (auto item : my_scene->getSelectionItems()) {
                                item->moveBy(0, 0);
                            }
                        } else {
                            origin_locked = true;
                        }

                        if (my_scene->getSelectionItems().count() <= 1) {
                            m_editor_relay->buildInspector( { origin_item_key } );
                            m_editor_relay->updateItemSelection(Editor_Widgets::Stage_View, { origin_item_key } );
                        }
                    }

                // ******************** If clicked while control is down, add to selection group, or take out
                } else if (event->modifiers() & Qt::KeyboardModifier::ControlModifier && m_origin_item != nullptr) {

                    if (m_origin_item != nullptr) {
                         m_origin_item->setSelected(!m_origin_item->isSelected());
                    }

                }


                // ******************* If theres no item under mouse, start selection box
                if (m_origin_item == nullptr || origin_locked) {
                    m_view_mode = View_Mode::Selecting;
                    startSelect(event);
                    processSelection(event->pos());
                    return;
                }

            }


        // ******************** Process mouse press event for hand drag
        } else {
            QGraphicsView::mousePressEvent(event);
            m_view_mode = View_Mode::Dragging;
        }
    }

    update();
}


//####################################################################################
//##    SLOT:   Fired from single shot timer when mouse is down,
//##            starts tooltip after x milliseconds if user pressed mouse but hasn't started moving it yet
//####################################################################################
void EditorView::checkTranslateToolTipStarted() {
    if (m_view_mode == View_Mode::Translating) {
        if (m_tool_tip->getTipType() != View_Mode::Translating) {
            my_scene->updateSelectionBox();
            m_tool_tip->startToolTip(View_Mode::Translating, m_origin, my_scene->getSelectionTransform().map(my_scene->getSelectionBox().center()) );
            m_allow_movement = true;
            update();
        }
    }
}


//####################################################################################
//##    For Camera Mode
//##        Looks through Items and if it finds an Item representing DrThing argument, clears selection and sets
//##        Object Inspector to that Item. Makes it easier to interact with camera settings without selection box in the way
//####################################################################################
QGraphicsItem* EditorView::setInspectorClearSelection(DrThing *thing) {
    QGraphicsItem *found_item = nullptr;
    for (auto &item : items()) {
        EditorItem *graphics_item = dynamic_cast<EditorItem*>(item);
        if (graphics_item == nullptr) continue;
        if (graphics_item->getThing() == thing) {
            found_item = graphics_item;
            if (my_scene->selectedItems().count() > 0) my_scene->clearSelection();
            if (m_editor_relay->getInspector()->getSelectedKeys().contains(thing->getKey()) == false) {
                m_editor_relay->buildInspector( { thing->getKey() } );
                m_editor_relay->updateItemSelection(Editor_Widgets::Stage_View, { thing->getKey() } );
            }
            break;
        }
    }
    return found_item;
}









