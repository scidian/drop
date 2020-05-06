//
//      Created by Stephens Nunnally on 5/6/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QMouseEvent>
#include <QTimer>

#include "editor/interface_editor_relay.h"
#include "editor/world_map/world_map_item.h"
#include "editor/world_map/world_map_view.h"
#include "project/dr_project.h"


//####################################################################################
//##
//##    Mouse Pressed
//##
//####################################################################################
// Forwards a double click as just another mouse down
void WorldMapView::mouseDoubleClickEvent(QMouseEvent *event) { mousePressEvent(event); }

void WorldMapView::mousePressEvent(QMouseEvent *event) {
    // Test for GraphicsScene
    if (scene() == nullptr) return;

    // On initial mouse down, store mouse origin point
    m_origin =          event->pos();
    m_origin_in_scene = mapToScene(m_origin);

    // Get top most unlocked item
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

                        // ***** Process press event for item movement (Translation)

                        // Disable item changes before messing with Z-Order
                        WorldMapItem *graphics_item = dynamic_cast<WorldMapItem*>(m_origin_item);
                        bool flags_enabled_before = false;
                        if (graphics_item) {
///                            flags_enabled_before = graphics_item->itemChangeFlagsEnabled();
///                            graphics_item->disableItemChangeFlags();
                        }

                        // Make sure item is on top before firing QGraphicsView event so we start translating properly
                        double original_z = m_origin_item->zValue();
                        m_origin_item->setZValue(std::numeric_limits<double>::max());
                        QGraphicsView::mousePressEvent(event);
                        m_origin_item->setZValue(original_z);

                        // Restore item changes
///                        if (graphics_item && flags_enabled_before) graphics_item->enableItemChangeFlags();

                        // Prep Translating start
                        viewport()->setCursor(Qt::CursorShape::SizeAllCursor);

                        m_hide_bounding = true;
                        m_view_mode = View_Mode::Translating;
                        m_origin_item_start_pos = m_origin_item->pos();

                        // Pass on event to allow movement, store starting selection center for snapping calculations
                        if (m_allow_movement) {
///                            QPointF pre_center = my_scene->getSelectionTransform().map( my_scene->getSelectionBox().center() );
///                            my_scene->setPreMoveSelectionCenter(pre_center);
///                            my_scene->setHasCalculatedAdjustment(false);
                            QGraphicsView::mousePressEvent(event);
                        }

                        // Force itemChange signals on items
///                        for (auto item : my_scene->getSelectionItems()) {
///                            item->moveBy(0, 0);
///                        }

///                        if (my_scene->getSelectionItems().count() <= 1) {
                            m_editor_relay->buildInspector( { origin_item_key } );
                            m_editor_relay->updateItemSelection(Editor_Widgets::Map_View, { origin_item_key } );
///                        }
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
///                    startSelect(event);
///                    processSelection(event->pos());
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



