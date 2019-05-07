//
//      Created by Stephens Nunnally on 5/6/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "engine.h"
#include "engine_texture.h"

//######################################################################################################
//##    Constructor
//######################################################################################################
Engine::Engine() {

}

// Needs to be explicitly called by OpenGLWidget
void Engine::deleteResources() {
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
long Engine::addTexture(QString from_asset_string) {
    long new_number = static_cast<long>(m_textures.size());
    m_textures[new_number] = new EngineTexture(from_asset_string);
    return new_number;
}
