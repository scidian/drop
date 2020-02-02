//
//      Created by Stephens Nunnally on 6/19/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Abstract Item representing a thing that has a body in the Physics Engine (object, character, light, etc)
//
//
#include "engine/thing_component/thing_comp_3d.h"


//####################################################################################
//##    Constructor / Destructor
//####################################################################################
ThingComp3D::ThingComp3D(DrEngineWorld *engine_world, DrEngineThing *parent_thing)
    : DrEngineComponent(engine_world, parent_thing, Comps::Thing_3D) {

}

ThingComp3D::~ThingComp3D() {

}










