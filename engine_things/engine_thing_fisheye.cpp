//
//      Created by Stephens Nunnally on 8/10/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "engine_thing_fisheye.h"


//####################################################################################
//##    Constructor / Destructor
//####################################################################################
DrEngineFisheye::DrEngineFisheye(DrEngineWorld *world, long unique_key, double x, double y, double z, double angle, float opacity, DrPointF size,
                                 QColor start_color, float tint, float zoom) :
    DrEngineThing(world, unique_key) {

    this->setAngle(angle);
    this->setOpacity( opacity );
    this->setPosition( DrPointF(x, y) );
    this->setSize(size);
    this->z_order = z;

    this->start_color = start_color;
    this->color_tint = tint;
    this->lens_zoom = zoom;
}

DrEngineFisheye::~DrEngineFisheye() {

}








