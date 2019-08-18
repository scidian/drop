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
                           Fire_Mask mask, QColor color_1, QColor color_2, QColor smoke, float color_intensity, float smooth, float wave, float speed,
                           float opacity) :
    DrEngineThing(world, unique_key) {

    this->setAngle(angle);
    this->setOpacity(opacity);
    this->setPosition(QPointF(x, y));
    this->setScaleX(scale.x());                                                 // Save x scale for later
    this->setScaleY(scale.y());                                                 // Save y scale for later
    this->setSize(size);
    this->z_order = z;

    this->fire_mask = mask;
    this->start_color = color_1;
    this->end_color = color_2;
    this->smoke_color = smoke;
    this->intensity = color_intensity;
    this->smoothness = smooth;
    this->wavy = wave;
    this->flame_speed = speed;
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







