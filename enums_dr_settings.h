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
//##        Numbers are SAVE FILE CRITICAL and CANNOT BE CHANGED once set
//##
//##    !!!!! NEXT AVAILABLE NUMBER:  29
//##
//############################
enum class Property_Type {
    Bool            =  0,   // bool             True or False
    BoolDouble      =  1,   // bool w/double    True or False, and when True shows double spinbox
                            //                  QList<QVariant> of 6 values: bool, double value, min, max, double step size, string spinText
    BoolInt         =  28,  // bool w/int       True or False, and when True shows int spinbox
                            //                  QList<QVariant> of 6 values: bool, int value,    min, max, int step size,    string spinText

    Int             =  2,   // long             Integer
    Positive        =  3,   // long             Integer >= 0
    RangedInt       =  4,   // long             QList<QVariant> of 4 values: long, min long value, max long value, long step size

    Double          =  5,   // double           Floating point number
    Angle           =  6,   // double           Floating point for showing degrees, shows degree symbol in spinbox
    PositiveDouble  =  7,   // double           Floating point number >= 0.0
    RangedDouble    =  8,   // double           QList<QVariant> of 4 values, double, min double value, max double value, double step size
    Slider          =  9,   // double w/slider  QList<QVariant> of 5 values, double, min double value, max double value, double step size, string suffix ("%", etc)
    Percent         = 10,   // double w/slider  Floating point from 0.0 to 100.0, shows percent symbol

    String          = 11,   // QString          Uses QLineEdit
    Textbox         = 12,   // QString          Uses QTextEdit for multi-line htmnl text input

    PointF          = 13,   // QPointF          Floating pair x and y
    PositionF       = 14,   // QPointF          Floating pair x and y, used for object positions in scene
                            //                      Y is shown flipped (i.e. * -1), Chipmunk uses different coordinate system than Qt
                            //                      Y flipped in: createDoubleSpinBoxPair(), updateSettingsFromNewValue(),
                            //                                    updateInspectorPropertyBoxes(), updateToolTipData()
    ScaleF          = 15,   // QPointF          Floating pair x and y, has smaller increment step in spin box
    SizeF           = 16,   // QPointF          Floating pair w and h
    PositiveSizeF   = 17,   // QPointF          Floating pair w and h, both numbers are >= 1.0
    Point3D         = 27,   // QVector3D        Floating point trio, x, y, z

    GridF           = 18,   // QPointF          Floating pair x and y, minimum value of c_minimum_grid_size  for both x and y
    GridScaleF      = 19,   // QPointF          Floating pair x and y, minimum value of c_minimum_grid_scale for both x and y
    Variable        = 20,   // QPointF          Floating point pair, number followed by a +/- number

    List            = 21,   // enum list

    Collision       = 22,   // DrShapeList      For Collision Shapes
    Color           = 23,   // QColor.rgba()    For Colors
    Image           = 24,   // QPixmap
    Icon            = 25,   // QPixmap

    Vector3D        = 26,   // QVector3D        X, Y, Z
};


//####################################################################################
//##    Possible DrComponents for DrSettings
//##        Numbers are SAVE FILE CRITICAL and CANNOT BE CHANGED once set
//##
//##    !!!!! NEXT AVAILABLE NUMBER:                    41,
//##    !!!!! MUST ALSO CHANGE:     Components::Total = 41 !!!!!
//##
//############################
enum class Components {

    // ***** General Components
    Entity_Settings                     = 0,
    Hidden_Settings                     = 1,
    Size_Settings                       = 37,


    // ***** DrAsset Components
    Asset_Settings_Character            = 2,
    Asset_Settings_Object               = 3,
    Asset_Settings_Font                 = 4,

    Asset_Collision                     = 5,
    Asset_Animation                     = 6,
    Asset_Health                        = 34,
    Asset_Physics                       = 35,
    Asset_Controls                      = 40,


