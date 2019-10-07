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

    Double,                 // double           Floating point number
    Angle,                  // double           Floating point for showing degrees, shows degree symbol in spinbox
    PositiveDouble,         // double           Floating point number >= 0.0
    RangedDouble,           // double           QList<QVariant> of 4 values, double, min double value, max double value, double step size
    Slider,                 // double w/slider  QList<QVariant> of 5 values, double, min double value, max double value, double step size, string suffix ("%", etc)
    Percent,                // double w/slider  Floating point from 0.0 to 100.0, shows percent symbol

    String,                 // QString          Uses QLineEdit
    Textbox,                // QString          Uses QTextEdit for multi-line htmnl text input

    PointF,                 // QPointF          Floating pair x and y
    PositionF,              // QPointF          Floating pair x and y, used for object positions in scene
                            //                      Y is shown flipped (i.e. * -1), Chipmunk uses different coordinate system than Qt
                            //                      Y flipped in: createDoubleSpinBoxPair(), updateSettingsFromNewValue(),
                            //                                    updateInspectorPropertyBoxes(), updateToolTipData()
    ScaleF,                 // QPointF          Floating pair x and y, has smaller increment step in spin box
    SizeF,                  // QPointF          Floating pair w and h
    PositiveSizeF,          // QPointF          Floating pair w and h, both numbers are >= 1.0

    GridF,                  // QPointF          Floating pair x and y, minimum value of c_minimum_grid_size  for both x and y
    GridScaleF,             // QPointF          Floating pair x and y, minimum value of c_minimum_grid_scale for both x and y
    Variable,               // QPointF          Floating point pair, number followed by a +/- number

    List,                   // enum list

    Collision,              // DrShapeList      For Collision Shapes
    Color,                  // QColor.rgba()    For Colors
    Image,                  // QPixmap
    Icon,                   // QPixmap

    Vector3D,               // QVector3D        X, Y, Z
};


//####################################################################################
//##    Possible components and their properties
//############################
enum class Components {

    // ***** General Components
    Entity_Name,
    Hidden_Settings,


    // ***** DrAsset Components
    Asset_Settings_Character,
    Asset_Settings_Object,
    Asset_Settings_Font,

    Asset_Collision,
    Asset_Animation,


    // ***** DrWorld Components
    World_Settings,
    World_Physics,
    World_Lighting,
    World_Appearance,
    World_Special_Effects,


    // ***** DrStage Components
    Stage_Settings,
    Stage_Grid,


    // ***** DrThing Components
    Thing_Settings_Camera,
    Thing_Settings_Object,
    Thing_Settings_Text,

    Thing_Settings_Fire,
    Thing_Settings_Fisheye,
    Thing_Settings_Light,
    Thing_Settings_Mirror,
    Thing_Settings_Swirl,
    Thing_Settings_Water,
    Thing_Settings_Water_Ripple,
    Thing_Settings_Water_Wave,
    Thing_Settings_Water_Refract,
    Thing_Settings_Water_Foam,  

    Thing_Transform,
    Thing_Layering,
    Thing_Movement,
    Thing_Lighting,
    Thing_3D,
    Thing_Appearance,
    Thing_Special_Effects,

};


enum class Properties {

    // ******************** General Entity Properties
    Entity_Name,


    // ******************** Hidden Properties, for Droplets
    Hidden_Advisor_Description,             // string, html capable textbox
    Hidden_Item_Locked,                     // bool
    Hidden_Hide_From_Trees,                 // bool


    // ******************** DrAsset Component Properties
    // Asset_Settings_Character
    Asset_Character_Max_Speed,              // pointf
    Asset_Character_Forced_Speed,           // pointf
    Asset_Character_Move_Speed,             // pointf
    Asset_Character_Jump_Force,             // pointf
    Asset_Character_Jump_Timeout,           // positive int
    Asset_Character_Jump_Counter,           // int

    Asset_Character_Jump_Air,               // bool
    Asset_Character_Jump_Wall,              // bool

    Asset_Character_Air_Drag,               // positive double
    Asset_Character_Ground_Drag,            // positive double
    Asset_Character_Rotation_Drag,          // positive double

    Asset_Character_Can_Rotate,             // bool
    Asset_Character_Feels_Gravity,          // bool


    // Asset_Settings_Font
    Asset_Font_Family,
    Asset_Font_Size,

    // Asset_Collision
    Asset_Collision_Shape,                  // DrShapeList (polygons)

    // Asset_Animation
    Asset_Animation_Default,                // image


    // ******************** DrWorld Component Properties
    // Settings
    World_Game_Direction,                   // double
    World_Score_Multiplier,                 // double
    World_Use_Background_Color,             // bool
    World_Background_Color,                 // color

    // Physics
    World_Gravity,                          // pointf (x, y)
    World_Time_Warp,                        // double
    World_Friction,                         // double
    World_Drag,                             // double
    World_Bounce ,                          // double

    // Lighting
    World_Light_Ambient,                    // double
    World_Light_Layer,                      // percent
    World_Light_Blend,                      // list, enum class Blend_Mode

    // Appearance
    World_Filter_Bitrate,                   // ranged int, 0 to 256
    World_Filter_Pixelation,                // positive size
    World_Filter_Brightness,                // slider
    World_Filter_Contrast,                  // slider
    World_Filter_Saturation,                // slider
    World_Filter_Hue,                       // slider
    World_Filter_Grayscale,                 // bool
    World_Filter_Negative,                  // bool

    // Special Effects
    World_Filter_Wireframe,                 // bool
    World_Filter_Cartoon,                   // bool
    World_Filter_Wavy,                      // bool


