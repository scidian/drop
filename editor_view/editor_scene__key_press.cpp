//
//      Created by Stephens Nunnally on 2/28/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//

#include <QKeyEvent>
#include <QUndoView>

#include "debug.h"
#include "editor_view/editor_item.h"
#include "editor_view/editor_scene.h"
#include "editor_view/editor_view.h"
#include "globals.h"
#include "interface_editor_relay.h"
#include "helper.h"
#include "project/project.h"
#include "project/project_world.h"
#include "project/project_world_stage.h"
#include "project/project_world_stage_thing.h"
#include "settings/settings.h"
#include "settings/settings_component.h"
#include "settings/settings_component_property.h"


//####################################################################################
//##    Key Events
//####################################################################################
void DrScene::keyReleaseEvent(QKeyEvent *event) {
    // Reset key timer
    Qt::Key key_released = static_cast<Qt::Key>(event->key());
    m_key_down[key_released] = false;

    QGraphicsScene::keyReleaseEvent(event);
}

void DrScene::keyPressEvent(QKeyEvent *event) {
    bool update_widgets_when_done = false;

    // Start tracking time key has been pressed
    Qt::Key key_pressed = static_cast<Qt::Key>(event->key());
    if (m_key_down[key_pressed] == false) {
        m_key_timers[key_pressed] = QTime();
        m_key_timers[key_pressed].restart();
        m_key_down[key_pressed] = true;
    }

    // Amount to move items when arrow keys are pressed
    QPointF move_by = m_current_stage->getComponentPropertyValue(Components::Stage_Grid, Properties::Stage_Grid_Size).toPointF();
    if (Dr::GetPreference(Preferences::World_Editor_Snap_To_Grid).toBool() == false) {
        if        (m_key_timers[key_pressed].elapsed() <  500) {    move_by.setX(   1.0 );  move_by.setY(   1.0 );
        } else if (m_key_timers[key_pressed].elapsed() < 1000) {    move_by.setX(   5.0 );  move_by.setY(   5.0 );
        } else if (m_key_timers[key_pressed].elapsed() < 1500) {    move_by.setX(  25.0 );  move_by.setY(  25.0 );
        } else {                                                    move_by.setX( 100.0 );  move_by.setY( 100.0 ); }
    }

    // If no selected items, pass on key press event and exit
    if (getSelectionCount() < 1) {
        QGraphicsScene::keyPressEvent(event);
        return;
    }

    // Find total bounding box of all selected items
    QRectF source_rect = totalSelectionSceneRect();

    // ***** Process movement key press
    for (auto item : getSelectionItems()) {

        // Arrow Keys
        double move_x = 0.0;
        double move_y = 0.0;
        if (key_pressed == Qt::Key_Up)     move_y = -move_by.x();
        if (key_pressed == Qt::Key_Down)   move_y =  move_by.x();
        if (key_pressed == Qt::Key_Left)   move_x = -move_by.y();
        if (key_pressed == Qt::Key_Right)  move_x =  move_by.y();
        if (Dr::FuzzyCompare(0.0, move_x) == false || Dr::FuzzyCompare(0.0, move_y) == false) {
            item->moveBy( move_x, move_y);
        }

        // Layering Keys
        if (key_pressed == Qt::Key_Comma)  item->setZValue(selectedItems().first()->zValue() - 1);
        if (key_pressed == Qt::Key_Period) item->setZValue(selectedItems().first()->zValue() + 1);

    }
    updateSelectionBox();

    // Perform key press event on all items in selection group
    if (scene_mutex.tryLock(10) == false) return;


    // ***** Go through any items selected and make copies of them if the copy keys (wasd) were pressed
    QList<QGraphicsItem*>  list_old_items = getSelectionItems();
    QList<QGraphicsItem*>  list_new_items { };

    for (auto item : list_old_items) {
        DrItem   *dritem   = dynamic_cast<DrItem*>(item);
        DrThing  *drthing =  dritem->getThing();
        DrStage  *drstage  = drthing->getParentStage();
        DrThing  *new_object;

        double  new_x, new_y, new_z;

        switch (key_pressed) {
            // ***** Clone selected items
            case Qt::Key::Key_W:
            case Qt::Key::Key_A:
            case Qt::Key::Key_S:
            case Qt::Key::Key_D:
                new_x = drthing->getComponentPropertyValue(Components::Thing_Transform, Properties::Thing_Position).toPointF().x();
                new_y = drthing->getComponentPropertyValue(Components::Thing_Transform, Properties::Thing_Position).toPointF().y();
                new_z = drthing->getComponentPropertyValue(Components::Thing_Layering,  Properties::Thing_Z_Order).toDouble();

                if (key_pressed == Qt::Key::Key_W) new_y = new_y - source_rect.height();
                if (key_pressed == Qt::Key::Key_A) new_x = new_x - source_rect.width();
                if (key_pressed == Qt::Key::Key_S) new_y = new_y + source_rect.height();
                if (key_pressed == Qt::Key::Key_D) new_x = new_x + source_rect.width();

                new_object = drstage->addThing(drthing->getThingType(), drthing->getAssetKey(), new_x, new_y, new_z);
                drstage->copyThingSettings(drthing, new_object);
                new_object->setComponentPropertyValue(Components::Thing_Transform, Properties::Thing_Position, QPointF(new_x, new_y));
                new_object->setComponentPropertyValue(Components::Thing_Layering,  Properties::Thing_Z_Order, new_z);

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

    scene_mutex.unlock();

    // ***** Update Editor Widgets
    if (update_widgets_when_done) {
        m_editor_relay->buildProjectTree();
        m_editor_relay->updateItemSelection(Editor_Widgets::Stage_View);
    }

    //QGraphicsScene::keyPressEvent(event);         // !!!!! #NOTE: Don't pass on, if we pass on arrow key presses, it moves view sliders
}


















