//
//      Created by Stephens Nunnally on 8/15/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "engine_thing_fire.h"


//####################################################################################
//##    Constructor / Destructor
//####################################################################################
DrEngineFire::DrEngineFire(DrEngineWorld *world, long unique_key, double x, double y, double z, QPointF scale, double angle, QPointF size,
                           QColor color_1, QColor color_2, float color_intensity,
                           float opacity) :
    DrEngineThing(world, unique_key) {

    this->setPosition( QPointF(x, y) );
    this->z_order = z;
    this->setScaleX(scale.x());                                                 // Save x scale for later
    this->setScaleY(scale.y());                                                 // Save y scale for later
    this->fire_size = size;
    this->setAngle(angle);

    this->start_color = color_1;
    this->end_color = color_2;
    this->intensity = color_intensity;

    this->setOpacity( opacity );
}

DrEngineFire::~DrEngineFire() {

}



//####################################################################################
//##    Override for DrEngineThing::addToWorld()
//####################################################################################
void DrEngineFire::addToWorld() {

}



//####################################################################################
//##    Override for DrEngineThing::update() - Pulses Light
//####################################################################################
bool DrEngineFire::update(double time_passed, double time_warp, QRectF &area) {
    Q_UNUSED( time_passed )
    Q_UNUSED( time_warp )

    bool remove = false;

    // ***** Delete object if ends up outside the deletion threshold
    if (area.contains(getPosition()) == false) remove = true;
    return remove;
}







