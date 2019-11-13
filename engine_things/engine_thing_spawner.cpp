//
//      Created by Stephens Nunnally on 11/12/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "engine_thing_spawner.h"

DrEngineSpawner::DrEngineSpawner(DrEngineWorld *world, long unique_key, double x, double y, double z, DrPointF scale, double angle, float opacity, DrPointF size,
                                 Fire_Mask mask, QColor color_1, QColor color_2, QColor smoke, float color_intensity, float smooth, float wave, float speed) :
          DrEngineThing(world, unique_key) {

}

DrEngineSpawner::~DrEngineSpawner() {

}