    // ***** DrWorld Components
    World_Settings                      = 7,
    World_Physics                       = 8,
    World_Lighting                      = 9,
    World_Camera                        = 38,
    World_Appearance                    = 10,
    World_Special_Effects               = 11,


    // ***** DrStage Components
    Stage_Settings                      = 12,
    Stage_Grid                          = 13,


    // ***** DrThing Components
    Thing_Settings_Camera               = 14,
    Thing_Settings_Character            = 36,
    Thing_Settings_Object               = 15,
    Thing_Settings_Text                 = 16,

    Thing_Settings_Fire                 = 17,
    Thing_Settings_Fisheye              = 18,
    Thing_Settings_Light                = 19,
    Thing_Settings_Mirror               = 20,
    Thing_Settings_Swirl                = 21,
    Thing_Settings_Water                = 22,
    Thing_Settings_Water_Ripple         = 23,
    Thing_Settings_Water_Wave           = 24,
    Thing_Settings_Water_Refract        = 25,
    Thing_Settings_Water_Foam           = 26,

    Thing_Transform                     = 27,
    Thing_Layering                      = 28,
    Thing_Movement                      = 29,
    Thing_Spawn                         = 39,
    Thing_Lighting                      = 30,
    Thing_3D                            = 31,
    Thing_Appearance                    = 32,
    Thing_Special_Effects               = 33,

    Total = 41,
};


//####################################################################################
//##    Possible DrProperties of DrComponents
//##        Numbers are SAVE FILE CRITICAL and CANNOT BE CHANGED once set
//##
//##    !!!!! NEXT AVAILABLE NUMBER:   204
//##
//##    Checked for doubles up through 204 on 11/11/2019
//############################
enum class Properties {

    // ******************** General Entity Properties
    Entity_Name                         = 0,        // string
    Entity_Key                          = 1,        // int
    Entity_Asset_Key                    = 2,        // int

    // ******************** Hidden Properties, for Droplets
    Hidden_Advisor_Description          = 3,        // string, html capable textbox
    Hidden_Item_Locked                  = 4,        // bool
    Hidden_Hide_From_Trees              = 5,        // bool

    // ******************** Size Properties, for locking shape / size in editor
    Size_Keep_Square                    = 167,      // bool
    Size_Max_Size                       = 168,      // pointf
    Size_Min_Size                       = 169,      // pointf


    // ******************** DrAsset Component Properties
    // Asset_Settings_Character
    Asset_Character_Max_Speed           = 6,        // pointf
    Asset_Character_Forced_Speed        = 7,        // pointf
    Asset_Character_Move_Speed          = 8,        // pointf
    Asset_Character_Angle_Movement      = 184,      // bool
    Asset_Character_Jump_Force          = 9,        // pointf
    Asset_Character_Jump_Timeout        = 10,       // positive int
    Asset_Character_Jump_Counter        = 11,       // int

    Asset_Character_Jump_Air            = 12,       // bool
    Asset_Character_Jump_Wall           = 13,       // bool

    Asset_Character_Acceleration        = 188,      // positive double
    Asset_Character_Air_Drag            = 14,       // positive double
    Asset_Character_Ground_Drag         = 15,       // positive double
    Asset_Character_Rotation_Drag       = 16,       // positive double

    Asset_Character_Flip_Image_X        = 159,      // bool
    Asset_Character_Flip_Image_Y        = 160,      // bool
    Asset_Character_Mouse_Rotate        = 200,      // bool


    // Asset_Settings_Object




    // Asset_Settings_Font
    Asset_Font_Family                   = 19,
    Asset_Font_Size                     = 20,

    // Asset_Collision
    Asset_Collision_Shape               = 21,       // list, enum class Collision_Shape (image shape, circle, square, triangle)
    Asset_Collision_One_Way_Type        = 141,      // list, enum class One_Way (none, pass_through, weak_spot)
    Asset_Collision_One_Way_Direction   = 142,      // point
    Asset_Collision_Gravity_Multiplier  = 143,      // double
    Asset_Collision_Image_Shape         = 155,      // DrShapeList (polygons)
    Asset_Collision_Circles             = 156,      // future use storing circles?
    Asset_Collision_Squares             = 157,      // future use storing squares?
    Asset_Collision_Polygons            = 158,      // future use storing polygons?
    Asset_Collision_Surface_Velocity    = 161,      // future use storing polygons?

