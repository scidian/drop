//
//      Created by Stephens Nunnally on 5/6/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "engine.h"
#include "engine_camera.h"
#include "engine_texture.h"
#include "engine_world.h"
#include "project/project.h"

//####################################################################################
//##    Constructor
//####################################################################################
DrEngine::DrEngine(FormEngine *form_engine, DrProject *project) : m_form_engine(form_engine), m_project(project) {

    m_current_editor_world = m_project->getOption(Project_Options::Current_World).toLongLong();
    m_current_world = m_current_editor_world;

    m_worlds[m_current_world] = new DrEngineWorld(project, this->getTextureMap(), m_current_world);

}

// Needs to be explicitly called by OpenGLWidget
void DrEngine::clearWorlds() {
    for (auto world : m_worlds) {
        delete world.second;
    }
}

// Needs to be explicitly called by OpenGLWidget
void DrEngine::deleteTextures() {
    for (auto texture : m_textures) {
        if (texture.second->isTextureLoaded()) {
            texture.second->deleteTexture();
        }
        delete texture.second;
    }
}


//####################################################################################
//##    Convenience Functions
//####################################################################################
std::string DrEngine::pedalAsString() {
    switch (g_pedal) {
        case Pedal::None:               return "None";
        case Pedal::Brake:              return "Brake";
        case Pedal::Clockwise:          return "Clockwise";
        case Pedal::CounterClockwise:   return "Counter Clockwise";
    }
}


//####################################################################################
//##    DrEngineTexture
//####################################################################################
DrEngineTexture* DrEngine::addTexture(long texture_id, QPixmap &pixmap) {
    m_textures[texture_id] = new DrEngineTexture(pixmap);
    return m_textures[texture_id];
}




















