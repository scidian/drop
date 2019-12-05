//
//      Created by Stephens Nunnally on 8/10/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "engine/things/engine_thing_fisheye.h"


//####################################################################################
//##    Constructor / Destructor
//####################################################################################
DrEngineFisheye::DrEngineFisheye(DrEngineWorld *world, long unique_key, long original_key,
                                 double x, double y, double z, double angle, float opacity, DrPointF size,
                                 QColor start_color_, float tint_, float zoom_) :
    DrEngineThing(world, unique_key, original_key) {

    this->setAngle(angle);
    this->setOpacity(opacity);
    this->setPosition(DrPointF(x, y));
    this->setSize(size);
    this->setZOrder(z);

    this->start_color = start_color_;
    this->color_tint =  tint_;
    this->lens_zoom =   zoom_;
}

DrEngineFisheye::~DrEngineFisheye() {

}








