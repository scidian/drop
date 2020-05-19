//
//      Created by Stephens Nunnally on 5/7/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QDebug>
#include <QWidget>

#include "editor/helper_library.h"
#include "editor/interface_editor_relay.h"
#include "editor/world_map/world_map_item.h"
#include "editor/world_map/world_map_scene.h"
#include "editor/world_map/world_map_view.h"
#include "project/dr_project.h"
#include "project/entities/dr_world.h"
#include "project/settings/settings.h"
#include "project/settings/settings_component.h"


//####################################################################################
//##    Clear / Build
//####################################################################################
void WorldMapScene::buildScene() {

    // Clear scene, set scene rect
    this->clear();
    this->setSceneRect(-2500, -2500, 5000, 5000);

    // Add Nodes
    for (auto &world_pair : m_project->getWorldMap()) {
        DrComponent *connections = world_pair.second->getComponent(Comps::World_Connections);
        if (connections != nullptr) this->addItemToSceneFromComponent(connections);
    }

    // Center the view on the new stage, set zoom level
    DrPointF new_center =       m_project->getOption(Project_Options::World_Map_Center).toPointF();
    double   new_zoom_scale =   m_project->getOption(Project_Options::World_Map_Zoom).toDouble();
    getEditorRelay()->viewCenterOnPoint( Dr::ToQPointF(new_center) );
    getEditorRelay()->viewZoomToScale( new_zoom_scale );

    // Mark scene as rebuilt
    this->setNeedRebuild(false);
}


WorldMapItem* WorldMapScene::addItemToSceneFromComponent(DrComponent *component) {
    // Create new item representing this Thing
    WorldMapItem *graphics_item = new WorldMapItem(m_project, this->getEditorRelay(), component);

    // Temporarily disable geometry signal itemChange updates
    bool flags_enabled_before = graphics_item->itemChangeFlagsEnabled();
    graphics_item->disableItemChangeFlags();

    // Add item to scene, set starting position
    this->addItem(graphics_item);
    this->setPositionByOrigin(graphics_item, Position_Flags::Center, graphics_item->startPosition().x(), graphics_item->startPosition().y());

    // Create a temporary group and destroy it, this causes some unknown but importatnt changes to the items
    // sceneTransform that we really seem to need before we try to move a scaled item with View_Mode::Translating
    QGraphicsItemGroup *group = this->createItemGroup({ graphics_item });
    this->destroyItemGroup(group);

    // Re enable geometry signal itemChange() updates
    if (flags_enabled_before) graphics_item->enableItemChangeFlags();

    return graphics_item;
}



















