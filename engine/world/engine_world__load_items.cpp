//
//      Created by Stephens Nunnally on 4/2/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "engine/engine.h"
#include "engine/thing/components/thing_comp_tile.h"
#include "engine/thing/engine_thing.h"
#include "engine/world/engine_world.h"
#include "project/dr_project.h"
#include "project/entities/dr_asset.h"
#include "project/entities/dr_stage.h"
#include "project/entities/dr_thing.h"


//####################################################################################
//##    Loads one DrProject DrThingType::Fire to World / Space
//####################################################################################
void DrEngineWorld::loadTileToWorld(DrThing *thing, double offset_x, double offset_y) {
    Thing_Info  info =              loadThingBasicInfo( thing );

    long        animation_key =     thing->getComponentPropertyValue(Comps::Thing_Settings_Tile, Props::Thing_Tile_Animation).toLong();

    // Calculate original size used in editor
    info.size.x = info.size.x / info.scale.x;
    info.size.y = info.size.y / info.scale.y;

    DrEngineThing *tile = new DrEngineThing(this, getNextKey(), thing->getKey(), info.position.x + offset_x, -info.position.y + offset_y,
                                            info.z_order, info.scale, info.angle, info.opacity, info.size);
    tile->setComponent(Comps::Thing_Settings_Tile, new ThingCompTile(this, tile, animation_key));
    addThing( tile );

    // ***** Apply additional settings
    loadThingAppearanceSettings(thing, tile, true);             // Appearance settings
}





