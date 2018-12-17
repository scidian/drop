//
//
//      List of enumerations used by this project
//
//

#ifndef ENUMS_H
#define ENUMS_H

#include <string>


//#######################################################################
//
//#######################################################################
enum class Color_Scheme
{
    Dark,       Light,      Blue,       Autumn
};


//
//
//
//  Line 25 is the stupidest line duh
//
//


//#######################################################################
//##
//##    Used to track what the QVariant m_value data type really is
//##
//#######################################################################
enum class Property_Type
{
    BOOL,       INT,        LONG,       FLOAT,      DOUBLE,
    CHAR,       STRING,
    IMAGE,      ICON,       COLOR,
    POINT2D,    POINT3D,
};


//#######################################################################
//##
//##    Types of objects possible in game
//##
//#######################################################################
enum class DrTypes
{
    Project,
    World,      Folder,
    Scene,      Background,     Foreground,     StartScene,     Variable,
    Object,     Character,      Camera,         Action,         Light,          Logic,          Particle,

    UI,
    Label,      Button,         Joystick,

    NotFound,   BaseClass,
};


//#######################################################################
//##
//##    Some public forward function declarations
//##
//#######################################################################
std::string StringFromType(DrTypes drtype);
bool        CheckTypesAreSame(DrTypes type1, DrTypes type2);
bool        IsDrObjectClass(DrTypes type_to_check);


//#######################################################################
//##
//##    World - Possible components and their properties
//##
//#######################################################################

enum class World_Components
{
    settings,
    physics,
};

enum class World_Properties
{
    // settings
    name,                   //string
    game_direction,         //float
    score_multiplier,       //float

    // physics
    use_physics,            //bool
    gravity,                //point2d (x, y) - float
    time_warp,              //float
    friction,               //float
    drag,                   //float
    bounce                  //float
};


//#######################################################################
//##
//##    Scene - Possible components and their properties
//##
//#######################################################################

enum class Scene_Components
{
    settings,
};

enum class Scene_Properties
{
    // settings
    name,                   //string
    start,                  //int
    end,                    //int
    size,                   //int
    cooldown,               //int
};


//#######################################################################
//##
//##    Object - Possible components and their properties
//##
//#######################################################################

enum class Object_Components
{
    settings,
    camera_settings,
    character_settings,
};

enum class Object_Properties
{
    // settings
    name,                   //string
    x,                      //float
    y,                      //float

    // camera_settings
    camera_zoom,            //float

    // character_settings
    character_jump_x,       //float
    character_jump_y,       //float
};




#endif // ENUMS_H
