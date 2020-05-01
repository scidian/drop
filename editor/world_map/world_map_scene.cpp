//
//      Created by Stephens Nunnally on 4/30/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QWidget>

#include "editor/world_map/world_map_scene.h"


//####################################################################################
//##    Constructor / Destructor
//####################################################################################
WorldMapScene::WorldMapScene(QWidget *parent, DrProject *project, IEditorRelay *editor_relay)
    : QGraphicsScene(parent), m_project(project), m_editor_relay(editor_relay) {



}

WorldMapScene::~WorldMapScene() {

}


