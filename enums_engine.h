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
enum Test_Textures {
    Ball =  -1,
    Block = -2,
    Plant = -3,
    Rover = -4,
    Wheel = -5,
    Spare = -6
};
// ########## !!!!! END



//####################################################################################
//##    DrEngineObject Enumerations
//############################
enum class Body_Type {
    Dynamic,
    Kinematic,
    Static,
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























