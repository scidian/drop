//
//      Created by Stephens Nunnally on 1/15/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
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


//####################################################################################
//##        Returns a scene rect containing all the selected items
//####################################################################################
QRectF SceneGraphicsScene::totalSelectedItemsSceneRect()
{
    // If no items selected, return empty rect
    QRectF total_rect;
    if (selectedItems().count() < 1) return total_rect;

    return m_selection_group->sceneBoundingRect();
}


//####################################################################################
//##        Selection Group Handling
//####################################################################################
void SceneGraphicsScene::addToSelectionGroup(QGraphicsItem *item, QPoint position)
{
    // Check if we were passed the selection group itself
    if (item->data(User_Roles::Is_Selection_Group).toBool()) {
        // If so, see if point is over an item already in the group, and if so, remove it
        QGraphicsItem *child_item = getItemAtPosition(position);
        if (child_item != nullptr)
            m_selection_group->removeFromGroup(child_item);

    } else {
        // If not passed group, see if item is in group
        bool has_it = false;
        for (auto child : m_selection_group->childItems())
            if (child == item) has_it = true;

        // If item is in group, remove it
        if (has_it) {
            m_selection_group->removeFromGroup(item);

        // Otherwise add the item, if there were no items, make sure selection group is selected in scene
        } else {
            m_selection_group->addToGroup(item);
            m_selection_group->setEnabled(true);
            if (m_selection_group->isSelected() == false)
                m_selection_group->setSelected(true);
        }
    }

    if (m_selection_group->childItems().count() < 1)
        emptySelectionGroup();

    // !!!!! TEMP
    m_relay->setLabelText(Label_Names::Label_Object_1, "Group Pos  X: " + QString::number(m_selection_group->boundingRect().x()) +
                                                                ", Y: " + QString::number(m_selection_group->boundingRect().y()) );
    m_relay->setLabelText(Label_Names::Label_Object_2, "Group Size X: " + QString::number(m_selection_group->boundingRect().width()) +
                                                                ", Y: " + QString::number(m_selection_group->boundingRect().height()) );
    m_relay->setLabelText(Label_Names::Label_Object_3, "Group Items: " + QString::number(m_selection_group->childItems().count()) );
    // !!!!! END
}

void SceneGraphicsScene::emptySelectionGroup()
{
    // Deselect all items in scene
    for (auto item: selectedItems()) item->setSelected(false);

    // Remove all items from selection group
    for (auto child : m_selection_group->childItems())
        m_selection_group->removeFromGroup(child);

    // Reset transform and reset user data
    m_selection_group->setTransform(QTransform());
    m_selection_group->setData(User_Roles::Scale, QPointF(1, 1));
    m_selection_group->setData(User_Roles::Rotation, 0);
    m_selection_group->setEnabled(false);

    // !!!!! TEMP
    if (m_relay)
        m_relay->setLabelText(Label_Names::Label_Object_3, "Group Items: " + QString::number(m_selection_group->childItems().count()) );
    // !!!!! END
}

QGraphicsItem* SceneGraphicsScene::getItemAtPosition(QPoint position)
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

QList<QGraphicsItem*> SceneGraphicsScene::getSelectionGroupItems()  { return m_selection_group->childItems(); }
void SceneGraphicsScene::selectSelectionGroup() { m_selection_group->setSelected(true);   }



//####################################################################################
//##        SelectionGroup Handling
//####################################################################################
SelectionGroup::~SelectionGroup() {}        // Necessary external definition

void SelectionGroup::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(painter);
    Q_UNUSED(option);
    Q_UNUSED(widget);
    ///QGraphicsItemGroup::paint(painter, option, widget);       // Paints black selection box
}













