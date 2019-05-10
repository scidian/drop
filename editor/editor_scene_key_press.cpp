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
#include "editor_item.h"
#include "editor_scene.h"
#include "globals.h"
#include "interface_editor_relay.h"
#include "helper.h"
#include "project/project.h"
#include "project/project_world.h"
#include "project/project_world_stage.h"
#include "project/project_world_stage_object.h"
#include "settings/settings.h"
#include "settings/settings_component.h"
#include "settings/settings_component_property.h"


//####################################################################################
//##        Key Events
//####################################################################################
void DrScene::keyReleaseEvent(QKeyEvent *event) { QGraphicsScene::keyReleaseEvent(event); }

void DrScene::keyPressEvent(QKeyEvent *event) {
    bool update_widgets_when_done = false;

    // Amount to move items when arrow keys are pressed
    double move_by = 5;

    // If no selected items, pass on key press event and exit
    if (getSelectionCount() < 1) {
        QGraphicsScene::keyPressEvent(event);
        return;
    }

    // Find total bounding box of all selected items
    QRectF source_rect = totalSelectionSceneRect();

    // Process movement key press
    for (auto item : getSelectionItems()) {
        switch (event->key()) {
            case Qt::Key::Key_Up:     item->moveBy( 0, -move_by);      break;
            case Qt::Key::Key_Down:   item->moveBy( 0,  move_by);      break;
            case Qt::Key::Key_Left:   item->moveBy(-move_by,  0);      break;
            case Qt::Key::Key_Right:  item->moveBy( move_by,  0);      break;
            case Qt::Key::Key_Comma:  item->setZValue(selectedItems().first()->zValue() - 1);    break;
            case Qt::Key::Key_Period: item->setZValue(selectedItems().first()->zValue() + 1);    break;
        }
    }
    updateSelectionBox();

    // Perform key press event on all items in selection group
    if (scene_mutex.tryLock(10) == false) return;


    // Go through any items selected and make copies of them if the copy keys (wasd) were pressed
    QList<QGraphicsItem*>  list_old_items = getSelectionItems();
    QList<QGraphicsItem*>  list_new_items { };

    for (auto item : list_old_items) {
        DrItem   *dritem   = dynamic_cast<DrItem*>(item);
        DrObject *drobject = dritem->getObject();
        DrStage  *drstage  = drobject->getParentStage();
        DrObject *new_object;

        double  new_x, new_y;
        int     new_z;

        switch (event->key()) {
            // Clone selected items
            case Qt::Key::Key_W:
            case Qt::Key::Key_A:
            case Qt::Key::Key_S:
            case Qt::Key::Key_D:
                new_x = drobject->getComponentPropertyValue(Components::Object_Transform, Properties::Object_Position).toPointF().x();
                new_y = drobject->getComponentPropertyValue(Components::Object_Transform, Properties::Object_Position).toPointF().y();
                new_z = drobject->getComponentPropertyValue(Components::Object_Layering,  Properties::Object_Z_Order).toInt();

                if (event->key() == Qt::Key::Key_W) new_y = new_y - source_rect.height();
                if (event->key() == Qt::Key::Key_A) new_x = new_x - source_rect.width();
                if (event->key() == Qt::Key::Key_S) new_y = new_y + source_rect.height();
                if (event->key() == Qt::Key::Key_D) new_x = new_x + source_rect.width();

                new_object = drstage->addObject(drobject->getObjectType(), drobject->getAssetKey(), new_x, new_y, new_z);
                drstage->copyObjectSettings(drobject, new_object);
                new_object->setComponentPropertyValue(Components::Object_Transform, Properties::Object_Position, QPointF(new_x, new_y));
                new_object->setComponentPropertyValue(Components::Object_Layering,  Properties::Object_Z_Order, new_z);

                list_new_items.append( this->addItemToSceneFromObject(new_object) );
                break;


            // Delete selected items
            case Qt::Key::Key_Delete:
            case Qt::Key::Key_Backspace:
                drstage->deleteObject(drobject);
                delete item;
                update_widgets_when_done = true;
                break;
        }
    }

    // If we added (copied) new items to scene, select those items
    if (list_new_items.count() > 0) {
        blockSignals(true);
        this->clearSelection();
        for (auto item : list_new_items)
            item->setSelected(true);
        update_widgets_when_done = true;
        blockSignals(false);
        selectionChanged();
    }

    scene_mutex.unlock();

    // Update Editor Widgets
    if (update_widgets_when_done) {

        //######################
        //######################
        //######################
        // !! Don't want to rebuild it, only update it
        m_editor_relay->buildProjectTree();
        //######################
        //######################
        //######################

        m_editor_relay->updateItemSelection(Editor_Widgets::Scene_View);
    }

    //QGraphicsScene::keyPressEvent(event);         // Don't pass on, if we pass on arrow key presses, it moves view sliders
}


















