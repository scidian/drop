//
//      Created by Stephens Nunnally on 8/15/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "engine/thing/engine_thing_fire.h"


//####################################################################################
//##    Constructor / Destructor
//####################################################################################
DrEngineFire::DrEngineFire(DrEngineWorld *world, long unique_key, long original_key, double x, double y, double z,
                           DrPointF scale, double angle, float opacity, DrPointF size,
                           Fire_Mask mask, DrColor color_1, DrColor color_2, DrColor smoke, float color_intensity, float smooth, float wave, float speed) :
    DrEngineThing(world, unique_key, original_key) {

    this->setAngle(angle);
    this->setOpacity(opacity);
    this->setPosition(DrPointF(x, y));
    this->setScaleX(scale.x);                                                   // Save x scale for later
    this->setScaleY(scale.y);                                                   // Save y scale for later
    this->setSize(size);
    this->setZOrder(z);

    this->fire_mask =   mask;
    this->start_color = color_1;
    this->end_color =   color_2;
    this->smoke_color = smoke;
    this->intensity =   color_intensity;
    this->smoothness =  smooth;
    this->wavy =        wave;
    this->flame_speed = speed;
}

DrEngineFire::~DrEngineFire() {

}







