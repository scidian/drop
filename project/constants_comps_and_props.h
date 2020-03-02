//
//      Created by Stephens Nunnally on 6/25/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef COMPONENTS_AND_PROPERTIES_H
#define COMPONENTS_AND_PROPERTIES_H

#include <map>
#include <string>

// Type Definitions
typedef std::pair<std::string, std::string>     ComponentProperty;      // Component / Property key set, first string is Component, second is Property


//####################################################################################
//##    Used to track what the DrVariant m_value data type really is
//##        Numbers are SAVE FILE CRITICAL and CANNOT BE CHANGED once set
//##
//##    !!!!! NEXT AVAILABLE NUMBER:    33
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
    Array           = 28,   // vector<variant>      List of DrVariants, some parts NOT IMPLEMENTED !!!!!

    Color           = 29,   // unsigned int         For colors (compatible with DrColor.rgba())
    Image           = 30,   // long                 Index of a particular DrAnimation within the Project

    Collision       = 31,   // DrPropertyCollision  For Collision Shapes
    Icon            = 32,   // -----                NOT IMPLEMENTED !!!!!
};


//####################################################################################
//##
//##    Possible DrComponents for DrSettings
//##
//############################
namespace Comps {

    // TWO LETTER CODES
    //  En = Entity
    //  Lo = Local
    //  Hi = Hidden
    //  Si = Size

    //  An = Animation
    //  Fr = Frame
    //  As = Asset
    //  Wo = World
    //  St = Stage
    //  Th = Thing

    // ***** General Components
    const std::string Entity_Settings                   = "EnSe";
    const std::string Local_Variables                   = "LoVariables";
    const std::string Hidden_Settings                   = "HiSe";
    const std::string Size_Settings                     = "SiSe";

    // ***** DrAnimation Components
    const std::string Animation_Settings                = "AnSe";

    // ***** DrFrame Components
    const std::string Frame_Settings                    = "FrSe";

    // ***** DrAsset Components
    const std::string Asset_Collision                   = "AsCollision";
    const std::string Asset_Animation                   = "AsAnimation";
    const std::string Asset_Health                      = "AsHealth";
    const std::string Asset_Physics                     = "AsPhysics";
    const std::string Asset_Controls                    = "AsControls";

    const std::string Asset_Settings_Character          = "AsSeCharacter";
    const std::string Asset_Settings_Object             = "AsSeObject";
    const std::string Asset_Settings_Font               = "AsSeFont";

    // ***** DrWorld Components
    const std::string World_Settings                    = "WoSe";
    const std::string World_Physics                     = "WoPhysics";
    const std::string World_Lighting                    = "WoLighting";
    const std::string World_Camera                      = "WoCamera";
    const std::string World_Appearance                  = "WoAppearance";
    const std::string World_Special_Effects             = "WoSpecialEffects";

    // ***** DrStage Components
    const std::string Stage_Settings                    = "StSe";
    const std::string Stage_Grid                        = "StGrid";

    // ***** DrThing Components
    const std::string Thing_Transform                   = "Transform";
    const std::string Thing_Layering                    = "Layering";
    const std::string Thing_Movement                    = "Movement";
    const std::string Thing_Spawn                       = "Spawn";
    const std::string Thing_Lighting                    = "Lighting";
    const std::string Thing_3D                          = "3D";
    const std::string Thing_Appearance                  = "Appearance";
    const std::string Thing_Special_Effects             = "SpecialEffects";

    const std::string Thing_Player                      = "Player";
    const std::string Thing_Soft_Body                   = "SoftBody";

    const std::string Thing_Settings_Camera             = "Camera";
    const std::string Thing_Settings_Character          = "Character";
    const std::string Thing_Settings_Object             = "Object";
    const std::string Thing_Settings_Text               = "Text";

    const std::string Thing_Settings_Fire               = "Fire";
    const std::string Thing_Settings_Fisheye            = "Fisheye";
    const std::string Thing_Settings_Light              = "Light";
    const std::string Thing_Settings_Mirror             = "Mirror";
    const std::string Thing_Settings_Swirl              = "Swirl";
    const std::string Thing_Settings_Water              = "Water";
    const std::string Thing_Settings_Water_Ripple       = "WaterRipple";
    const std::string Thing_Settings_Water_Wave         = "WaterWave";
    const std::string Thing_Settings_Water_Refract      = "WaterRefract";
    const std::string Thing_Settings_Water_Foam         = "WaterFoam";
};


