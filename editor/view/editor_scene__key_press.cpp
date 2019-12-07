//
//      Created by Stephens Nunnally on 2/28/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QDebug>
#include <QKeyEvent>
#include <QUndoView>

#include "editor/debug.h"
#include "editor/globals_editor.h"
#include "editor/interface_editor_relay.h"
#include "editor/trees/tree_inspector.h"
#include "editor/trees/tree_project.h"
#include "editor/view/editor_item.h"
#include "editor/view/editor_scene.h"
#include "editor/view/editor_view.h"
#include "library/dr_math.h"
#include "model/project/project.h"
#include "model/project/project_world.h"
#include "model/project/project_stage.h"
#include "model/project/project_thing.h"
#include "model/settings/settings.h"
#include "model/settings/settings_component.h"
#include "model/settings/settings_component_property.h"


//####################################################################################
//##    Key Release
//####################################################################################
void DrScene::keyReleaseEvent(QKeyEvent *event) {
    // Reset key timer
    Qt::Key key_released = static_cast<Qt::Key>(event->key());
    m_key_down[key_released] = false;

    QGraphicsScene::keyReleaseEvent(event);
}


//####################################################################################
//##    Key Press
//####################################################################################
void DrScene::keyPressEvent(QKeyEvent *event) {

    // ***** If no selected items, pass on key press event and exit
    if (getSelectionCount() < 1) {
        QGraphicsScene::keyPressEvent(event);
        return;
    }

    // ***** Start tracking time key has been pressed
    Qt::Key key_pressed = static_cast<Qt::Key>(event->key());
    if (m_key_down[key_pressed] == false) {
        m_key_timers[key_pressed] = QTime();
        m_key_timers[key_pressed].restart();
        m_key_down[key_pressed] = true;
    }


    // ********** Process Movement key press on all selected items
    if (key_pressed == Qt::Key_Up ||
        key_pressed == Qt::Key_Down ||
        key_pressed == Qt::Key_Left ||
        key_pressed == Qt::Key_Right) {
        if (m_current_stage == nullptr) return;
        QPointF grid_scale = m_current_stage->getComponentPropertyValue(Components::Stage_Grid, Properties::Stage_Grid_Scale).toPointF();
        QPointF grid_size =  m_current_stage->getComponentPropertyValue(Components::Stage_Grid, Properties::Stage_Grid_Size).toPointF();

        // Amount to move items when arrow keys are pressed
        QPointF move_by (grid_size.x() * grid_scale.x(), grid_size.y() * grid_scale.y());
        if (Dr::GetPreference(Preferences::World_Editor_Snap_To_Grid).toBool() == false) {
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

        for (auto item : getSelectionItems()) {
            double move_x = 0.0;
            double move_y = 0.0;
            if (key_pressed == Qt::Key_Up)     move_y = -move_by.y();
            if (key_pressed == Qt::Key_Down)   move_y =  move_by.y();
            if (key_pressed == Qt::Key_Left)   move_x = -move_by.x();
            if (key_pressed == Qt::Key_Right)  move_x =  move_by.x();
            if (Dr::FuzzyCompare(0.0, move_x) == false || Dr::FuzzyCompare(0.0, move_y) == false) {
                // Store current settings and prepare for ItemChange Event
                View_Mode before_mode = m_editor_relay->currentViewMode();
                bool      snap_center = Dr::GetPreference(Preferences::World_Editor_Snap_To_Center_Of_Selection_Box).toBool();
                Dr::SetPreference(Preferences::World_Editor_Snap_To_Center_Of_Selection_Box, false);
                if (m_editor_relay->getStageView()) m_editor_relay->getStageView()->setViewMode(View_Mode::Holding_Keys);

                // Move Item, ItemChange Event is activated for snapping to grid
                item->moveBy( move_x, move_y);

                // Restore settings after ItemChange Event is over
                if (m_editor_relay->getStageView()) m_editor_relay->getStageView()->setViewMode(before_mode);
                Dr::SetPreference(Preferences::World_Editor_Snap_To_Center_Of_Selection_Box, snap_center);
            }
        }
        updateSelectionBox();
    }


    // ********** Process Layering key press on all selected items
    if (key_pressed == Qt::Key_Comma ||
        key_pressed == Qt::Key_Period ||
        key_pressed == Qt::Key_Less ||
        key_pressed == Qt::Key_Greater) {

        // Get current Stage selected items as list of Things
        DrStage *stage = m_current_stage;
        if (stage == nullptr) return;
        std::list<DrThing*> selected_things = getSelectionItemsAsThings().toStdList();

        // ***** Send to Front
        if (key_pressed == Qt::Key_Greater) {
            std::vector<DrThing*> things = stage->thingsSortedByZOrder(Qt::AscendingOrder, false, selected_things);
            for (auto &thing : things) {
                while (stage->thingKeysSortedByZOrder(Qt::DescendingOrder).front() != thing->getKey()) {
                    thing->moveForward();
                }
            }

        // ***** Send Forward
        } else if (key_pressed == Qt::Key_Period) {
            std::vector<DrThing*> things = stage->thingsSortedByZOrder(Qt::DescendingOrder, false, selected_things);
            size_t i = 0;
            for (auto thing : things) {
                if (stage->thingKeysSortedByZOrder(Qt::DescendingOrder).at(i) != thing->getKey()) {
                    thing->moveForward();
                }
                i++;
            }

        // ***** Send to Back
        } else if (key_pressed == Qt::Key_Less) {
            std::vector<DrThing*> things = stage->thingsSortedByZOrder(Qt::DescendingOrder, false, selected_things);
            for (auto &thing : things) {
                while (stage->thingKeysSortedByZOrder(Qt::DescendingOrder).back() != thing->getKey()) {
                    thing->moveBackward();
                }
            }

        // ***** Send Backward
        } else if (key_pressed == Qt::Key_Comma) {
            std::vector<DrThing*> things = stage->thingsSortedByZOrder(Qt::AscendingOrder, false, selected_things);
            size_t i = stage->thingKeysSortedByZOrder(Qt::DescendingOrder).size() - 1;
            for (auto thing : things) {
                if (stage->thingKeysSortedByZOrder(Qt::DescendingOrder).at(i) != thing->getKey()) {
                    thing->moveBackward();
                }
                i--;
            }
        }

        this->updateItemZValues();
        m_editor_relay->buildProjectTree();
        m_editor_relay->getProjectTree()->ensureSelectionVisible();
        m_editor_relay->getInspector()->updateInspectorPropertyBoxesOfSelectedItem( { static_cast<int>(Properties::Thing_Z_Order),
                                                                                      static_cast<int>(Properties::Thing_Sub_Z_Order) } );
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
        if (scene_mutex.tryLock(10) == false) return;

        QRectF source_rect =  totalSelectionSceneRect();
        QList<QGraphicsItem*> list_old_items = getSelectionItems();
        QList<QGraphicsItem*> list_new_items { };

        for (auto item : list_old_items) {
            DrItem   *dritem   = dynamic_cast<DrItem*>(item);   if (dritem == nullptr)  continue;
            DrThing  *drthing =  dritem->getThing();            if (drthing == nullptr) continue;
            DrStage  *drstage  = drthing->getParentStage();
            DrThing  *new_object;

            double  new_x, new_y, new_z;
            int     sub_z;

            switch (key_pressed) {
                // ***** Clone selected items
                case Qt::Key::Key_W:
                case Qt::Key::Key_A:
                case Qt::Key::Key_S:
                case Qt::Key::Key_D:
                    new_x = drthing->getComponentPropertyValue(Components::Thing_Transform, Properties::Thing_Position).toPointF().x();
                    new_y = drthing->getComponentPropertyValue(Components::Thing_Transform, Properties::Thing_Position).toPointF().y();
                    new_z = drthing->getComponentPropertyValue(Components::Thing_Layering,  Properties::Thing_Z_Order).toDouble();
                    sub_z = drthing->getComponentPropertyValue(Components::Thing_Layering,  Properties::Thing_Sub_Z_Order).toInt();

                    if (key_pressed == Qt::Key::Key_W) new_y = new_y - source_rect.height();
                    if (key_pressed == Qt::Key::Key_A) new_x = new_x - source_rect.width();
                    if (key_pressed == Qt::Key::Key_S) new_y = new_y + source_rect.height();
                    if (key_pressed == Qt::Key::Key_D) new_x = new_x + source_rect.width();

                    new_object = drstage->addThing(drthing->getThingType(), drthing->getAssetKey(), new_x, new_y, new_z);
                    new_object->copyEntitySettings(drthing);
                    new_object->setComponentPropertyValue(Components::Thing_Transform, Properties::Thing_Position, QPointF(new_x, new_y));
                    new_object->setZOrderWithSub(new_z, Z_Insert::At_Position, sub_z + 1);
                    this->updateItemZValues();

                    list_new_items.append( this->addItemToSceneFromThing(new_object) );
                    break;

                // ***** Delete selected items
                case Qt::Key::Key_Delete:
                case Qt::Key::Key_Backspace:
                    blockSignals(true);
                    drstage->deleteThing(drthing);
                    list_old_items.removeOne(item);
                    removeItem( item );
                    delete item;
                    update_widgets_when_done = true;
                    blockSignals(false);
                    break;
                default: ;
            }
        }

        // ***** If we added (copied) new items to scene, select those items
        if (list_new_items.count() > 0 || list_old_items.count() == 0) {
            blockSignals(true);
            this->clearSelection();
            for (auto item : list_new_items) {
                item->setSelected(true);
            }
            update_widgets_when_done = true;
            blockSignals(false);
            selectionChanged();
        }

        // ***** Unlock Scene Mutex
        scene_mutex.unlock();

        // ********** Update Editor Widgets
        if (update_widgets_when_done) {
            m_editor_relay->buildProjectTree();
            m_editor_relay->updateItemSelection(Editor_Widgets::Stage_View);
        }
    }


    // ********** #NOTE: Don't pass on, if we pass on arrow key presses, it moves view sliders
    ///QGraphicsScene::keyPressEvent(event);
}


















