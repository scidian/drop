//
//      Created by Stephens Nunnally on 11/13/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "core/dr_debug.h"
#include "engine/engine.h"
#include "engine/thing/engine_thing_object.h"
#include "engine/world/engine_world.h"
#include "project/dr_project.h"
#include "project/entities/dr_asset.h"
#include "project/entities/dr_stage.h"
#include "project/entities/dr_thing.h"
#include "project/properties/property_collision.h"


//####################################################################################
//##   Loads Camera Info from Character Camera component
//####################################################################################
Cam_Info DrEngineWorld::loadCharacterCameraSettings(DrThing *thing) {
    Cam_Info cam;
    cam.position =      thing->getComponentPropertyValue(Comps::Thing_Settings_Character, Props::Thing_Character_Camera_Position).toPointF();
    cam.rotation =      thing->getComponentPropertyValue(Comps::Thing_Settings_Character, Props::Thing_Character_Camera_Rotation).toPointF();
    cam.tilt =          thing->getComponentPropertyValue(Comps::Thing_Settings_Character, Props::Thing_Character_Camera_Tilt).toDouble();
    cam.distance =      thing->getComponentPropertyValue(Comps::Thing_Settings_Character, Props::Thing_Character_Camera_Distance).toDouble();
    cam.zoom =          thing->getComponentPropertyValue(Comps::Thing_Settings_Character, Props::Thing_Character_Camera_Zoom).toDouble();
    cam.lag =           thing->getComponentPropertyValue(Comps::Thing_Settings_Character, Props::Thing_Character_Camera_Lag).toPointF();
    int up_vector =     thing->getComponentPropertyValue(Comps::Thing_Settings_Character, Props::Thing_Character_Camera_Up_Vector).toInt();
    cam.match_angle =   thing->getComponentPropertyValue(Comps::Thing_Settings_Character, Props::Thing_Character_Camera_Match_Angle).toBool();
    cam.up =            static_cast<Up_Vector>(up_vector);
    auto sides =        thing->getComponentPropertyValue(Comps::Thing_Settings_Character, Props::Thing_Character_Camera_Sides).toVector();
    cam.frame_top =     static_cast<Frame_Edge>(sides[0].toInt());
    cam.frame_right =   static_cast<Frame_Edge>(sides[1].toInt());
    cam.frame_bottom =  static_cast<Frame_Edge>(sides[2].toInt());
    cam.frame_left =    static_cast<Frame_Edge>(sides[3].toInt());
    return cam;
}


