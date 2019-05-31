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
#include "project/project.h"

//######################################################################################################
//##    Constructor
//######################################################################################################
DrEngine::DrEngine(FormEngine *form_engine, DrProject *project) : m_form_engine(form_engine), m_project(project) {

    keyboard_x = 0;
    keyboard_y = 0;

}

// Needs to be explicitly called by OpenGLWidget
void DrEngine::deleteResources() {
    clearSpace();
    clearCameras();

    for (auto texture : m_textures) {
        if (texture.second->isTextureLoaded()) {
            texture.second->deleteTexture();
        }
        delete texture.second;
    }
}


//######################################################################################################
//##    DrEngineTexture
//######################################################################################################
DrEngineTexture* DrEngine::addTexture(long texture_id, QString from_asset_string) {
    m_textures[texture_id] = new DrEngineTexture(from_asset_string);
    return m_textures[texture_id];
}

DrEngineTexture* DrEngine::addTexture(long texture_id, QPixmap &pixmap) {
    m_textures[texture_id] = new DrEngineTexture(pixmap);
    return m_textures[texture_id];
}




















