//
//      Created by Stephens Nunnally on 2/2/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "core/dr_random.h"
#include "engine/engine_signal.h"
#include "engine/thing_component/thing_comp_camera.h"
#include "engine/thing/engine_thing.h"
#include "engine/world/engine_world.h"


//####################################################################################
//##    Constructor / Destructor
//####################################################################################
ThingCompCamera::ThingCompCamera(DrEngineWorld *engine_world, DrEngineThing *parent_thing) :
    DrEngineComponent(engine_world, parent_thing, Comps::Thing_Settings_Camera) {

    m_camera_edges[Edge_Location::Top] =    Frame_Edge::Normal;
    m_camera_edges[Edge_Location::Right] =  Frame_Edge::Normal;
    m_camera_edges[Edge_Location::Bottom] = Frame_Edge::Normal;
    m_camera_edges[Edge_Location::Left] =   Frame_Edge::Normal;
}

ThingCompCamera::~ThingCompCamera() {

}


//####################################################################################
//##    Basic Virtual Component Functions
//####################################################################################
// Called when component is first created
void ThingCompCamera::init() {

}

// Called during DrEngineWorld->updateWorld() step
void ThingCompCamera::update(double time_passed, double time_warp) {
    (void) time_passed;
    (void) time_warp;

}

// Called when component is destroyed
void ThingCompCamera::destroy() {

}






















