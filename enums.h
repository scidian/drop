//
//      Created by Stephens Nunnally on 12/7/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      List of enumerations used by this project
//
//

#ifndef ENUMS_H
#define ENUMS_H

#include <QGraphicsItem>

// Forward declarations
class DrSettings;
enum class Editor_Widgets;
enum class Properties;

// Global Enum Constants
constexpr int   c_no_key =        -1;               // Value that represents no item selected
constexpr int   c_same_key =    -100;               // Value signifying to use the value already obtained


//####################################################################################
//##
//##    Main Types of entities possible in Project
//##        - All entities inherit DrSettings to use DrComponents which contain DrProperties compatible with the Inspector
//##
//############################
enum class DrType {
    Asset,
    Font,
    Image,
    World,
        Stage,
            Thing,

    //        Background,
    //        Foreground,

    //Variable,

    //UI,
    //    Label,
    //    Button,
    //    Joystick,

    NotFound,
    BaseClass,
};

// ################## Sub Types ####################
enum class DrAssetType {
    Character,
    Object,
    Effect,
    Text,

    Device,

    //Action,
    //Logic,
};

enum class DrThingType {
    None,

    Character,
    Object,
    Text,

    Fire,
    Fisheye,
    Light,
    Mirror,
    Swirl,
    Water,

    Camera,

    //Action,
    //Logic,
    //Particle,
};

enum class DrEffectType {
    Light,
    Water,
    Fire,
    Mirror,
    Fisheye,
    Swirl,
    Flag,
    Rain,
    Snow,
    Clouds,
    Fog,
};

enum class DrDeviceType {
    Camera,
};


//####################################################################################
//##    Custom Qt::UserRole 's for storing data in QWidgets using setData
//############################
enum User_Roles {
    Key = Qt::UserRole,                             // Stores Object Key in User Data of widget / graphicsitem

    Scale,      Pre_Resize_Scale,
    Rotation,   Pre_Rotate_Rotation,
    Z_Order,
    Opacity,

    Name,
    Type,

    Toy,
    Color,
    Selected,
};


//####################################################################################
//##    Possible handle rects - Order used to be vector / array index critical, but it shouldn't be now...
//##                            Probably still best to leave Top thru Bottom_Right as values 0 thru 7
//############################
enum class Position_Flags {                                 // !!!!! Order is important
    Top          = 0,
    Bottom       = 1,
    Right        = 2,
    Left         = 3,
    Top_Left     = 4,
    Bottom_Left  = 5,
    Top_Right    = 6,
    Bottom_Right = 7,
    Center,
    Rotate,
    Move_Item,
    No_Position,
};


//####################################################################################
//##    Custom QStrings for storing data in QWidgets using setProperty
//############################
namespace User_Property {
    const char Key[7] =             "dr_key";               // Stores Object Key in User Property of widget
    const char Color[9] =           "dr_color";             // Stores a color value (QColor.rgba())
    const char Order[9] =           "dr_order";             // Used for properties with mulitple values (like Size has X and Y),
                                                            //      the index of the single property we want (0, 1, 2, etc)

    const char Header[10] =         "dr_header";            // Used for Advisor Text
    const char Body[8] =            "dr_body";              // Used for Advisor Text

    const char Mouse_Over[14] =     "dr_mouse_over";        // Set to true by DrFilterHoverHandler when mouse is over widget
    const char Mouse_Pos[13] =      "dr_mouse_pos";         // Set to mouse position (QPoint) by DrFilterHoverHandler when mouse is moving over widget
    const char Mouse_Down[14] =     "dr_mouse_down";        // Set to true when mouse is down so we can prepare for a drag event (used for DrAssets)
    const char Mouse_Down_Pos[18] = "dr_mouse_down_pos";    // Set to mouse position when mouse button is first pressed (used for DrAssets)
}


//####################################################################################
//##    Some public forward function declarations for some enum functions
//############################
namespace Dr {
    QString     StringFromEditorWidget(Editor_Widgets widget);
    QString     StringFromType(DrType type);
    QString     StringFromAssetType(DrAssetType type);
    QString     StringFromDeviceType(DrDeviceType type);
    QString     StringFromEffectType(DrEffectType type);
    QString     StringFromThingType(DrThingType type);
    QString     StringFromPositionFlag(Position_Flags flag);

    QList<long>         ConvertPropertyListToLongs(QList<Properties> list);
    QList<DrSettings*>  ConvertItemListToSettings(QList<QGraphicsItem*> list);
}





#endif // ENUMS_H
















