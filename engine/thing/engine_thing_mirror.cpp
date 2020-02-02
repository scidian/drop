//
//      Created by Stephens Nunnally on 8/19/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "engine/thing/engine_thing_mirror.h"


//####################################################################################
//##    Constructor / Destructor
//####################################################################################
DrEngineMirror::DrEngineMirror(DrEngineWorld *world, long unique_key, long original_key,
                               double x, double y, double z, double angle, float opacity, DrPointF size,
                               DrColor color_1, DrColor color_2, float tint_amount, float blur, float blur_stretch, float scale) :
    DrEngineThing(world, unique_key, original_key) {

    this->setAngle(angle);
    this->setOpacity(opacity);
    this->setPosition(DrPointF(x, y));
    this->setSize(size);
    this->setZOrder(z);

    this->start_color =     color_1;
    this->end_color =       color_2;
    this->tint_percent =    tint_amount;
    this->blur =            blur;
    this->blur_stretch =    blur_stretch;
    this->scale =           scale;
}

DrEngineMirror::~DrEngineMirror() {

}





