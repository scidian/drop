//
//      Created by Stephens Nunnally on 4/30/2020, (c) 2020 Scidian Studios, All Rights Reserved
//
//  File:
//
//
//
#include <QWidget>

#include "editor/helper_library.h"
#include "editor/interface_editor_relay.h"
#include "editor/view_node/node_item.h"
#include "editor/view_node/node_scene.h"
#include "editor/view_node/node_view.h"
#include "project/dr_project.h"
#include "project/entities/dr_world.h"
#include "project/settings/settings.h"


//####################################################################################
//##    Constructor / Destructor
//####################################################################################
NodeScene::NodeScene(QWidget *parent, DrProject *project, IEditorRelay *editor_relay, long node_map_entity)
    : QGraphicsScene(parent), m_project(project), m_editor_relay(editor_relay), m_node_map_entity(node_map_entity) {

    connect(this, SIGNAL(changed(QList<QRectF>)),   this, SLOT(sceneChanged(QList<QRectF>)));
    connect(this, SIGNAL(selectionChanged()),       this, SLOT(selectionChanged()));

}

NodeScene::~NodeScene() { }

void NodeScene::clearSceneOverride() {
    this->clearSelection();
    emit aboutToClear();                                // NodeView::sceneIsAboutToClear() is connected to this signal, prevents dangling pointers
    this->setNeedRebuild(true);
    this->clear();
}


//####################################################################################
//##    SLOT's: sceneRectChanged, sceneChanged
//####################################################################################
// Connected from SIGNAL: QGraphicsScene::changed
void NodeScene::sceneChanged(QList<QRectF>) {
    QRectF my_rect = sceneRect();
    QRectF items_rect = itemsBoundingRect();
    double buffer = 5000;

    double left_diff =   (items_rect.left())   - (my_rect.left() + buffer);
    double top_diff =    (items_rect.top())    - (my_rect.top() + buffer);
    double right_diff =  (items_rect.right())  - (my_rect.right() - buffer);
    double bottom_diff = (items_rect.bottom()) - (my_rect.bottom() - buffer);

    if (left_diff   > 0)   left_diff = 0;
    if (top_diff    > 0)    top_diff = 0;
    if (right_diff  < 0)  right_diff = 0;
    if (bottom_diff < 0) bottom_diff = 0;

    my_rect.adjust(left_diff, top_diff, right_diff, bottom_diff);
    if (my_rect != sceneRect()) setSceneRect(my_rect);
}



//####################################################################################
//##    setPositionByOrigin - Sets item to new_x, new_y position in scene, offset by_origin point
//####################################################################################
void NodeScene::setPositionByOrigin(QGraphicsItem *item, Position_Flags by_origin, double new_x, double new_y) {
    item->setPos(new_x, new_y);

    QRectF      item_rect = item->boundingRect();
    QPointF     item_pos;
    switch (by_origin) {
        case Position_Flags::Top_Left:      item_pos = item_rect.topLeft();         break;
        case Position_Flags::Top_Right:     item_pos = item_rect.topRight();        break;
        case Position_Flags::Center:        item_pos = item_rect.center();          break;
        case Position_Flags::Bottom_Left:   item_pos = item_rect.bottomLeft();      break;
        case Position_Flags::Bottom_Right:  item_pos = item_rect.bottomRight();     break;
        case Position_Flags::Top:           item_pos = QPointF( QLineF(item_rect.topLeft(),    item_rect.topRight()).pointAt(.5) );     break;
        case Position_Flags::Bottom:        item_pos = QPointF( QLineF(item_rect.bottomLeft(), item_rect.bottomRight()).pointAt(.5) );  break;
        case Position_Flags::Left:          item_pos = QPointF( QLineF(item_rect.topLeft(),    item_rect.bottomLeft()).pointAt(.5) );   break;
        case Position_Flags::Right:         item_pos = QPointF( QLineF(item_rect.topRight(),   item_rect.bottomRight()).pointAt(.5) );  break;

        // We don't want to process any other options, use top left as position
        default:                            item_pos = item_rect.topLeft();
    }
    item_pos = item->sceneTransform().map(item_pos);

    setPositionByOrigin(item, item_pos, new_x, new_y);
}

void NodeScene::setPositionByOrigin(QGraphicsItem *item, QPointF origin_point, double new_x, double new_y) {
    QPointF new_pos;
    double  x_diff = origin_point.x() - new_x;
    double  y_diff = origin_point.y() - new_y;

    new_pos.setX(new_x - x_diff);
    new_pos.setY(new_y - y_diff);
    item->setPos(new_pos);
}


//####################################################################################
//##    Return scene()->items() as list of NodeItem pointers
//####################################################################################
QList<NodeItem*> NodeScene::nodeItems() {
    QList<NodeItem*> world_items;
    world_items.clear();

    for (auto item : items()) {
        NodeItem *world_item = dynamic_cast<NodeItem*>(item);
        if (world_item != nullptr) world_items.push_back(world_item);
    }
    return world_items;
}

//####################################################################################
//##    Returns NodeItem from list with the designated key
//####################################################################################
NodeItem* NodeScene::nodeItemWithKey(QList<NodeItem*> &world_items, long entity_key) {
    if (entity_key == c_no_key) return nullptr;
    for (auto world_item : world_items) {
        if (world_item->getEntity()->getKey() == entity_key) return world_item;
    }
    return nullptr;
}














