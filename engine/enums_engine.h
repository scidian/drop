//
//      Created by Stephens Nunnally on 5/31/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      To be Engine compatible, this file does NOT, and should NOT #include any Qt Library types
//
//
#ifndef ENUMS_ENGINE_H
#define ENUMS_ENGINE_H

#include <string>


//####################################################################################
//##    !!!!! #TEMP: Temporary Demo Enums
//############################
enum class Demo_Player {
    Spawn,
    Car,
    Jump,
    Light,
    Player,
};
// ########## !!!!! END

enum Asset_Textures {
    Numbers =               -1,

    Fire_Noise =            -50,
    Fire_Flame_None =       -51,
    Fire_Flame_Torch =      -52,
    Fire_Flame_Candle =     -53,
    Fire_Flame_Square =     -54,
    Fire_Flame_Triangle =   -55,


    Mirror_Noise_1 =        -60,

    Water_Normal_1  =       -70,
    Water_Texture_1 =       -81,
    Water_Texture_2 =       -82,
    Water_Texture_3 =       -83,
    Water_Texture_4 =       -84,

    Ball  =         -100,
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
    Torch =     0,
    Candle =    1,
    Square =    2,
    Triangle =  3,
};


//####################################################################################
//##    Render Enumerations
//############################
enum class Render_Type {
    Perspective,
    Orthographic,
};

enum class Render_Mode {
    Mode_2D,
    Mode_3D,
};

enum class Matrix_Type {
    Model,
    View,
    Projection,
};

enum class Up_Vector {
    Y = 0,                              // Normal 2D Platformer
    Z = 1,                              // Useful for 2.5D First Person Shooter
    ///X = 2,                           // -- Unused Currently --
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
enum class Jump_State {
    Need_To_Jump,
    Jumped,
};

enum class One_Way {                    // One Way Collide
    None,
    Pass_Through,                       // Objects can pass through going one_way_direction
    Weak_Spot,                          // Only takes damage from one_way_direction (bustable block, turtle enemy)
    ///Damage_Direction,                // Only gives damage from one_way_direction
};

enum class Pedal {
    Brake,
    None,
    Clockwise,
    CounterClockwise,
};


//####################################################################################
//##    DrEngineObject Enumerations
//##        !!!!! #NOTE: Order is important for enums that have set values
//############################
enum class Body_Type {
    Static =                0,
    Kinematic =             1,
    Dynamic =               2,
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

enum class Collision_Shape {
    Image,
    Circle,
    Square,
    Triangle,
};

enum class Collision_Groups {
    None =                  0,
    All =                   1,
    Active_Players_Only =   2,
    Players =               3,
    Enemies =               4,

    // Future Use
    Water =                 5,
    Fire =                  6,
};

enum class Spawn_Type {
    Permanent,
    Shoot_Button,
    Jump_Button,
    Object_Death,
};

enum class Death_Animation {
    None,
    Fade,
    Shrink,
};

enum class Light_Type {
    Opaque,
    Glow,
};

enum class Convert_3D_Type {
    Extrusion =             0,
    Cube =                  1,
    Cone =                  2,
    None =                  3,
};


//####################################################################################
//##    Some public forward function declarations for some enum functions
//############################
namespace Dr {
    std::string StringFromBodyType(Body_Type type);
    std::string StringFromShapeType(Shape_Type type);
}





#endif // ENUMS_ENGINE_H






















