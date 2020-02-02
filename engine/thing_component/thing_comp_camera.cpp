//
//      Created by Stephens Nunnally on 2/2/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "engine/thing_component/thing_comp_camera.h"


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




















