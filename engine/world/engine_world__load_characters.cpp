//
//      Created by Stephens Nunnally on 11/13/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "engine/engine.h"
#include "engine/things/engine_thing_object.h"
#include "engine/world/engine_world.h"
#include "project/dr_project.h"
#include "project/entities/dr_asset.h"
#include "project/entities/dr_stage.h"
#include "project/entities/dr_thing.h"
#include "project/properties/property_collision.h"


//####################################################################################
//##    Loads one DrProject DrThingType::Object to World / Space
//####################################################################################
void DrEngineWorld::loadCharacterToWorld(DrThing *thing) {

    // ***** Load Character Thing Properties
    long        asset_key = thing->getAssetKey();
    DrAsset    *asset =     m_project->findAssetFromKey(asset_key);
            if (asset == nullptr) return;
    ThingInfo   info =      loadThingBasicInfo( thing );

    // ***** Load Character Settings
    DrPointF cam_position =     thing->getComponentPropertyValue(Components::Thing_Settings_Character, Properties::Thing_Character_Camera_Position).toPointF();
    DrPointF cam_rotation =     thing->getComponentPropertyValue(Components::Thing_Settings_Character, Properties::Thing_Character_Camera_Rotation).toPointF();
    double   cam_tilt =         thing->getComponentPropertyValue(Components::Thing_Settings_Character, Properties::Thing_Character_Camera_Tilt).toDouble();
    double   cam_zoom =         thing->getComponentPropertyValue(Components::Thing_Settings_Character, Properties::Thing_Character_Camera_Zoom).toDouble();
    DrPointF cam_lag =          thing->getComponentPropertyValue(Components::Thing_Settings_Character, Properties::Thing_Character_Camera_Lag).toPointF();
    int      up_vector =        thing->getComponentPropertyValue(Components::Thing_Settings_Character, Properties::Thing_Character_Camera_Up_Vector).toInt();
    bool     cam_match =        thing->getComponentPropertyValue(Components::Thing_Settings_Character, Properties::Thing_Character_Camera_Match_Angle).toBool();
    Up_Vector   cam_up =        static_cast<Up_Vector>(up_vector);

    DrPointF max_speed =        asset->getComponentPropertyValue(Components::Asset_Settings_Character, Properties::Asset_Character_Max_Speed).toPointF();
    DrPointF forced_speed =     asset->getComponentPropertyValue(Components::Asset_Settings_Character, Properties::Asset_Character_Forced_Speed).toPointF();
    DrPointF move_speed =       asset->getComponentPropertyValue(Components::Asset_Settings_Character, Properties::Asset_Character_Move_Speed).toPointF();
    bool     angle_move =       asset->getComponentPropertyValue(Components::Asset_Settings_Character, Properties::Asset_Character_Angle_Movement).toBool();

    DrPointF jump_force =       asset->getComponentPropertyValue(Components::Asset_Settings_Character, Properties::Asset_Character_Jump_Force).toPointF();
    int      jump_timeout =     asset->getComponentPropertyValue(Components::Asset_Settings_Character, Properties::Asset_Character_Jump_Timeout).toInt();
    int      jump_count =       asset->getComponentPropertyValue(Components::Asset_Settings_Character, Properties::Asset_Character_Jump_Counter).toInt();
    bool     jump_air =         asset->getComponentPropertyValue(Components::Asset_Settings_Character, Properties::Asset_Character_Jump_Air).toBool();
    bool     jump_wall =        asset->getComponentPropertyValue(Components::Asset_Settings_Character, Properties::Asset_Character_Jump_Wall).toBool();

    double   acceleration =     asset->getComponentPropertyValue(Components::Asset_Settings_Character, Properties::Asset_Character_Acceleration).toDouble();
    double   air_drag =         asset->getComponentPropertyValue(Components::Asset_Settings_Character, Properties::Asset_Character_Air_Drag).toDouble();
    double   ground_drag =      asset->getComponentPropertyValue(Components::Asset_Settings_Character, Properties::Asset_Character_Ground_Drag).toDouble();
    double   rotate_drag =      asset->getComponentPropertyValue(Components::Asset_Settings_Character, Properties::Asset_Character_Rotation_Drag).toDouble();

    bool     flip_image_x =     asset->getComponentPropertyValue(Components::Asset_Settings_Character, Properties::Asset_Character_Flip_Image_X).toBool();
    bool     flip_image_y =     asset->getComponentPropertyValue(Components::Asset_Settings_Character, Properties::Asset_Character_Flip_Image_Y).toBool();
    bool     mouse_rotate =     asset->getComponentPropertyValue(Components::Asset_Settings_Character, Properties::Asset_Character_Mouse_Rotate).toBool();

    DrPointF gravity_scale =            asset->getComponentPropertyValue(Components::Asset_Physics, Properties::Asset_Physics_Gravity_Scale).toPointF();
    std::vector<DrVariant> friction =   asset->getComponentPropertyValue(Components::Asset_Physics, Properties::Asset_Physics_Custom_Friction).toVector();
    std::vector<DrVariant> bounce =     asset->getComponentPropertyValue(Components::Asset_Physics, Properties::Asset_Physics_Custom_Bounce).toVector();
    bool     can_rotate =               asset->getComponentPropertyValue(Components::Asset_Physics, Properties::Asset_Physics_Can_Rotate).toBool();
    double   use_friction = (friction[0].toBool()) ? friction[1].toDouble() : c_friction;
    double   use_bounce =   (bounce[0].toBool())   ? bounce[1].toDouble()   : c_bounce;

    // ***** Add the player to the cpSpace
    DrEngineObject *player = new DrEngineObject(this, getNextKey(), thing->getKey(), Body_Type::Dynamic, asset->getKey(),
                                                info.position.x, -info.position.y, info.z_order,
                                                info.scale, use_friction, use_bounce,
                                                c_collide_true, can_rotate, info.angle, info.opacity);
    loadThingCollisionShape(asset, player);                                                                 // Load collision shape(s)
    player->setCameraPositionXY( cam_position );                                                            // Set active camera position
    player->setCameraRotation( float(cam_rotation.x), float(cam_rotation.y), float(cam_tilt) );             // Set active camera rotation
    player->setCameraZoom( cam_zoom );                                                                      // Set active camera zoom
    player->setCameraLag( cam_lag );                                                                        // Set active camera lag
    player->setCameraUpVector(cam_up);                                                                      // Set active camera up vector
    player->setCameraMatch(cam_match);

    // ***** Apply Character Settings
    player->setMaxSpeedX( max_speed.x );
    player->setMaxSpeedY( max_speed.y );
    player->setForcedSpeedX( forced_speed.x );
    player->setForcedSpeedY( forced_speed.y );
    player->setMoveSpeedX( move_speed.x );
    player->setMoveSpeedY( move_speed.y );
    player->setAngleMovement( angle_move );

    player->setJumpForceX( jump_force.x );
    player->setJumpForceY( jump_force.y );
    player->setJumpTimeout( jump_timeout );
    player->setJumpCount( jump_count );
    player->setCanAirJump( jump_air );
    player->setCanWallJump( jump_wall );

    player->setAcceleration( acceleration );
    player->setAirDrag( air_drag );
    player->setGroundDrag( ground_drag );
    player->setRotateDrag( rotate_drag );
    player->setGravityScale( gravity_scale );

    player->setFlipImageX( flip_image_x );
    player->setFlipImageY( flip_image_y );
    player->setMouseRotate( mouse_rotate );

    // ***** Appearance settings
    loadThingAppearanceSettings(thing, player);

    // ***** Health / Damage Settings
    loadThingHealthSettings(asset, player);

    // ***** 3D Settings
    loadThing3DSettings(thing, player);

    // ***** Controls Settings
    loadThingControlsSettings(asset, player);


    // ********** Add to world
    addThing(player);

    // Check if there are any active characters, if not, give controls
    bool should_we_give_control = (countCharacters() == 0);
    bool give_camera = should_we_give_control && (this->getActiveCamera() <= 0);
    assignPlayerControls(player, should_we_give_control, true, give_camera);
}










