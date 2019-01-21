//
//      Created by Stephens Nunnally on 1/3/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Graphics Scene Definitions
//
//

#include "library.h"

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


//####################################################################################
//##        Constructor & destructor
//####################################################################################
SceneGraphicsScene::SceneGraphicsScene(QWidget *parent, DrProject *project, InterfaceRelay *relay) :
                                       QGraphicsScene(parent = nullptr),
                                       m_project(project), m_relay(relay)
{
    connect(this, SIGNAL(changed(QList<QRectF>)), this, SLOT(sceneChanged(QList<QRectF>)));

    m_selection_group = new SelectionGroup;
    m_selection_group->setFlags(QGraphicsItem::ItemIsSelectable |
                                QGraphicsItem::ItemIsMovable |
                                QGraphicsItem::ItemSendsScenePositionChanges |
                                QGraphicsItem::ItemSendsGeometryChanges);
    addItem(m_selection_group);
    emptySelectionGroup();
    m_selection_group->setData(User_Roles::Name, "Captain");
}

SceneGraphicsScene::~SceneGraphicsScene()
{

}



//####################################################################################
//##        Item Handling
//####################################################################################

// Adds a square DrItem (QGraphicsItem) to scene
void SceneGraphicsScene::addSquare(qreal new_x, qreal new_y, double new_width, double new_height, double z_order, QString name, QColor color)
{
    DrItem *item;
    item = new DrItem(color, new_width, new_height, z_order, name);

    setPositionByOrigin(item, item->getOrigin(), new_x, new_y);

    addItem(item);
}


//####################################################################################
//##        SLOT: sceneChanged
//####################################################################################
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


//####################################################################################
//##        Key Events
//####################################################################################
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

    // Process movement key press
    switch (event->key())
    {
    // Move selected items
    case Qt::Key::Key_Up:    selectedItems().first()->moveBy( 0, -move_by);      break;
    case Qt::Key::Key_Down:  selectedItems().first()->moveBy( 0,  move_by);      break;
    case Qt::Key::Key_Left:  selectedItems().first()->moveBy(-move_by,  0);      break;
    case Qt::Key::Key_Right: selectedItems().first()->moveBy( move_by,  0);      break;
    }



    //
    // !!!!! TEMP
    //
    if (event->key() == Qt::Key::Key_M && m_selection_group->childItems().count() == 1) {
        if (scene_mutex.tryLock(10) == false) return;
        QList<QGraphicsItem*>  my_items = getSelectionGroupItems();
        for (auto item: my_items) removeFromGroupNoUpdate(item);
        QGraphicsItem *item = my_items.first();
        QString msg = "";

        // Load local transformations
        QList<QGraphicsTransform*> transformList = item->transformations();
        msg += "Transformations Count: " + QString::number(transformList.size()) + QString("\n");

        // Show local rotation
        msg += "Rotation: " + QString::number(item->rotation()) + QString("\n");

        // Show local scale
        double scale = item->scale();
        msg += "Scale: " + QString::number(scale) + QString("\n");

        // Load local sceneTransform
        QTransform t = item->sceneTransform();
        qreal m11 = t.m11(), m12 = t.m12(), m13 = t.m13();
        qreal m21 = t.m21(), m22 = t.m22(), m23 = t.m23();
        msg += "11: " + QString::number(m11) + ", 12: " + QString::number(m12) + ", 13: " + QString::number(m13) + QString("\n");
        msg += "21: " + QString::number(m21) + ", 22: " + QString::number(m22) + ", 23: " + QString::number(m23) + QString("\n");

        // Show data
        Dr::ShowMessageBox(msg);

        for (auto item: my_items) addToGroupNoUpdate(item);
        scene_mutex.unlock();
        return;
    }
    //
    // !!!!! END
    //


    // Perform key press event on all items in selection group
    if (scene_mutex.tryLock(10) == false) return;

    SceneGraphicsScene    *my_scene = dynamic_cast<SceneGraphicsScene *>(this);
    QList<QGraphicsItem*>  list_new_items;
    list_new_items.clear();

    for (auto item : my_scene->getSelectionGroupItems()) {
        DrItem *new_item;
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
            my_scene->getSelectionGroup()->removeFromGroup(item);

            new_color = QColor::fromRgb(QRandomGenerator::global()->generate()).light(100);
            new_x = item->scenePos().x();
            new_y = item->scenePos().y();

            if (event->key() == Qt::Key::Key_W) new_y = new_y - source_rect.height();
            if (event->key() == Qt::Key::Key_A) new_x = new_x - source_rect.width();
            if (event->key() == Qt::Key::Key_S) new_y = new_y + source_rect.height();
            if (event->key() == Qt::Key::Key_D) new_x = new_x + source_rect.width();

            new_item = new DrItem(new_color, item->boundingRect().width(), item->boundingRect().height(),
                                  this->items().count() + 1, item->data(User_Roles::Name).toString());
            new_item->setPos(new_x, new_y);
            new_item->setTransform(item->transform());          // Includes rotation and scaling

            new_item->setData(User_Roles::Scale, item->data(User_Roles::Scale).toPointF());
            new_item->setData(User_Roles::Rotation, item->data(User_Roles::Rotation).toDouble());

            addItem(new_item);
            list_new_items.append(new_item);
            break;

        // Delete selected items
        case Qt::Key::Key_Delete:
        case Qt::Key::Key_Backspace:
            emptySelectionGroup(true);
            break;
        }
    }

    // If we added (copied) new items to scene, select those items
    if (list_new_items.count() > 0) {
        emptySelectionGroup();
        for (auto item : list_new_items) addItemToSelectionGroup(item);
    }

    scene_mutex.unlock();

    //QGraphicsScene::keyPressEvent(event);         // Don't pass on, if we pass on arrow key presses, it moves view sliders
    emit updateViews();
}



