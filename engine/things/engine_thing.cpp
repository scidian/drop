//
//      Created by Stephens Nunnally on 6/19/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Abstract Item representing a thing that has a body in the Physics Engine (object, character, light, etc)
//
//
#include <QRectF>

#include "engine/things/engine_thing.h"
#include "engine/things/engine_thing_object.h"


//####################################################################################
//##    Constructor / Destructor
//####################################################################################
DrEngineThing::DrEngineThing(DrEngineWorld* world, long unique_key, long original_key) {
    m_world = world;
    m_key = unique_key;
    m_original_project_key = original_key;

    m_camera_rotation = c_default_camera_rot;
}

DrEngineThing::~DrEngineThing() {

}


//####################################################################################
//##    Add to World, called when Thing is added to world
//####################################################################################
void DrEngineThing::addToWorld() {

}


//####################################################################################
//##    Get time since last update
//####################################################################################
void DrEngineThing::calculateTimeSinceLastUpdate() {
    time_since_last_update = Dr::MillisecondsElapsed( update_timer );
    Dr::ResetTimer( update_timer );
}


//####################################################################################
//##    Update Function, called every physics frame
//####################################################################################
bool DrEngineThing::update(double time_passed, double time_warp, QRectF &area) {
    Q_UNUSED( time_passed )
    Q_UNUSED( time_warp )

    bool remove = false;


    // ***** Delete object if ends up outside the deletion threshold
    if (area.contains(QPointF(getPosition().x, getPosition().y)) == false) remove = true;
    return remove;
}










