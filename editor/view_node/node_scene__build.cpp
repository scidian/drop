//
//      Created by Stephens Nunnally on 5/7/2020, (c) 2020 Scidian Studios, All Rights Reserved
//
//  File:
//
//
//
#include <QDebug>
#include <QWidget>

#include "editor/helper_library.h"
#include "editor/interface_editor_relay.h"
#include "editor/view_node/node_item.h"
#include "editor/view_node/node_scene.h"
#include "editor/view_node/node_view.h"
#include "project/dr_project.h"
#include "project/entities/dr_world.h"
#include "project/settings/settings.h"
#include "project/settings/settings_component.h"


//####################################################################################
//##    Builds Scene from DrProject Worlds
//####################################################################################
void NodeScene::buildScene() {

    // Clear scene, set scene rect
    this->clearSceneOverride();
    this->setSceneRect(-2500, -2500, 5000, 5000);

    // Add Nodes from DrProject if no Entity Key
    if (getNodeMapEntityKey() == c_no_key) {
        for (auto &world_pair : m_project->getWorldMap()) {
            DrComponent *world_connections = world_pair.second->getComponent(Comps::World_Connections);
            if (world_connections != nullptr) this->addItemToSceneFromComponent(world_connections);
        }

    // Add Nodes from DrSettings of Entity Key
    } else {

    }

    // Center the view on the new stage, set zoom level
    if (getEditorRelay()->currentProject()->worldGraphShown() == false) {
        getEditorRelay()->viewFitToContents();
        getEditorRelay()->currentProject()->setWorldGraphShown(true);
    } else {
        DrPointF new_center =       m_project->getOption(Project_Options::World_Graph_Center).toPointF();
        double   new_zoom_scale =   m_project->getOption(Project_Options::World_Graph_Zoom).toDouble();
        getEditorRelay()->viewCenterOnPoint( Dr::ToQPointF(new_center) );
        getEditorRelay()->viewZoomToScale( new_zoom_scale );
    }

    // Mark scene as rebuilt
    this->setNeedRebuild(false);
}


//####################################################################################
//##    Builds Scene from DrProject Worlds
//####################################################################################
NodeItem* NodeScene::addItemToSceneFromComponent(DrComponent *component) {
    // Create new item representing this Thing
    NodeItem *graphics_item = new NodeItem(m_project, this->getEditorRelay(), component);

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



















