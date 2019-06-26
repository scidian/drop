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

//######################################################################################################
//##    Constructor / Destructor
//######################################################################################################
DrEngineLight::DrEngineLight() {

}


DrEngineLight::~DrEngineLight() {

}


//######################################################################################################
//##    Override for DrEngineThing::update() - Pulses Light
//######################################################################################################
bool DrEngineLight::update(double time_passed, double time_warp, QRectF &area) {
    bool remove = false;

    // ***** Pulse light
    if (qFuzzyCompare(pulse.z(), 0) == false) {
        if (pulse.y() < pulse.x()) {
            float temp = pulse.y();
            pulse.setY( pulse.x() );
            pulse.setX( temp );
        }
        if (qFuzzyCompare(m_pulse_direction, 0)) {
            m_pulse_direction = pulse.z();
            m_pulse_target = (pulse.z() < 0) ? pulse.x() : pulse.y();
        }

        intensity += m_pulse_direction * static_cast<float>((time_passed / 1000.0) * time_warp);;

        if        (m_pulse_direction > 0 && intensity > m_pulse_target) {
            m_pulse_direction = -(abs(pulse.z()));
            m_pulse_target = pulse.x();// - (pulse.x() * static_cast<float>(QRandomGenerator().bounded(0.5)));
        } else if (m_pulse_direction < 0 && intensity < m_pulse_target) {
            m_pulse_direction =  (abs(pulse.z()));
            m_pulse_target = pulse.y();// + (pulse.y() * static_cast<float>(QRandomGenerator().bounded(0.5)));
        }
    }

    // ***** Delete object if ends up outside the deletion threshold
    if (area.contains(getBodyPosition()) == false) remove = true;

    return false;
}
















