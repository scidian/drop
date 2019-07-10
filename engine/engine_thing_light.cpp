//
//      Created by Stephens Nunnally on 6/19/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QRandomGenerator>

#include "engine.h"
#include "engine_thing_light.h"
#include "engine_world.h"
#include "form_engine.h"
#include "opengl/opengl.h"

//####################################################################################
//##    Constructor / Destructor
//####################################################################################
DrEngineLight::DrEngineLight(DrEngineWorld *world, long unique_key,
                             double x, double y, double z, Light_Type type, QColor color, float diameter, QPointF cone, float intensity,
                             float shadows, bool draw_shadows, float blur, float pulse, float pulse_speed, float opacity)
    : DrEngineThing(world, unique_key) {

    this->setPosition( QPointF(x, y) );
    this->z_order = z;
    this->light_type = type;
    this->color = color;
    this->light_size = diameter;
    this->cone = cone;
    this->intensity = intensity;
    this->setStartIntensity( intensity );
    this->shadows = shadows;
    this->draw_shadows = draw_shadows;
    this->blur = blur;
    this->pulse = pulse;
    this->pulse_speed = pulse_speed;
    this->setOpacity( opacity );
}


DrEngineLight::~DrEngineLight() {
    getWorld()->mark_light_as_deleted << getKey();
}


//####################################################################################
//##    Override for DrEngineThing::addToWorld()
//####################################################################################
void DrEngineLight::addToWorld() {
    getWorld()->light_count++;
}


//####################################################################################
//##    Override for DrEngineThing::update() - Pulses Light
//####################################################################################
bool DrEngineLight::update(double time_passed, double time_warp, QRectF &area) {
    bool remove = false;

    // ***** Pulse light
    if (qFuzzyCompare(pulse_speed, 0) == false) {
        if (qFuzzyCompare(m_pulse_direction, 0)) {
            m_pulse_direction = pulse_speed;
            m_pulse_target = (pulse_speed < 0) ? m_start_intensity - pulse : m_start_intensity + pulse;
        }

        intensity += m_pulse_direction * static_cast<float>((time_passed / 1000.0) * time_warp);;

        if        (m_pulse_direction > 0 && intensity > m_pulse_target) {
            m_pulse_direction = -(abs(pulse_speed));
            m_pulse_target = m_start_intensity - pulse;
        } else if (m_pulse_direction < 0 && intensity < m_pulse_target) {
            m_pulse_direction =  (abs(pulse_speed));
            m_pulse_target = m_start_intensity + pulse;
        }
    }

    // !!!!! TEMP: Move light
    this->setPosition(QPointF(this->getPosition().x() + 2, this->getPosition().y()));

    // ***** Delete object if ends up outside the deletion threshold
    if (area.contains(getPosition()) == false) remove = true;
    return remove;
}














