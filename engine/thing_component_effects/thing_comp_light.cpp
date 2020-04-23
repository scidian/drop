//
//      Created by Stephens Nunnally on 3/16/2020, (c) 2020 Scidian Software, All Rights Reserved
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
#include "engine/thing_component_effects/thing_comp_light.h"
#include "engine/thing/engine_thing.h"
#include "engine/world/engine_world.h"


//####################################################################################
//##    Constructor / Destructor
//####################################################################################
ThingCompLight::ThingCompLight(DrEngineWorld *engine_world, DrEngineThing *parent_thing,
                               Light_Type type_,
                               DrColor    color_,
                               float      diameter_,
                               DrPointF   cone_,
                               float      intensity_,
                               float      shadows_,
                               bool       draw_shadows_,
                               float      blur_,
                               float      pulse_,
                               float      pulse_speed_)
    : DrThingComponent(engine_world, parent_thing, Comps::Thing_Soft_Body) {

    this->light_type =      type_;
    this->color =           color_;
    this->light_size =      abs(diameter_);
    this->cone =            this->m_rotated_cone =      cone_;
    this->intensity =       this->m_start_intensity =   intensity_;
    this->shadows =         shadows_;
    this->draw_shadows =    draw_shadows_;
    this->blur =            blur_;
    this->pulse =           pulse_;
    this->pulse_speed =     pulse_speed_;
}

ThingCompLight::~ThingCompLight() {

}



//####################################################################################
//##    Basic Virtual Component Functions
//####################################################################################
// Called when component is first created
void ThingCompLight::init() {

}

// Called when Thing is added to m_things DrEngineWorld vector
void ThingCompLight::addToWorld() {
    world()->light_count++;
}

// Called when it is time to Render Thing
void ThingCompLight::draw() {
    world()->getEngine()->getOpenGL()->drawEffect(thing(), DrThingType::Light);
}

// Called during DrEngineWorld->updateWorld() step, returns true if parent DrEngineThing should be removed
bool ThingCompLight::update(double time_passed, double time_warp) {

    // ***** Pulse light
    if (Dr::FuzzyCompare(pulse_speed, 0.f) == false) {
        if (Dr::FuzzyCompare(m_pulse_direction, 0.f)) {
            m_pulse_direction = pulse_speed;
            m_pulse_target = (pulse_speed < 0) ? m_start_intensity - pulse : m_start_intensity + pulse;
        }

        intensity += m_pulse_direction * static_cast<float>((time_passed / 1000.0) * time_warp);

        if        (m_pulse_direction > 0 && intensity > m_pulse_target) {
            m_pulse_direction = -(abs(pulse_speed));
            m_pulse_target = m_start_intensity - pulse;
        } else if (m_pulse_direction < 0 && intensity < m_pulse_target) {
            m_pulse_direction =  (abs(pulse_speed));
            m_pulse_target = m_start_intensity + pulse;
        }
    }

    return false;
}

// Called when component is destroyed
void ThingCompLight::destroy() {
    world()->mark_light_as_deleted.push_back( thing()->getKey() );
}



//####################################################################################
//##    Override for DrEngineThing::setAngle() - Sets rotated cone
//####################################################################################
void ThingCompLight::setAngle(double new_angle) {
    while (new_angle <   0.0) new_angle += 360.0;
    while (new_angle > 360.0) new_angle -= 360.0;
    thing()->setAngle(new_angle);

    double new_x = cone.x + new_angle;
    double new_y = cone.y + new_angle;

    if (new_y > 360.0) {
        new_y -= 360.0;
    }
    if (new_x > 360.0) {
        new_x -= 360.0;
    }

    m_rotated_cone = DrPointF(new_x, new_y);
}


















