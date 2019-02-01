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
//##    Change Scene in View
//############################
class ChangeSceneCommand : public QUndoCommand
{
public:
    ChangeSceneCommand(QUndoStack *undo_stack, DrProject *project, SceneGraphicsScene *scene, long old_scene, long new_scene,
                       QUndoCommand *parent = nullptr);

    void        undo() override;
    void        redo() override;

    QString     changeScene(long old_scene, long new_scene, bool is_undo);

private:
    QUndoStack             *m_undo_stack;
    DrProject              *m_project;
    SceneGraphicsScene     *m_scene;
    long                    m_old_scene;
    long                    m_new_scene;
};



//####################################################################################
//##    Selection group moved position
//############################
class MoveCommand : public QUndoCommand
{
public:
    MoveCommand(SelectionGroup *group, const QPointF &old_pos, QUndoCommand *parent = nullptr);

    void        undo() override;
    void        redo() override;

private:
    SelectionGroup         *m_group;
    QPointF                 m_old_pos;
    QPointF                 m_new_pos;
};


//####################################################################################
//##    Selected new group command
//############################
class SelectionNewGroupCommand : public QUndoCommand
{
public:
    SelectionNewGroupCommand(SelectionGroup *group,
                             QList<QGraphicsItem*> old_list,
                             QList<QGraphicsItem*> new_list,
                             DrObject *old_first,
                             DrObject *new_first,
                             QUndoCommand *parent = nullptr);
    void        undo() override;
    void        redo() override;

private:
    SelectionGroup         *m_group;
    QList<QGraphicsItem*>   m_old_list;
    QList<QGraphicsItem*>   m_new_list;
    DrObject               *m_old_first_selected;
    DrObject               *m_new_first_selected;
};



#endif // EDITOR_SCENE_SCENE_UNDO_H














