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
#include "engine/thing_component_effects/thing_comp_mirror.h"
#include "engine/thing/engine_thing_object.h"
#include "engine/thing/engine_thing.h"
#include "engine/world/engine_world.h"


//####################################################################################
//##    Constructor / Destructor
//####################################################################################
ThingCompMirror::ThingCompMirror(DrEngineWorld *engine_world, DrEngineThing *parent_thing,
                                 DrColor color_1, DrColor color_2, float tint_amount, float blur, float blur_stretch, float scale)
    : DrThingComponent(engine_world, parent_thing, Comps::Thing_Soft_Body) {

    this->start_color =     color_1;
    this->end_color =       color_2;
    this->tint_percent =    tint_amount;
    this->blur =            blur;
    this->blur_stretch =    blur_stretch;
    this->scale =           scale;
}

ThingCompMirror::~ThingCompMirror() {

}



//####################################################################################
//##    Basic Virtual Component Functions
//####################################################################################
// Called when component is first created
void ThingCompMirror::init() {

}

// Called during DrEngineWorld->updateWorld() step
void ThingCompMirror::update(double time_passed, double time_warp) {
    (void) time_passed;
    (void) time_warp;

}

// Called when component is destroyed
void ThingCompMirror::destroy() {

}



















