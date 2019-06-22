//
//      Created by Stephens Nunnally on 6/19/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "engine.h"
#include "engine_light.h"
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
//##    DrEngine - Light Functions
//######################################################################################################
// Removes all lights from engine
void DrEngineWorld::clearLights() {
    m_engine->getFormEngine()->getOpenGL()->makeCurrent();
    for (DrEngineLight *light : lights) {
        delete light->occluder_fbo;
        delete light->shadow_fbo;
        delete light;
    }
    lights.clear();
    m_engine->getFormEngine()->getOpenGL()->doneCurrent();
}
