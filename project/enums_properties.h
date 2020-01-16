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
//##    Used to track what the DrVariant m_value data type really is
//##        Numbers are SAVE FILE CRITICAL and CANNOT BE CHANGED once set
//##
//##    !!!!! NEXT AVAILABLE NUMBER:    32
//##
//############################
enum class Property_Type {

    Bool            =  1,   // bool                 True or False
    BoolDouble      =  2,   // vector<variant>      True or False, when True shows double spinbox
                            //                          6 values: bool, double value, min, max, double step size, string spinText
    BoolInt         =  3,   // vector<variant>      True or False, when True shows int spinbox
                            //                          6 values: bool, int value,    min, max, int step size,    string spinText
    BoolEnabled     =  4,   // vector<variant>      True or False, when True shows additional properties
                            //                          2 values: bool for enabled or not, and a std::vector<DrVariant(int)> (of Properties) that are included

    Int             =  5,   // long                 Integer
    Positive        =  6,   // long                 Integer >= 0
    RangedInt       =  7,   // vector<variant>      Integer >= min and <= max
                            //                          4 values: long, long min value, long max value, long step size

    Double          =  8,   // double               Decimal
    Angle           =  9,   // double               Decimal for showing degrees, shows degree symbol in spinbox
    PositiveDouble  = 10,   // double               Decimal >= 0.0
    RangedDouble    = 11,   // vector<variant>      Decimal >= min and <= max
                            //                          4 values: double, double min value, double max value, double step size
    Slider          = 12,   // vector<variant>      Decimal >= min and <= max, with a slider
                            //                          5 values: double, double min value, double max value, double step size, string suffix ("%", etc)
    Percent         = 13,   // double               Decimal from 0.0 to 100.0, shows percent symbol, with a slider

    String          = 14,   // string               Uses QLineEdit
    Textbox         = 15,   // string               Uses QTextEdit for multi-line html text input

    PointF          = 16,   // DrPointF             Decimal pair x and y
    PositionF       = 17,   // DrPointF             Decimal pair x and y, used for object positions in scene
                            //                          Y is shown flipped (i.e. * -1), Chipmunk uses different coordinate system than Qt
                            //                          Y flipped in: createDoubleSpinBoxPair(), updateSettingsFromNewValue(),
                            //                                        updateInspectorPropertyBoxes(), updateToolTipData()
    ScaleF          = 18,   // DrPointF             Decimal pair x and y, has smaller increment step in spin box
    PositiveScaleF  = 19,   // DrPointF             Decimal pair x and y, both numbers are >= 0.0
    SizeF           = 20,   // DrPointF             Decimal pair w and h
    PositiveSizeF   = 21,   // DrPointF             Decimal pair w and h, both numbers are >= 0.0
    OneSizeF        = 22,   // DrPointF             Decimal pair w and h, both numbers are >= 1.0
    Point3D         = 23,   // DrVec3               Decimal point trio, x, y, z
                            //                          3 values: double x, double y, double z

    GridF           = 24,   // DrPointF             Decimal pair x and y, minimum value of c_minimum_grid_size  for both x and y
    GridScaleF      = 25,   // DrPointF             Decimal pair x and y, minimum value of c_minimum_grid_scale for both x and y
    Variable        = 26,   // DrPointF             Decimal point pair, number followed by a +/- number

    List            = 27,   // long                 Index of a particular enum list

    Color           = 28,   // unsigned int         For colors (compatible with DrColor.rgba())
    Image           = 29,   // long                 Index of a particular DrAnimation within the Project

    Collision       = 30,   // DrPropertyCollision  For Collision Shapes


    // !!!!! NOT IMPLEMENTED:
    Icon            = 31,   // -----
};


//####################################################################################
//##    Possible DrComponents for DrSettings
//##        Numbers are SAVE FILE CRITICAL and CANNOT BE CHANGED once set
//##
//##    !!!!! NEXT AVAILABLE NUMBER:                    44,
//##    !!!!! MUST ALSO CHANGE:     Components::Total = 44 !!!!!
//##
//############################
enum class Components {

    // ***** General Components
    Entity_Settings                     = 0,
    Local_Variables                     = 1,
    Hidden_Settings                     = 2,
    Size_Settings                       = 3,

    // ***** DrAnimation Components
    Animation_Settings                  = 4,

    // ***** DrFrame Components
    Frame_Settings                      = 5,

    // ***** DrAsset Components
    Asset_Collision                     = 6,
    Asset_Animation                     = 7,
    Asset_Health                        = 8,
    Asset_Physics                       = 9,
    Asset_Controls                      = 10,

