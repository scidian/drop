//
//      Created by Stephens Nunnally on 1/23/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef EDITOR_SCENE_SCENE_COMMANDS_H
#define EDITOR_SCENE_SCENE_COMMANDS_H

#include "editor_scene_scene.h"


//####################################################################################
//##    Undo Commands
//##        Commands used with local m_undo QUndoStack
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
QString createCommandString(QString item_name, const QPointF &point);


//####################################################################################
//##    Add to Selection Group Command
//############################
class SelectionAddItemCommand : public QUndoCommand
{
public:
    SelectionAddItemCommand(SelectionGroup *group, QGraphicsItem *new_item, QUndoCommand *parent = nullptr);
    void undo() override;
    void redo() override;

private:
    SelectionGroup          *m_group;
    QGraphicsItem           *m_new_item;
};


//####################################################################################
//##    Add to Selection Group Command
//############################
class SelectionEmptyCommand : public QUndoCommand
{
public:
    SelectionEmptyCommand(SelectionGroup *group, QList<QGraphicsItem*> old_list, QUndoCommand *parent = nullptr);
    void undo() override;
    void redo() override;

private:
    SelectionGroup          *m_group;
    QList<QGraphicsItem*>    m_old_list;
};


//####################################################################################
//##    Add to Selection Group Command
//############################
class SelectionNewGroupCommand : public QUndoCommand
{
public:
    SelectionNewGroupCommand(SelectionGroup *group, QList<QGraphicsItem*> old_list, QGraphicsItem *new_item, QUndoCommand *parent = nullptr);
    void undo() override;
    void redo() override;

private:
    SelectionGroup          *m_group;
    QList<QGraphicsItem*>    m_old_list;
    QGraphicsItem           *m_new_item;
};



#endif // EDITOR_SCENE_SCENE_COMMANDS_H














