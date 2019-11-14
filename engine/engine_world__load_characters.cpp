//
//      Created by Stephens Nunnally on 11/13/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QtMath>
#include <QDebug>
#include <QRandomGenerator>

#include "engine/engine.h"
#include "engine/engine_world.h"
#include "engine_things/engine_thing_object.h"
#include "project/project.h"
#include "project/project_asset.h"
#include "project/project_stage.h"
#include "project/project_thing.h"
#include "project/thing_shape_list.h"


//####################################################################################
//##    Loads one DrProject DrThingType::Object to World / Space
//####################################################################################
void DrEngineWorld::loadCharacterToWorld(DrThing *thing) {

    // ***** Load Character Thing Properties
    long        asset_key = thing->getAssetKey();
    DrAsset    *asset = m_project->getAsset(asset_key);
            if (asset == nullptr) return;
    ThingInfo   info =      loadThingBasicInfo( thing );

    // ***** Load Character Settings
    ///QVariant cam_position = thing->getComponentPropertyValue(Components::Thing_Settings_Character, Properties::Thing_Character_Camera_Position);
    ///QVariant cam_rotation = thing->getComponentPropertyValue(Components::Thing_Settings_Character, Properties::Thing_Character_Camera_Rotation);
    ///player->setCameraPosition( cam_position.value<QVector3D>() );
    ///player->setCameraRotation( cam_rotation.value<QVector3D>() );
    QPointF cam_position =  thing->getComponentPropertyValue(Components::Thing_Settings_Character, Properties::Thing_Character_Camera_Position).toPointF();
    QPointF cam_rotation =  thing->getComponentPropertyValue(Components::Thing_Settings_Character, Properties::Thing_Character_Camera_Rotation).toPointF();
    double  cam_tilt =      thing->getComponentPropertyValue(Components::Thing_Settings_Character, Properties::Thing_Character_Camera_Tilt).toDouble();
    double  cam_zoom =      thing->getComponentPropertyValue(Components::Thing_Settings_Character, Properties::Thing_Character_Camera_Zoom).toDouble();
    QPointF cam_lag =       thing->getComponentPropertyValue(Components::Thing_Settings_Character, Properties::Thing_Character_Camera_Lag).toPointF();
    int     up_vector =     thing->getComponentPropertyValue(Components::Thing_Settings_Character, Properties::Thing_Character_Camera_Up_Vector).toInt();
    Up_Vector cam_up =      static_cast<Up_Vector>(up_vector);

    QPointF max_speed =     asset->getComponentPropertyValue(Components::Asset_Settings_Character, Properties::Asset_Character_Max_Speed).toPointF();
    QPointF forced_speed =  asset->getComponentPropertyValue(Components::Asset_Settings_Character, Properties::Asset_Character_Forced_Speed).toPointF();
    QPointF move_speed =    asset->getComponentPropertyValue(Components::Asset_Settings_Character, Properties::Asset_Character_Move_Speed).toPointF();
    bool    angle_move =    asset->getComponentPropertyValue(Components::Asset_Settings_Character, Properties::Asset_Character_Angle_Movement).toBool();

    QPointF jump_force =    asset->getComponentPropertyValue(Components::Asset_Settings_Character, Properties::Asset_Character_Jump_Force).toPointF();
    int     jump_timeout =  asset->getComponentPropertyValue(Components::Asset_Settings_Character, Properties::Asset_Character_Jump_Timeout).toInt();
    int     jump_count =    asset->getComponentPropertyValue(Components::Asset_Settings_Character, Properties::Asset_Character_Jump_Counter).toInt();
    bool    jump_air =      asset->getComponentPropertyValue(Components::Asset_Settings_Character, Properties::Asset_Character_Jump_Air).toBool();
    bool    jump_wall =     asset->getComponentPropertyValue(Components::Asset_Settings_Character, Properties::Asset_Character_Jump_Wall).toBool();

    double  acceleration =  asset->getComponentPropertyValue(Components::Asset_Settings_Character, Properties::Asset_Character_Acceleration).toDouble();
    double  air_drag =      asset->getComponentPropertyValue(Components::Asset_Settings_Character, Properties::Asset_Character_Air_Drag).toDouble();
    double  ground_drag =   asset->getComponentPropertyValue(Components::Asset_Settings_Character, Properties::Asset_Character_Ground_Drag).toDouble();
    double  rotate_drag =   asset->getComponentPropertyValue(Components::Asset_Settings_Character, Properties::Asset_Character_Rotation_Drag).toDouble();

    bool    flip_image_x =  asset->getComponentPropertyValue(Components::Asset_Settings_Character, Properties::Asset_Character_Flip_Image_X).toBool();
    bool    flip_image_y =  asset->getComponentPropertyValue(Components::Asset_Settings_Character, Properties::Asset_Character_Flip_Image_Y).toBool();

    bool    feels_gravity =     asset->getComponentPropertyValue(Components::Asset_Physics, Properties::Asset_Physics_Feels_Gravity).toBool();
    QList<QVariant> friction =  asset->getComponentPropertyValue(Components::Asset_Physics, Properties::Asset_Physics_Custom_Friction).toList();
    QList<QVariant> bounce =    asset->getComponentPropertyValue(Components::Asset_Physics, Properties::Asset_Physics_Custom_Bounce).toList();
    bool    can_rotate =        asset->getComponentPropertyValue(Components::Asset_Physics, Properties::Asset_Physics_Can_Rotate).toBool();
    double  rotate_speed =      asset->getComponentPropertyValue(Components::Asset_Physics, Properties::Asset_Physics_Rotate_Speed).toDouble();
    double  use_friction = (friction[0].toBool()) ? friction[1].toDouble() : c_friction;
    double  use_bounce =   (bounce[0].toBool())   ? bounce[1].toDouble()   : c_bounce;

    // ***** Add the player to the cpSpace
    DrEngineObject *player = new DrEngineObject(this, getNextKey(), Body_Type::Dynamic, asset_key,
                                                info.position.x, -info.position.y, info.z_order,
                                                info.scale, use_friction, use_bounce,
                                                c_collide_true, can_rotate, info.angle, info.opacity);
    loadThingCollisionShape(asset, player);                                                                 // Load collision shape(s)
    player->setCameraPositionXY( cam_position );                                                            // Set active camera position
    player->setCameraRotation( float(cam_rotation.x()), float(cam_rotation.y()), float(cam_tilt) );         // Set active camera rotation
    player->setCameraZoom( cam_zoom );                                                                      // Set active camera zoom
    player->setCameraLag( DrPointF(cam_lag.x(), cam_lag.y()) );                                             // Set active camera lag
    player->setCameraUpVector(cam_up);                                                                      // Set active camera up vector

    // ***** Apply Character Settings
    if (Dr::FuzzyCompare(rotate_speed, 0.0) == false) {
        player->setCanRotate( true );
        player->setRotateSpeedZ(rotate_speed);
    }
    player->setMaxSpeedX( max_speed.x() );
    player->setMaxSpeedY( max_speed.y() );
    player->setForcedSpeedX( forced_speed.x() );
    player->setForcedSpeedY( forced_speed.y() );
    player->setMoveSpeedX( move_speed.x() );
    player->setMoveSpeedY( move_speed.y() );
    player->setAngleMovement( angle_move );

    player->setJumpForceX( jump_force.x() );
    player->setJumpForceY( jump_force.y() );
    player->setJumpTimeout( jump_timeout );
    player->setJumpCount( jump_count );
    player->setCanAirJump( jump_air );
    player->setCanWallJump( jump_wall );

    player->setAcceleration( acceleration );
    player->setAirDrag( air_drag );
    player->setGroundDrag( ground_drag );
    player->setRotateDrag( rotate_drag );
    player->setIgnoreGravity( !feels_gravity );

    player->setFlipImageX( flip_image_x );
    player->setFlipImageY( flip_image_y );

    // ***** Add to world
    addThing(player);

    // Check if there are any active characters, if not, give controls
    bool should_we_give_control = (countCharacters() == 0);
    bool give_camera = should_we_give_control && (this->getActiveCamera() <= 0);
    assignPlayerControls(player, should_we_give_control, true, give_camera);

    // ***** Appearance settings
    loadThingAppearanceSettings(thing, player);

    // ***** Health / Damage Settings
    loadThingHealthSettings(asset, player);

    // ***** 3D Settings
    loadThing3DSettings(thing, player);
}










