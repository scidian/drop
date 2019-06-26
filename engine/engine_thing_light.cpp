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
DrEngineLight::DrEngineLight(DrEngineWorld *world, DrOpenGL *opengl, long unique_key) : DrEngineObject(unique_key), m_world(world), m_opengl(opengl) {
    m_world->light_count++;
}


DrEngineLight::~DrEngineLight() {
    should_process = false;
    m_world->light_count--;

    m_opengl->makeCurrent();
    delete occluder_fbo;
    delete shadow_fbo;
    m_opengl->doneCurrent();
}


//######################################################################################################
//##    Override for DrEngineThing::update() - Pulses Light
//######################################################################################################
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

    // ***** Delete object if ends up outside the deletion threshold
    if (area.contains(getPosition()) == false) remove = true;
    return remove;
}



//######################################################################################################
//##    DrEngineWorld - Light Functions
//######################################################################################################
DrEngineLight* DrEngineWorld::addLight(double x, double y, double z, QColor color, float diameter, QPointF cone,
                                       float intensity, float shadows, bool draw_shadows, float blur,
                                       float pulse, float pulse_speed, float opacity) {
    DrEngineLight *light = new DrEngineLight(this, m_engine->getFormEngine()->getOpenGL(), getNextKey());
    light->setPosition( QPointF(x, y) );
    light->z_order = z - 0.0001;
    light->color = color;
    light->light_size = diameter;
    light->cone = cone;
    light->intensity = intensity;
    light->setStartIntensity( intensity );
    light->shadows = shadows;
    light->draw_shadows = draw_shadows;
    light->blur = blur;
    light->pulse = pulse;
    light->pulse_speed = pulse_speed;
    light->setOpacity( opacity );

    light->should_process = false;
    light->has_been_processed = true;

    m_things.append(light);
    return light;
}














