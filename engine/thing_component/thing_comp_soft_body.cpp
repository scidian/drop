//
//      Created by Stephens Nunnally on 2/2/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "core/dr_random.h"
#include "engine/engine_signal.h"
#include "engine/thing_component/thing_comp_soft_body.h"
#include "engine/thing/engine_thing.h"
#include "engine/world/engine_world.h"


//####################################################################################
//##    Constructor / Destructor
//####################################################################################
ThingCompSoftBody::ThingCompSoftBody(DrEngineWorld *engine_world, DrEngineThing *parent_thing)
    : DrEngineComponent(engine_world, parent_thing, Comps::Thing_Soft_Body) {

}

ThingCompSoftBody::~ThingCompSoftBody() {

}
