//
//      Created by Stephens Nunnally on 5/7/2020, (c) 2020 Scidian Software, All Rights Reserved
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
#include "project/dr_project.h"
#include "project/entities/dr_world.h"
#include "project/settings/settings.h"


//####################################################################################
//##    Clear / Build
//####################################################################################
void WorldMapScene::buildScene() {

    this->clear();
    this->setSceneRect(-2500, -2500, 5000, 5000);

    for (auto &world_pair : m_project->getWorldMap()) {
        this->addItemToSceneFromEntity(world_pair.second);
    }

    getEditorRelay()->viewCenterOnPoint( QPoint(0, 0) );

}

WorldMapItem* WorldMapScene::addItemToSceneFromEntity(DrSettings *entity) {
    // Create new item representing this Thing
    WorldMapItem *graphics_item = new WorldMapItem(m_project, this->getEditorRelay(), entity->getKey());

    // Temporarily disable geometry signal itemChange updates
    bool flags_enabled_before = graphics_item->itemChangeFlagsEnabled();
    graphics_item->disableItemChangeFlags();

    // Add item to scene, set starting position
    this->addItem(graphics_item);
    this->setPositionByOrigin(graphics_item, Position_Flags::Center, graphics_item->startX(), graphics_item->startY());

    // Create a temporary group and destroy it, this causes some unknown but importatnt changes to the items
    // sceneTransform that we really seem to need before we try to move a scaled item with View_Mode::Translating
    QGraphicsItemGroup *group = this->createItemGroup({ graphics_item });
    this->destroyItemGroup(group);

    // Re enable geometry signal itemChange() updates
    if (flags_enabled_before) graphics_item->enableItemChangeFlags();

    return graphics_item;
}



















