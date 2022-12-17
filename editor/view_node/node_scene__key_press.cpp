//
//      Created by Stephens Nunnally on 5/23/2020, (c) 2020 Scidian Studios, All Rights Reserved
//
//  File:
//
//
//
#include <QGraphicsItem>
#include <QKeyEvent>

#include "core/dr_math.h"
#include "editor/helper_library.h"
#include "editor/interface_editor_relay.h"
#include "editor/preferences.h"
#include "editor/view_node/node_item.h"
#include "editor/view_node/node_scene.h"
#include "editor/view_node/node_view.h"
#include "project/dr_project.h"
#include "project/entities/dr_world.h"


//####################################################################################
//##    Key Release
//####################################################################################
void NodeScene::keyReleaseEvent(QKeyEvent *event) {
    // Reset key timer
    Qt::Key key_released = static_cast<Qt::Key>(event->key());
    m_key_down[key_released] = false;

    QGraphicsScene::keyReleaseEvent(event);
}


//####################################################################################
//##    Key Press
//####################################################################################
void NodeScene::keyPressEvent(QKeyEvent *event) {

    // ***** If no selected items, pass on key press event and exit
    if (selectedItems().size() < 1) {
        QGraphicsScene::keyPressEvent(event);
        return;
    }

    // ***** Start tracking time key has been pressed
    Qt::Key key_pressed = static_cast<Qt::Key>(event->key());
    if (m_key_down[key_pressed] == false) {
        m_key_timers[key_pressed] = QElapsedTimer();
        m_key_timers[key_pressed].restart();
        m_key_down[key_pressed] = true;
    }


    // ********** Process Movement key press on all selected items
    if (key_pressed == Qt::Key_Up ||
        key_pressed == Qt::Key_Down ||
        key_pressed == Qt::Key_Left ||
        key_pressed == Qt::Key_Right) {

        DrPointF grid_scale = DrPointF(1.0, 1.0);
        DrPointF grid_size =  DrPointF(c_node_grid_size, c_node_grid_size);

        // Amount to move items when arrow keys are pressed
        QPointF move_by (grid_size.x * grid_scale.x, grid_size.y * grid_scale.y);
        if (Dr::GetPreference(Preferences::Editor_Snap_To_Grid).toBool() == false) {
            if        (m_key_timers[key_pressed].elapsed() <  750) {    move_by.setX(   1.0 );  move_by.setY(   1.0 );
            } else if (m_key_timers[key_pressed].elapsed() < 1500) {    move_by.setX(   5.0 );  move_by.setY(   5.0 );
            } else if (m_key_timers[key_pressed].elapsed() < 2250) {    move_by.setX(  25.0 );  move_by.setY(  25.0 );
            } else {                                                    move_by.setX( 100.0 );  move_by.setY( 100.0 ); }
        } else {
            if        (m_key_timers[key_pressed].elapsed() < 1000) {    move_by.setX( move_by.x() *  1.0 );  move_by.setY( move_by.y() *  1.0 );
            } else if (m_key_timers[key_pressed].elapsed() < 2000) {    move_by.setX( move_by.x() *  2.0 );  move_by.setY( move_by.y() *  2.0 );
            } else if (m_key_timers[key_pressed].elapsed() < 3000) {    move_by.setX( move_by.x() *  5.0 );  move_by.setY( move_by.y() *  5.0 );
            } else {                                                    move_by.setX( move_by.x() * 10.0 );  move_by.setY( move_by.y() * 10.0 ); }
        }

        for (auto item : selectedItems()) {
            double move_x = 0.0;
            double move_y = 0.0;
            if (key_pressed == Qt::Key_Up)     move_y = -move_by.y();
            if (key_pressed == Qt::Key_Down)   move_y =  move_by.y();
            if (key_pressed == Qt::Key_Left)   move_x = -move_by.x();
            if (key_pressed == Qt::Key_Right)  move_x =  move_by.x();
            if (Dr::FuzzyCompare(0.0, move_x) == false || Dr::FuzzyCompare(0.0, move_y) == false) {
                // Store current settings and prepare for ItemChange Event
                View_Mode before_mode = m_editor_relay->currentViewMode();
                bool      snap_center = Dr::GetPreference(Preferences::Editor_Snap_To_Center_Of_Selection_Box).toBool();
                Dr::SetPreference(Preferences::Editor_Snap_To_Center_Of_Selection_Box, false);
                if (m_editor_relay->getViewNode()) m_editor_relay->getViewNode()->setViewMode(View_Mode::Holding_Keys);

                // Move Item, ItemChange Event is activated for snapping to grid
                item->moveBy(move_x, move_y);

                // Restore settings after ItemChange Event is over
                if (m_editor_relay->getViewNode()) m_editor_relay->getViewNode()->setViewMode(before_mode);
                Dr::SetPreference(Preferences::Editor_Snap_To_Center_Of_Selection_Box, snap_center);
            }
        }
    }






    // ********** Process Duplicate and Delete key press on all selected items
    if (key_pressed == Qt::Key_W ||
        key_pressed == Qt::Key_A ||
        key_pressed == Qt::Key_S ||
        key_pressed == Qt::Key_D ||
        key_pressed == Qt::Key_Delete ||
        key_pressed == Qt::Key_Backspace    ) {

        // ***** Lock Scene Mutex
        bool update_widgets_when_done = false;

        ///QRectF source_rect =  totalSelectionSceneRect();
        QList<QGraphicsItem*> list_old_items = selectedItems();
        QList<QGraphicsItem*> list_new_items { };
        QList<DrSettings*>    list_old_entities { };

        // ***** Process Delete, Convert to DrSettings
        for (auto item : list_old_items) {
            NodeItem  *map_item = dynamic_cast<NodeItem*>(item);    if (map_item == nullptr) continue;
            DrSettings  *entity = map_item->getEntity();            if (entity == nullptr) continue;

             // Delete selected items
            switch (key_pressed) {
                case Qt::Key::Key_Delete:
                case Qt::Key::Key_Backspace:
                    blockSignals(true);
                    if (entity->getType() == DrType::World) {
                        getProject()->deleteEntity(entity->getKey());
                        list_old_items.removeOne(item);
                        removeItem( item );
                        delete item;
                        update_widgets_when_done = true;
                    }
                    blockSignals(false);
                    continue;
                default: ;
            }

            // Add to list of DrThings to be processed
            list_old_entities.push_back(entity);
        }

        // ***** Process Clone
        for (auto entity : list_old_entities) {
            switch (key_pressed) {
                case Qt::Key::Key_W:
                case Qt::Key::Key_A:
                case Qt::Key::Key_S:
                case Qt::Key::Key_D:

                    if (entity->getType() == DrType::World) {
                        DrWorld *from_world = static_cast<DrWorld*>(entity);

                        // Find a new name for World
                        std::vector<QString> world_names;
                        for (auto &world_pair : getProject()->getWorldMap()) {
                            world_names.push_back( QString::fromStdString(world_pair.second->getName()) );
                        }
                        QString new_name = Dr::FindCopyName(QString::fromStdString(from_world->getName()), world_names);

                        // Copy World
                        DrWorld *copy_world = getProject()->addWorldCopyFromWorld(from_world, new_name.toStdString());

                        // Set Node Position
                        if      (key_pressed == Qt::Key_W) copy_world->setNodePositionFromOtherWorld(from_world, Direction::Up);
                        else if (key_pressed == Qt::Key_A) copy_world->setNodePositionFromOtherWorld(from_world, Direction::Left);
                        else if (key_pressed == Qt::Key_S) copy_world->setNodePositionFromOtherWorld(from_world, Direction::Down);
                        else if (key_pressed == Qt::Key_D) copy_world->setNodePositionFromOtherWorld(from_world, Direction::Right);

                        // Make new Node item from copied World
                        DrComponent *world_connections = copy_world->getComponent(Comps::World_Connections);
                        if (world_connections != nullptr) {
                            list_new_items.append( this->addItemToSceneFromComponent(world_connections) );
                        }
                    }
                    break;
                default: ;
            }
        }

        // ***** If we added (copied) new items to scene, select those items
        if (list_new_items.count() > 0 || list_old_items.count() == 0) {
            bool signals_blocked = signalsBlocked();
            blockSignals(true);
            this->clearSelection();
            for (auto item : list_new_items) {
                item->setSelected(true);
            }
            update_widgets_when_done = true;
            blockSignals(signals_blocked);
            this->selectionChanged();
        }

        // ********** Update Editor Widgets
        if (update_widgets_when_done) {
            m_editor_relay->updateItemSelection(Editor_Widgets::View);
        }
    }





    // ********** #NOTE: Don't pass on, if we pass on arrow key presses, it moves view sliders
    ///QGraphicsScene::keyPressEvent(event);
}

















