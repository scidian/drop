//
//      Created by Stephens Nunnally on 4/30/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef WORLD_MAP_SCENE_H
#define WORLD_MAP_SCENE_H

#include <QGraphicsScene>

// Forward Declarations
class DrProject;
class IEditorRelay;


//####################################################################################
//##    WorldMapScene
//##        Holds layers (DrProject::DrWorld, DrProject::DrUI) of DrProject
//############################
class WorldMapScene : public QGraphicsScene
{
    Q_OBJECT

private:
    // External Borrowed Pointers
    DrProject      *m_project;                              // Pointer to currently loaded Project
    IEditorRelay   *m_editor_relay;                         // Pointer to IEditorRelay class of parent form

};

#endif // WORLD_MAP_SCENE_H








