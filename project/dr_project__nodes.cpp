//
//      Created by Stephens Nunnally on 5/21/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "project/dr_project.h"
#include "project/entities/dr_world.h"
#include "project/settings/settings_component.h"


//####################################################################################
//##    World Graph Node Functions
//####################################################################################
// Finds a location for an additional World in the World Graph
DrPointF DrProject::getNewWorldGraphPosition() {
    double left = 0, right = 0, top = 0, bottom = 0;

    for (auto &world_pair : getWorldMap()) {
        DrComponent *world_connections = world_pair.second->getComponent(Comps::World_Connections);
        if (world_connections != nullptr) {
            world_connections->updateNodeSize();

            DrPointF top_left =  world_connections->getNodePosition();
            DrPointF node_size = world_connections->getNodeSize();
            DrPointF bottom_right = DrPointF(top_left.x + node_size.x, top_left.y + node_size.y);

            if (top_left.x < left)          left =   top_left.x;
            if (top_left.y < top)           top =    top_left.y;
            if (bottom_right.x > right)     right =  bottom_right.x;
            if (bottom_right.y > bottom)    bottom = bottom_right.y;
        }
    }
    return DrPointF(right + c_node_spacing, top);
}

