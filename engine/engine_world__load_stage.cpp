//
//      Created by Stephens Nunnally on 10/20/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "engine.h"
#include "engine_world.h"
#include "project/project.h"
#include "project/project_asset.h"
#include "project/project_stage.h"
#include "project/project_thing.h"
#include "project/thing_shape_list.h"



//####################################################################################
//##    Load DrProject Stage to World / Space
//####################################################################################
void DrEngineWorld::loadStageToWorld(DrStage *stage, double offset_x, double offset_y) {

    // ***** Load Things
    for (auto thing_pair : stage->getThingMap()) {

        // Grab current Thing
        DrThing *thing = thing_pair.second;

        // Check if Thing type, if it is, call appropriate Thing Loader
        if (thing->getType() != DrType::Thing) continue;

        switch (thing->getThingType()) {
            // Load Thing
            case DrThingType::Object:   loadObjectToWorld(thing, offset_x, offset_y);       break;

            // Load Thing Effect
            case DrThingType::Fire:     loadFireToWorld(thing, offset_x, offset_y);         break;
            case DrThingType::Fisheye:  loadFisheyeToWorld(thing, offset_x, offset_y);      break;
            case DrThingType::Light:    loadLightToWorld(thing, offset_x, offset_y);        break;
            case DrThingType::Mirror:   loadMirrorToWorld(thing, offset_x, offset_y);       break;
            case DrThingType::Swirl:    loadSwirlToWorld(thing, offset_x, offset_y);        break;
            case DrThingType::Water:    loadWaterToWorld(thing, offset_x, offset_y);        break;

        default:
            break;
        }

    }

    // ***** Update distance we've loaded scenes to
    m_loaded_to += stage->getComponentPropertyValue(Components::Stage_Settings, Properties::Stage_Size).toInt();
}
























