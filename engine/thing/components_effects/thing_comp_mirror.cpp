//
//      Created by Stephens Nunnally on 3/10/2020, (c) 2020 Scidian Studios, All Rights Reserved
//
//  File:
//
//
//
#include "3rd_party/chipmunk/chipmunk.h"
#include "core/dr_debug.h"
#include "core/dr_random.h"
#include "engine/engine.h"
#include "engine/engine_message.h"
#include "engine/mesh/engine_mesh.h"
#include "engine/opengl/opengl.h"
#include "engine/thing/components_effects/thing_comp_mirror.h"
#include "engine/thing/engine_thing.h"
#include "engine/world/engine_world.h"


//####################################################################################
//##    Constructor / Destructor
//####################################################################################
ThingCompMirror::ThingCompMirror(DrEngineWorld *engine_world, DrEngineThing *parent_thing,
                                 DrColor color_1, DrColor color_2, float tint_amount, float blur, float blur_stretch, float mirror_scale)
    : DrThingComponent(engine_world, parent_thing, Comps::Thing_Soft_Body) {

    this->start_color =     color_1;
    this->end_color =       color_2;
    this->tint_percent =    tint_amount;
    this->blur =            blur;
    this->blur_stretch =    blur_stretch;
    this->scale =           mirror_scale;
}

ThingCompMirror::~ThingCompMirror() {

}



//####################################################################################
//##    Basic Virtual Component Functions
//####################################################################################
// Called when it is time to Render Thing
void ThingCompMirror::draw() {
    world()->getEngine()->getOpenGL()->drawEffect(thing(), DrThingType::Mirror);
}

















