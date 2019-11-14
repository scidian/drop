//
//      Created by Stephens Nunnally on 10/20/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QDebug>

#include "engine/engine.h"
#include "engine/engine_camera.h"
#include "engine/engine_spawner.h"
#include "engine/engine_world.h"
#include "engine_things/engine_thing.h"
#include "engine_things/engine_thing_object.h"
#include "project/project.h"
#include "project/project_asset.h"
#include "project/project_stage.h"
#include "project/project_thing.h"
#include "project/thing_shape_list.h"


//####################################################################################
//##    Load DrProject Stage to World / Space
//####################################################################################
void DrEngineWorld::loadStageToWorld(DrStage *stage, double offset_x, double offset_y, bool start_stage) {

    // Use to keep a list of Things that were added, so we can attach Spawners later
    std::vector<DrEngineSpawner*>   spawners;           spawners.clear();
    std::vector<DrEngineThing*>     things_in_stage;    things_in_stage.clear();


    // ********** Load Things
    for (auto thing_pair : stage->getThingMap()) {

        // Grab current Thing
        DrThing *thing = thing_pair.second;

        // Check if Thing type, if it is, call appropriate Thing Loader
        if (thing->getType() != DrType::Thing) continue;
        switch (thing->getThingType()) {

            // ***** Load Thing
            case DrThingType::Object: {
                // Load Spawning Info
                int spawn_count =    thing->getComponentPropertyValue(Components::Thing_Spawn,  Properties::Thing_Spawn_Count).toInt();
                if (spawn_count == 0) continue;
                QList<QVariant> spawn_object;
                spawn_object =       thing->getComponentPropertyValue(Components::Thing_Spawn,  Properties::Thing_Spawn_At_Object).toList();
                QPoint spawn_rate =  thing->getComponentPropertyValue(Components::Thing_Spawn,  Properties::Thing_Spawn_Rate).toPoint();
                int spawn_type =     thing->getComponentPropertyValue(Components::Thing_Spawn,  Properties::Thing_Spawn_Type).toInt();
                QPointF spawn_x =    thing->getComponentPropertyValue(Components::Thing_Spawn,  Properties::Thing_Spawn_Offset_X).toPointF();
                QPointF spawn_y =    thing->getComponentPropertyValue(Components::Thing_Spawn,  Properties::Thing_Spawn_Offset_Y).toPointF();
                bool attached_to_object = spawn_object[0].toBool();

                // Load Object if Spawns Permanent
                bool make_spawner = true;
                bool spawned_once = false;
                if (attached_to_object == false && static_cast<Spawn_Type>(spawn_type) == Spawn_Type::Permanent) {
                    DrEngineObject *object = loadObjectToWorld(  thing, offset_x, offset_y);
                    if (object != nullptr) things_in_stage.push_back(object);

                    if (spawn_count > 1) {
                        make_spawner = true;
                        spawned_once = true;
                    } else {
                        make_spawner = false;
                    }
                }

                if (make_spawner) {
                    long attached_id = (attached_to_object) ? spawn_object[1].toLongLong() : c_no_key;
                    DrEngineSpawner *spawner = new DrEngineSpawner(thing, static_cast<Spawn_Type>(spawn_type),
                                                                   DrPointF(offset_x, offset_y), spawn_rate.x(), spawn_rate.y(),
                                                                   spawn_count, nullptr, attached_id,
                                                                   spawn_x.x(), spawn_x.y(), spawn_y.x(), spawn_y.y());
                    spawners.push_back(spawner);
                }
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


    // ***** Add Spawners



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
    int         up_vector =     thing->getComponentPropertyValue(Components::Thing_Settings_Camera, Properties::Thing_Camera_Up_Vector).toInt();

    float x = static_cast<float>( info.position.x + offset_x);
    float y = static_cast<float>(-info.position.y + offset_y);
    DrEngineCamera *camera = addCamera(0, x, y);
    camera->setSpeed( cam_speed.x(), cam_speed.y(), 0.0 );
    camera->setRotation( cam_rotation.x(), cam_rotation.y(), info.angle );
    camera->setWantActive( wants_active );
    camera->setZoom( cam_zoom );
    camera->setUpVector( static_cast<Up_Vector>(up_vector) );
}






















