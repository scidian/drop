//
//      Created by Stephens Nunnally on 8/26/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "engine/things/engine_thing_swirl.h"


//####################################################################################
//##    Constructor / Destructor
//####################################################################################
DrEngineSwirl::DrEngineSwirl(DrEngineWorld *world, long unique_key, long original_key,
                             double x, double y, double z, double angle, float opacity, DrPointF size,
                             DrColor color, float tint, float rotation_amount) :
    DrEngineThing(world, unique_key, original_key) {

    this->setAngle(angle);
    this->setOpacity(opacity);
    this->setPosition(DrPointF(x, y));
    this->setSize(size);
    this->setZOrder(z);

    this->start_color = color;
    this->color_tint =  tint;
    this->rotation =    rotation_amount;
}

DrEngineSwirl::~DrEngineSwirl() {

}










