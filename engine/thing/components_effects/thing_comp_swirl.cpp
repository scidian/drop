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
#include "engine/thing/components_effects/thing_comp_swirl.h"
#include "engine/thing/engine_thing.h"
#include "engine/world/engine_world.h"


//####################################################################################
//##    Constructor / Destructor
//####################################################################################
ThingCompSwirl::ThingCompSwirl(DrEngineWorld *engine_world, DrEngineThing *parent_thing,
                               DrColor color, float tint, float rotation_amount)
    : DrThingComponent(engine_world, parent_thing, Comps::Thing_Soft_Body) {

    this->start_color = color;
    this->color_tint =  tint;
    this->rotation =    rotation_amount;
}

ThingCompSwirl::~ThingCompSwirl() {

}



//####################################################################################
//##    Basic Virtual Component Functions
//####################################################################################
// Called when it is time to Render Thing
void ThingCompSwirl::draw() {
    world()->getEngine()->getOpenGL()->drawEffect(thing(), DrThingType::Swirl);
}









