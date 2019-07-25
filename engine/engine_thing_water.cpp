//
//      Created by Stephens Nunnally on 7/18/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "engine_thing_water.h"


//####################################################################################
//##    Constructor / Destructor
//####################################################################################
DrEngineWater::DrEngineWater(DrEngineWorld *world, long unique_key,
                             double x, double y, double z, QPointF size, QColor color, float tint, float opacity) :
    DrEngineThing(world, unique_key) {

    this->setPosition( QPointF(x, y) );
    this->z_order = z;
    this->water_color = color;
    this->water_tint = tint;
    this->water_size = size;
    this->setOpacity( opacity );
}

DrEngineWater::~DrEngineWater() {

}



//####################################################################################
//##    Override for DrEngineThing::addToWorld()
//####################################################################################
void DrEngineWater::addToWorld() {

}



//####################################################################################
//##    Override for DrEngineThing::update() - Pulses Light
//####################################################################################
bool DrEngineWater::update(double time_passed, double time_warp, QRectF &area) {
    Q_UNUSED( time_passed )
    Q_UNUSED( time_warp )

    bool remove = false;

    // ***** Delete object if ends up outside the deletion threshold
    if (area.contains(getPosition()) == false) remove = true;
    return remove;
}

