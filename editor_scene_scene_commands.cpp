//
//      Created by Stephens Nunnally on 1/23/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//

#include "editor_scene_scene.h"

//undoStack->push(new MoveCommand(movedItem, oldPosition));


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
    setText(QObject::tr("Move %1").arg(createCommandString(m_group, m_new_pos)));
}

void MoveCommand::redo()
{
    m_group->setPos(m_new_pos);
    m_group->getParentScene()->updateView();
    setText(QObject::tr("Move %1").arg(createCommandString(m_group, m_new_pos)));
}



//####################################################################################
//##        Adds command data onto QUndoView list
//####################################################################################
QString createCommandString(SelectionGroup *group, const QPointF &pos)
{
    Q_UNUSED(group)
    return QObject::tr("%1 at (%2, %3)")
        .arg("Group")
        .arg(pos.x()).arg(pos.y());
}
