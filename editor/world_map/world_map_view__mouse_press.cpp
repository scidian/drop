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
#include "editor/world_map/world_map_item.h"
#include "editor/world_map/world_map_scene.h"
#include "editor/world_map/world_map_view.h"
#include "project/dr_project.h"
#include "project/entities/dr_world.h"


//####################################################################################
//##
//##    Mouse Pressed
//##
//####################################################################################
// Forwards a double click to mouse down
void WorldMapView::mouseDoubleClickEvent(QMouseEvent *event) {
    m_wants_double_click = true;
    mousePressEvent(event);
}

void WorldMapView::mousePressEvent(QMouseEvent *event) {
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
        if (origin_item_settings != nullptr) {
            if (origin_item_settings->getType() == DrType::World) {
                DrWorld *world = dynamic_cast<DrWorld*>(origin_item_settings);
                m_project->setOption(Project_Options::Current_Stage, world->getStartStageKey());
                m_editor_relay->updateItemSelection(Editor_Widgets::Stage_View, { world->getKey() });
                m_editor_relay->setEditorMode(Editor_Mode::World_Editor);
            }
        }
        m_wants_double_click = false;
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
                    if (origin_item_settings != nullptr) {

                        // ***** If we clicked clicked a new item, set selection group to that
                        if (scene()->selectedItems().contains(m_origin_item) == false) {
                            scene()->clearSelection();
                            m_origin_item->setSelected(true);
                        }

                        // ***** Process press event for item movement (Translation)                        
                        if (origin_item_settings->isLocked() == false) {
                            WorldMapItem *graphics_item = dynamic_cast<WorldMapItem*>(m_origin_item);

                            // Mouse down on Slot Circle
                            if (graphics_item != nullptr) {
                                DrSlot over_slot = graphics_item->slotAtPoint(m_origin_in_scene);
                                if (over_slot.owner_key != c_no_key) {
                                    m_view_mode = View_Mode::Node_Connect;
                                    m_slot_start = over_slot;
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

                                m_hide_bounding = true;
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
                        m_editor_relay->updateItemSelection(Editor_Widgets::Map_View, { origin_item_key } );
                    }

                // ******************** If clicked while control is down, add to selection group, or take out
                } else if (event->modifiers() & Qt::KeyboardModifier::ControlModifier && m_origin_item != nullptr) {

                    if (m_origin_item != nullptr) {
                         m_origin_item->setSelected(!m_origin_item->isSelected());
                    }

                }


                // ******************* If theres no item under mouse, start selection box
                if (m_origin_item == nullptr) {
                    ///m_view_mode = View_Mode::Selecting;
                    scene()->clearSelection();
                    m_editor_relay->buildInspector( { } );
                    m_editor_relay->updateItemSelection(Editor_Widgets::Map_View, { } );
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


















