//
//      Created by Stephens Nunnally on 6/19/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Abstract Item representing a thing that has a body in the Physics Engine (object, character, light, etc)
//
//
#include "engine_thing.h"
#include "helper.h"

//####################################################################################
//##    Constructor / Destructor
//####################################################################################
DrEngineThing::DrEngineThing(DrEngineWorld* world, long unique_key) {
    m_world = world;
    m_key = unique_key;
}

DrEngineThing::~DrEngineThing() {

}


//####################################################################################
//##    Get time since last update
//####################################################################################
void DrEngineThing::calculateTimeSinceLastUpdate() {
    time_since_last_update = Dr::MillisecondsElapsed( update_timer );
    Dr::ResetTimer( update_timer );
}
