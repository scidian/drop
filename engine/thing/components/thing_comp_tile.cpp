//
//      Created by Stephens Nunnally on 4/2/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "core/dr_random.h"
#include "engine/engine.h"
#include "engine/engine_message.h"
#include "engine/opengl/opengl.h"
#include "engine/thing/components/thing_comp_tile.h"
#include "engine/thing/engine_thing.h"
#include "engine/world/engine_world.h"


//####################################################################################
//##    Constructor / Destructor
//####################################################################################
ThingCompTile::ThingCompTile(DrEngineWorld *engine_world, DrEngineThing *parent_thing, long animation_key)
    : DrThingComponent(engine_world, parent_thing, Comps::Thing_Settings_Tile) {

    m_animation_key = animation_key;
}

ThingCompTile::~ThingCompTile() {

}



//####################################################################################
//##    Basic Virtual Component Functions
//####################################################################################
// Called when it is time to Render Thing
void ThingCompTile::draw() {
    world()->getEngine()->getOpenGL()->drawObjectTile(thing());
}



















