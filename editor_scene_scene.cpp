//
//      Created by Stephens Nunnally on 1/3/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Graphics Scene Definitions
//
//

#include "project.h"
#include "project_world.h"
#include "project_world_scene.h"
#include "project_world_scene_object.h"
#include "editor_scene_item.h"

#include "settings.h"
#include "settings_component.h"
#include "settings_component_property.h"

#include "editor_scene_scene.h"
#include "interface_relay.h"


// Adds a square DrItem (QGraphicsItem) to scene
void SceneGraphicsScene::addSquare(qreal new_x, qreal new_y, double new_width, double new_height, double z_order, QColor color)
{
    DrItem *item;
    item = new DrItem(color, new_width, new_height, z_order);

    item->setPositionByOrigin(item->getOrigin(), new_x, new_y);

    addItem(item);
}


// Connected from scene().changed, resizes scene when objects are added / subtracted
void SceneGraphicsScene::sceneChanged(QList<QRectF>)
{
    double min_size = 500;              // Minimum size fo scene rect (-500 to 500 for x and y)
    double buffer =   200;              // Buffer to add on to items at edge of scene to allow for better scrolling

    double left =  -min_size, right =  min_size;
    double top =   -min_size, bottom = min_size;

    if (items().count() > 0) {
        QRectF total_rect = items().first()->sceneBoundingRect();
        for (auto item: items())
            total_rect = total_rect.united(item->sceneBoundingRect());

        if (left > total_rect.x()) left = total_rect.x();
        if (top  > total_rect.y()) top =  total_rect.y();
        if (right <  total_rect.bottomRight().x()) right =  total_rect.bottomRight().x();
        if (bottom < total_rect.bottomRight().y()) bottom = total_rect.bottomRight().y();

        if (abs(left) > right) right = abs(left);
        if (abs(top) > bottom) bottom = abs(top);
        if (right > abs(left)) left = -right;
        if (bottom > abs(top)) top = -bottom;

        QRectF new_rect = QRectF(left - buffer, top - buffer, (right - left) + buffer * 2, (bottom - top) + buffer * 2);
        setSceneRect( new_rect );
    }
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
    QRectF source_rect = totalSelectedItemsSceneRect();

    // Perform key press event on all selected items
    for (auto item: this->selectedItems()) {
        DrItem *new_item;
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

            new_item = new DrItem(new_color, item->boundingRect().width(), item->boundingRect().height(), this->items().count() + 1);
            new_item->setPos(new_x, new_y);
            new_item->setTransform(item->transform());          // Includes rotation and scaling

            new_item->setData(User_Roles::Position, QPointF(new_item->pos().x(), new_item->pos().y()));
            new_item->setData(User_Roles::Scale, item->data(User_Roles::Scale).toPointF());
            new_item->setData(User_Roles::Rotation, item->data(User_Roles::Rotation).toDouble());

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











