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
DrEngineFisheye::DrEngineFisheye(DrEngineWorld *world, long unique_key, double x, double y, double z, double angle, QPointF size,
                                 QColor start_color, float tint, float zoom, float opacity) :
    DrEngineThing(world, unique_key) {

    this->setAngle(angle);
    this->setPosition( QPointF(x, y) );
    this->setSize(size);
    this->z_order = z;

    this->start_color = start_color;
    this->color_tint = tint;
    this->lens_zoom = zoom;

    this->setOpacity( opacity );
}

DrEngineFisheye::~DrEngineFisheye() {

}



//####################################################################################
//##    Override for DrEngineThing::addToWorld()
//####################################################################################
void DrEngineFisheye::addToWorld() {

}



//####################################################################################
//##    Override for DrEngineThing::update() - Pulses Light
//####################################################################################
bool DrEngineFisheye::update(double time_passed, double time_warp, QRectF &area) {
    Q_UNUSED( time_passed )
    Q_UNUSED( time_warp )

    bool remove = false;

    // ***** Delete object if ends up outside the deletion threshold
    if (area.contains(getPosition()) == false) remove = true;
    return remove;
}








