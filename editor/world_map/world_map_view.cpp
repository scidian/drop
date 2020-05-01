//
//      Created by Stephens Nunnally on 4/30/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "editor/world_map/world_map_scene.h"
#include "editor/world_map/world_map_view.h"
#include "editor/interface_editor_relay.h"


//####################################################################################
//##    Constructor / Destructor
//####################################################################################
WorldMapView::WorldMapView(QWidget *parent, DrProject *project, WorldMapScene *scene, IEditorRelay *editor_relay)
    : QGraphicsView(parent), m_project(project), m_editor_relay(editor_relay) {

    setScene(scene);

}

WorldMapView::~WorldMapView() {

}



