//
//      Created by Stephens Nunnally on 1/23/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//

#include "editor_scene_scene.h"
#include "editor_scene_scene_commands.h"

/*
 *  To Implement:
 *      - Moving item with key press
 *
 *      - Selection change
 *
 *      - Geometry changes (rotation, scaling)
 *
 *      - Delete item
 *
 *      - Add item
 */

//####################################################################################
//##        Move Command on the QUndoStack
//####################################################################################
MoveCommand::MoveCommand(SelectionGroup *group, const QPointF &old_pos, QUndoCommand *parent) : QUndoCommand(parent)
{
    m_group = group;
    m_new_pos = group->pos();
    m_old_pos = old_pos;
}

void MoveCommand::undo()
{
    m_group->setPos(m_old_pos);
    m_group->getParentScene()->updateView();
    setText(QObject::tr("Move %1").arg(createCommandString(m_group->data(User_Roles::Name).toString(), m_new_pos)));
}

void MoveCommand::redo()
{
    m_group->setPos(m_new_pos);
    m_group->getParentScene()->updateView();
    setText(QObject::tr("Move %1").arg(createCommandString(m_group->data(User_Roles::Name).toString(), m_new_pos)));
}

QString createCommandString(QString item_name, const QPointF &pos)
{
    return QObject::tr("%1 at (%2, %3)")
        .arg(item_name)
        .arg(pos.x()).arg(pos.y());
}


//####################################################################################
//##        Adds item to selection group
//####################################################################################
SelectionAddItemCommand::SelectionAddItemCommand(SelectionGroup *group, QGraphicsItem* new_item, QUndoCommand *parent) : QUndoCommand(parent)
{
    m_group = group;
    m_new_item = new_item;
}

void SelectionAddItemCommand::undo()
{
    m_group->removeFromGroup(m_new_item);

    if (m_group->childItems().count() < 1) m_group->getParentScene()->emptySelectionGroup();

    m_group->getParentScene()->updateView();
    setText("Added Item to Selection: " + m_new_item->data(User_Roles::Name).toString() );
}

void SelectionAddItemCommand::redo()
{
    m_group->getParentScene()->addItemToSelectionGroup(m_new_item);
    m_group->getParentScene()->updateView();
    setText("Added Item to Selection: " + m_new_item->data(User_Roles::Name).toString() );
}


//####################################################################################
//##        Empties Selection Group
//####################################################################################
SelectionEmptyCommand::SelectionEmptyCommand(SelectionGroup *group, QList<QGraphicsItem*> old_list, QUndoCommand *parent) : QUndoCommand(parent)
{
    m_group = group;
    m_old_list = old_list;
}

void SelectionEmptyCommand::undo()
{
    for (auto item : m_old_list) m_group->getParentScene()->addItemToSelectionGroup(item);
    m_group->getParentScene()->updateView();
    setText("Cleared Selection Group");
}

void SelectionEmptyCommand::redo()
{
    m_group->getParentScene()->emptySelectionGroup();
    m_group->getParentScene()->updateView();
    setText("Cleared Selection Group");
}


//####################################################################################
//##        Deselects old items, Selects one new item
//####################################################################################
SelectionNewGroupCommand::SelectionNewGroupCommand(SelectionGroup *group, QList<QGraphicsItem*> old_list,
                                                   QGraphicsItem *new_item, QUndoCommand *parent) :
    QUndoCommand(parent)
{
    m_group = group;
    m_old_list = old_list;
    m_new_item = new_item;
}

void SelectionNewGroupCommand::undo()
{
    m_group->getParentScene()->emptySelectionGroup();
    for (auto item : m_old_list) m_group->getParentScene()->addItemToSelectionGroup(item);
    m_group->getParentScene()->updateView();
    setText("New Item Selected: " + m_new_item->data(User_Roles::Name).toString() );
}

void SelectionNewGroupCommand::redo()
{
    m_group->getParentScene()->emptySelectionGroup();
    m_group->getParentScene()->addItemToSelectionGroup(m_new_item);
    m_group->getParentScene()->updateView();
    setText("New Item Selected: " + m_new_item->data(User_Roles::Name).toString() );
}










