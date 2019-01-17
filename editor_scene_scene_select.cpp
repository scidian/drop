//
//      Created by Stephens Nunnally on 1/15/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Handles item selection in the scene
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
    if (selectedItems().count() < 1) return QRectF();

    // Return scene rect of m_selection_group which holds all the scenes selected items
    return m_selection_group->sceneBoundingRect();
}


//####################################################################################
//##        Selection Group Handling
//####################################################################################
void SceneGraphicsScene::addItemToSelectionGroup(QGraphicsItem *item)
{
    int start_count = m_selection_group->childItems().count();
    double angle = 0;

    // Check if we were passed the selection group itself
    if (item->data(User_Roles::Is_Selection_Group).toBool()) return;

    // If item is in group, remove it (i.e. item was clicked while control key is held down)
    if (m_selection_group->childItems().contains(item) == false) {
        if (start_count == 0) {
            angle = item->data(User_Roles::Rotation).toDouble();
            QPointF center = m_selection_group->boundingRect().center();
            QTransform t = QTransform().translate(center.x(), center.y()).rotate(angle).translate(-center.x(), -center.y());
            m_selection_group->setTransform(t);
            m_selection_group->setData(User_Roles::Rotation, angle);
            m_selection_group->setZValue(item->zValue());
        }

        if (item->zValue() > m_selection_group->zValue()) m_selection_group->setZValue(item->zValue());

        m_selection_group->addToGroup(item);
        m_selection_group->setEnabled(true);
        m_selection_group->setSelected(true);
    }

    // If we removed all the items from the selection group, zero it out
    if (m_selection_group->childItems().count() < 1) emptySelectionGroup();


    // !!!!! TEMP
    m_relay->setLabelText(Label_Names::Label_1, "First Item Angle: " + QString::number(angle));
    m_relay->setLabelText(Label_Names::Label_Object_1, "Group Pos  X: " + QString::number(m_selection_group->boundingRect().x()) +
                                                                ", Y: " + QString::number(m_selection_group->boundingRect().y()) );
    m_relay->setLabelText(Label_Names::Label_Object_2, "Group Size X: " + QString::number(m_selection_group->boundingRect().width()) +
                                                                ", Y: " + QString::number(m_selection_group->boundingRect().height()) );
    m_relay->setLabelText(Label_Names::Label_Object_3, "Group Items: " + QString::number(m_selection_group->childItems().count()) );
    m_relay->setLabelText(Label_Names::Label_Object_4, "Group Z: " + QString::number(m_selection_group->zValue() ) );
    // !!!!! END
}

void SceneGraphicsScene::emptySelectionGroup(bool delete_items_during_empty)
{
    // Remove all items from selection group
    for (auto child : m_selection_group->childItems()) {
        m_selection_group->removeFromGroup(child);
        if (delete_items_during_empty) removeItem(child);
    }
    resetSelectionGroup();

    // !!!!! TEMP
    if (m_relay)
        m_relay->setLabelText(Label_Names::Label_Object_3, "Group Items: " + QString::number(m_selection_group->childItems().count()) );
    // !!!!! END
}

// Reset transform and reset user data
void SceneGraphicsScene::resetSelectionGroup()
{
    for (auto item: selectedItems()) item->setSelected(false);
    m_selection_group->setTransform(QTransform());
    m_selection_group->setData(User_Roles::Scale, QPointF(1, 1));
    m_selection_group->setData(User_Roles::Rotation, 0);
    m_selection_group->setEnabled(false);
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

SelectionGroup* SceneGraphicsScene::getSelectionGroup()
{       return m_selection_group; }

QList<QGraphicsItem*> SceneGraphicsScene::getSelectionGroupItems()
{       return m_selection_group->childItems();     }

QGraphicsItem* SceneGraphicsScene::getSelectionGroupAsGraphicsItem()
{       return m_selection_group; }

void SceneGraphicsScene::selectSelectionGroup()
{       m_selection_group->setSelected(true);       }



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













