//
//      Created by Stephens Nunnally on 4/22/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "3rd_party/delaunator.h"
#include "3rd_party/chipmunk/chipmunk.h"
#include "core/dr_debug.h"
#include "core/dr_random.h"
#include "engine/engine.h"
#include "engine/engine_signal.h"
#include "engine/mesh/engine_mesh.h"
#include "engine/thing_component/thing_comp_physics.h"
#include "engine/thing/engine_thing_object.h"
#include "engine/thing/engine_thing.h"
#include "engine/world/engine_world.h"


//####################################################################################
//##    Constructor / Destructor
//####################################################################################
ThingCompPhysics::ThingCompPhysics(DrEngineWorld *engine_world, DrEngineThing *parent_thing)
    : DrThingComponent(engine_world, parent_thing, Comps::Thing_Soft_Body) {

    DrEngineObject *parent_object = dynamic_cast<DrEngineObject*>(parent_thing);
    if (parent_object == nullptr) return;

}

ThingCompPhysics::~ThingCompPhysics() {

}



//####################################################################################
//##    Basic Virtual Component Functions
//####################################################################################
// Called when component is first created
void ThingCompPhysics::init() {

}

// Called when Thing is added to m_things DrEngineWorld vector
void ThingCompPhysics::addToWorld() {

}

// Called when it is time to Render Thing
void ThingCompPhysics::draw() {

}

// Called during DrEngineWorld->updateWorld() step, returns true if parent DrEngineThing should be removed
bool ThingCompPhysics::update(double time_passed, double time_warp) {
    (void) time_passed;
    (void) time_warp;

    return false;
}

// Called when component is destroyed
void ThingCompPhysics::destroy() {

}