    // Asset_Animation
    Asset_Animation_Default             = 22,       // image

    // Asset_Health
    Asset_Health_Max_Health             = 144,      // double
    Asset_Health_Health                 = 145,      // double
    Asset_Health_Damage                 = 146,      // double
    Asset_Health_Damage_Delay           = 147,      // int
    Asset_Health_Auto_Damage            = 148,      // double
    Asset_Health_Death_Delay            = 149,      // int
    Asset_Health_Fade_On_Death          = 150,      // bool
    Asset_Health_Death_Animation        = 164,      // list, enum class Death_Animation (none, fade, shrink)
    Asset_Health_Death_Duration         = 151,      // int (used to be called Asset_Health_Fade_Delay)
    Asset_Health_Damage_Recoil          = 152,      // double
    Asset_Health_Invincible             = 153,      // bool
    Asset_Health_Death_Touch            = 154,      // bool

    // Asset_Physics
    Asset_Physics_Feels_Gravity         = 18,       // bool
    Asset_Physics_Custom_Friction       = 162,      // bool double combo
    Asset_Physics_Custom_Bounce         = 163,      // bool double combo
    Asset_Physics_Can_Rotate            = 17,       // bool

    // Asset_Controls
    Asset_Controls_Touch_Damage         = 202,      // bool double
    Asset_Controls_Touch_Drag           = 203,      // bool
    Asset_Controls_Rotate_Speed         = 181,      // double


    // ******************** DrWorld Component Properties
    // Settings
    World_Game_Direction                = 23,       // double
    World_Score_Multiplier              = 24,       // double
    World_Deletion_Threshold            = 166,      // positive int
    World_Use_Background_Color          = 25,       // bool
    World_Background_Color              = 26,       // color

    // Physics
    World_Gravity                       = 27,       // pointf (x, y)
    World_Time_Warp                     = 28,       // double
    World_Friction                      = 29,       // double
    World_Drag                          = 30,       // double
    World_Bounce                        = 31,       // double

    // Lighting
    World_Light_Ambient                 = 32,       // double
    World_Light_Layer                   = 33,       // percent
    World_Light_Blend                   = 34,       // list, enum class Blend_Mode

    // Camera
    World_Camera_Type                   = 174,      // list, enum class Render_Type
    World_Camera_Slop                   = 177,      // percent
    World_Camera_Switch_Speed           = 175,      // positive double

    // Appearance
    World_Filter_Bitrate                = 35,       // ranged int, 0 to 256
    World_Filter_Pixelation             = 36,       // positive size
    World_Filter_Brightness             = 37,       // slider
    World_Filter_Contrast               = 38,       // slider
    World_Filter_Saturation             = 39,       // slider
    World_Filter_Hue                    = 40,       // slider
    World_Filter_Grayscale              = 41,       // bool
    World_Filter_Negative               = 42,       // bool

    // Special Effects
    World_Filter_Convert_3D             = 171,      // bool
    World_Filter_Wireframe              = 43,       // bool
    World_Filter_Cartoon                = 44,       // bool double combo
    World_Filter_Cross_Hatch            = 194,      // bool double combo
    World_Filter_Wavy                   = 45,       // bool


    // ******************** DrStage Component Properties
    // Settings
    Stage_Enabled                       = 201,      // bool
    Stage_Start                         = 46,       // positive
    Stage_End                           = 47,       // positive
    Stage_Size                          = 48,       // positive
    Stage_Cooldown                      = 49,       // positive

