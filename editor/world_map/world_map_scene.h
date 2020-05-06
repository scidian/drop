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
//##        Holds nodes (WorldMapItem) representing layers (DrProject::DrNode - Worlds, UI, etc) of DrProject
//############################
class WorldMapScene : public QGraphicsScene
{
    Q_OBJECT

private:
    // External Borrowed Pointers
    DrProject              *m_project;                                      // Pointer to currently loaded Project
    IEditorRelay           *m_editor_relay;                                 // Pointer to IEditorRelay class of parent form


public:
    // Constructor / Destructor
    explicit WorldMapScene(QWidget *parent, DrProject *project, IEditorRelay *editor_relay);
    virtual ~WorldMapScene() override;

    // Construction
    void                    buildScene();                                   // Clears / builds node scene

    // Getters / Setters
    IEditorRelay*           getEditorRelay()        { return m_editor_relay; }

};

#endif // WORLD_MAP_SCENE_H








