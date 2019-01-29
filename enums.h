//
//      Created by Stephens Nunnally on 12/7/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      List of enumerations used by this project
//
//

#ifndef ENUMS_H
#define ENUMS_H

#include <map>
#include <vector>

#include <QColor>
#include <QPalette>
#include <QVariant>
#include <QGraphicsItem>


//####################################################################################
//##    Types of objects possible in game
//####################################################################################
enum class Form_Main_Mode {
    Edit_Scene,
    Edit_UI,
    Node_Map,
    Scene_Map,
    Clear,
};

// Form Main enum for referencing debug labels
enum class Label_Names
{
    Label_1,            Label_2,            Label_3,            Label_Mouse_1,      Label_Mouse_2,
    Label_Object_1,     Label_Object_2,     Label_Object_3,     Label_Object_4,     Label_Object_5,
    Label_Position,     Label_Center,       Label_Scale,        Label_Rotate,       Label_Z_Order,      Label_Pos_Flag,
    Label_Bottom,
};

//####################################################################################
//##    Sections of FormMain that can have focus
//####################################################################################
enum class Form_Main_Focus {
    Assets,
    Scene_List,
    Scene_View,
    Object_Inspector,
    Bottom_Area,
};

//####################################################################################
//##    Types of objects possible in game
//####################################################################################
enum class DrTypes {
    Project,
    World,      Folder,         Variable,
    Scene,      Background,     Foreground,     StartScene,
    Object,     Character,      Camera,         Action,         Light,          Logic,          Particle,

    UI,
    Label,      Button,         Joystick,

    Asset,

    NotFound,   BaseClass,
};

//####################################################################################
//##    Types of objects possible in game
//####################################################################################
enum class DrAsset_Types {
    Character,
    Object,
};

//####################################################################################
//##    Possible handle rects, order is vector / array index critical
//####################################################################################
enum class Position_Flags {
    Top          = 0,
    Bottom       = 1,
    Right        = 2,
    Left         = 3,
    Top_Left     = 4,
    Bottom_Left  = 5,
    Top_Right    = 6,
    Bottom_Right = 7,
    Total,
    No_Position,
    Move_Item,
    Rotate,
    Center
};


//####################################################################################
//##    Custom Qt::UserRole 's for storing data in QWidgets using setData
//####################################################################################
typedef enum {
    Key = Qt::UserRole,
    Scale,
    Rotation,
    Pre_Rotate_Rotation,

    Name,
    Add_More,
} User_Roles;


//####################################################################################
//##    Custom QStrings for storing data in QWidgets using setProperty
//####################################################################################
namespace User_Property {
    const char Header[10] = "dr_header";
    const char Body[8] = "dr_body";
}

//####################################################################################
//##    Custom QGraphicsItem::UserType 's for referencing types in QGraphicsScene
//####################################################################################
typedef enum {
    Object = QGraphicsItem::UserType,
    No_Type,
} User_Types;


//####################################################################################
//##    Some public forward function declarations
//####################################################################################
bool        CheckTypesAreSame(DrTypes type1, DrTypes type2);
bool        IsDrObjectClass(DrTypes type_to_check);
QString     StringFromType(DrTypes drtype);


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
    const QString Transform     { QString(":/inspector_icons/comp_transform.png") };
    const QString Movement      { QString(":/inspector_icons/comp_movement.png") };
    const QString Camera        { QString(":/inspector_icons/comp_camera.png") };
    const QString Character     { QString(":/inspector_icons/comp_character.png") };
    const QString None          { QString("") };
};


//####################################################################################
//##    Possible Headers and Descriptions for items within editor
//####################################################################################
typedef QList<QString> HeaderBodyList;

namespace Advisor_Info
{
    const HeaderBodyList Asset_List            { "Asset List", "These are items that can be dragged into your scene. Changing the properties "
                                                               "of these items will affect all instances of those items project wide." };
    const HeaderBodyList Object_Inspector      { "Object Inspector", "Displays editable properties of currently selected item." };
    const HeaderBodyList Scene_Area            { "Scene View", "Shows objects and layout of currently selected Scene. Drop assets into Scene "
                                                               "View to add to Scene." };
    const HeaderBodyList Scene_List            { "Scene List", "Lists the items contained within the currently displayed scene. Select items "
                                                               "to view / adjust properties for each item." };
    const HeaderBodyList Advisor_Window        { "Advisor Window", "Shows a brief description of editor objects." };

    const HeaderBodyList World_Object          { "World Object", "A World is a container of Scenes." };
    const HeaderBodyList Scene_Object          { "Scene Object", "A Scene is a container of Objects." };
    const HeaderBodyList Camera_Object         { "Camera Object", "This is a camera object. This object will decide what the player sees." };
    const HeaderBodyList Character_Object      { "Character Object", "This is a character object" };
    const HeaderBodyList Object_Object         { "Object", "This is an object in a Scene." };

    const HeaderBodyList Not_Set               { "Not Set", "Fix me!!!!!!" };
};



//####################################################################################
//##    Used to track what the QVariant m_value data type really is
//####################################################################################
enum class Property_Type {
    Bool,       Int,        Double,     Variable,   Percent,    Angle,
    Char,       String,
    Image,      Icon,       Color,      Polygon,
    Point,      PointF,     SizeF,      Vector3D,
    List,
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
    gravity,                //pointf (x, y)
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

enum class Scene_Properties {
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
    transform,
    movement,

    camera_settings,
    character_settings,
};

enum class Object_Properties
{
    // settings
    name,                   //string
    physics,                //bool
    collide,                //bool
    damage,                 //list

    // transform
    position,               //pointf
    rotation,               //angle
    size,                   //sizef
    scale,                  //pointf
    opacity,                //double

    // movement
    velocity_x,             //variable
    velocity_y,             //variable
    angular_velocity,       //variable

    // camera_settings
    camera_zoom,            //double

    // character_settings
    character_jump_x,       //double
    character_jump_y,       //double
};


//####################################################################################
//##    Object - Possible components and their properties
//####################################################################################

enum class Asset_Components
{
    settings,
    animation,
};

enum class Asset_Properties
{
    // settings
    name,                   //string
    collision_shape,        //polygon

    //animation
    animation_default,      //image

};




#endif // ENUMS_H
















