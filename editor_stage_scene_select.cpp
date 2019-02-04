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
#include "editor_stage_item.h"

#include "settings.h"
#include "settings_component.h"
#include "settings_component_property.h"

#include "editor_stage_scene.h"
#include "interface_relay.h"


//####################################################################################
//##        Returns a scene rect containing all the selected items
//####################################################################################
QRectF StageGraphicsScene::totalSelectedItemsSceneRect()
{
    // If no items selected, return empty rect
    if (selectedItems().count() < 1) return QRectF();

    // Return scene rect of m_selection_group which holds all the scenes selected items
    return m_selection_group->sceneBoundingRect();
}


//####################################################################################
//##        Selection Group Handling
//####################################################################################
void StageGraphicsScene::addItemToSelectionGroup(QGraphicsItem *item)
{
    int start_count = m_selection_group->childItems().count();

    // Check if we were passed the selection group itself
    if (m_selection_group != nullptr)
        if (item == dynamic_cast<QGraphicsItem*>(m_selection_group))
            return;

    // If item is not in group, add it in
    if (m_selection_group->childItems().contains(item) == false) {

        // If this is the first item, set all properties to first item, if second item, reset scale to 1, 1
        if ((start_count == 0 || start_count == 1) && m_first_selected != nullptr) {
            QGraphicsItem *first = nullptr;
            QPointF scale = QPointF(1, 1);

            if (start_count == 0) {
                scale = m_first_selected->getDrItem()->data(User_Roles::Scale).toPointF();
            } else {
                first = m_selection_group->childItems().first();
                m_selection_group->removeFromGroup(first);
            }

            double  angle = m_first_selected->getDrItem()->data(User_Roles::Rotation).toDouble();
            QPointF center = m_selection_group->boundingRect().center();
            QTransform t = QTransform().translate(center.x(), center.y()).rotate(angle).scale(scale.x(), scale.y()).translate(-center.x(), -center.y());
            m_selection_group->setTransform(t);
            m_selection_group->setData(User_Roles::Rotation, angle);
            m_selection_group->setData(User_Roles::Scale, scale);
            m_selection_group->setZValue(m_first_selected->getDrItem()->zValue());

            if (start_count == 1) m_selection_group->addToGroup(first);
        }

        if (item->zValue() > m_selection_group->zValue())
            m_selection_group->setZValue(item->zValue());

        m_selection_group->addToGroup(item);
        m_selection_group->setEnabled(true);
        m_selection_group->setSelected(true);
    }
}

// Empties selection group, delete_items_during_empty used for item copying with StageGraphicsScene keyPressEvent
void StageGraphicsScene::emptySelectionGroup(bool delete_items_during_empty)
{
    m_first_selected = nullptr;

    // Remove all items from selection group
    for (auto child : m_selection_group->childItems()) {
        m_selection_group->removeFromGroup(child);
        if (delete_items_during_empty) removeItem(child);
    }
    resetSelectionGroup();
}

// Reset transform and reset user data
void StageGraphicsScene::resetSelectionGroup()
{
    for (auto item: selectedItems()) item->setSelected(false);
    m_selection_group->setTransform(QTransform());
    m_selection_group->setData(User_Roles::Scale, QPointF(1, 1));
    m_selection_group->setData(User_Roles::Rotation, 0);
    m_selection_group->setEnabled(false);
}


QGraphicsItem* StageGraphicsScene::getItemAtPosition(QPointF position)
{
    QGraphicsItem *item = nullptr;
    bool found_one = false;
    double zorder = 0;

    // Go through items in selection group and get top most item contaning 'position'
    for (auto child : m_selection_group->childItems()) {
        if (child->contains(position)) {
            if ((found_one == false) || (child->zValue() > zorder)) {
                item = child;
                zorder = item->zValue();
                found_one = true;
            }
        }
    }
    return item;
}

void StageGraphicsScene::updateChildrenPositionData()
{
    QList<QGraphicsItem*>  my_items = this->getSelectionGroupItems();
    for (auto child : my_items) {
        QPointF center = child->sceneTransform().map( child->boundingRect().center() );
        dynamic_cast<DrItem*>(child)->updateProperty(User_Roles::Position, center);
    }
}

