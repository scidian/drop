//
//      Created by Stephens Nunnally on 5/31/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef ENUMS_ENGINE_H
#define ENUMS_ENGINE_H

//####################################################################################
//##    !!!!! #TEMP: Temporary Demo Enums
//############################
enum class Demo_Player {
    Spawn,
    Car,
    Jump,
};
enum class Demo_Space {
    Lines1,
    Lines2,
    Blocks,
    Project,
};
// ########## !!!!! END


enum Asset_Textures {
    Numbers = -1,
    Ball =  -4,
    Block = -5,
    Plant = -6,
    Rover = -7,
    Wheel = -8,
    Spare = -9
};


//####################################################################################
//##    DrEngineObject Enumerations
//############################
enum class Body_Type {
    Static =    0,                      // Order is important
    Kinematic = 1,
    Dynamic =   2,
};

enum class Shape_Type {
    Circle,                             // cpCircleShapeNew
    Box,                                // cpBoxShapeNew
    Segment,    // "Line"               // cpSegmentShapeNew
    Polygon,                            // cpPolyShapeNew
};

enum class Collision_Type {
    Damage_None =           1,
    Damage_Player =         2,
    Damage_Enemy =          3,
    Damage_All =            4,
};








#endif // ENUMS_ENGINE_H























