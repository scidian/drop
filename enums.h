//
//
//      List of enumerations used by this project
//
//

#ifndef ENUMS_H
#define ENUMS_H

#include <map>
#include <string>
#include <vector>

#include <QColor>
#include <QPalette>
#include <QVariant>

//####################################################################################
//##    Palette options for windows
//####################################################################################
enum class Color_Scheme
{
    Dark,       Light,      Blue,       Autumn
};


//####################################################################################
//##    Used to track what the QVariant m_value data type really is
//####################################################################################
enum class Property_Type
{
    BOOL,       INT,        LONG,       FLOAT,      DOUBLE,
    CHAR,       STRING,
    IMAGE,      ICON,       COLOR,
    POINT2D,    POINT3D,
};


//####################################################################################
//##    Types of objects possible in game
//####################################################################################
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



//####################################################################################
//##    Some public forward function declarations
//####################################################################################
bool        CheckTypesAreSame(DrTypes type1, DrTypes type2);
bool        IsDrObjectClass(DrTypes type_to_check);
std::string StringFromType(DrTypes drtype);


//####################################################################################
//##    Possible background colors for Component Categories
//####################################################################################
namespace Component_Colors
{
    const QColor White_Snow     { QColor(255, 252, 249, 255) };
    const QColor Silver_Sonic   { QColor(117, 112, 131, 255) };

    const QColor Orange_Medium  { QColor(221, 110,  66, 255) };

    const QColor Pink_Pearl     { QColor(228, 180, 194, 255) };
    const QColor Red_Faded      { QColor(255, 105, 120, 255) };

    const QColor Red_Tuscan     { QColor(104,  71,  86, 255) };
    const QColor Purple_Pastel  { QColor(176, 161, 186, 255) };
    const QColor Blue_Yonder    { QColor(165, 181, 191, 255) };

    const QColor Green_SeaGrass { QColor(154, 225, 157, 255) };

    const QColor Brown_Sugar    { QColor(165, 117,  72, 255) };
    const QColor Beige_Apricot  { QColor(252, 215, 173, 255) };


};


//####################################################################################
//##    Possible icons for Component Categories
//####################################################################################
namespace Component_Icons
{
    const QString Settings      { QString(":/inspector_icons/comp_settings.png") };
    const QString Physics       { QString(":/inspector_icons/comp_physics.png") };
    const QString Camera        { QString(":/inspector_icons/comp_camera.png") };
    const QString Character     { QString(":/inspector_icons/comp_character.png") };
    const QString None          { QString("") };
};


//####################################################################################
//##    World - Possible components and their properties
//####################################################################################

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


//####################################################################################
//##    Scene - Possible components and their properties
//####################################################################################

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


//####################################################################################
//##    Object - Possible components and their properties
//####################################################################################

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
