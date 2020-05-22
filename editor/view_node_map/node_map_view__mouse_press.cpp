//
//      Created by Stephens Nunnally on 5/6/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QMouseEvent>
#include <QTimer>

#include "core/dr_debug.h"
#include "editor/interface_editor_relay.h"
#include "editor/view_node_map/node_map_item.h"
#include "editor/view_node_map/node_map_scene.h"
#include "editor/view_node_map/node_map_view.h"
#include "project/dr_project.h"
#include "project/entities/dr_world.h"
#include "project/settings/settings_component_slot.h"


//####################################################################################
//##
//##    Mouse Pressed
//##
//####################################################################################
// Forwards a double click to mouse down
void NodeMapView::mouseDoubleClickEvent(QMouseEvent *event) {
    m_wants_double_click = true;
    mousePressEvent(event);
}

void NodeMapView::mousePressEvent(QMouseEvent *event) {
    // ***** Test for GraphicsScene
    if (scene() == nullptr) return;

    // ***** On initial mouse down, store mouse origin point
    m_origin =          event->pos();
    m_origin_in_scene = mapToScene(m_origin);

    // ***** Get top most unlocked item
    long        origin_item_key = c_no_key;
    DrSettings *origin_item_settings = nullptr;
    m_origin_item =  itemAt(event->pos());
    for (auto item : items(event->pos())) {
        origin_item_key = item->data(User_Roles::Key).toLongLong();
        origin_item_settings = m_project->findSettingsFromKey(origin_item_key);
        if (origin_item_settings != nullptr) {
            if (origin_item_settings->isLocked() == false) {
                m_origin_item = item;
                break;
            }
        }
    }

    // ***** Handle double click
    if (m_wants_double_click) {
        if (m_mouse_mode == Mouse_Mode::Pointer && origin_item_settings != nullptr) {
            if (origin_item_settings->getType() == DrType::World) {
                DrWorld *world = dynamic_cast<DrWorld*>(origin_item_settings);
                m_project->setOption(Project_Options::Current_Stage, world->getStartStageKey());
                m_editor_relay->updateItemSelection(Editor_Widgets::Node_View, { world->getKey() });
                m_editor_relay->setEditorMode(Editor_Mode::World_Editor);
            }
        }
        m_wants_double_click = false;
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


                // ******************* If no modifier keys are down, only select item under mouse. Also updates the Inspector
                //                           if the item clicked was already selected (but the Inspector was showing something
                //                           else like an asset or something)
                //                    #NOTE: If object was not already selected the Inspector will be updated when the
                //                           EditorScene::selectionChanged slot fires
                if (event->modifiers() == Qt::KeyboardModifier::NoModifier) {
                    if (m_origin_item != nullptr && origin_item_settings != nullptr) {

                        // ***** If we clicked clicked a new item, set selection group to that
                        if (scene()->selectedItems().contains(m_origin_item) == false) {
                            scene()->clearSelection();
                            m_origin_item->setSelected(true);
                        }

                        // ***** Process press event for item movement (Translation)                        
                        if (origin_item_settings->isLocked() == false) {
                            NodeMapItem *graphics_item = dynamic_cast<NodeMapItem*>(m_origin_item);

                            // Mouse down on Slot Circle
                            if (graphics_item != nullptr) {
                                DrSlot *over_slot = graphics_item->slotAtPoint(m_origin_in_scene);
                                if (over_slot != nullptr) {
                                    m_view_mode = View_Mode::Node_Connect;

                                    if (over_slot->getSlotType() == DrSlotType::Signal && over_slot->recentLineSlotCount() > 0) {
                                        m_slot_start = (*over_slot->recentLineSlots().begin());
                                        m_slot_start->removeConnection(over_slot);
                                    } else {
                                        m_slot_start = over_slot;
                                    }
                                }
                            }

                            // Mouse down over Item, if wasnt over Node Circle, start Translating
                            if (m_view_mode != View_Mode::Node_Connect) {
                                // Disable item changes before messing with Z-Order
                                bool flags_enabled_before = false;
                                if (graphics_item) {
                                    flags_enabled_before = graphics_item->itemChangeFlagsEnabled();
                                    graphics_item->disableItemChangeFlags();
                                }

                                // Make sure item is on top before firing QGraphicsView event so we start translating properly
                                double  original_z =   m_origin_item->zValue();
                                m_origin_item->setZValue(std::numeric_limits<double>::max());
                                QGraphicsView::mousePressEvent(event);
                                m_origin_item->setZValue(original_z);

                                // Restore item changes
                                if (graphics_item && flags_enabled_before) graphics_item->enableItemChangeFlags();

                                // Prep Translating start
                                viewport()->setCursor(Qt::CursorShape::SizeAllCursor);

                                m_view_mode = View_Mode::Translating;
                                m_origin_item_start_pos = m_origin_item->scenePos();

                                // Try to update mouseFirstPress Qt internal variables
                                ///QGraphicsView::mousePressEvent(event);

                                // Force itemChange signals on items
                                for (auto item : scene()->selectedItems()) {
                                    item->moveBy(0, 0);
                                }
                            }
                        }

                        m_editor_relay->buildInspector( { origin_item_key } );
                        m_editor_relay->updateItemSelection(Editor_Widgets::Node_View, { origin_item_key } );
                    }

                // ******************** If clicked while control is down, add to selection group, or take out
                } else if (event->modifiers() & Qt::KeyboardModifier::ControlModifier && m_origin_item != nullptr) {

                    if (m_origin_item != nullptr) {
                         m_origin_item->setSelected(!m_origin_item->isSelected());
                    }

                }


                // ******************* If theres no item under mouse, start selection box
                if (m_origin_item == nullptr) {
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


















