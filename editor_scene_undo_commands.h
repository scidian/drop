//
//      Created by Stephens Nunnally on 1/23/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Commands used with local m_undo QUndoStack
//
//
#ifndef EDITOR_SCENE_UNDO_H
#define EDITOR_SCENE_UNDO_H

#include "editor_scene.h"



//####################################################################################
//##    Change Stage in SceneView
//############################
class ChangeStageCommand : public QUndoCommand
{
public:
    ChangeStageCommand(DrProject *project, DrScene *scene, long old_stage, long new_stage, QUndoCommand *parent = nullptr);

    void        undo() override;
    void        redo() override;

    QString     changeStage(long old_stage, long new_stage, bool is_undo);

private:
    DrProject  *m_project;
    DrScene    *m_scene;
    long        m_old_stage;
    long        m_new_stage;
};



//####################################################################################
//##    Selection group moved position
//############################
class MoveCommand : public QUndoCommand
{
public:
    MoveCommand(DrScene *scene, const QPointF &old_pos, QUndoCommand *parent = nullptr);

    void        undo() override;
    void        redo() override;

private:
    DrScene    *m_scene;
    QPointF     m_old_pos;
    QPointF     m_new_pos;
};


//####################################################################################
//##    Selected new group command
//############################
class SelectionNewGroupCommand : public QUndoCommand
{
public:
    SelectionNewGroupCommand(DrScene *scene,
                             QList<DrObject*> old_list,
                             QList<DrObject*> new_list,
                             QUndoCommand *parent = nullptr);
    void        undo() override;
    void        redo() override;

private:
    DrScene             *m_scene;
    QList<DrObject*>     m_old_list;
    QList<DrObject*>     m_new_list;
};



#endif // EDITOR_SCENE_UNDO_H














