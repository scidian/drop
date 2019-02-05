//
//      Created by Stephens Nunnally on 1/15/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Handles item selection in the GraphicsScene
//
//


#include "project.h"
#include "project_world.h"
#include "project_world_stage.h"
#include "project_world_stage_object.h"
#include "editor_item.h"

#include "settings.h"
#include "settings_component.h"
#include "settings_component_property.h"

#include "editor_scene.h"
#include "interface_relay.h"




//####################################################################################
//##        SLOT: selectionChanged
//####################################################################################
//  QList<QGraphicsItem*>   m_selection_items;          // List of selected items
//  double                  m_selection_angle;          // Angle current selection has been rotated to
//  QPointF                 m_selection_scale;          // Scaling applied to current selection
//  QPointF                 m_selection_translate;      // Translation applied to current selection
//  QRectF                  m_selection_box;            // Starting outline of selected items
//
void DrScene::selectionChanged()
{
    m_selection_items = selectedItems();

    m_selection_angle = 0;
    m_selection_scale = QPointF(1, 1);

    m_selection_box = totalSelectionSceneRect();
}



//####################################################################################
//##        Returns a scene rect containing all the selected items
//####################################################################################
QRectF DrScene::totalSelectionSceneRect()
{
    // If no items selected, return empty rect
    QRectF total_rect;
    if (selectedItems().count() < 1) return total_rect;

    // Start with rect of first item, add on each additional items rect
    total_rect = selectedItems().first()->sceneBoundingRect();
    for (auto item: selectedItems())
        total_rect = total_rect.united(item->sceneBoundingRect());

    return total_rect;
}

//####################################################################################
//##        Returns a transform representing the total changes that have been
//##        applied since selection group creation
//####################################################################################
QTransform DrScene::getSelectionTransform()
{
    QPointF center = m_selection_box.center();
    QTransform t =   QTransform()
            .translate(center.x(), center.y())
            .rotate(m_selection_angle)
            .scale(m_selection_scale.x(), m_selection_scale.y())
            .translate(-center.x(), -center.y());
    return t;
}

void DrScene::updateSelectionBox()
{
    // Recreate selection bounding box based on new item locations
    QGraphicsItemGroup *group = new QGraphicsItemGroup();
    this->addItem(group);

    double  angle = getSelectionAngle();
    QPointF scale = getSelectionScale();

    QPointF center = group->boundingRect().center();
    QTransform t = QTransform().translate(center.x(), center.y()).rotate(angle).translate(-center.x(), -center.y());
    group->setTransform(t);
    for (auto item : this->getSelectionItems()) group->addToGroup(item);

    QPointF top_left =   group->sceneTransform().map( group->boundingRect().topLeft() );
    QPointF bot_right =  group->sceneTransform().map( group->boundingRect().bottomRight() );
    QPointF map_center = group->sceneTransform().map( group->boundingRect().center() );

    t = QTransform()
            .translate(map_center.x(), map_center.y())
            .scale(1 / scale.x(), 1 / scale.y())
            .rotate(-angle)
            .translate(-map_center.x(), -map_center.y());

    top_left =  t.map(top_left);
    bot_right = t.map(bot_right);
    QRectF new_box = QRectF(top_left, bot_right);
    this->setSelectionBox( new_box );

    this->destroyItemGroup(group);
}


//####################################################################################
//##        Keeps track of which items are selected as DrObjects in the data model
//####################################################################################
// Returns list of objects represented from selected items
QList<DrObject*> DrScene::getSelectionGroupObjects() {
    return convertListItemsToObjects(m_selection_items);
}

// Returns list of objects represented from item list
QList<DrObject*> DrScene::convertListItemsToObjects(QList<QGraphicsItem*> graphics_items)
{
    QList<DrObject*> objects {};
    for (auto item : graphics_items) {
        DrItem *dritem = dynamic_cast<DrItem*>(item);
        objects.append(dritem->getObject());
    }
    return objects;
}


//####################################################################################
//##        Calls updateProperty which forces an update of the object inspector
//####################################################################################
void DrScene::updateChildrenPositionData()
{
    for (auto item : selectedItems()) {
        QPointF center = item->sceneTransform().map( item->boundingRect().center() );
        dynamic_cast<DrItem*>(item)->updateProperty(User_Roles::Position, center);
    }
}