    // ******************** DrStage Component Properties
    // Settings
    Stage_Start,                            // positive
    Stage_End,                              // positive
    Stage_Size,                             // positive
    Stage_Cooldown,                         // positive

    // Grid
    Stage_Grid_Style,                       // list, enum class Grid_Style (Lines,  Dots)
    Stage_Grid_Origin_Point,                // pointf
    Stage_Grid_Size,                        // sizef
    Stage_Grid_Scale,                       // scale
    Stage_Grid_Rotation,                    // angle
    Stage_Grid_Color,                       // color


    // ******************** DrThing Component Properties
    // Transform
    Thing_Position,                         // positionf
    Thing_Rotation,                         // angle
    Thing_Size,                             // sizef
    Thing_Scale,                            // scale

    // Layering
    Thing_Z_Order,                          // double
    Thing_Opacity,                          // percent

    // Movement
    Thing_Velocity_X,                       // variable
    Thing_Velocity_Y,                       // variable
    Thing_Spin_Velocity,                    // variable
    Thing_Angle_Velocity,                   // bool

    // Lighting
    Thing_Lighting_Cast_Shadows,            // bool

    // Appearance
    Thing_Filter_Bitrate,                   // ranged int, 0 to 256
    Thing_Filter_Pixelation,                // positive size
    Thing_Filter_Brightness,                // slider
    Thing_Filter_Contrast,                  // slider
    Thing_Filter_Saturation,                // slider
    Thing_Filter_Hue,                       // slider, 0 to 360
    Thing_Filter_Grayscale,                 // bool
    Thing_Filter_Negative,                  // bool

    // Special Effects
    Thing_Filter_Wireframe,                 // bool

    // 3D Variables
    Thing_3D_Type,                          // list, enum class Convert_3D_Type (Extrusion, Cube)
    Thing_3D_Depth,                         // positive double
    Thing_3D_X_Axis_Rotation,               // variable
    Thing_3D_Y_Axis_Rotation,               // variable
    Thing_3D_X_Axis_Speed,                  // variable
    Thing_3D_Y_Axis_Speed,                  // variable
    Thing_3D_Billboard,                     // bool


    // ********** Thing Specific
    // Camera Settings
    Thing_Camera_Zoom,                      // double

    // Object Settings
    Thing_Object_Physics_Type,              // list, enum class Body_Type (Static, Kinematic, Dynamic)
    Thing_Object_Collide,                   // bool
    Thing_Object_Damage,                    // list, enum class Collision_Type (none, player, enemy, all)

    // Text Settings
    Thing_Text_User_Text,                   // string

    // ********** Effect Specific
    // Fire Settings
    Thing_Fire_Shape,                       // list, enum class Fire_Mask
    Thing_Fire_Color_1,                     // color
    Thing_Fire_Color_2,                     // color
    Thing_Fire_Color_Smoke,                 // color
    Thing_Fire_Intensity,                   // percent
    Thing_Fire_Smoothness,                  // percent
    Thing_Fire_Wavy,                        // percent
    Thing_Fire_Speed,                       // ranged double, 1.0 to ?

    // Fisheye Settings
    Thing_Fisheye_Color,                    // color
    Thing_Fisheye_Color_Tint,               // percent
    Thing_Fisheye_Lens_Zoom,                // ranged double, 0 to 10

    // Light Settings
    Thing_Light_Type,                       // list, enum class Light_Type (Opaque, Glow)
    Thing_Light_Color,                      // color
    Thing_Light_Cone_Start,                 // slider, 0 to 360
    Thing_Light_Cone_End,                   // slider, 0 to 360
    Thing_Light_Intensity,                  // percent, 0 to 100
    Thing_Light_Blur,                       // percent, 0 to 100
    Thing_Light_Draw_Shadows,               // bool
    Thing_Light_Shadows,                    // percent, 0 to 100
    Thing_Light_Pulse,                      // double
    Thing_Light_Pulse_Speed,                // double

    // Mirror Settings
    Thing_Mirror_Start_Color,               // color
    Thing_Mirror_End_Color,                 // color
    Thing_Mirror_Color_Tint,                // percent
    Thing_Mirror_Blur,                      // double
    Thing_Mirror_Blur_Stretch,              // double
    Thing_Mirror_Scale,                     // ranged double, 0.001 to no limit

    // Swirl Settings
    Thing_Swirl_Start_Color,                // color
    Thing_Swirl_Color_Tint,                 // percent
    Thing_Swirl_Angle,                      // double

    // Water Settings
    Thing_Water_Texture,                    // list, enum class Water_Texture
    Thing_Water_Start_Color,                // color
    Thing_Water_End_Color,                  // color
    Thing_Water_Color_Tint,                 // percent
    Thing_Water_Reflection,                 // percent
    Thing_Water_Movement_Speed,             // double
    Thing_Water_Ripple_Frequency,           // double
    Thing_Water_Ripple_Speed,               // double
    Thing_Water_Ripple_Amplitude,           // double
    Thing_Water_Ripple_Stretch,             // double
    Thing_Water_Wave_Frequency,             // double
    Thing_Water_Wave_Speed,                 // double
    Thing_Water_Wave_Amplitude,             // double

    // Water Refract Settings
    Thing_Water_Refract_Reflection,         // percent
    Thing_Water_Refract_Underwater,         // percent
    Thing_Water_Refract_Texture,            // percent
    Thing_Water_Refract_Foam,               // percent

    //Water Foam Settings
    Thing_Water_Surface_Color,              // color
    Thing_Water_Surface_Tint,               // percent
    Thing_Water_Surface_Height,             // double
    Thing_Water_Surface_Is_Flat,            // bool

};



#endif // ENUMS_DR_SETTINGS_H


















