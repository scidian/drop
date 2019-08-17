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
    Numbers =               -1,

    Water_Normal_1 =        -50,
    Water_Texture_1 =       -61,
    Water_Texture_2 =       -62,
    Water_Texture_3 =       -63,
    Water_Texture_4 =       -64,

    Fire_Noise =            -70,
    Fire_Flame_None =       -71,
    Fire_Flame_Torch =      -72,
    Fire_Flame_Candle =     -73,

    Ball =          -100,
    Block =         -101,
    Plant =         -102,
    Rover =         -103,
    Wheel =         -104,
    Spare =         -105
};

enum class Water_Texture {
    None     = 0,
    Ripples  = 1,
    Cells    = 2,
    Caustic  = 3,
    Liquid   = 4,
};

enum class Fire_Mask {
    Torch = 0,
    Candle = 1,
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

enum class Blend_Mode {                 // !!!!! Order is important
    Standard    = 0,
    Multiply    = 1,
    Overlay     = 2,
    Hard_Light  = 3,
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
enum class Body_Type {                  // !!!!! Order is important
    Static =    0,
    Kinematic = 1,
    Dynamic =   2,
};

enum class Shape_Type {
    Circle,                             // cpCircleShapeNew
    Box,                                // cpBoxShapeNew
    Segment,    // "Line"               // cpSegmentShapeNew
    Polygon,                            // cpPolyShapeNew
};

enum class Collision_Type {             // !!!!! Order is important
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