//####################################################################################
//##
//##    Possible DrProperties of DrComponents
//##
//############################
namespace Props {

    // TWO LETTER CODES
    //  En = Entity
    //  Lo = Local
    //  Hi = Hidden
    //  Si = Size

    //  An = Animation
    //  Fr = Frame
    //  As = Asset
    //  Wo = World
    //  St = Stage
    //  Th = Thing

    // ******************** General Entity Properties
    const std::string Entity_Name                           = "EnName";                     // string
    const std::string Entity_Key                            = "EnKey";                      // int
    const std::string Entity_Asset_Key                      = "EnAssetKey";                 // int

    // ******************** Hidden Properties, for Droplets
    const std::string Hidden_Advisor_Description            = "HiAdvisorDescription";       // string, html capable textbox
    const std::string Hidden_Item_Locked                    = "HiItemLocked";               // bool
    const std::string Hidden_Hide_From_Trees                = "HiHideFromTrees";            // bool

    // ******************** Size Properties, for locking shape / size in editor
    const std::string Size_Keep_Square                      = "SiKeepSquare";               // bool
    const std::string Size_Max_Size                         = "SiMaxSize";                  // pointf
    const std::string Size_Min_Size                         = "SiMinSize";                  // pointf


    // ******************** DrAnimation Components
    // Animation_Settings
    const std::string Animation_Settings_Speed              = "AnSeSpeed";                  // double
    const std::string Animation_Settings_Loops              = "AnSeLoops";                  // int
    const std::string Animation_Settings_Start_Frame        = "AnSeStartFrame";             // positive int
    const std::string Animation_Settings_Future_1           = "AnSeFuture1";
    const std::string Animation_Settings_Future_2           = "AnSeFuture2";
    const std::string Animation_Settings_Future_3           = "AnSeFuture3";



    // ******************** DrAsset Component Properties
    // Asset_Settings_Character
    const std::string Asset_Character_Max_Speed             = "AsChMaxSpeed";               // pointf
    const std::string Asset_Character_Forced_Speed          = "AsChForcedSpeed";            // pointf
    const std::string Asset_Character_Move_Speed            = "AsChMoveSpeed";              // pointf
    const std::string Asset_Character_Angle_Movement        = "AsChAngleMovement";          // bool
    const std::string Asset_Character_Jump_Force            = "AsChJumpForce";              // pointf
    const std::string Asset_Character_Jump_Timeout          = "AsChJumpTimeout";            // positive int
    const std::string Asset_Character_Jump_Counter          = "AsChJumpCounter";            // int

    const std::string Asset_Character_Jump_Air              = "AsChJumpAir";                // bool
    const std::string Asset_Character_Jump_Wall             = "AsChJumpWall";               // bool

    const std::string Asset_Character_Acceleration          = "AsChAcceleration";           // positive double
    const std::string Asset_Character_Air_Drag              = "AsChAir_Drag";               // positive double
    const std::string Asset_Character_Ground_Drag           = "AsChGround_Drag";            // positive double
    const std::string Asset_Character_Rotation_Drag         = "AsChRotation_Drag";          // positive double

    const std::string Asset_Character_Flip_Image_X          = "AsChX_Flip_Image";           // bool
    const std::string Asset_Character_Flip_Image_Y          = "AsChY_Flip_Image";           // bool
    const std::string Asset_Character_Mouse_Rotate          = "AsChMouse_Rotate";           // bool

    // Asset_Settings_Object


    // Asset_Settings_Font
    const std::string Asset_Font_Family                     = "AsFontFamily";
    const std::string Asset_Font_Size                       = "AsFontSize";

