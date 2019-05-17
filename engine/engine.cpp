//
//      Created by Stephens Nunnally on 5/6/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "project/project.h"
#include "engine.h"
#include "engine_camera.h"
#include "engine_texture.h"

//######################################################################################################
//##    Constructor
//######################################################################################################
DrEngine::DrEngine(DrProject *project) : m_project(project) {

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
//##    DrEngineCamera
//######################################################################################################
long DrEngine::addCamera(SceneObject* object_to_follow) {
    DrEngineCamera *camera = new DrEngineCamera(this, 0, 0, 800);
    m_cameras[m_camera_keys] = camera;

    if (object_to_follow != nullptr) {
        camera->followObject(object_to_follow);
        camera->setPositionX( static_cast<float>(object_to_follow->position.x()) );
        camera->setPositionY( static_cast<float>(object_to_follow->position.y()) );
    }
    // Increment camera ID generator, return current camera ID
    m_camera_keys++;
    return (m_camera_keys - 1);
}

long DrEngine::addCamera(float x, float y, float z) {
    DrEngineCamera *camera = new DrEngineCamera(this, x, y, z);
    m_cameras[m_camera_keys] = camera;
    // Increment camera ID generator, return current camera ID
    m_camera_keys++;
    return (m_camera_keys - 1);
}

void DrEngine::clearCameras() {
    for (auto camera_pair : m_cameras)
        delete camera_pair.second;
    m_cameras.clear();
}

QVector3D DrEngine::getCameraPos() {
    if (m_active_camera == 0)   return c_no_camera;
    else                        return m_cameras[m_active_camera]->getPosition();
}

void DrEngine::updateCameras() {
    for (auto camera_pair : m_cameras) {
        camera_pair.second->updateCamera();
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




















