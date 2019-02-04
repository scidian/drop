//
//      Created by Stephens Nunnally on 1/23/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Commands used with local m_undo QUndoStack
//
//
#ifndef EDITOR_STAGE_SCENE_UNDO_H
#define EDITOR_STAGE_SCENE_UNDO_H

#include "editor_stage_scene.h"



//####################################################################################
//##    Change Stage in SceneView
//############################
class ChangeStageCommand : public QUndoCommand
{
public:
    ChangeStageCommand(DrProject *project, StageGraphicsScene *scene, long old_stage, long new_stage, QUndoCommand *parent = nullptr);

    void        undo() override;
    void        redo() override;

    QString     changeStage(long old_stage, long new_stage, bool is_undo);

private:
    DrProject              *m_project;
    StageGraphicsScene     *m_scene;
    long                    m_old_stage;
    long                    m_new_stage;
};



//####################################################################################
//##    Selection group moved position
//############################
class MoveCommand : public QUndoCommand
{
public:
    MoveCommand(StageGraphicsScene *scene, const QPointF &old_pos, QUndoCommand *parent = nullptr);

    void        undo() override;
    void        redo() override;

private:
    StageGraphicsScene     *m_scene;
    QPointF                 m_old_pos;
    QPointF                 m_new_pos;
};


//####################################################################################
//##    Selected new group command
//############################
class SelectionNewGroupCommand : public QUndoCommand
{
public:
    SelectionNewGroupCommand(StageGraphicsScene *scene,
                             QList<DrObject*> old_list,
                             QList<DrObject*> new_list,
                             DrObject *old_first,
                             DrObject *new_first,
                             QUndoCommand *parent = nullptr);
    void        undo() override;
    void        redo() override;

private:
    StageGraphicsScene     *m_scene;
    QList<DrObject*>        m_old_list;
    QList<DrObject*>        m_new_list;
    DrObject               *m_old_first_selected;
    DrObject               *m_new_first_selected;
};



#endif // EDITOR_STAGE_SCENE_UNDO_H