    // Asset_Collision
    const std::string Asset_Collision_Shape                 = "AsCoShape";                  // list, enum class Collision_Shape (Image_Shape, Circle, Square, Triangle)
    const std::string Asset_Collision_One_Way_Type          = "AsCoOneWayType";             // list, enum class One_Way (None, Pass_Through, Weak_Spot)
    const std::string Asset_Collision_One_Way_Direction     = "AsCoOneWayDirection";        // point
    const std::string Asset_Collision_Gravity_Multiplier    = "AsCoGravityMultiplier";      // double
    const std::string Asset_Collision_Image_Shape           = "AsCoImageShape";             // DrPropertyCollision (polygons)
    const std::string Asset_Collision_Circles               = "AsCoCircles";                // future use storing circles?
    const std::string Asset_Collision_Squares               = "AsCoSquares";                // future use storing squares?
    const std::string Asset_Collision_Polygons              = "AsCoPolygons";               // future use storing polygons?
    const std::string Asset_Collision_Surface_Velocity      = "AsCoSurfaceVelocity";        // future use storing polygons?

    // Asset_Animation
    const std::string Asset_Animation_Idle                  = "AsAnIdle";                   // image
    const std::string Asset_Animation_Walking               = "AsAnWalking";                // image
    const std::string Asset_Animation_Running               = "AsAnRunning";                // image
    const std::string Asset_Animation_Taking_Damage         = "AsAnTakingDamage";           // image
    const std::string Asset_Animation_Death                 = "AsAnDeath";                  // image
    const std::string Asset_Animation_Invincible            = "AsAnInvincible";             // image
    const std::string Asset_Animation_Shooting_A            = "AsAnAShoot";                 // image
    const std::string Asset_Animation_Shooting_B            = "AsAnBShoot";                 // image
    const std::string Asset_Animation_Jumping               = "AsAnJumping";                // image
    const std::string Asset_Animation_Sliding               = "AsAnSliding";                // image
    const std::string Asset_Animation_Dashing               = "AsAnDashing";                // image
    const std::string Asset_Animation_Crouching             = "AsAnCrouching";              // image
    const std::string Asset_Animation_Future_01             = "AsAnFuture1";                // image
    const std::string Asset_Animation_Future_02             = "AsAnFuture2";                // image
    const std::string Asset_Animation_Future_03             = "AsAnFuture3";                // image
    const std::string Asset_Animation_Future_04             = "AsAnFuture4";                // image
    const std::string Asset_Animation_Future_05             = "AsAnFuture5";                // image

    // Asset_Health
    const std::string Asset_Health_Max_Health               = "AsHeMaxHealth";              // double
    const std::string Asset_Health_Health                   = "AsHeHealth";                 // double
    const std::string Asset_Health_Damage                   = "AsHeDamage";                 // double
    const std::string Asset_Health_Damage_Delay             = "AsHeDamageDelay";            // int
    const std::string Asset_Health_Auto_Damage              = "AsHeAutoDamage";             // double
    const std::string Asset_Health_Death_Delay              = "AsHeDeathDelay";             // int
    const std::string Asset_Health_Fade_On_Death            = "AsHeFadeOnDeath";            // bool
    const std::string Asset_Health_Death_Animation          = "AsHeDeathAnimation";         // list, enum class Death_Animation (None, Fade, Shrink)
    const std::string Asset_Health_Death_Duration           = "AsHeDeathDuration";          // int (used to be called Asset_Health_Fade_Delay)
    const std::string Asset_Health_Damage_Recoil            = "AsHeDamageRecoil";           // double
    const std::string Asset_Health_Invincible               = "AsHeInvincible";             // bool
    const std::string Asset_Health_Death_Touch              = "AsHeDeathTouch";             // bool

    // Asset_Physics
    const std::string Asset_Physics_Body_Style              = "AsPhBodyStyle";              // list, enum class Body_Style (Rigid, Blob, Foliage, etc)
    const std::string Asset_Physics_Body_Rigidness          = "AsPhBodyRigidness";          // percent, 0 == gooey, 1 == rigid
    const std::string Asset_Physics_Gravity_Scale           = "AsPhGravityScale";           // bool
    const std::string Asset_Physics_Custom_Friction         = "AsPhFrictionCustom";         // bool double combo
    const std::string Asset_Physics_Custom_Bounce           = "AsPhBounceCustom";           // bool double combo
    const std::string Asset_Physics_Can_Rotate              = "AsPhCanRotate";              // bool

