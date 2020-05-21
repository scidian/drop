//
//      Created by Stephens Nunnally on 4/30/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QDebug>
#include "editor/helper_library.h"
#include "editor/interface_editor_relay.h"
#include "editor/world_map/world_map_scene.h"
#include "editor/world_map/world_map_view.h"


//####################################################################################
//##    Constructor / Destructor
//####################################################################################
WorldMapView::WorldMapView(QWidget *parent, DrProject *project, WorldMapScene *scene, IEditorRelay *editor_relay)
    : QGraphicsView(parent), m_project(project), m_editor_relay(editor_relay) {

    updateGrid();

    my_scene = scene;
    setScene(scene);

    connect(my_scene, SIGNAL(aboutToClear()),   this, SLOT(sceneIsAboutToClear()));

}

WorldMapView::~WorldMapView() {

}

// SLOT:    Chance to reset local pointer variables when Scene is about to clear() (like when switching Editor_Mode)
//       ...After Scene is rebuilt local reference pointers will be dangling
void WorldMapView::sceneIsAboutToClear() {
    ///m_origin_item = nullptr;
}

















