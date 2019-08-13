//
//      Created by Stephens Nunnally on 6/25/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef ENUMS_DR_SETTINGS_H
#define ENUMS_DR_SETTINGS_H


//####################################################################################
//##    Used to track what the QVariant m_value data type really is
//############################
enum class Property_Type {
    Bool,                   // bool             True or False
    Int,                    // long             Integer
    Positive,               // long             Integer >= 0
    RangedInt,              // long             QList<QVariant> of 4 values, long, min long value, max long value, long step size

    Angle,                  // double           Floating point for showing degrees, shows degree symbol in spinbox
    Double,                 // double           Floating point number
    PositiveDouble,         // double           Floating point number >= 0.0
    RangedDouble,           // double           QList<QVariant> of 4 values, double, min double value, max double value, double step size
    Slider,                 // double w/slider  QList<QVariant> of 5 values, double, min double value, max double value, double step size, string suffix ("%", etc)
    Percent,                // slider           Floating point from 0.0 to 100.0, shows percent symbol

    String,                 // QString

    PositionF,              // QPointF              Floating pair x and y, used for object positions in scene
                            //                      Y is shown flipped (i.e. * -1), Box2D and Monogame use different coordinate system than Qt
                            //                      Y flipped in: createDoubleSpinBoxPair(), updateSettingsFromNewValue(),
                            //                                    updateInspectorPropertyBoxes(), updateToolTipData()
    PointF,                 // QPointF          Floating pair x and y
    SizeF,                  // QPointF          Floating pair w and h
    PositiveSizeF,          // QPointF          Floating pair x and y, both numbers are >= 1.0
    ScaleF,                 // QPointF          Floating pair, has smaller increment step in spin box
    GridF,                  // QPointF          Floating pair x and y, minimum value of c_minimum_grid_size for both x and y
    GridScaleF,             // QPointF          Floating pair x and y, minimum value of c_minimum_grid_scale for both x and y
    Variable,               // QPointF          Floating point pair, number followed by a +/- number

    Image,                  // QPixmap
    Icon,                   // QPixmap
    Color,                  // QColor.rgba()    For Colors
    Polygon,                // QPolygon         For Collision Shapes
    Vector3D,
    List,                   // enum list
};


//####################################################################################
//##    Possible components and their properties
//############################
enum class Components {

    Entity_Name,

    Asset_Object_Settings,
    Asset_Font_Settings,
    Asset_Animation,

    World_Settings,
    World_Physics,
    World_Lighting,
    World_Special_Effects,
    World_Appearance,

    Stage_Settings,
    Stage_Grid,

    Thing_Settings_Character,
    Thing_Settings_Object,
    Thing_Settings_Light,
    Thing_Settings_Water,
    Thing_Settings_Water_Ripple,
    Thing_Settings_Water_Wave,
    Thing_Settings_Water_Refract,
    Thing_Settings_Water_Foam,
    Thing_Settings_Fisheye,
    Thing_Settings_Text,
    Thing_Settings_Camera,

    Thing_Transform,
    Thing_Layering,
    Thing_Movement,
    Thing_Lighting,
    Thing_Appearance,
};

enum class Properties {

    Entity_Name,

    // ********************

    // Settings
    Asset_Collision_Shape,          //polygon

    // Animation
    Asset_Animation_Default,        //image

    // Font
    Asset_Font_Family,
    Asset_Font_Size,

    // ******************** DrWorld Component Properties

    // Settings
    World_Game_Direction,           //double
    World_Score_Multiplier,         //double
    World_Use_Background_Color,     //bool
    World_Background_Color,         //color

    // Physics
    World_Gravity,                  //pointf (x, y)
    World_Time_Warp,                //double
    World_Friction,                 //double
    World_Drag,                     //double
    World_Bounce ,                  //double

    // Lighting
    World_Light_Ambient,            //double
    World_Light_Layer,              //percent
    World_Light_Blend,              //list, enum class Blend_Mode

    // Appearance
    World_Filter_Pixelation,        //positive size
    World_Filter_Brightness,        //filter
    World_Filter_Contrast,          //filter
    World_Filter_Saturation,        //filter
    World_Filter_Hue,               //slider
    World_Filter_Grayscale,         //bool
    World_Filter_Negative,          //bool