//####################################################################################
//##    Loads one DrProject DrThingType::Object to World / Space
//####################################################################################
void DrEngineWorld::loadCharacterToWorld(DrThing *thing) {

    // ***** Load Character Thing Properties
    long        asset_key = thing->getAssetKey();
    DrAsset    *asset =     m_project->findAssetFromKey(asset_key);
            if (asset == nullptr) return;
    Thing_Info  info =      loadThingBasicInfo(thing);

    // ***** Load Character Camera Properties
    Cam_Info    cam =       loadCharacterCameraSettings(thing);

    // ***** Load Character Settings
    DrPointF max_speed =        asset->getComponentPropertyValue(Comps::Asset_Settings_Character, Props::Asset_Character_Max_Speed).toPointF();
    DrPointF forced_speed =     asset->getComponentPropertyValue(Comps::Asset_Settings_Character, Props::Asset_Character_Forced_Speed).toPointF();
    DrPointF move_speed =       asset->getComponentPropertyValue(Comps::Asset_Settings_Character, Props::Asset_Character_Move_Speed).toPointF();
    bool     angle_move =       asset->getComponentPropertyValue(Comps::Asset_Settings_Character, Props::Asset_Character_Angle_Movement).toBool();

    DrPointF jump_force =       asset->getComponentPropertyValue(Comps::Asset_Settings_Character, Props::Asset_Character_Jump_Force).toPointF();
    int      jump_timeout =     asset->getComponentPropertyValue(Comps::Asset_Settings_Character, Props::Asset_Character_Jump_Timeout).toInt();
    int      jump_count =       asset->getComponentPropertyValue(Comps::Asset_Settings_Character, Props::Asset_Character_Jump_Counter).toInt();
    bool     jump_air =         asset->getComponentPropertyValue(Comps::Asset_Settings_Character, Props::Asset_Character_Jump_Air).toBool();
    bool     jump_wall =        asset->getComponentPropertyValue(Comps::Asset_Settings_Character, Props::Asset_Character_Jump_Wall).toBool();

    double   acceleration =     asset->getComponentPropertyValue(Comps::Asset_Settings_Character, Props::Asset_Character_Acceleration).toDouble();
    double   air_drag =         asset->getComponentPropertyValue(Comps::Asset_Settings_Character, Props::Asset_Character_Air_Drag).toDouble();
    double   ground_drag =      asset->getComponentPropertyValue(Comps::Asset_Settings_Character, Props::Asset_Character_Ground_Drag).toDouble();
    double   rotate_drag =      asset->getComponentPropertyValue(Comps::Asset_Settings_Character, Props::Asset_Character_Rotation_Drag).toDouble();

    bool     flip_image_x =     asset->getComponentPropertyValue(Comps::Asset_Settings_Character, Props::Asset_Character_Flip_Image_X).toBool();
    bool     flip_image_y =     asset->getComponentPropertyValue(Comps::Asset_Settings_Character, Props::Asset_Character_Flip_Image_Y).toBool();
    bool     mouse_rotate =     asset->getComponentPropertyValue(Comps::Asset_Settings_Character, Props::Asset_Character_Mouse_Rotate).toBool();


    int         physics_body_style =    asset->getComponentPropertyValue(Comps::Asset_Physics, Props::Asset_Physics_Body_Style).toInt();
    double      body_rigidness =        asset->getComponentPropertyValue(Comps::Asset_Physics, Props::Asset_Physics_Body_Rigidness).toDouble() / 100.0;
    DrPointF    gravity_scale =         asset->getComponentPropertyValue(Comps::Asset_Physics, Props::Asset_Physics_Gravity_Scale).toPointF();
    std::vector<DrVariant> friction =   asset->getComponentPropertyValue(Comps::Asset_Physics, Props::Asset_Physics_Custom_Friction).toVector();
    std::vector<DrVariant> bounce =     asset->getComponentPropertyValue(Comps::Asset_Physics, Props::Asset_Physics_Custom_Bounce).toVector();
    bool        can_rotate =            asset->getComponentPropertyValue(Comps::Asset_Physics, Props::Asset_Physics_Can_Rotate).toBool();
    Body_Style  body_style =    static_cast<Body_Style>(physics_body_style);
    double      use_friction =  (friction[0].toBool()) ? friction[1].toDouble() : c_friction;
    double      use_bounce =    (bounce[0].toBool())   ? bounce[1].toDouble()   : c_bounce;

    // ***** Add the player to the cpSpace
    DrEngineObject *player = nullptr;
    switch (body_style) {
        case Body_Style::Rigid_Body:
            player = new DrEngineObject(this, getNextKey(), thing->getKey(), Body_Type::Dynamic, asset->getKey(),
                                        info.position.x, -info.position.y, info.z_order, info.scale, use_friction, use_bounce,
                                        c_collide_true, can_rotate, info.angle, info.opacity);
            loadThingCollisionShape(asset, player);
            addThing(player);
            break;
        case Body_Style::Circular_Blob:
            player = addSoftBodyCircle( thing->getKey(), asset->getKey(), info.position.x, -info.position.y, info.z_order,
                                        info.size, info.scale, body_rigidness, use_friction, use_bounce, can_rotate);
            break;
        case Body_Style::Square_Blob:
            player = addSoftBodySquare( thing->getKey(), asset->getKey(), info.position.x, -info.position.y, info.z_order,
                                        info.size, info.scale, body_rigidness, use_friction, use_bounce, can_rotate);
            break;
        case Body_Style::Mesh_Blob:
            player = addSoftBodyMesh(   thing->getKey(), asset->getKey(), info.position.x, -info.position.y, info.z_order,
                                        info.size, info.scale, body_rigidness, use_friction, use_bounce, can_rotate);
            break;
    }

    // Soft Body constructors dont set angle or opacity, do it now
    if (body_style != Body_Style::Rigid_Body) {
        player->setOpacity(info.opacity);
        player->setAngle(-info.angle);
    }

    player->setComponentCamera(new ThingCompCamera(this, player));
    player->setComponentPlayer(new ThingCompPlayer(this, player));

    // ***** Camera settings
    player->compCamera()->setCameraPositionXY( cam.position );
    player->compCamera()->setCameraPositionZ( cam.distance );
    player->compCamera()->setCameraRotation( float(cam.rotation.x), float(cam.rotation.y), float(cam.tilt) );
    player->compCamera()->setCameraZoom( cam.zoom );
    player->compCamera()->setCameraLag( cam.lag );
    player->compCamera()->setCameraUpVector(cam.up);
    player->compCamera()->setCameraMatch(cam.match_angle);
    player->compCamera()->setCameraEdge(Edge_Location::Top,    cam.frame_top);
    player->compCamera()->setCameraEdge(Edge_Location::Right,  cam.frame_right);
    player->compCamera()->setCameraEdge(Edge_Location::Bottom, cam.frame_bottom);
    player->compCamera()->setCameraEdge(Edge_Location::Left,   cam.frame_left);

    // ***** Apply Character Settings
    player->compPlayer()->setMaxSpeedX( max_speed.x );
    player->compPlayer()->setMaxSpeedY( max_speed.y );
    player->compPlayer()->setForcedSpeedX( forced_speed.x );
    player->compPlayer()->setForcedSpeedY( forced_speed.y );
    player->compPlayer()->setMoveSpeedX( move_speed.x );
    player->compPlayer()->setMoveSpeedY( move_speed.y );
    player->compPlayer()->setAngleMovement( angle_move );

    player->compPlayer()->setJumpForceX( jump_force.x );
    player->compPlayer()->setJumpForceY( jump_force.y );
    player->compPlayer()->setJumpTimeout( jump_timeout );
    player->compPlayer()->setJumpCount( jump_count );
    player->compPlayer()->setCanAirJump( jump_air );
    player->compPlayer()->setCanWallJump( jump_wall );

    player->compPlayer()->setAcceleration( acceleration );
    player->compPlayer()->setAirDrag( air_drag );
    player->compPlayer()->setGroundDrag( ground_drag );
    player->compPlayer()->setRotateDrag( rotate_drag );
    player->compPlayer()->setMouseRotate( mouse_rotate );

    // ***** Object Movement Settings
    player->setFlipImageX( flip_image_x );
    player->setFlipImageY( flip_image_y );
    player->setGravityScale( gravity_scale );

    // ***** Appearance settings
    loadThingAppearanceSettings(thing, player);

    // ***** Health / Damage Settings
    loadThingHealthSettings(asset, player);

    // ***** 3D Settings
    loadThing3DSettings(thing, player);

    // ***** Controls Settings
    loadThingControlsSettings(asset, player);

    // ***** Check if there are any active characters, if not, give controls
    bool should_we_give_control = (countCharacters() == 0);
    bool give_camera = should_we_give_control && (this->getActiveCamera() <= 0);
    assignPlayerControls(player, should_we_give_control, give_camera);
}