    Asset_Settings_Character            = 11,
    Asset_Settings_Object               = 12,
    Asset_Settings_Font                 = 13,

    // ***** DrWorld Components
    World_Settings                      = 14,
    World_Physics                       = 15,
    World_Lighting                      = 16,
    World_Camera                        = 17,
    World_Appearance                    = 18,
    World_Special_Effects               = 19,

    // ***** DrStage Components
    Stage_Settings                      = 20,
    Stage_Grid                          = 21,

    // ***** DrThing Components
    Thing_Transform                     = 22,
    Thing_Layering                      = 23,
    Thing_Movement                      = 24,
    Thing_Spawn                         = 25,
    Thing_Lighting                      = 26,
    Thing_3D                            = 27,
    Thing_Appearance                    = 28,
    Thing_Special_Effects               = 29,

    Thing_Settings_Camera               = 30,
    Thing_Settings_Character            = 31,
    Thing_Settings_Object               = 32,
    Thing_Settings_Text                 = 33,

    Thing_Settings_Fire                 = 34,
    Thing_Settings_Fisheye              = 35,
    Thing_Settings_Light                = 36,
    Thing_Settings_Mirror               = 37,
    Thing_Settings_Swirl                = 38,
    Thing_Settings_Water                = 39,
    Thing_Settings_Water_Ripple         = 40,
    Thing_Settings_Water_Wave           = 41,
    Thing_Settings_Water_Refract        = 42,
    Thing_Settings_Water_Foam           = 43,

    Total = 44,
};