    // Grid
    Stage_Grid_Style                    = 50,       // list, enum class Grid_Style (Lines,  Dots)
    Stage_Grid_Origin_Point             = 51,       // pointf
    Stage_Grid_Size                     = 52,       // sizef
    Stage_Grid_Scale                    = 53,       // scale
    Stage_Grid_Rotation                 = 54,       // angle
    Stage_Grid_Color                    = 55,       // color


    // ******************** DrThing Component Properties
    // Transform
    Thing_Position                      = 56,       // positionf
    Thing_Rotation                      = 57,       // angle
    Thing_Size                          = 58,       // sizef
    Thing_Scale                         = 59,       // scale

    // Layering
    Thing_Z_Order                       = 60,       // double
    Thing_Sub_Z_Order                   = 139,      // int
    Thing_Opacity                       = 140,      // percent

    // Movement
    Thing_Velocity_X                    = 62,       // variable
    Thing_Velocity_Y                    = 63,       // variable
    Thing_Spin_Velocity                 = 64,       // variable
    Thing_Angle_Velocity                = 65,       // bool
    Thing_Angle_Player                  = 199,      // bool

    // Spawn
    Thing_Spawn_Rate                    = 189,      // variable
    Thing_Spawn_Count                   = 190,      // int
    Thing_Spawn_Type                    = 191,      // list, enum class Spawn_Type
    Thing_Spawn_At_Object               = 192,      // bool int combo
    Thing_Spawn_Offset_X                = 193,      // variable
    Thing_Spawn_Offset_Y                = 196,      // variable
    Thing_Spawn_Instantly               = 197,      // bool


    // Lighting
    Thing_Lighting_Cast_Shadows         = 66,       // bool

    // Appearance
    Thing_Filter_Bitrate                = 67,       // ranged int, 0 to 256
    Thing_Filter_Pixelation             = 68,       // positive size
    Thing_Filter_Brightness             = 69,       // slider
    Thing_Filter_Contrast               = 70,       // slider
    Thing_Filter_Saturation             = 71,       // slider
    Thing_Filter_Hue                    = 72,       // slider, 0 to 360
    Thing_Filter_Grayscale              = 73,       // bool
    Thing_Filter_Negative               = 74,       // bool

    // Special Effects
    Thing_Filter_Convert_3D             = 198,      // bool
    Thing_Filter_Wireframe              = 75,       // bool
    Thing_Filter_Cartoon                = 187,      // bool double combo
    Thing_Filter_Cross_Hatch            = 195,      // bool double combo

    // 3D Variables
    Thing_3D_Type                       = 76,       // list, enum class Convert_3D_Type (extrusion, cube, cone, none)
    Thing_3D_Depth                      = 77,       // positive double
    Thing_3D_X_Axis_Rotation            = 78,       // variable
    Thing_3D_Y_Axis_Rotation            = 79,       // variable
    Thing_3D_X_Axis_Speed               = 80,       // variable
    Thing_3D_Y_Axis_Speed               = 81,       // variable
    Thing_3D_Billboard                  = 82,       // bool


    // ********** Thing Specific
    // Camera Settings
    Thing_Camera_Set_As_Active          = 178,      // bool
    Thing_Camera_Speed                  = 179,      // pointf
    Thing_Camera_Rotation               = 180,      // pointf
    Thing_Camera_Up_Vector              = 183,      // list, enum class Up_Vector
    Thing_Camera_Zoom                   = 83,       // double


    // Character Settings
    Thing_Character_Camera_Position     = 170,      // pointf
    Thing_Character_Camera_Rotation     = 165,      // pointf
    Thing_Character_Camera_Tilt         = 172,      // double
    Thing_Character_Camera_Up_Vector    = 182,      // list, enum class Up_Vector
    Thing_Character_Camera_Zoom         = 173,      // double
    Thing_Character_Camera_Lag          = 176,      // double
    Thing_Character_Camera_Match_Angle  = 185,      // bool

