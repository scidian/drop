//
//      Created by Stephens Nunnally on 4/30/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef WORLD_MAP_VIEW_H
#define WORLD_MAP_VIEW_H

#include <QGraphicsView>

// Forward Declarations
class DrProject;
class IEditorRelay;
class WorldMapScene;


//####################################################################################
//##    WorldMapView
//##        A sub classed QGraphicsView to show our QGraphicsScene (WorldMapScene)
//############################
class WorldMapView : public QGraphicsView
{
    Q_OBJECT

private:
    // External Borrowed Pointers
    DrProject              *m_project;                                      // Pointer to currently loaded Project
    IEditorRelay           *m_editor_relay;                                 // Pointer to IEditorRelay class of parent form


public:
    // Constructor / Destructor
    explicit WorldMapView(QWidget *parent, DrProject *project, WorldMapScene *scene, IEditorRelay *editor_relay);
    virtual ~WorldMapView() override;


};

#endif // WORLD_MAP_VIEW_H