SelectionGroup* StageGraphicsScene::getSelectionGroup()
{       return m_selection_group; }

QList<QGraphicsItem*> StageGraphicsScene::getSelectionGroupItems()
{       return m_selection_group->childItems();     }

QGraphicsItem* StageGraphicsScene::getSelectionGroupAsGraphicsItem()
{       return m_selection_group; }

int StageGraphicsScene::getSelectionGroupCount()
{       return m_selection_group->childItems().count();     }

void StageGraphicsScene::selectSelectionGroup()
{       m_selection_group->setSelected(true);       }


// Returns list of objects represented from selected items
QList<DrObject*> StageGraphicsScene::getSelectionGroupObjects() {
    return convertListItemsToObjects(m_selection_group->childItems());
}

// Returns list of objects represented from item list
QList<DrObject*> StageGraphicsScene::convertListItemsToObjects(QList<QGraphicsItem*> graphics_items)
{
    QList<DrObject*> objects {};
    for (auto item : graphics_items) {
        DrItem *dritem = dynamic_cast<DrItem*>(item);
        objects.append(dritem->getObject());
    }
    return objects;
}


//####################################################################################
//##        SelectionGroup Handling
//####################################################################################
SelectionGroup::~SelectionGroup() {}        // Necessary external definition

// Calling this custom paint allows us to bypass built in custom black bounding box of QGraphicsItemGroup
void SelectionGroup::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(painter);
    Q_UNUSED(option);
    Q_UNUSED(widget);

    ///QGraphicsItemGroup::paint(painter, option, widget);       // Allows black selection bounding box to be painted
}


//####################################################################################
//##        Custom slimmed from Qt Source Code, "QGraphicsItemGroup->removeFromGroup"
void StageGraphicsScene::removeFromGroupNoUpdate(QGraphicsItem *item)
{
    if (!item) return;

    QTransform itemTransform = item->sceneTransform();
    QPointF oldPos = item->mapToScene(0, 0);
    item->setParentItem(nullptr);
    item->setPos(oldPos);
    if (!item->pos().isNull()) itemTransform *= QTransform::fromTranslate(-item->x(), -item->y());

    QPointF origin = item->transformOriginPoint();
    QMatrix4x4 m;
    QList<QGraphicsTransform*> transformList = item->transformations();
    for (int i = 0; i < transformList.size(); ++i)
        transformList.at(i)->applyTo(&m);
    itemTransform *= m.toTransform().inverted();
    itemTransform.translate(origin.x(), origin.y());
    itemTransform.rotate(-item->rotation());
    itemTransform.scale(1 / item->scale(), 1 / item->scale());
    itemTransform.translate(-origin.x(), -origin.y());
    item->setTransform(itemTransform);
}

//####################################################################################
//##        Custom slimmmed from Qt Source Code, "QGraphicsItemGroup->addToGroup"
void StageGraphicsScene::addToGroupNoUpdate(QGraphicsItem *item)
{
    QGraphicsItemGroup *group = m_selection_group;
    if (!item || item == group) return;

    bool ok;
    QTransform itemTransform = item->itemTransform(group, &ok);
    QTransform newItemTransform(itemTransform);
    item->setPos(group->mapFromItem(item, 0, 0));
    item->setParentItem(group);
    if (!item->pos().isNull()) newItemTransform *= QTransform::fromTranslate(-item->x(), -item->y());

    QPointF origin = item->transformOriginPoint();
    QMatrix4x4 m;
    QList<QGraphicsTransform*> transformList = item->transformations();
    for (int i = 0; i < transformList.size(); ++i)
        transformList.at(i)->applyTo(&m);
    newItemTransform *= m.toTransform().inverted();
    newItemTransform.translate(origin.x(), origin.y());
    newItemTransform.rotate(-item->rotation());
    newItemTransform.scale(1/item->scale(), 1/item->scale());
    newItemTransform.translate(-origin.x(), -origin.y());
    item->setTransform(newItemTransform);
}












