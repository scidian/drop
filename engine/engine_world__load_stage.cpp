//
//      Created by Stephens Nunnally on 10/20/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "engine.h"
#include "engine_camera.h"
#include "engine_world.h"
#include "project/project.h"
#include "project/project_asset.h"
#include "project/project_stage.h"
#include "project/project_thing.h"
#include "project/thing_shape_list.h"


//####################################################################################
//##    Load DrProject Stage to World / Space
//####################################################################################
void DrEngineWorld::loadStageToWorld(DrStage *stage, double offset_x, double offset_y, bool start_stage) {

    // ***** Load Things
    for (auto thing_pair : stage->getThingMap()) {

        // Grab current Thing
        DrThing *thing = thing_pair.second;

        // Check if Thing type, if it is, call appropriate Thing Loader
        if (thing->getType() != DrType::Thing) continue;

        switch (thing->getThingType()) {
            // Load Thing
            case DrThingType::Object:   loadObjectToWorld(  thing, offset_x, offset_y);     break;

            // Load Thing Effect
            case DrThingType::Fire:     loadFireToWorld(    thing, offset_x, offset_y);     break;
            case DrThingType::Fisheye:  loadFisheyeToWorld( thing, offset_x, offset_y);     break;
            case DrThingType::Light:    loadLightToWorld(   thing, offset_x, offset_y);     break;
            case DrThingType::Mirror:   loadMirrorToWorld(  thing, offset_x, offset_y);     break;
            case DrThingType::Swirl:    loadSwirlToWorld(   thing, offset_x, offset_y);     break;
            case DrThingType::Water:    loadWaterToWorld(   thing, offset_x, offset_y);     break;

            // Load Device
            case DrThingType::Camera:   loadCameraToWorld(  thing, offset_x, offset_y);     break;

            // Don't load Character, only on Start Stage for now
            case DrThingType::Character:
                break;

            default:
                break;
        }

    }

    // Check cameras for another non-character following camera that wants to be active
    if (start_stage) {
        for (auto &camera_pair : m_cameras) {
            DrEngineCamera *camera = camera_pair.second;
            if (camera->getWantsActive()) {
                this->setActiveCamera( camera->getKey() );
                break;
            }
        }
    }

    // ***** Update distance we've loaded scenes to
    m_loaded_to += stage->getComponentPropertyValue(Components::Stage_Settings, Properties::Stage_Size).toInt();
}


//####################################################################################
//##    Load DrEngineCamera
//####################################################################################
void DrEngineWorld::loadCameraToWorld(DrThing *thing, double offset_x, double offset_y) {
    ThingInfo   info =          loadThingBasicInfo( thing );
    bool        wants_active =  thing->getComponentPropertyValue(Components::Thing_Settings_Camera, Properties::Thing_Camera_Set_As_Active).toBool();
    QPointF     cam_speed =     thing->getComponentPropertyValue(Components::Thing_Settings_Camera, Properties::Thing_Camera_Speed).toPointF();
    QPointF     cam_rotation =  thing->getComponentPropertyValue(Components::Thing_Settings_Camera, Properties::Thing_Camera_Rotation).toPointF();
    double      cam_zoom =      thing->getComponentPropertyValue(Components::Thing_Settings_Camera, Properties::Thing_Camera_Zoom).toDouble();
    int         up_vector =     thing->getComponentPropertyValue(Components::Thing_Settings_Character, Properties::Thing_Character_Camera_Up_Vector).toInt();

    float x = static_cast<float>( info.position.x + offset_x);
    float y = static_cast<float>(-info.position.y + offset_y);
    DrEngineCamera *camera = addCamera(0, x, y);
    camera->setSpeed( cam_speed.x(), cam_speed.y(), 0.0 );
    camera->setRotation( cam_rotation.x(), cam_rotation.y(), info.angle );
    camera->setWantActive( wants_active );
    camera->setZoom( cam_zoom );
    camera->setUpVector( static_cast<Up_Vector>(up_vector) );
}






















