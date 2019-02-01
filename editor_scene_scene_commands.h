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
    ChangeSceneCommand(DrProject *project, SceneGraphicsScene *scene, long old_scene, long new_scene, QUndoCommand *parent = nullptr);

    void        undo() override;
    void        redo() override;

    QString     changeScene(long old_scene, long new_scene, bool is_undo);

private:
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
    MoveCommand(SceneGraphicsScene *scene, const QPointF &old_pos, QUndoCommand *parent = nullptr);

    void        undo() override;
    void        redo() override;

private:
    SceneGraphicsScene     *m_scene;
    QPointF                 m_old_pos;
    QPointF                 m_new_pos;
};


//####################################################################################
//##    Selected new group command
//############################
class SelectionNewGroupCommand : public QUndoCommand
{
public:
    SelectionNewGroupCommand(SceneGraphicsScene *scene,
                             QList<DrObject*> old_list,
                             QList<DrObject*> new_list,
                             DrObject *old_first,
                             DrObject *new_first,
                             QUndoCommand *parent = nullptr);
    void        undo() override;
    void        redo() override;

private:
    SceneGraphicsScene     *m_scene;
    QList<DrObject*>        m_old_list;
    QList<DrObject*>        m_new_list;
    DrObject               *m_old_first_selected;
    DrObject               *m_new_first_selected;
};



#endif // EDITOR_SCENE_SCENE_UNDO_H














