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


//####################################################################################
//##    WorldMapView
//##        A sub classed QGraphicsView to show our QGraphicsScene (WorldMapScene)
//############################
class WorldMapView : public QGraphicsView
{
    Q_OBJECT

private:
    // External Borrowed Pointers
    DrProject              *m_project;                                      // Pointer to currently loaded project
    IEditorRelay           *m_editor_relay;                                 // Pointer to IEditorRelay class of parent form

};

#endif // WORLD_MAP_VIEW_H