//####################################################################################
//##        setPositionByOrigin - Sets item to new_x, new_y position in scene, offset by_origin point
//####################################################################################
void SceneGraphicsScene::setPositionByOrigin(QGraphicsItem *item, Position_Flags by_origin, double new_x, double new_y)
{
    item->setPos(new_x, new_y);

    QRectF      item_rect = item->boundingRect();
    QPointF     item_pos;

    switch (by_origin) {
    case Position_Flags::Top_Left:      item_pos = item_rect.topLeft();                  break;
    case Position_Flags::Top_Right:     item_pos = item_rect.topRight();                 break;
    case Position_Flags::Center:        item_pos = item_rect.center();                   break;
    case Position_Flags::Bottom_Left:   item_pos = item_rect.bottomLeft();               break;
    case Position_Flags::Bottom_Right:  item_pos = item_rect.bottomRight();              break;
    case Position_Flags::Top:           item_pos = QPointF( QLineF(item_rect.topLeft(),    item_rect.topRight()).pointAt(.5) );    break;
    case Position_Flags::Bottom:        item_pos = QPointF( QLineF(item_rect.bottomLeft(), item_rect.bottomRight()).pointAt(.5) ); break;
    case Position_Flags::Left:          item_pos = QPointF( QLineF(item_rect.topLeft(),    item_rect.bottomLeft()).pointAt(.5) );  break;
    case Position_Flags::Right:         item_pos = QPointF( QLineF(item_rect.topRight(),   item_rect.bottomRight()).pointAt(.5) ); break;
    default:
        item_pos = item_rect.topLeft();
    }
    item_pos = item->sceneTransform().map(item_pos);

    setPositionByOrigin(item, item_pos, new_x, new_y);
}

void SceneGraphicsScene::setPositionByOrigin(QGraphicsItem *item, QPointF origin_point, double new_x, double new_y)
{
    QPointF     new_pos;

    double x_diff = origin_point.x() - new_x;
    double y_diff = origin_point.y() - new_y;

    new_pos.setX( new_x - x_diff);
    new_pos.setY( new_y - y_diff);

    item->setPos(new_pos);
}













