//
//      Created by Stephens Nunnally on 6/19/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef ENGINE_LIGHT_H
#define ENGINE_LIGHT_H

#include "chipmunk/chipmunk.h"
#include "enums_engine.h"
#include "helper.h"

class DrEngineLight
{
public:

    double          light_size = 1500;                  // Diameter of light
    QPointF         position { 0.0, 0.0 };              // Light position in the world
    QPointF         cone { 0.0, 360.0 };                // Start / end angle of light
    double          shadows = 1.0;                      // Visibility of shadows, 0 to 100
    double          intensity = 1.0;                    // Intensity of light, 0 to 100


public:
    DrEngineLight();


};




#endif // ENGINE_LIGHT_H