    // Special Effects
    World_Filter_Bitrate,           //ranged int, 0 to 16
    World_Filter_Cartoon,           //bool
    World_Filter_Wavy,              //bool


    // ******************** DrStage Component Properties

    // Settings
    Stage_Start,                    //positive
    Stage_End,                      //positive
    Stage_Size,                     //positive
    Stage_Cooldown,                 //Positive

    // Grid
    Stage_Grid_Style,               //list, enum class Grid_Style (Lines,  Dots)
    Stage_Grid_Origin_Point,        //pointf
    Stage_Grid_Size,                //sizef
    Stage_Grid_Scale,               //scale
    Stage_Grid_Rotation,            //angle
    Stage_Grid_Color,               //color

    // ******************** DrThing Component Properties

    // Transform
    Thing_Position,                 //positionf
    Thing_Rotation,                 //angle
    Thing_Size,                     //sizef
    Thing_Scale,                    //scale

    // Layering
    Thing_Z_Order,                  //double
    Thing_Opacity,                  //percent

    // Movement
    Thing_Velocity_X,               //variable
    Thing_Velocity_Y,               //variable
    Thing_Spin_Velocity,            //variable
    Thing_Angle_Velocity,           //bool

    // Lighting
    Thing_Lighting_Cast_Shadows,    //bool

    // Appearance
    Thing_Filter_Pixelation,        //positive size
    Thing_Filter_Brightness,        //filter
    Thing_Filter_Contrast,          //filter
    Thing_Filter_Saturation,        //filter
    Thing_Filter_Hue,               //slider, 0 to 360
    Thing_Filter_Grayscale,         //bool
    Thing_Filter_Negative,          //bool

    // *****

    // Character Settings
    Thing_Character_Jump_X,         //double
    Thing_Character_Jump_Y,         //double

    // Object Settings
    Thing_Object_Physics_Type,      //list, enum class Body_Type (Static, Kinematic, Dynamic)
    Thing_Object_Collide,           //bool
    Thing_Object_Damage,            //list, enum class Collision_Type (none, player, enemy, all)

    // Light Settings
    Thing_Light_Type,               //list, enum class Light_Type (Opaque, Glow)
    Thing_Light_Color,              //color
    Thing_Light_Cone_Start,         //slider, 0 to 360
    Thing_Light_Cone_End,           //slider, 0 to 360
    Thing_Light_Intensity,          //0 to 100
    Thing_Light_Blur,               //0 to 100
    Thing_Light_Draw_Shadows,       //bool
    Thing_Light_Shadows,            //0 to 100
    Thing_Light_Pulse,              //double
    Thing_Light_Pulse_Speed,        //double

    // Water Settings
    Thing_Water_Texture,            //list, enum class Water_Texture
    Thing_Water_Start_Color,        //color
    Thing_Water_End_Color,          //color
    Thing_Water_Color_Tint,         //percent
    Thing_Water_Reflection,         //percent
    Thing_Water_Movement_Speed,     //double
    Thing_Water_Ripple_Frequency,   //double
    Thing_Water_Ripple_Speed,       //double
    Thing_Water_Ripple_Amplitude,   //double
    Thing_Water_Ripple_Stretch,     //double
    Thing_Water_Wave_Frequency,     //double
    Thing_Water_Wave_Speed,         //double
    Thing_Water_Wave_Amplitude,     //double

    //W ater Refract Settings
    Thing_Water_Refract_Reflection, //percent
    Thing_Water_Refract_Underwater, //percent
    Thing_Water_Refract_Texture,    //percent
    Thing_Water_Refract_Foam,       //percent

    //Water Foam Settings
    Thing_Water_Surface_Color,      //color
    Thing_Water_Surface_Tint,       //percent
    Thing_Water_Surface_Height,     //double

    // Fisheye Settings
    Thing_Fisheye_Color,            //color
    Thing_Fisheye_Color_Tint,       //percent
    Thing_Fisheye_Lens_Zoom,        //ranged double, 0 to 10

    // Text Settings
    Thing_Text_User_Text,           //string

    // Camera Settings
    Thing_Camera_Zoom,              //double

};



#endif // ENUMS_DR_SETTINGS_H


















