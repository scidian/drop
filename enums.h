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
enum class Properties;

// Global Enum Constants
constexpr int    c_no_key = -1;             // Value that represents no item selected

//####################################################################################
//##
//##    Main Types of entities possible in project
//##
//############################
enum class DrType {
    Asset,
    Font,
    Image,
    World,
        Stage,
        StartStage,
            Background,
            Foreground,
            Object,
    Variable,

    UI,
        Label,
        Button,
        Joystick,

    NotFound,
    BaseClass,
};

// ################## Sub Types ####################
enum class DrAssetType {
    Object,
    Text,
    Character,

    //Action
    //Logic
};

enum class DrObjectType {
    Object,
    Text,
    Character,

    //Action,
    //Camera,
    //Light,
    //Logic,
    //Particle,
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
};


//####################################################################################
//##    Possible handle rects - Order used to be vector / array index critical, but it shouldn't be now...
//##                            Probably still best to leave Top thru Bottom_Right as values 0 thru 7
//############################
enum class Position_Flags {
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
    const char Key[7] =         "dr_key";           // Stores Object Key in User Property of widget
    const char Color[9] =       "dr_color";         // Stores a color value (QColor.rgba())
    const char Order[9] =       "dr_order";         // Used for properties with mulitple values (like Size has X and Y),
                                                    //      the index of the single property we want (0, 1, 2, etc)

    const char Header[10] =     "dr_header";        // Used for Advisor Text
    const char Body[8] =        "dr_body";          // Used for Advisor Text

    const char Mouse_Over[14] =     "dr_mouse_over";        // Set to true by DrFilterHoverHandler when mouse is over widget
    const char Mouse_Pos[13] =      "dr_mouse_pos";         // Set to mouse position (QPoint) by DrFilterHoverHandler when mouse is moving over widget
    const char Mouse_Down[14] =     "dr_mouse_down";        // Set to true when mouse is down so we can prepare for a drag event (used for DrAssets)
    const char Mouse_Down_Pos[18] = "dr_mouse_down_pos";    // Set to mouse position when mouse button is first pressed (used for DrAssets)
}


//####################################################################################
//##    Some public forward function declarations for some enum functions
//############################
namespace Dr {
    QString     StringFromType(DrType type);
    QString     StringFromAssetType(DrAssetType type);
    QString     StringFromObjectType(DrObjectType type);
    QString     StringFromPositionFlag(Position_Flags flag);

    QList<long>         ConvertPropertyListToLongs(QList<Properties> list);
    QList<DrSettings*>  ConvertItemListToSettings(QList<QGraphicsItem*> list);
}


//####################################################################################
//##    Used to track what the QVariant m_value data type really is
//############################
enum class Property_Type {
    Bool,                   // bool         true or false
    Int,                    // long         any integer
    Positive,               // long         integer >= 0
    Filter,                 // long         integer from -255 to 255, used for image filters
    FilterAngle,            // long         integer from    0 to 360, used for image filters

    Double,                 // double       any floating point number
    PositiveDouble,         // double       any floating point number >= 0.0
    Percent,                // double       floating point from 0.0 to 100.0
    Angle,                  // double       floating point for showing degrees, shows degree symbol in spinbox
    String,                 // QString

    PositionF,              // QPointF      Floating pair x and y, used for object positions in scene
                            //                  Y is shown flipped (i.e. * -1), Box2D and Monogame use different coordinate system than Qt
                            //                  Y flipped in: createDoubleSpinBoxPair(), updateSettingsFromNewValue(),
                            //                                updateInspectorPropertyBoxes(), updateToolTipData()
    PointF,                 // QPointF      Floating pair x and y
    SizeF,                  // QPointF      Floating pair w and h
    PositiveSizeF,          // QPointF      Floating pair x and y, both numbers are >= 1.0
    ScaleF,                 // QPointF      Floating pair, has smaller increment step in spin box
    GridF,                  // QPointF      Floating pair x and y, minimum value of c_minimum_grid_size for both x and y
    GridScaleF,             // QPointF      Floating pair x and y, minimum value of c_minimum_grid_scale for both x and y
    Variable,               // QPointF      floating point pair, number followed by a +/- number

    Image,                  // QPixmap
    Icon,
    Color,                  // QColor.rgba()    For Colors
    Polygon,                // QPolygon         For Collision Shapes
    Vector3D,
    List,
};


//####################################################################################
//##    Possible components and their properties
//############################
enum class Components {
    Asset_Settings,
    Asset_Animation,
    Asset_Font,

    World_Settings,
    World_Physics,
    World_Appearance,

    Stage_Settings,
    Stage_Grid,

    Object_Settings,
    Object_Settings_Camera,
    Object_Settings_Character,
    Object_Settings_Text,

    Object_Transform,
    Object_Layering,
    Object_Movement,
    Object_Appearance,
};

enum class Properties {

    // Settings
    Asset_Name,                     //string
    Asset_Collision_Shape,          //polygon

    // Animation
    Asset_Animation_Default,        //image

    // Font
    Asset_Font_Family,
    Asset_Font_Size,

    // ********************

    // Settings
    World_Name,                     //string
    World_Game_Direction,           //float
    World_Score_Multiplier,         //float
    World_Use_Background_Color,     //bool
    World_Background_Color,         //color

    // Physics
    World_Gravity,                  //pointf (x, y)
    World_Time_Warp,                //float
    World_Friction,                 //float
    World_Drag,                     //float
    World_Bounce ,                  //float

    // Appearance
    World_Filter_Pixelation,        //positive size
    World_Filter_Brightness,        //filter
    World_Filter_Contrast,          //filter
    World_Filter_Saturation,        //filter
    World_Filter_Hue,               //filter angle
    World_Filter_Grayscale,         //bool
    World_Filter_Negative,          //bool


    // ********************

    // Settings
    Stage_Name,                     //string
    Stage_Start,                    //positive
    Stage_End,                      //positive
    Stage_Size,                     //positive
    Stage_Cooldown,                 //Positive

    // Grid
    Stage_Grid_Style,               //list
    Stage_Grid_Origin_Point,        //pointf
    Stage_Grid_Size,                //sizef
    Stage_Grid_Scale,               //scale
    Stage_Grid_Rotation,            //angle
    Stage_Grid_Color,               //color

    // ********************

    // Settings
    Object_Name,                    //string
    Object_Physics_Type,            //list (static, kinematic, dynamic)
    Object_Collide,                 //bool
    Object_Damage,                  //list (none, player, enemy, all)

    // Transform
    Object_Position,                //positionf
    Object_Rotation,                //angle
    Object_Size,                    //sizef
    Object_Scale,                   //scale

    // Layering
    Object_Z_Order,                 //int
    Object_Opacity,                 //percent

    // Movement
    Object_Velocity_X,              //variable
    Object_Velocity_Y,              //variable
    Object_Angular_Velocity,        //variable

    // Appearance
    Object_Filter_Pixelation,       //positive size
    Object_Filter_Brightness,       //filter
    Object_Filter_Contrast,         //filter
    Object_Filter_Saturation,       //filter
    Object_Filter_Hue,              //filter angle
    Object_Filter_Grayscale,        //bool
    Object_Filter_Negative,         //bool


    // *****

    // Character Settings
    Object_Character_Jump_X,        //double
    Object_Character_Jump_Y,        //double

    // Text Settings
    Object_Text_User_Text,          //string

    // Camera Settings
    Object_Camera_Zoom,             //double
};





#endif // ENUMS_H
















