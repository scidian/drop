//
//
//      Handles dealing with the Scene View
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

#include "form_main.h"


void FormMain::populateScene()
{
    scene = new SceneGraphicsScene(this, this);



    // Populate scene
    QColor color;
    QGraphicsItem *item;

    color = QColor::fromRgb(QRandomGenerator::global()->generate());
    item = new DrItem(color, 0, 0);
    item->setPos(QPointF(0, 0));
    scene->addItem(item);

    color = QColor::fromRgb(QRandomGenerator::global()->generate());
    item = new DrItem(color, 200, 0);
    item->setPos(QPointF(200, 0));
    scene->addItem(item);

    color = QColor::fromRgb(QRandomGenerator::global()->generate());
    item = new DrItem(color, 0, 200);
    item->setPos(QPointF(0, 200));
    scene->addItem(item);

    color = QColor::fromRgb(QRandomGenerator::global()->generate());
    item = new DrItem(color, 200, 200);
    item->setPos(QPointF(200, 200));
    scene->addItem(item);
}

void SceneGraphicsScene::keyPressEvent(QKeyEvent *event)
{
    QGraphicsScene::keyPressEvent(event);
}

void SceneGraphicsScene::keyReleaseEvent(QKeyEvent *event)
{
    QGraphicsScene::keyReleaseEvent(event);
}

void SceneGraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mouseMoveEvent(event);
}


void SceneGraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    // Delete item on under cursaor on right mouse button down
    if (event->button() & Qt::RightButton) {
        // check whether there is an item under the cursor
        QGraphicsItem *itemToRemove = nullptr;
        foreach (auto item, items(event->scenePos())) {
            if (item->type() == User_Types::Object) {
                itemToRemove = item;
                break;
            }
        }
        // remove the item from the graphicsScene
        if (itemToRemove) { removeItem(itemToRemove); }
    }
    QGraphicsScene::mousePressEvent(event);
    update();
}























