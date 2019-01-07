//
//      Created by Stephens Nunnally on 1/3/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Graphics Scene Definitions
//
//

#include "01_project.h"
#include "02_world.h"
#include "03_scene.h"
#include "04_object.h"
#include "05_item.h"

#include "30_settings.h"
#include "31_component.h"
#include "32_property.h"

#include "editor_scene_scene.h"
#include "interface_relay.h"


// Create square at location
void SceneGraphicsScene::addSquare(qreal new_x, qreal new_y, QColor color)
{
    QGraphicsItem *item;
    item = new DrItem(color);
    item->setPos(new_x, new_y);
    addItem(item);
}



void SceneGraphicsScene::keyReleaseEvent(QKeyEvent *event) { QGraphicsScene::keyReleaseEvent(event); }

void SceneGraphicsScene::keyPressEvent(QKeyEvent *event)
{
    // Amount to move items when arrow keys are pressed
    qreal move_by = 5;

    if (this->selectedItems().count() < 1) {
        QGraphicsScene::keyPressEvent(event);
        return;
    }

    // Find total bounding box of all selected items
    QRectF source_rect = this->selectedItems().first()->sceneBoundingRect();



    for (auto item: this->selectedItems()) {
        source_rect = source_rect.united(item->sceneBoundingRect());
    }

    // Perform key press event on all selected items
    for (auto item: this->selectedItems()) {
        QGraphicsItem *new_item;
        QColor new_color = QColor::fromRgb(QRandomGenerator::global()->generate()).light(100);
        qreal new_x = item->scenePos().x();
        qreal new_y = item->scenePos().y();

        switch (event->key())
        {
        // Move selected items
        case Qt::Key::Key_Up:    item->moveBy( 0, -move_by);      break;
        case Qt::Key::Key_Down:  item->moveBy( 0,  move_by);      break;
        case Qt::Key::Key_Left:  item->moveBy(-move_by,  0);      break;
        case Qt::Key::Key_Right: item->moveBy( move_by,  0);      break;

        // Clone selected items
        case Qt::Key::Key_W:
        case Qt::Key::Key_A:
        case Qt::Key::Key_S:
        case Qt::Key::Key_D:
            if (event->key() == Qt::Key::Key_W) new_y = new_y - source_rect.height();
            if (event->key() == Qt::Key::Key_A) new_x = new_x - source_rect.width();
            if (event->key() == Qt::Key::Key_S) new_y = new_y + source_rect.height();
            if (event->key() == Qt::Key::Key_D) new_x = new_x + source_rect.width();

            new_item = new DrItem(new_color);
            new_item->setPos(new_x, new_y);
            new_item->setTransform(item->transform());
            addItem(new_item);
            new_item->setSelected(true);
            item->setSelected(false);
            break;

        // Delete selected items
        case Qt::Key::Key_Delete:
        case Qt::Key::Key_Backspace:
            if (item->type() == User_Types::Object)
                removeItem(item);
            break;
        }
    }
    QGraphicsScene::keyPressEvent(event);
}