    // Asset_Controls
    const std::string Asset_Controls_Touch_Damage           = "AsConTouchDamage";           // bool double
    const std::string Asset_Controls_Touch_Drag             = "AsConTouchDrag";             // bool

    const std::string Asset_Slingshot_Enabled               = "AsSliEnabled";               // bool enabled
    const std::string Asset_Slingshot_Arrow_Image           = "AsSliArrowImage";            // SUB: image
    const std::string Asset_Slingshot_Arrow_Offset          = "AsSliArrowOffset";           // SUB: positive double
    const std::string Asset_Slingshot_Arrow_Scale           = "AsSliArrowScale";            // SUB: positive scalef
    const std::string Asset_Slingshot_Tail_Image            = "AsSliTailImage";             // SUB: image
    const std::string Asset_Slingshot_Tail_Offset           = "AsSliTailOffset";            // SUB: positive double
    const std::string Asset_Slingshot_Tail_Scale            = "AsSliTailScale";             // SUB: positive scalef
    const std::string Asset_Slingshot_Max_Distance          = "AsSliMaxDistance";           // SUB: positive double
    const std::string Asset_Slingshot_Force_Multiplier      = "AsSliForceMultiplier";       // SUB: double

    const std::string Asset_Controls_Rotate_Speed           = "AsConRotateSpeed";           // double


    // ******************** DrWorld Component Properties
    // Settings
    const std::string World_Game_Direction                  = "WoGameDirection";            // double
    const std::string World_Score_Multiplier                = "WoScoreMultiplier";          // double
    const std::string World_Deletion_Threshold              = "WoDeletionThreshold";        // positive int
    const std::string World_Use_Background_Color            = "WoUseBackGroundColor";       // bool enabled
    const std::string World_Background_Color                = "WoBackgroundColor";          // SUB: color

    // Physics
    const std::string World_Gravity                         = "WoGravity";                  // pointf (x, y)
    const std::string World_Time_Warp                       = "WoTimeWarp";                 // double
    const std::string World_Friction                        = "WoFriction";                 // double
    const std::string World_Drag                            = "WoDrag";                     // double
    const std::string World_Bounce                          = "WoBounce";                   // double

    // Lighting
    const std::string World_Light_Ambient                   = "WoLiAmbient";                // double
    const std::string World_Light_Layer                     = "WoLiLayer";                  // percent
    const std::string World_Light_Blend                     = "WoLiBlend";                  // list, enum class Blend_Mode

    // Camera
    const std::string World_Camera_Type                     = "WoCamType";                  // list, enum class Render_Type
    const std::string World_Camera_Slop                     = "WoCamSlop";                  // percent
    const std::string World_Camera_Switch_Speed             = "WoCamSwitchSpeed";           // positive double

    const std::string World_Camera_Zoom_Enabled             = "WoCamZoomEnabled";           // bool enabled
    const std::string World_Camera_Zoom_In_Or_Out           = "WoCamZoomInOrOut";           // SUB: list
    const std::string World_Camera_Zoom_Multiplier          = "WoCamZoomMultiplier";        // SUB: ranged double, 0 to 100
    const std::string World_Camera_Zoom_Damping             = "WoCamZoomDamping";           // SUB: ranged double, 0 to 100


    // Appearance
    const std::string World_Filter_Bitrate                  = "WoFiBitrate";                // ranged int, 0 to 256
    const std::string World_Filter_Pixelation               = "WoFiPixelation";             // positive size
    const std::string World_Filter_PixelType                = "WoFiPixelType";              // list, enum class Pixel_Texture (Pixel, Knit, Woven, etc)
    const std::string World_Filter_Brightness               = "WoFiBrightness";             // slider
    const std::string World_Filter_Contrast                 = "WoFiContrast";               // slider
    const std::string World_Filter_Saturation               = "WoFiSaturation";             // slider
    const std::string World_Filter_Hue                      = "WoFiHue";                    // slider
    const std::string World_Filter_Grayscale                = "WoFiGrayscale";              // bool
    const std::string World_Filter_Negative                 = "WoFiNegative";               // bool

    // Special Effects
    const std::string World_Filter_Convert_3D               = "WoFiConvert3D";              // bool
    const std::string World_Filter_Wireframe                = "WoFiWireframe";              // bool double combo
    const std::string World_Filter_Cartoon                  = "WoFiCartoon";                // bool double combo
    const std::string World_Filter_Cross_Hatch              = "WoFiCrossHatch";             // bool double combo
    const std::string World_Filter_Wavy                     = "WoFiWavy";                   // bool


