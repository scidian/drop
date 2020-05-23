//
//      Created by Stephens Nunnally on 5/7/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QTreeWidgetItem>
#include <QWidget>

#include "editor/helper_library.h"
#include "editor/interface_editor_relay.h"
#include "editor/view_node_map/node_map_item.h"
#include "editor/view_node_map/node_map_scene.h"
#include "editor/view_node_map/node_map_view.h"
#include "project/dr_project.h"
#include "project/entities/dr_world.h"
#include "project/settings/settings.h"


//####################################################################################
//##    Update
//##        Called after some change has been made to items on another widget besides
//##        the QGraphicsView, updates the items in the scene
//####################################################################################
void NodeMapScene::updateChangesInScene(std::list<DrSettings*> changed_items, std::list<ComponentProperty> component_property_pairs) {
    if (changed_items.empty()) return;

    (void) component_property_pairs;

    for (auto settings_item : changed_items) {
        DrType my_type = settings_item->getType();

        if (my_type == DrType::Thing) {

        } else if (my_type == DrType::Stage || my_type == DrType::World) {

        }

        //################ !!!!!!!!!!!!!!!!!!!!!!!
        //
        //      ROOM FOR MORE TYPES
        //
        //################ !!!!!!!!!!!!!!!!!!!!!!!
    }

    // ***** Try to redraw in case items have changed
    this->update();
    for (auto item : this->items()) {           // Forces repaint of all GraphicsItems
        item->update();
    }
}





















