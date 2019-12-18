//
//      Created by Stephens Nunnally on 6/19/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "engine/engine.h"
#include "engine/form_engine.h"
#include "engine/opengl/opengl.h"
#include "engine/things/engine_thing_light.h"
#include "engine/world/engine_world.h"


//####################################################################################
//##    Constructor / Destructor
//####################################################################################
DrEngineLight::DrEngineLight(DrEngineWorld *world, long unique_key, long original_key,
                             double x, double y, double z, float  opacity,
                             Light_Type type_,
                             DrColor    color_,
                             float      diameter_,
                             DrPointF   cone_,
                             float intensity_,
                             float shadows_,
                             bool  draw_shadows_,
                             float blur_,
                             float pulse_,
                             float pulse_speed_)
    : DrEngineThing(world, unique_key, original_key) {

    this->setOpacity(opacity);
    this->setPosition(DrPointF(x, y));
    this->setZOrder(z);

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


DrEngineLight::~DrEngineLight() {
    getWorld()->mark_light_as_deleted.push_back( getKey() );
}


//####################################################################################
//##    Override for DrEngineThing::addToWorld()
//####################################################################################
void DrEngineLight::addToWorld() {
    getWorld()->light_count++;

    DrEngineThing::addToWorld();
}


//####################################################################################
//##    Override for DrEngineThing::update() - Pulses Light
//####################################################################################
bool DrEngineLight::update(double time_passed, double time_warp, QRectF &area) {
    bool remove = false;

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

    return (remove || DrEngineThing::update(time_passed, time_warp, area));
}


//####################################################################################
//##    Override for DrEngineThing::setAgnle() - Sets rotated cone
//####################################################################################
void DrEngineLight::setAngle(double new_angle) {
    while (new_angle <   0.0) new_angle += 360.0;
    while (new_angle > 360.0) new_angle -= 360.0;
    DrEngineThing::setAngle(new_angle);

    double new_x = cone.x + new_angle;
    double new_y = cone.y + new_angle;

    if (new_y > 360.0) {
        new_y -= 360.0;
    }
    if (new_x > 360.0) {
        new_x -= 360.0;
    }

    m_rotated_cone = DrPointF( new_x, new_y );
}










