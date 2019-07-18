//
//      Created by Stephens Nunnally on 7/18/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "engine_thing_water.h"

DrEngineWater::DrEngineWater(DrEngineWorld *world, long unique_key,
                             double x, double y, double z) :
    DrEngineThing(world, unique_key) {

    this->setPosition( QPointF(x, y) );
    this->z_order = z;


}