    // ******************** DrStage Component Properties
    // Settings
    const std::string Stage_Enabled                         = "StEnabled";                  // bool
    const std::string Stage_Start                           = "StStart";                    // positive
    const std::string Stage_End                             = "StEnd";                      // positive
    const std::string Stage_Size                            = "StSize";                     // positive
    const std::string Stage_Cooldown                        = "StCooldown";                 // positive

    // Grid
    const std::string Stage_Grid_Style                      = "StGrStyle";                  // list, enum class Grid_Style (Lines, Dots)
    const std::string Stage_Grid_Origin_Point               = "StGrOriginPoint";            // pointf
    const std::string Stage_Grid_Size                       = "StGrSize";                   // sizef
    const std::string Stage_Grid_Scale                      = "StGrScale";                  // scale
    const std::string Stage_Grid_Rotation                   = "StGrRotation";               // angle
    const std::string Stage_Grid_Color                      = "StGrColor";                  // color


    // ******************** DrThing Component Properties
    // Transform
    const std::string Thing_Position                        = "ThPosition";                 // positionf
    const std::string Thing_Rotation                        = "ThRotation";                 // angle
    const std::string Thing_Size                            = "ThSize";                     // sizef
    const std::string Thing_Scale                           = "ThScale";                    // scale

    // Layering
    const std::string Thing_Z_Order                         = "ThZOrder";                   // double
    const std::string Thing_Sub_Z_Order                     = "ThSubZOrder";                // int
    const std::string Thing_Opacity                         = "ThOpacity";                  // percent

    // Movement
    const std::string Thing_Velocity_X                      = "ThXVelocity";                // variable
    const std::string Thing_Velocity_Y                      = "ThYVelocity";                // variable
    const std::string Thing_Spin_Velocity                   = "ThSpinVelocity";             // variable
    const std::string Thing_Angle_Velocity                  = "ThAngleVelocity";            // bool
    const std::string Thing_Angle_Player                    = "ThAnglePlayer";              // bool

    // Spawn
    const std::string Thing_Spawn_Rate                      = "ThSpRate";                   // variable
    const std::string Thing_Spawn_Count                     = "ThSpCount";                  // int
    const std::string Thing_Spawn_Type                      = "ThSpType";                   // list, enum class Spawn_Type
    const std::string Thing_Spawn_At_Object                 = "ThSpAtObject";               // bool int combo
    const std::string Thing_Spawn_Offset_X                  = "ThSpXOffset";                // variable
    const std::string Thing_Spawn_Offset_Y                  = "ThSpYOffset";                // variable
    const std::string Thing_Spawn_Instantly                 = "ThSpInstantly";              // bool
    const std::string Thing_Spawn_Chance                    = "ThSpChance";                 // percent

    // Lighting
    const std::string Thing_Lighting_Blend_Object           = "ThLiBlendObject";            // list, enum class Blend_Object (Standard, Add, Sub)
    const std::string Thing_Lighting_Cast_Shadows           = "ThLiCastShadows";            // bool

    // Appearance
    const std::string Thing_Filter_Bitrate                  = "ThFiBitrate";                // ranged int, 0 to 256
    const std::string Thing_Filter_Pixelation               = "ThFiPixelation";             // positive size
    const std::string Thing_Filter_PixelType                = "ThFiPixelType";              // list, enum class Pixel_Texture (Pixel, Knit, Woven, etc)
    const std::string Thing_Filter_Brightness               = "ThFiBrightness";             // slider
    const std::string Thing_Filter_Contrast                 = "ThFiContrast";               // slider
    const std::string Thing_Filter_Saturation               = "ThFiSaturation";             // slider
    const std::string Thing_Filter_Hue                      = "ThFiHue";                    // slider, 0 to 360
    const std::string Thing_Filter_Grayscale                = "ThFiGrayscale";              // bool
    const std::string Thing_Filter_Negative                 = "ThFiNegative";               // bool

