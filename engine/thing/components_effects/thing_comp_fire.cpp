//
//      Created by Stephens Nunnally on 3/10/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "3rd_party/chipmunk/chipmunk.h"
#include "core/dr_debug.h"
#include "core/dr_random.h"
#include "engine/engine.h"
#include "engine/engine_signal.h"
#include "engine/mesh/engine_mesh.h"
#include "engine/opengl/opengl.h"
#include "engine/thing/components_effects/thing_comp_fire.h"
#include "engine/thing/engine_thing.h"
#include "engine/world/engine_world.h"


//####################################################################################
//##    Constructor / Destructor
//####################################################################################
ThingCompFire::ThingCompFire(DrEngineWorld *engine_world, DrEngineThing *parent_thing,
                             Fire_Mask mask, DrColor color_1, DrColor color_2, DrColor smoke, float color_intensity, float smooth, float wave, float speed)
    : DrThingComponent(engine_world, parent_thing, Comps::Thing_Soft_Body) {

    this->fire_mask =       mask;
    this->start_color =     color_1;
    this->end_color =       color_2;
    this->smoke_color =     smoke;
    this->intensity =       color_intensity;
    this->smoothness =      smooth;
    this->wavy =            wave;
    this->flame_speed =     speed;
}

ThingCompFire::~ThingCompFire() {

}



//####################################################################################
//##    Basic Virtual Component Functions
//####################################################################################
// Called when component is first created
void ThingCompFire::init() {

}

// Called when Thing is added to m_things DrEngineWorld vector
void ThingCompFire::addToWorld() {

}

// Called when it is time to Render Thing
void ThingCompFire::draw() {
    world()->getEngine()->getOpenGL()->drawObjectFire(thing());
}

// Called during DrEngineWorld->updateWorld() step, returns true if parent DrEngineThing should be removed
bool ThingCompFire::update(double time_passed, double time_warp) {
    (void) time_passed;
    (void) time_warp;

    return false;
}

// Called when component is destroyed
void ThingCompFire::destroy() {

}

















