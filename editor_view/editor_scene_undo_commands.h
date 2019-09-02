//
//      Created by Stephens Nunnally on 1/23/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Commands used with local m_undo QUndoStack
//
//
#ifndef EDITOR_SCENE_UNDO_H
#define EDITOR_SCENE_UNDO_H

#include <QPointF>
#include <QUndoCommand>

// Forward declarations
class DrProject;
class DrScene;
class DrThing;

//####################################################################################
//##    Change Stage in SceneView
//############################
class UndoCommandChangeStage : public QUndoCommand
{
private:
    // External Borrowed Pointers
    DrProject  *m_project;
    DrScene    *m_scene;

    // Local Variables
    long        m_old_stage;
    long        m_new_stage;

public:
    UndoCommandChangeStage(DrProject *project, DrScene *scene, long old_stage, long new_stage, QUndoCommand *parent = nullptr);

    void        undo() override;
    void        redo() override;

    QString     changeStage(long old_stage, long new_stage, bool is_undo);
};



//####################################################################################
//##    Selection group moved position
//############################
class UndoCommandMove : public QUndoCommand
{
private:
    // External Borrowed Pointers
    DrScene    *m_scene;

    // Local Variables
    QPointF     m_old_pos;
    QPointF     m_new_pos;

public:
    UndoCommandMove(DrScene *scene, const QPointF &old_pos, QUndoCommand *parent = nullptr);

    void        undo() override;
    void        redo() override;
};


//####################################################################################
//##    Selected new group command
//############################
class UndoCommandNewSelection : public QUndoCommand
{ 
private:
    // External Borrowed Pointers
    DrScene    *m_scene;

    // Local Variables
    QList<DrThing*>     m_old_list;
    QList<DrThing*>     m_new_list;

public:
    UndoCommandNewSelection(DrScene *scene, QList<DrThing*> old_list, QList<DrThing*> new_list, QUndoCommand *parent = nullptr);
    void        undo() override;
    void        redo() override;
};



#endif // EDITOR_SCENE_UNDO_H














