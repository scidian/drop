//
//      Created by Stephens Nunnally on 2/2/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "core/dr_random.h"
#include "engine/engine_camera.h"
#include "engine/engine_signal.h"
#include "engine/thing/components/thing_comp_camera.h"
#include "engine/thing/engine_thing.h"
#include "engine/world/engine_world.h"


//####################################################################################
//##    Constructor / Destructor
//####################################################################################
ThingCompCamera::ThingCompCamera(DrEngineWorld *engine_world, DrEngineThing *parent_thing) :
    DrThingComponent(engine_world, parent_thing, Comps::Thing_Settings_Camera) {

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
// Called when it is time to Render Thing
///void ThingCompCamera::draw() { }

// Called during DrEngineWorld->updateWorld() step, returns true if parent DrEngineThing should be removed
///bool ThingCompCamera::update(double time_passed, double time_warp) { return false; }



//####################################################################################
//##    Setters
//####################################################################################v
void ThingCompCamera::setCameraRotation(glm::vec3 rotation) {
    m_camera_rotation = rotation;
    if (hasActiveCamera()) {
        DrEngineCamera *cam = world()->getCamera(m_active_camera);
        if (cam != nullptr) {
            cam->setRotation(rotation);
        }
    }
}
void ThingCompCamera::setCameraRotation(float x_up_down, float y_left_right, float z_rotate) {
    setCameraRotation(glm::vec3(x_up_down, y_left_right, z_rotate));
}




















