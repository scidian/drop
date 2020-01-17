//
//      Created by Stephens Nunnally on 10/20/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "engine/engine.h"
#include "engine/engine_camera.h"
#include "engine/engine_spawner.h"
#include "engine/things/engine_thing.h"
#include "engine/things/engine_thing_object.h"
#include "engine/world/engine_world.h"
#include "project/dr_project.h"
#include "project/entities/dr_asset.h"
#include "project/entities/dr_stage.h"
#include "project/entities/dr_thing.h"
#include "project/properties/property_collision.h"


//####################################################################################
//##    Load DrProject Stage to World / Space
//####################################################################################
void DrEngineWorld::loadStageToWorld(DrStage *stage, double offset_x, double offset_y, bool start_stage) {

    // Use to keep a list of Things that were added, so we can attach Spawners later
    std::vector<DrEngineSpawner*>   spawners;           spawners.clear();
    std::vector<DrEngineThing*>     things_in_stage;    things_in_stage.clear();

    // Add Character to thing list for Start Stages
    if (start_stage) {
        for (auto thing : m_things) {
            things_in_stage.push_back(thing);
        }
    }


    // ********** Load Things
    for (auto &thing_pair : stage->getThingMap()) {

        // Grab current Thing
        DrThing *thing = thing_pair.second;

        // Check if Thing type, if it is, call appropriate Thing Loader
        if (thing->getType() != DrType::Thing) continue;
        switch (thing->getThingType()) {

            // ***** Load Thing
            case DrThingType::Object: {
                // Load Spawning Info
                int spawn_count =                   thing->getComponentPropertyValue(Components::Thing_Spawn,     Properties::Thing_Spawn_Count).toInt();
                if (spawn_count == 0)   continue;
                int spawns_remaining =  spawn_count;

                std::vector<DrVariant> spawn_obj =  thing->getComponentPropertyValue(Components::Thing_Spawn,     Properties::Thing_Spawn_At_Object).toVector();
                bool        spawn_instant =         thing->getComponentPropertyValue(Components::Thing_Spawn,     Properties::Thing_Spawn_Instantly).toBool();
                int         spawn_type =            thing->getComponentPropertyValue(Components::Thing_Spawn,     Properties::Thing_Spawn_Type).toInt();
                DrPointF    spawn_rate =            thing->getComponentPropertyValue(Components::Thing_Spawn,     Properties::Thing_Spawn_Rate).toPointF();
                double      spawn_chance =          thing->getComponentPropertyValue(Components::Thing_Spawn,     Properties::Thing_Spawn_Chance).toDouble();
                DrPointF    spawn_x =               thing->getComponentPropertyValue(Components::Thing_Spawn,     Properties::Thing_Spawn_Offset_X).toPointF();
                DrPointF    spawn_y =               thing->getComponentPropertyValue(Components::Thing_Spawn,     Properties::Thing_Spawn_Offset_Y).toPointF();
                DrPointF    pos =                   thing->getComponentPropertyValue(Components::Thing_Transform, Properties::Thing_Position).toPointF();
                bool attached_to_object = spawn_obj[0].toBool();

                // Add Spawner to list for processing when scene finishes loading
                long attached_id = (attached_to_object) ? spawn_obj[1].toLong() : c_no_key;
                DrEngineSpawner *spawner;
                spawner = new DrEngineSpawner(this, thing, static_cast<Spawn_Type>(spawn_type), DrPointF(pos.x + offset_x, -pos.y + offset_y),
                                              spawn_rate.x, spawn_rate.y, spawn_instant, spawn_count, spawns_remaining, spawn_chance,
                                              nullptr, attached_id, spawn_x.x, spawn_x.y, spawn_y.x, spawn_y.y);
                DrEngineObject *object = spawner->update(0.0, 1.0, DrRectF(), false);
                if (object != nullptr) things_in_stage.push_back(object);
                spawners.push_back(spawner);
                break;
            }

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

    // ***** Check cameras for another non-character following camera that wants to be active
    if (start_stage) {
        for (auto &camera_pair : m_cameras) {
            DrEngineCamera *camera = camera_pair.second;
            if (camera->getWantsActive()) {
                this->setActiveCamera( camera->getKey() );
                break;
            }
        }
    }


    // ***** Add Spawners, attach to Object if necessary
    for (auto spawner : spawners) {
        if (spawner->getAttachedIDKey() != c_no_key) {
            for (auto thing : things_in_stage) {
                if (thing->getOriginalKey() == spawner->getAttachedIDKey()) {
                    spawner->setAttachedThing( thing );
                    break;
                }
            }
        }
        addSpawner(spawner);
    }


    // ***** Update distance we've loaded scenes to
    m_loaded_to += stage->getComponentPropertyValue(Components::Stage_Settings, Properties::Stage_Size).toInt();
}


//####################################################################################
//##    Load DrEngineCamera
//####################################################################################
void DrEngineWorld::loadCameraToWorld(DrThing *thing, double offset_x, double offset_y) {
    Thing_Info  info =          loadThingBasicInfo( thing );
    bool        wants_active =  thing->getComponentPropertyValue(Components::Thing_Settings_Camera, Properties::Thing_Camera_Set_As_Active).toBool();
    DrPointF    cam_speed =     thing->getComponentPropertyValue(Components::Thing_Settings_Camera, Properties::Thing_Camera_Speed).toPointF();
    DrPointF    cam_rotation =  thing->getComponentPropertyValue(Components::Thing_Settings_Camera, Properties::Thing_Camera_Rotation).toPointF();
    double      cam_distance =  thing->getComponentPropertyValue(Components::Thing_Settings_Camera, Properties::Thing_Camera_Distance).toDouble();
    double      cam_zoom =      thing->getComponentPropertyValue(Components::Thing_Settings_Camera, Properties::Thing_Camera_Zoom).toDouble();
    int         up_vector =     thing->getComponentPropertyValue(Components::Thing_Settings_Camera, Properties::Thing_Camera_Up_Vector).toInt();

    float x = static_cast<float>( info.position.x + offset_x);
    float y = static_cast<float>(-info.position.y + offset_y);
    DrEngineCamera *camera = addCamera(0, x, y, static_cast<float>(cam_distance));
    camera->setSpeed( cam_speed.x, cam_speed.y, 0.0 );
    camera->setRotation( cam_rotation.x, cam_rotation.y, info.angle );
    camera->setWantActive( wants_active );
    camera->setZoom( cam_zoom, true );
    camera->setUpVector( static_cast<Up_Vector>(up_vector) );
}






















