//
//      Created by Stephens Nunnally on 5/6/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "project/project.h"
#include "engine.h"
#include "engine_texture.h"

//######################################################################################################
//##    Constructor
//######################################################################################################
DrEngine::DrEngine(DrProject *project) : m_project(project) { }

// Needs to be explicitly called by OpenGLWidget
void DrEngine::deleteResources() {
    clearSpace();

    for (auto texture : m_textures) {
        if (texture.second->isTextureLoaded()) {
            texture.second->deleteTexture();
        }
        delete texture.second;
    }

    ///for (auto world : m_worlds) { delete world.second; }
}

//######################################################################################################
//##    Add Texture
//######################################################################################################
long DrEngine::addTexture(QString from_asset_string) {
    long new_number = static_cast<long>(m_textures.size());
    m_textures[new_number] = new DrEngineTexture(from_asset_string);
    return new_number;
}
