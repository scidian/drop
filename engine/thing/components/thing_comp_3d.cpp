//
//      Created by Stephens Nunnally on 2/2/2020, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "core/dr_random.h"
#include "engine/engine_signal.h"
#include "engine/thing/components/thing_comp_3d.h"
#include "engine/thing/engine_thing.h"
#include "engine/world/engine_world.h"


//####################################################################################
//##    Constructor / Destructor
//####################################################################################
ThingComp3D::ThingComp3D(DrEngineWorld *engine_world, DrEngineThing *parent_thing)
    : DrThingComponent(engine_world, parent_thing, Comps::Thing_3D) {

}

ThingComp3D::~ThingComp3D() {

}



//####################################################################################
//##    Basic Virtual Component Functions
//####################################################################################
// Called when it is time to Render Thing
///void ThingComp3D::draw() { }

// Called during DrEngineWorld->updateWorld() step, returns true if parent DrEngineThing should be removed
///bool ThingComp3D::update(double time_passed, double time_warp) { return false; }
