//####################################################################################
//##    Possible DrProperties of DrComponents
//##        Numbers are SAVE FILE CRITICAL and CANNOT BE CHANGED once set
//##
//##    !!!!! NEXT AVAILABLE NUMBER:   243
//##
//##    Checked for doubles up through 243 on 01/14/2020
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
    Size_Keep_Square                    = 6,        // bool
    Size_Max_Size                       = 7,        // pointf
    Size_Min_Size                       = 8,        // pointf


    // ******************** DrAnimation Components
    // Animation_Settings
    Animation_Settings_Speed            = 9,        // double
    Animation_Settings_Loops            = 10,       // int
    Animation_Settings_Start_Frame      = 11,       // positive int
    Animation_Settings_Future_1         = 12,
    Animation_Settings_Future_2         = 13,
    Animation_Settings_Future_3         = 14,



    // ******************** DrAsset Component Properties
    // Asset_Settings_Character
    Asset_Character_Max_Speed           = 15,       // pointf
    Asset_Character_Forced_Speed        = 16,       // pointf
    Asset_Character_Move_Speed          = 17,       // pointf
    Asset_Character_Angle_Movement      = 18,       // bool
    Asset_Character_Jump_Force          = 19,       // pointf
    Asset_Character_Jump_Timeout        = 20,       // positive int
    Asset_Character_Jump_Counter        = 21,       // int

    Asset_Character_Jump_Air            = 22,       // bool
    Asset_Character_Jump_Wall           = 23,       // bool

    Asset_Character_Acceleration        = 24,       // positive double
    Asset_Character_Air_Drag            = 25,       // positive double
    Asset_Character_Ground_Drag         = 26,       // positive double
    Asset_Character_Rotation_Drag       = 27,       // positive double

    Asset_Character_Flip_Image_X        = 28,       // bool
    Asset_Character_Flip_Image_Y        = 29,       // bool
    Asset_Character_Mouse_Rotate        = 30,       // bool

    // Asset_Settings_Object


    // Asset_Settings_Font
    Asset_Font_Family                   = 31,
    Asset_Font_Size                     = 32,

    // Asset_Collision
    Asset_Collision_Shape               = 33,       // list, enum class Collision_Shape (image shape, circle, square, triangle)
    Asset_Collision_One_Way_Type        = 34,       // list, enum class One_Way (none, pass_through, weak_spot)
    Asset_Collision_One_Way_Direction   = 35,       // point
    Asset_Collision_Gravity_Multiplier  = 36,       // double
    Asset_Collision_Image_Shape         = 37,       // DrPropertyCollision (polygons)
    Asset_Collision_Circles             = 38,       // future use storing circles?
    Asset_Collision_Squares             = 39,       // future use storing squares?
    Asset_Collision_Polygons            = 40,       // future use storing polygons?
    Asset_Collision_Surface_Velocity    = 41,       // future use storing polygons?

    // Asset_Animation
    Asset_Animation_Idle                = 42,       // image
    Asset_Animation_Walking             = 43,       // image
    Asset_Animation_Running             = 44,       // image
    Asset_Animation_Taking_Damage       = 45,       // image
    Asset_Animation_Death               = 46,       // image
    Asset_Animation_Invincible          = 47,       // image
    Asset_Animation_Shooting_A          = 48,       // image
    Asset_Animation_Shooting_B          = 49,       // image
    Asset_Animation_Jumping             = 50,       // image
    Asset_Animation_Sliding             = 51,       // image
    Asset_Animation_Dashing             = 52,       // image
    Asset_Animation_Crouching           = 53,       // image
    Asset_Animation_Future_01           = 54,       // image
    Asset_Animation_Future_02           = 55,       // image
    Asset_Animation_Future_03           = 56,       // image
    Asset_Animation_Future_04           = 57,       // image
    Asset_Animation_Future_05           = 58,       // image

    // Asset_Health
    Asset_Health_Max_Health             = 59,       // double
    Asset_Health_Health                 = 60,       // double
    Asset_Health_Damage                 = 61,       // double
    Asset_Health_Damage_Delay           = 62,       // int
    Asset_Health_Auto_Damage            = 63,       // double
    Asset_Health_Death_Delay            = 64,       // int
    Asset_Health_Fade_On_Death          = 65,       // bool
    Asset_Health_Death_Animation        = 66,       // list, enum class Death_Animation (none, fade, shrink)
    Asset_Health_Death_Duration         = 67,       // int (used to be called Asset_Health_Fade_Delay)
    Asset_Health_Damage_Recoil          = 68,       // double
    Asset_Health_Invincible             = 69,       // bool
    Asset_Health_Death_Touch            = 70,       // bool

    // Asset_Physics
    Asset_Physics_Gravity_Scale         = 71,       // bool
    Asset_Physics_Custom_Friction       = 72,       // bool double combo
    Asset_Physics_Custom_Bounce         = 73,       // bool double combo
    Asset_Physics_Can_Rotate            = 74,       // bool

    // Asset_Controls
    Asset_Controls_Touch_Damage         = 75,       // bool double
    Asset_Controls_Touch_Drag           = 76,       // bool

    Asset_Slingshot_Enabled             = 77,       // bool enabled
    Asset_Slingshot_Arrow_Image         = 78,           // SUB: image
    Asset_Slingshot_Arrow_Offset        = 79,           // SUB: positive double
    Asset_Slingshot_Arrow_Scale         = 80,           // SUB: positive scalef
    Asset_Slingshot_Tail_Image          = 81,           // SUB: image
    Asset_Slingshot_Tail_Offset         = 82,           // SUB: positive double
    Asset_Slingshot_Tail_Scale          = 83,           // SUB: positive scalef
    Asset_Slingshot_Max_Distance        = 84,           // SUB: positive double
    Asset_Slingshot_Force_Multiplier    = 85,           // SUB: double
    Asset_Slingshot_Future_1            = 86,           // SUB: future
    Asset_Slingshot_Future_2            = 87,           // SUB: future
    Asset_Slingshot_Future_3            = 88,           // SUB: future

    Asset_Controls_Rotate_Speed         = 89,       // double


    // ******************** DrWorld Component Properties
    // Settings
    World_Game_Direction                = 90,       // double
    World_Score_Multiplier              = 91,       // double
    World_Deletion_Threshold            = 92,       // positive int
    World_Use_Background_Color          = 93,       // bool enabled
    World_Background_Color              = 94,           // SUB: color

    // Physics
    World_Gravity                       = 95,       // pointf (x, y)
    World_Time_Warp                     = 96,       // double
    World_Friction                      = 97,       // double
    World_Drag                          = 98,       // double
    World_Bounce                        = 99,       // double

    // Lighting
    World_Light_Ambient                 = 100,      // double
    World_Light_Layer                   = 101,      // percent
    World_Light_Blend                   = 102,      // list, enum class Blend_Mode

    // Camera
    World_Camera_Type                   = 103,      // list, enum class Render_Type
    World_Camera_Slop                   = 104,      // percent
    World_Camera_Switch_Speed           = 105,      // positive double

    World_Camera_Zoom_Enabled           = 240,      // bool enabled
    World_Camera_Zoom_In_Or_Out         = 241,          // SUB: list
    World_Camera_Zoom_Multiplier        = 242,          // SUB: ranged double, 0 to 100
    World_Camera_Zoom_Damping           = 243,          // SUB: ranged double, 0 to 100


    // Appearance
    World_Filter_Bitrate                = 106,      // ranged int, 0 to 256
    World_Filter_Pixelation             = 107,      // positive size
    World_Filter_Brightness             = 108,      // slider
    World_Filter_Contrast               = 109,      // slider
    World_Filter_Saturation             = 110,      // slider
    World_Filter_Hue                    = 111,      // slider
    World_Filter_Grayscale              = 112,      // bool
    World_Filter_Negative               = 113,      // bool

    // Special Effects
    World_Filter_Convert_3D             = 114,      // bool
    World_Filter_Wireframe              = 115,      // bool double combo
    World_Filter_Cartoon                = 116,      // bool double combo
    World_Filter_Cross_Hatch            = 117,      // bool double combo
    World_Filter_Wavy                   = 118,      // bool


    // ******************** DrStage Component Properties
    // Settings
    Stage_Enabled                       = 119,      // bool
    Stage_Start                         = 120,      // positive
    Stage_End                           = 121,      // positive
    Stage_Size                          = 122,      // positive
    Stage_Cooldown                      = 123,      // positive

    // Grid
    Stage_Grid_Style                    = 124,      // list, enum class Grid_Style (Lines,  Dots)
    Stage_Grid_Origin_Point             = 125,      // pointf
    Stage_Grid_Size                     = 126,      // sizef
    Stage_Grid_Scale                    = 127,      // scale
    Stage_Grid_Rotation                 = 128,      // angle
    Stage_Grid_Color                    = 129,      // color


    // ******************** DrThing Component Properties
    // Transform
    Thing_Position                      = 130,      // positionf
    Thing_Rotation                      = 131,      // angle
    Thing_Size                          = 132,      // sizef
    Thing_Scale                         = 133,      // scale

    // Layering
    Thing_Z_Order                       = 134,      // double
    Thing_Sub_Z_Order                   = 135,      // int
    Thing_Opacity                       = 136,      // percent

    // Movement
    Thing_Velocity_X                    = 137,      // variable
    Thing_Velocity_Y                    = 138,      // variable
    Thing_Spin_Velocity                 = 139,      // variable
    Thing_Angle_Velocity                = 140,      // bool
    Thing_Angle_Player                  = 141,      // bool

    // Spawn
    Thing_Spawn_Rate                    = 142,      // variable
    Thing_Spawn_Count                   = 143,      // int
    Thing_Spawn_Type                    = 144,      // list, enum class Spawn_Type
    Thing_Spawn_At_Object               = 145,      // bool int combo
    Thing_Spawn_Offset_X                = 146,      // variable
    Thing_Spawn_Offset_Y                = 147,      // variable
    Thing_Spawn_Instantly               = 148,      // bool
    Thing_Spawn_Chance                  = 149,      // percent

    // Lighting
    Thing_Lighting_Cast_Shadows         = 150,      // bool

    // Appearance
    Thing_Filter_Bitrate                = 151,      // ranged int, 0 to 256
    Thing_Filter_Pixelation             = 152,      // positive size
    Thing_Filter_Brightness             = 153,      // slider
    Thing_Filter_Contrast               = 154,      // slider
    Thing_Filter_Saturation             = 155,      // slider
    Thing_Filter_Hue                    = 156,      // slider, 0 to 360
    Thing_Filter_Grayscale              = 157,      // bool
    Thing_Filter_Negative               = 158,      // bool

    // Special Effects
    Thing_Filter_Convert_3D             = 159,      // bool
    Thing_Filter_Wireframe              = 160,      // bool double combo
    Thing_Filter_Cartoon                = 161,      // bool double combo
    Thing_Filter_Cross_Hatch            = 162,      // bool double combo

    // 3D Variables
    Thing_3D_Type                       = 163,      // list, enum class Convert_3D_Type (extrusion, cube, cone, none)
    Thing_3D_Depth                      = 164,      // positive double
    Thing_3D_X_Axis_Rotation            = 165,      // variable
    Thing_3D_Y_Axis_Rotation            = 166,      // variable
    Thing_3D_X_Axis_Speed               = 167,      // variable
    Thing_3D_Y_Axis_Speed               = 168,      // variable
    Thing_3D_Billboard                  = 169,      // bool


    // ********** Thing Specific
    // Camera Settings
    Thing_Camera_Set_As_Active          = 170,      // bool
    Thing_Camera_Speed                  = 171,      // pointf
    Thing_Camera_Rotation               = 172,      // pointf
    Thing_Camera_Up_Vector              = 173,      // list, enum class Up_Vector
    Thing_Camera_Distance               = 174,      // double
    Thing_Camera_Zoom                   = 175,      // double

    // Character Settings
    Thing_Character_Camera_Position     = 176,      // pointf
    Thing_Character_Camera_Rotation     = 177,      // pointf
    Thing_Character_Camera_Tilt         = 178,      // double
    Thing_Character_Camera_Up_Vector    = 179,      // list, enum class Up_Vector
    Thing_Character_Camera_Distance     = 180,      // double
    Thing_Character_Camera_Zoom         = 181,      // double
    Thing_Character_Camera_Lag          = 182,      // double
    Thing_Character_Camera_Match_Angle  = 183,      // bool

    // Object Settings
    Thing_Object_Physics_Type           = 184,      // list, enum class Body_Type (Static, Kinematic, Dynamic)
    Thing_Object_Collide                = 185,      // bool
    Thing_Object_Collision_Group        = 186,      // list, enum class Collision_Group (none, all, active players, players, enemies)
    Thing_Object_Damage                 = 187,      // list, enum class Collision_Type (none, player, enemy, all)

    // Text Settings
    Thing_Text_User_Text                = 188,      // string


    // ********** Effect Specific
    // Fire Settings
    Thing_Fire_Shape                    = 189,      // list, enum class Fire_Mask
    Thing_Fire_Color_1                  = 190,      // color
    Thing_Fire_Color_2                  = 191,      // color
    Thing_Fire_Color_Smoke              = 192,      // color
    Thing_Fire_Intensity                = 193,      // percent
    Thing_Fire_Smoothness               = 194,      // percent
    Thing_Fire_Wavy                     = 195,      // percent
    Thing_Fire_Speed                    = 196,      // ranged double, 1.0 to ?

    // Fisheye Settings
    Thing_Fisheye_Color                 = 197,      // color
    Thing_Fisheye_Color_Tint            = 198,      // percent
    Thing_Fisheye_Lens_Zoom             = 199,      // ranged double, 0 to 10

    // Light Settings
    Thing_Light_Type                    = 200,      // list, enum class Light_Type (Opaque, Glow)
    Thing_Light_Color                   = 201,      // color
    Thing_Light_Cone_Start              = 202,      // slider, 0 to 360
    Thing_Light_Cone_End                = 203,      // slider, 0 to 360
    Thing_Light_Intensity               = 204,      // percent, 0 to 100
    Thing_Light_Blur                    = 205,      // percent, 0 to 100
    Thing_Light_Draw_Shadows            = 206,      // bool
    Thing_Light_Shadows                 = 207,      // percent, 0 to 100
    Thing_Light_Pulse                   = 208,      // double
    Thing_Light_Pulse_Speed             = 209,      // double

    // Mirror Settings
    Thing_Mirror_Start_Color            = 210,      // color
    Thing_Mirror_End_Color              = 211,      // color
    Thing_Mirror_Color_Tint             = 212,      // percent
    Thing_Mirror_Blur                   = 213,      // double
    Thing_Mirror_Blur_Stretch           = 214,      // double
    Thing_Mirror_Scale                  = 215,      // ranged double, 0.001 to no limit

    // Swirl Settings
    Thing_Swirl_Start_Color             = 216,      // color
    Thing_Swirl_Color_Tint              = 217,      // percent
    Thing_Swirl_Angle                   = 218,      // double

    // Water Settings
    Thing_Water_Texture                 = 219,      // list, enum class Water_Texture
    Thing_Water_Start_Color             = 220,      // color
    Thing_Water_End_Color               = 221,      // color
    Thing_Water_Color_Tint              = 222,      // percent
    Thing_Water_Reflection              = 223,      // percent
    Thing_Water_Movement_Speed          = 224,      // double
    Thing_Water_Ripple_Frequency        = 225,      // double
    Thing_Water_Ripple_Speed            = 226,      // double
    Thing_Water_Ripple_Amplitude        = 227,      // double
    Thing_Water_Ripple_Stretch          = 228,      // double
    Thing_Water_Wave_Frequency          = 229,      // double
    Thing_Water_Wave_Speed              = 230,      // double
    Thing_Water_Wave_Amplitude          = 231,      // double

    // Water Refract Settings
    Thing_Water_Refract_Reflection      = 232,      // percent
    Thing_Water_Refract_Underwater      = 233,      // percent
    Thing_Water_Refract_Texture         = 234,      // percent
    Thing_Water_Refract_Foam            = 235,      // percent

    // Water Foam Settings
    Thing_Water_Surface_Color           = 236,      // color
    Thing_Water_Surface_Tint            = 237,      // percent
    Thing_Water_Surface_Height          = 238,      // double
    Thing_Water_Surface_Is_Flat         = 239,      // bool

};


#endif // ENUMS_DR_SETTINGS_H











