//
//      Created by Stephens Nunnally on 4/2/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "core/dr_random.h"
#include "engine/engine.h"
#include "engine/engine_signal.h"
#include "engine/opengl/opengl.h"
#include "engine/thing_component/thing_comp_tile.h"
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
// Called when component is first created
void ThingCompTile::init() {

}

// Called when Thing is added to m_things DrEngineWorld vector
void ThingCompTile::addToWorld() {

}

// Called when it is time to Render Thing
void ThingCompTile::draw() {
    world()->getEngine()->getOpenGL()->drawObjectTile(thing());
}

// Called during DrEngineWorld->updateWorld() step, returns true if parent DrEngineThing should be removed
bool ThingCompTile::update(double time_passed, double time_warp) {
    (void) time_passed;
    (void) time_warp;

    return false;
}

// Called when component is destroyed
void ThingCompTile::destroy() {

}



















