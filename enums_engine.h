//
//      Created by Stephens Nunnally on 5/31/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef ENUMS_ENGINE_H
#define ENUMS_ENGINE_H

#include <QString>

//####################################################################################
//##    !!!!! #TEMP: Temporary Demo Enums
//############################
enum class Demo_Player {
    Spawn,
    Car,
    Jump,
    Light,
};
enum class Demo_Space {
    Lines1,
    Lines2,
    Blocks,
    Project,
};
// ########## !!!!! END


enum Asset_Textures {
    Numbers =            -1,

    Water_Normal_1 =    -50,
    Water_Texture_1 =   -61,
    Water_Texture_2 =   -62,
    Water_Texture_3 =   -63,
    Water_Texture_5 =   -65,
    Water_Texture_7 =   -67,
    Water_Texture_8 =   -68,

    Ball =      -100,
    Block =     -101,
    Plant =     -102,
    Rover =     -103,
    Wheel =     -104,
    Spare =     -105
};


//####################################################################################
//##    Render Enumerations
//############################
enum class Render_Type {
    Perspective,
    Orthographic,
};

enum class Matrix_Type {
    Model_View,
    Projection,
};

//####################################################################################
//##    Control Enumerations
//############################
enum class Pedal {
    Brake,
    None,
    Clockwise,
    CounterClockwise,
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

enum class Light_Type {
    Opaque,
    Glow,
};




//####################################################################################
//##    Some public forward function declarations for some enum functions
//############################
namespace Dr {
    QString     StringFromBodyType(Body_Type type);
    QString     StringFromShapeType(Shape_Type type);
}





#endif // ENUMS_ENGINE_H