    // Special Effects
    const std::string Thing_Filter_Convert_3D               = "ThFiConvert3D";              // bool
    const std::string Thing_Filter_Wireframe                = "ThFiWireframe";              // bool double combo
    const std::string Thing_Filter_Cartoon                  = "ThFiCartoon";                // bool double combo
    const std::string Thing_Filter_Cross_Hatch              = "ThFiCrossHatch";             // bool double combo

    // 3D Variables
    const std::string Thing_3D_Type                         = "Th3DType";                   // list, enum class Convert_3D_Type (None, Extrusion, Cube, Cone)
    const std::string Thing_3D_Depth                        = "Th3DDepth";                  // positive double
    const std::string Thing_3D_X_Axis_Rotation              = "Th3DXAxisRotation";          // variable
    const std::string Thing_3D_Y_Axis_Rotation              = "Th3DYAxisRotation";          // variable
    const std::string Thing_3D_X_Axis_Speed                 = "Th3DXAxisSpeed";             // variable
    const std::string Thing_3D_Y_Axis_Speed                 = "Th3DYAxisApeed";             // variable
    const std::string Thing_3D_Billboard                    = "Th3DBillboard";              // bool


    // ********** Thing Specific
    // Camera Settings
    const std::string Thing_Camera_Set_As_Active            = "ThCamSetAsActive";           // bool
    const std::string Thing_Camera_Speed                    = "ThCamSpeed";                 // pointf
    const std::string Thing_Camera_Rotation                 = "ThCamRotation";              // pointf
    const std::string Thing_Camera_Up_Vector                = "ThCamUpVector";              // list, enum class Up_Vector
    const std::string Thing_Camera_Distance                 = "ThCamDistance";              // double
    const std::string Thing_Camera_Zoom                     = "ThCamZoom";                  // double

    // Character Settings
    const std::string Thing_Character_Camera_Position       = "ThChCamPosition";            // pointf
    const std::string Thing_Character_Camera_Rotation       = "ThChCamRotation";            // pointf
    const std::string Thing_Character_Camera_Tilt           = "ThChCamTilt";                // double
    const std::string Thing_Character_Camera_Up_Vector      = "ThChCamUpVector";            // list, enum class Up_Vector
    const std::string Thing_Character_Camera_Distance       = "ThChCamDistance";            // double
    const std::string Thing_Character_Camera_Zoom           = "ThChCamZoom";                // double
    const std::string Thing_Character_Camera_Lag            = "ThChCamLag";                 // double
    const std::string Thing_Character_Camera_Match_Angle    = "ThChCamMatchAngle";          // bool
    const std::string Thing_Character_Camera_Sides          = "ThChCameraSides";            // array

    // Object Settings
    const std::string Thing_Object_Physics_Type             = "ThObPhysicsType";            // list, enum class Body_Type (Static, Kinematic, Dynamic)
    const std::string Thing_Object_Collide                  = "ThObCollide";                // bool
    const std::string Thing_Object_Collision_Group          = "ThObColliisonGroup";         // list, enum class Collision_Group (None, All, Active_Players, Players, Enemies)
    const std::string Thing_Object_Damage                   = "ThObDamage";                 // list, enum class Collision_Type (None, Player, Enemy, All)

    // Text Settings
    const std::string Thing_Text_User_Text                  = "ThTextUserText";              // string


    // ********** Effect Specific
    // Fire Settings
    const std::string Thing_Fire_Shape                      = "ThFireShape";                // list, enum class Fire_Mask
    const std::string Thing_Fire_Color_1                    = "ThFireColor1";               // color
    const std::string Thing_Fire_Color_2                    = "ThFireColor2";               // color
    const std::string Thing_Fire_Color_Smoke                = "ThFireSmokeColor";           // color
    const std::string Thing_Fire_Intensity                  = "ThFireIntensity";            // percent
    const std::string Thing_Fire_Smoothness                 = "ThFireSmoothness";           // percent
    const std::string Thing_Fire_Wavy                       = "ThFireWavy";                 // percent
    const std::string Thing_Fire_Speed                      = "ThFireSpeed";                // ranged double, 1.0 to ?

