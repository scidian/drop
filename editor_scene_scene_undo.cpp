//
//      Created by Stephens Nunnally on 1/23/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//

#include "editor_scene_scene.h"
#include "editor_scene_scene_undo.h"

/*
 *  To Implement:
 *      - Selection change: Rubber band
 *
 *      - Moving Item: Key press
 *
 *      - Geometry Changes: Rotation, Scaling
 *
 *      - Delete item
 *
 *      - Add item
 */

//####################################################################################
//##        NOTE: Push function executes redo() function of command when called
//##
//##        SLOT, Needs to be connected to
//##            - Mouse release on View
//##            - Arrow key press on scene
//####################################################################################
void SceneGraphicsScene::undoAction() {
    m_undo->undo();
}
void SceneGraphicsScene::redoAction() {
    m_undo->redo();
}

void SceneGraphicsScene::selectionGroupMoved(SelectionGroup *moved_group, const QPointF &old_position)
{   m_undo->push(new MoveCommand(moved_group, old_position));   }

void SceneGraphicsScene::selectionGroupAddItem(SelectionGroup *moved_group, QGraphicsItem *new_item)
{   m_undo->push(new SelectionAddItemCommand(moved_group, new_item));   }

void SceneGraphicsScene::selectionGroupEmpty(SelectionGroup *moved_group, QList<QGraphicsItem*> old_list)
{   m_undo->push(new SelectionEmptyCommand(moved_group, old_list));     }

void SceneGraphicsScene::selectionGroupNewGroup(SelectionGroup *moved_group, QList<QGraphicsItem*> old_list, QList<QGraphicsItem*> new_list)
{   m_undo->push(new SelectionNewGroupCommand(moved_group, old_list, new_list));    }



//####################################################################################
//##        Move Command on the QUndoStack
//####################################################################################
MoveCommand::MoveCommand(SelectionGroup *group, const QPointF &old_pos, QUndoCommand *parent) : QUndoCommand(parent) {
    m_group = group;
    m_new_pos = group->pos();
    m_old_pos = old_pos;
}

void MoveCommand::undo() {
    m_group->setPos(m_old_pos);
    m_group->getParentScene()->updateView();
    setText(QObject::tr("Redo Move Item %1 to (%2, %3)").arg(m_group->data(User_Roles::Name).toString()).arg(m_new_pos.x()).arg(m_new_pos.y()));
}

void MoveCommand::redo() {
    m_group->setPos(m_new_pos);
    m_group->getParentScene()->updateView();
    setText(QObject::tr("Undo Move Item %1 to (%2, %3)").arg(m_group->data(User_Roles::Name).toString()).arg(m_new_pos.x()).arg(m_new_pos.y()));
}


//####################################################################################
//##        Adds item to selection group
//####################################################################################
SelectionAddItemCommand::SelectionAddItemCommand(SelectionGroup *group, QGraphicsItem* new_item, QUndoCommand *parent) : QUndoCommand(parent) {
    m_group = group;
    m_new_item = new_item;
}

void SelectionAddItemCommand::undo() {
    m_group->removeFromGroup(m_new_item);

    if (m_group->childItems().count() < 1) m_group->getParentScene()->emptySelectionGroup();

    m_group->getParentScene()->updateView();
    setText(QObject::tr("Redo Add Item %1 to Selection").arg(m_new_item->data(User_Roles::Name).toString()));
}

void SelectionAddItemCommand::redo() {
    m_group->getParentScene()->addItemToSelectionGroup(m_new_item);
    m_group->getParentScene()->updateView();
    setText(QObject::tr("Undo Add Item %1 to Selection").arg(m_new_item->data(User_Roles::Name).toString()));
}


//####################################################################################
//##        Empties Selection Group
//####################################################################################
SelectionEmptyCommand::SelectionEmptyCommand(SelectionGroup *group, QList<QGraphicsItem*> old_list, QUndoCommand *parent) : QUndoCommand(parent) {
    m_group = group;
    m_old_list = old_list;
}

void SelectionEmptyCommand::undo() {
    for (auto item : m_old_list) m_group->getParentScene()->addItemToSelectionGroup(item);
    m_group->getParentScene()->updateView();
    setText("Redo Select None");
}

void SelectionEmptyCommand::redo() {
    m_group->getParentScene()->emptySelectionGroup();
    m_group->getParentScene()->updateView();
    setText("Undo Select None");
}


//####################################################################################
//##        Deselects old items, Selects one new item
//####################################################################################
SelectionNewGroupCommand::SelectionNewGroupCommand(SelectionGroup *group, QList<QGraphicsItem*> old_list,
                                                   QList<QGraphicsItem*> new_list, QUndoCommand *parent) : QUndoCommand(parent) {
    m_group = group;
    m_old_list = old_list;
    m_new_list = new_list;
}

void SelectionNewGroupCommand::undo() {
    m_group->getParentScene()->emptySelectionGroup();
    for (auto item : m_old_list) m_group->getParentScene()->addItemToSelectionGroup(item);
    m_group->getParentScene()->updateView();
    if (m_new_list.count() > 1)
        setText("Redo Change Selection");
    else
        setText("Redo New Item Selected: " + m_new_list.first()->data(User_Roles::Name).toString() );
}

void SelectionNewGroupCommand::redo() {
    m_group->getParentScene()->emptySelectionGroup();
    for (auto item : m_new_list)
        m_group->getParentScene()->addItemToSelectionGroup(item);
    m_group->getParentScene()->updateView();
    if (m_new_list.count() > 1)
        setText("Undo Change Selection");
    else
        setText("Undo New Item Selected: " + m_new_list.first()->data(User_Roles::Name).toString() );
}










