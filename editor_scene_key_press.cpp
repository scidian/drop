//
//      Created by Stephens Nunnally on 2/28/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//

#include <QKeyEvent>
#include <QRandomGenerator>
#include <QUndoView>

#include "debug.h"

#include "editor_item.h"
#include "editor_scene.h"

#include "interface_editor_relay.h"
#include "library.h"

#include "project.h"
#include "project_world.h"
#include "project_world_stage.h"
#include "project_world_stage_object.h"
#include "settings.h"
#include "settings_component.h"
#include "settings_component_property.h"


//####################################################################################
//##        Key Events
//####################################################################################
void DrScene::keyReleaseEvent(QKeyEvent *event) { QGraphicsScene::keyReleaseEvent(event); }

void DrScene::keyPressEvent(QKeyEvent *event)
{
    // Amount to move items when arrow keys are pressed
    qreal move_by = 5;

    if (getSelectionCount() < 1) {
        QGraphicsScene::keyPressEvent(event);
        return;
    }

    // Find total bounding box of all selected items
    QRectF source_rect = totalSelectionSceneRect();

    // Process movement key press
    for (auto item : getSelectionItems()) {
        switch (event->key())
        {
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

    QList<QGraphicsItem*>  list_new_items {};

    for (auto item : getSelectionItems()) {
    //!!    DrItem *new_item;
        QColor new_color;
        qreal new_x, new_y;

        switch (event->key())
        {
        // Clone selected items
        case Qt::Key::Key_W:
        case Qt::Key::Key_A:
        case Qt::Key::Key_S:
        case Qt::Key::Key_D:
            // Send item back to scene before we copy it
            //my_scene->getSelectionGroup()->removeFromGroup(item);

            new_color = QColor::fromRgb(QRandomGenerator::global()->generate()).light(100);
            new_x = item->scenePos().x();
            new_y = item->scenePos().y();

            if (event->key() == Qt::Key::Key_W) new_y = new_y - source_rect.height();
            if (event->key() == Qt::Key::Key_A) new_x = new_x - source_rect.width();
            if (event->key() == Qt::Key::Key_S) new_y = new_y + source_rect.height();
            if (event->key() == Qt::Key::Key_D) new_x = new_x + source_rect.width();

    //!!        new_item = new DrItem(new_color, item->boundingRect().width(), item->boundingRect().height(),
    //!!                              this->items().count() + 1, item->data(User_Roles::Name).toString());
    //!!         new_item->setPos(new_x, new_y);
    //!!        new_item->setTransform(item->transform());          // Includes rotation and scaling

    //!!        new_item->setData(User_Roles::Scale, item->data(User_Roles::Scale).toPointF());
    //!!        new_item->setData(User_Roles::Rotation, item->data(User_Roles::Rotation).toDouble());

    //!!        addItem(new_item);
    //!!        list_new_items.append(new_item);
            break;

        // Delete selected items
        case Qt::Key::Key_Delete:
        case Qt::Key::Key_Backspace:
            //emptySelectionGroup(true);
            break;
        }
    }

    // If we added (copied) new items to scene, select those items
//    if (list_new_items.count() > 0) {
//        emptySelectionGroup();
//        for (auto item : list_new_items) addItemToSelectionGroup(item);
//    }

    scene_mutex.unlock();

    //QGraphicsScene::keyPressEvent(event);         // Don't pass on, if we pass on arrow key presses, it moves view sliders
    emit updateViews();                             // Custom signal to tell Views we're attached to, to update themselves
}


















