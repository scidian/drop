//
//      Created by Stephens Nunnally on 4/30/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QWidget>

#include "editor/helper_library.h"
#include "editor/interface_editor_relay.h"
#include "editor/world_map/world_map_item.h"
#include "editor/world_map/world_map_scene.h"


//####################################################################################
//##    Constructor / Destructor
//####################################################################################
WorldMapScene::WorldMapScene(QWidget *parent, DrProject *project, IEditorRelay *editor_relay)
    : QGraphicsScene(parent), m_project(project), m_editor_relay(editor_relay) {



}

WorldMapScene::~WorldMapScene() {

}



//####################################################################################
//##    Clear / Build
//####################################################################################
void WorldMapScene::buildScene() {

    this->clear();
    this->setSceneRect(-2500, -2500, 5000, 5000);

    this->addItem(new WorldMapItem(m_project, m_editor_relay, 1001));

    getEditorRelay()->viewCenterOnPoint( QPoint(0, 0) );

}