    // Fisheye Settings
    const std::string Thing_Fisheye_Color                   = "ThFishColor";                // color
    const std::string Thing_Fisheye_Color_Tint              = "ThFishTintColor";            // percent
    const std::string Thing_Fisheye_Lens_Zoom               = "ThFishLensZoom";             // ranged double, 0 to 10

    // Light Settings
    const std::string Thing_Light_Type                      = "ThLiType";                   // list, enum class Light_Type (Opaque, Glow)
    const std::string Thing_Light_Color                     = "ThLiColor";                  // color
    const std::string Thing_Light_Cone_Start                = "ThLiConeStart";              // slider, 0 to 360
    const std::string Thing_Light_Cone_End                  = "ThLiConeEnd";                // slider, 0 to 360
    const std::string Thing_Light_Intensity                 = "ThLiIntensity";              // percent, 0 to 100
    const std::string Thing_Light_Blur                      = "ThLiBlur";                   // percent, 0 to 100
    const std::string Thing_Light_Draw_Shadows              = "ThLiDrawShadows";            // bool
    const std::string Thing_Light_Shadows                   = "ThLiShadows";                // percent, 0 to 100
    const std::string Thing_Light_Pulse                     = "ThLiPulse";                  // double
    const std::string Thing_Light_Pulse_Speed               = "ThLiPulseSpeed";             // double

    // Mirror Settings
    const std::string Thing_Mirror_Start_Color              = "ThMiColorStart";             // color
    const std::string Thing_Mirror_End_Color                = "ThMiColorEnd";               // color
    const std::string Thing_Mirror_Color_Tint               = "ThMiTintColor";              // percent
    const std::string Thing_Mirror_Blur                     = "ThMiBlur";                   // double
    const std::string Thing_Mirror_Blur_Stretch             = "ThMiBlurStretch";            // double
    const std::string Thing_Mirror_Scale                    = "ThMiScale";                  // ranged double, 0.001 to no limit

    // Swirl Settings
    const std::string Thing_Swirl_Start_Color               = "ThSwStartColor";             // color
    const std::string Thing_Swirl_Color_Tint                = "ThSwTintColor";              // percent
    const std::string Thing_Swirl_Angle                     = "ThSwAngle";                  // double

    // Water Settings
    const std::string Thing_Water_Texture                   = "ThWaTexture";                // list, enum class Water_Texture
    const std::string Thing_Water_Start_Color               = "ThWaStartColor";             // color
    const std::string Thing_Water_End_Color                 = "ThWaEndColor";               // color
    const std::string Thing_Water_Color_Tint                = "ThWaTintColor";              // percent
    const std::string Thing_Water_Reflection                = "ThWaReflection";             // percent
    const std::string Thing_Water_Movement_Speed            = "ThWaMovementSpeed";          // double
    const std::string Thing_Water_Ripple_Frequency          = "ThWaRipFrequency";           // double
    const std::string Thing_Water_Ripple_Speed              = "ThWaRipSpeed";               // double
    const std::string Thing_Water_Ripple_Amplitude          = "ThWaRipAmplitude";           // double
    const std::string Thing_Water_Ripple_Stretch            = "ThWaRipStretch";             // double
    const std::string Thing_Water_Wave_Frequency            = "ThWaWaveFrequency";          // double
    const std::string Thing_Water_Wave_Speed                = "ThWaWaveSpeed";              // double
    const std::string Thing_Water_Wave_Amplitude            = "ThWaWaveAmplitude";          // double

    // Water Refract Settings
    const std::string Thing_Water_Refract_Reflection        = "ThWaRefractReflection";      // percent
    const std::string Thing_Water_Refract_Underwater        = "ThWaRefractUnderwater";      // percent
    const std::string Thing_Water_Refract_Texture           = "ThWaRefractTexture";         // percent
    const std::string Thing_Water_Refract_Foam              = "ThWaRefractFoam";            // percent

    // Water Foam Settings
    const std::string Thing_Water_Surface_Color             = "ThWaSurColor";               // color
    const std::string Thing_Water_Surface_Tint              = "ThWaSurTint";                // percent
    const std::string Thing_Water_Surface_Height            = "ThWaSurHeight";              // double
    const std::string Thing_Water_Surface_Is_Flat           = "ThWaSurIsFlat";              // bool

};


#endif // COMPONENTS_AND_PROPERTIES_H











