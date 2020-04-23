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
#include "engine/thing_component_effects/thing_comp_water.h"
#include "engine/thing/engine_thing.h"
#include "engine/world/engine_world.h"


//####################################################################################
//##    Constructor / Destructor
//####################################################################################
ThingCompWater::ThingCompWater(DrEngineWorld *engine_world, DrEngineThing *parent_thing,
                               Water_Texture texture, DrColor start_color, DrColor end_color,
                               float    tint,           float reflection,
                               float    rip_length,     float rip_speed,        float rip_amplitude,    float rip_stretch,
                               float    w_length,       float w_speed,          float w_amplitude,
                               DrColor  foam_color,     float foam_tint,        float foam_height,      bool foam_flat,
                               float    r_reflection,   float r_underwater,     float r_texture,        float r_foam,
                               float    texture_movement)
    : DrThingComponent(engine_world, parent_thing, Comps::Thing_Soft_Body) {

    this->water_texture =       texture;
    this->start_color =         start_color;
    this->end_color =           end_color;
    this->water_tint =          tint;
    this->reflection_opacity =  reflection;

    this->ripple_frequency =    rip_length;
    this->ripple_speed =        rip_speed;
    this->ripple_amplitude =    rip_amplitude;
    this->ripple_stretch =      rip_stretch;

    this->wave_frequency =      w_length;
    this->wave_speed =          w_speed;
    this->wave_amplitude =      w_amplitude;

    this->surface_color =       foam_color;
    this->surface_tint =        foam_tint;
    this->surface_height =      foam_height;
    this->surface_keep_flat =   foam_flat;

    this->refract_reflection =  r_reflection;
    this->refract_underwater =  r_underwater;
    this->refract_texture =     r_texture;
    this->refract_foam =        r_foam;
    this->movement_speed =      texture_movement;
}

ThingCompWater::~ThingCompWater() {

}



//####################################################################################
//##    Basic Virtual Component Functions
//####################################################################################
// Called when component is first created
void ThingCompWater::init() {

}

// Called when Thing is added to m_things DrEngineWorld vector
void ThingCompWater::addToWorld() {

}

// Called when it is time to Render Thing
void ThingCompWater::draw() {
    world()->getEngine()->getOpenGL()->drawEffect(thing(), DrThingType::Water);
}

// Called during DrEngineWorld->updateWorld() step, returns true if parent DrEngineThing should be removed
bool ThingCompWater::update(double time_passed, double time_warp) {
    (void) time_passed;
    (void) time_warp;

    return false;
}

// Called when component is destroyed
void ThingCompWater::destroy() {

}