    // Object Settings
    Thing_Object_Physics_Type           = 84,       // list, enum class Body_Type (Static, Kinematic, Dynamic)
    Thing_Object_Collide                = 85,       // bool
    Thing_Object_Collision_Group        = 186,      // list, enum class Collision_Group (none, all, active players, players, enemies)
    Thing_Object_Damage                 = 86,       // list, enum class Collision_Type (none, player, enemy, all)

    // Text Settings
    Thing_Text_User_Text                = 87,       // string


    // ********** Effect Specific
    // Fire Settings
    Thing_Fire_Shape                    = 88,       // list, enum class Fire_Mask
    Thing_Fire_Color_1                  = 89,       // color
    Thing_Fire_Color_2                  = 90,       // color
    Thing_Fire_Color_Smoke              = 91,       // color
    Thing_Fire_Intensity                = 92,       // percent
    Thing_Fire_Smoothness               = 93,       // percent
    Thing_Fire_Wavy                     = 94,       // percent
    Thing_Fire_Speed                    = 95,       // ranged double, 1.0 to ?

    // Fisheye Settings
    Thing_Fisheye_Color                 = 96,       // color
    Thing_Fisheye_Color_Tint            = 97,       // percent
    Thing_Fisheye_Lens_Zoom             = 98,       // ranged double, 0 to 10

    // Light Settings
    Thing_Light_Type                    = 99,       // list, enum class Light_Type (Opaque, Glow)
    Thing_Light_Color                   = 100,      // color
    Thing_Light_Cone_Start              = 101,      // slider, 0 to 360
    Thing_Light_Cone_End                = 102,      // slider, 0 to 360
    Thing_Light_Intensity               = 103,      // percent, 0 to 100
    Thing_Light_Blur                    = 104,      // percent, 0 to 100
    Thing_Light_Draw_Shadows            = 105,      // bool
    Thing_Light_Shadows                 = 106,      // percent, 0 to 100
    Thing_Light_Pulse                   = 107,      // double
    Thing_Light_Pulse_Speed             = 108,      // double

    // Mirror Settings
    Thing_Mirror_Start_Color            = 109,      // color
    Thing_Mirror_End_Color              = 110,      // color
    Thing_Mirror_Color_Tint             = 111,      // percent
    Thing_Mirror_Blur                   = 112,      // double
    Thing_Mirror_Blur_Stretch           = 113,      // double
    Thing_Mirror_Scale                  = 114,      // ranged double, 0.001 to no limit

    // Swirl Settings
    Thing_Swirl_Start_Color             = 115,      // color
    Thing_Swirl_Color_Tint              = 116,      // percent
    Thing_Swirl_Angle                   = 117,      // double

    // Water Settings
    Thing_Water_Texture                 = 118,      // list, enum class Water_Texture
    Thing_Water_Start_Color             = 119,      // color
    Thing_Water_End_Color               = 120,      // color
    Thing_Water_Color_Tint              = 121,      // percent
    Thing_Water_Reflection              = 122,      // percent
    Thing_Water_Movement_Speed          = 123,      // double
    Thing_Water_Ripple_Frequency        = 124,      // double
    Thing_Water_Ripple_Speed            = 125,      // double
    Thing_Water_Ripple_Amplitude        = 126,      // double
    Thing_Water_Ripple_Stretch          = 127,      // double
    Thing_Water_Wave_Frequency          = 128,      // double
    Thing_Water_Wave_Speed              = 129,      // double
    Thing_Water_Wave_Amplitude          = 130,      // double

    // Water Refract Settings
    Thing_Water_Refract_Reflection      = 131,      // percent
    Thing_Water_Refract_Underwater      = 132,      // percent
    Thing_Water_Refract_Texture         = 133,      // percent
    Thing_Water_Refract_Foam            = 134,      // percent

    // Water Foam Settings
    Thing_Water_Surface_Color           = 135,      // color
    Thing_Water_Surface_Tint            = 136,      // percent
    Thing_Water_Surface_Height          = 137,      // double
    Thing_Water_Surface_Is_Flat         = 138,      // bool

};



#endif // ENUMS_DR_SETTINGS_H


















