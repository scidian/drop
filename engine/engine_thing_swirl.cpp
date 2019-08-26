//
//      Created by Stephens Nunnally on 8/26/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "engine_thing_swirl.h"


//####################################################################################
//##    Constructor / Destructor
//####################################################################################
DrEngineSwirl::DrEngineSwirl(DrEngineWorld *world, long unique_key, double x, double y, double z, double angle, float opacity, QPointF size,
                             QColor color, float tint, float rotation_amount) :
    DrEngineThing(world, unique_key) {

    this->setAngle(angle);
    this->setOpacity( opacity );
    this->setPosition( QPointF(x, y) );
    this->setSize(size);
    this->z_order = z;

    this->start_color = color;
    this->color_tint = tint;
    this->rotation = rotation_amount;
}

DrEngineSwirl::~DrEngineSwirl() {

}



//####################################################################################
//##    Override for DrEngineThing::addToWorld()
//####################################################################################
void DrEngineSwirl::addToWorld() {

}



//####################################################################################
//##    Override for DrEngineThing::update() - Pulses Light
//####################################################################################
bool DrEngineSwirl::update(double time_passed, double time_warp, QRectF &area) {
    Q_UNUSED( time_passed )
    Q_UNUSED( time_warp )

    bool remove = false;

    // ***** Delete object if ends up outside the deletion threshold
    if (area.contains(getPosition()) == false) remove = true;
    return remove;
}



