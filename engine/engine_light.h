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
    // ***** Object Body and Shapes
    cpBody             *body;                                   // Physical Body of object
    const Body_Type     body_type = Body_Type::Kinematic;       // Body_Type


public:
    DrEngineLight();


};




#endif // ENGINE_LIGHT_H








