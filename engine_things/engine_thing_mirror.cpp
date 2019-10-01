//
//      Created by Stephens Nunnally on 8/19/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "engine_thing_mirror.h"


//####################################################################################
//##    Constructor / Destructor
//####################################################################################
DrEngineMirror::DrEngineMirror(DrEngineWorld *world, long unique_key, double x, double y, double z, double angle, float opacity, DrPoint size,
                               QColor color_1, QColor color_2, float tint_amount, float blur, float blur_stretch, float scale) :
    DrEngineThing(world, unique_key) {

    this->setAngle(angle);
    this->setOpacity( opacity );
    this->setPosition( DrPoint(x, y) );
    this->setSize(size);
    this->z_order = z;

    this->start_color = color_1;
    this->end_color = color_2;
    this->tint_percent = tint_amount;
    this->blur = blur;
    this->blur_stretch = blur_stretch;
    this->scale = scale;
}

DrEngineMirror::~DrEngineMirror() {

}



//####################################################################################
//##    Override for DrEngineThing::addToWorld()
//####################################################################################
void DrEngineMirror::addToWorld() {

}



//####################################################################################
//##    Override for DrEngineThing::update() - Pulses Light
//####################################################################################
bool DrEngineMirror::update(double time_passed, double time_warp, QRectF &area) {
    Q_UNUSED( time_passed )
    Q_UNUSED( time_warp )

    bool remove = false;

    // ***** Delete object if ends up outside the deletion threshold
    if (area.contains(QPointF(getPosition().x, getPosition().y)) == false) remove = true;
    return remove;
}




