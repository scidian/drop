//
//      Created by Stephens Nunnally on 1/23/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Commands used with local m_undo QUndoStack
//
//
#ifndef EDITOR_SCENE_SCENE_UNDO_H
#define EDITOR_SCENE_SCENE_UNDO_H

#include "editor_scene_scene.h"


//####################################################################################
//##    Selection group moved position
//############################
class MoveCommand : public QUndoCommand
{
public:
    enum { Id = 1234 };

    MoveCommand(SelectionGroup *group, const QPointF &old_pos, QUndoCommand *parent = nullptr);
    void undo() override;
    void redo() override;
    int  id() const override { return Id; }

private:
    SelectionGroup          *m_group;
    QPointF                  m_old_pos;
    QPointF                  m_new_pos;
};


//####################################################################################
//##    Selected new group command
//############################
class SelectionNewGroupCommand : public QUndoCommand
{
public:
    SelectionNewGroupCommand(SelectionGroup *group, QList<QGraphicsItem*> old_list, QList<QGraphicsItem*> new_list,
                             QGraphicsItem *old_first, QGraphicsItem *new_first, QUndoCommand *parent = nullptr);
    void undo() override;
    void redo() override;

private:
    SelectionGroup          *m_group;
    QList<QGraphicsItem*>    m_old_list;
    QList<QGraphicsItem*>    m_new_list;
    QGraphicsItem           *m_old_first_selected;
    QGraphicsItem           *m_new_first_selected;
};



#endif // EDITOR_SCENE_SCENE_UNDO_H














