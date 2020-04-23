//
//      Created by Stephens Nunnally on 5/15/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "core/dr_debug.h"
#include "core/dr_random.h"
#include "engine/engine.h"
#include "engine/thing/engine_thing.h"
#include "engine/world/engine_world.h"
#include "project/properties/property_collision.h"
#include "project/dr_project.h"
#include "project/entities/dr_asset.h"
#include "project/entities/dr_stage.h"
#include "project/entities/dr_thing.h"


//####################################################################################
//##    Loads Basic DrThing Info from Layering / Transform components
//####################################################################################
Thing_Info DrEngineWorld::loadThingBasicInfo(DrThing *thing) {
    Thing_Info info;
    info.angle =        thing->getComponentPropertyValue(Comps::Thing_Transform, Props::Thing_Rotation).toDouble();
    info.position =     thing->getComponentPropertyValue(Comps::Thing_Transform, Props::Thing_Position).toPointF();
    info.scale =        thing->getComponentPropertyValue(Comps::Thing_Transform, Props::Thing_Scale).toPointF();
    info.size =         thing->getComponentPropertyValue(Comps::Thing_Transform, Props::Thing_Size).toPointF();
    info.opacity =      thing->getComponentPropertyValue(Comps::Thing_Layering,  Props::Thing_Opacity).toFloat() / 100.0f;
    info.z_order =      thing->getZOrderWithSub();
    return info;
}


//####################################################################################
//##    Loads 3D Settings from DrThing in DrProject to DrEngineObject
//####################################################################################
void DrEngineWorld::loadThing3DSettings(DrThing *thing, DrEngineThing *engine_thing) {
    if (engine_thing->comp3D() == nullptr) return;
    int      convert_type =     thing->getComponentPropertyValue(Comps::Thing_3D, Props::Thing_3D_Type).toInt();
    double   depth =            thing->getComponentPropertyValue(Comps::Thing_3D, Props::Thing_3D_Depth).toDouble();
    DrPointF x_axis_rotate =    thing->getComponentPropertyValue(Comps::Thing_3D, Props::Thing_3D_X_Axis_Rotation).toPointF();
    DrPointF y_axis_rotate =    thing->getComponentPropertyValue(Comps::Thing_3D, Props::Thing_3D_Y_Axis_Rotation).toPointF();
    DrPointF x_axis_speed =     thing->getComponentPropertyValue(Comps::Thing_3D, Props::Thing_3D_X_Axis_Speed).toPointF();
    DrPointF y_axis_speed =     thing->getComponentPropertyValue(Comps::Thing_3D, Props::Thing_3D_Y_Axis_Speed).toPointF();
    bool     billboard =        thing->getComponentPropertyValue(Comps::Thing_3D, Props::Thing_3D_Billboard).toBool();
    engine_thing->comp3D()->set3DType(static_cast<Convert_3D_Type>(convert_type));
    engine_thing->comp3D()->setAngleX( x_axis_rotate.x + (Dr::RandomDouble(0.0, x_axis_rotate.y * 2.0) - x_axis_rotate.y) );
    engine_thing->comp3D()->setAngleY( y_axis_rotate.x + (Dr::RandomDouble(0.0, y_axis_rotate.y * 2.0) - y_axis_rotate.y) );
    engine_thing->comp3D()->setBillboard( billboard );
    engine_thing->comp3D()->setDepth(depth);
    engine_thing->comp3D()->setRotateSpeedX( (x_axis_speed.x + (Dr::RandomDouble(0.0, x_axis_speed.y * 2.0) - x_axis_speed.y)) / 100.0 );
    engine_thing->comp3D()->setRotateSpeedY( (y_axis_speed.x + (Dr::RandomDouble(0.0, y_axis_speed.y * 2.0) - y_axis_speed.y)) / 100.0 );
}


//####################################################################################
//##    Loads Appearance Settings from DrThing in DrProject to DrEngineObject
//####################################################################################
void DrEngineWorld::loadThingAppearanceSettings(DrThing *thing, DrEngineThing *engine_thing, bool simple_settings) {
    int         bit_rate =          thing->getComponentPropertyValue(Comps::Thing_Appearance, Props::Thing_Filter_Bitrate).toVector()[0].toInt();
    DrPointF    pixelation =        thing->getComponentPropertyValue(Comps::Thing_Appearance, Props::Thing_Filter_Pixelation).toPointF();
    engine_thing->bitrate =             bit_rate;
    engine_thing->pixel_x =             static_cast<float>(pixelation.x);
    engine_thing->pixel_y =             static_cast<float>(pixelation.y);
    if (simple_settings) return;

    int         blend_type =        thing->getComponentPropertyValue(Comps::Thing_Lighting,   Props::Thing_Lighting_Blend_Object).toInt();
    bool        cast_shadows =      thing->getComponentPropertyValue(Comps::Thing_Lighting,   Props::Thing_Lighting_Cast_Shadows).toBool();
    int         pixel_type =        thing->getComponentPropertyValue(Comps::Thing_Appearance, Props::Thing_Filter_PixelType).toInt();
    float       brightness =        thing->getComponentPropertyValue(Comps::Thing_Appearance, Props::Thing_Filter_Brightness).toVector()[0].toInt() / 255.f;
    float       contrast =          thing->getComponentPropertyValue(Comps::Thing_Appearance, Props::Thing_Filter_Contrast).toVector()[0].toInt() / 255.f;
    float       saturation =        thing->getComponentPropertyValue(Comps::Thing_Appearance, Props::Thing_Filter_Saturation).toVector()[0].toInt() / 255.f;
    float       hue =               thing->getComponentPropertyValue(Comps::Thing_Appearance, Props::Thing_Filter_Hue).toVector()[0].toInt() / 360.f;
    bool        grayscale =         thing->getComponentPropertyValue(Comps::Thing_Appearance, Props::Thing_Filter_Grayscale).toBool();
    bool        negative =          thing->getComponentPropertyValue(Comps::Thing_Appearance, Props::Thing_Filter_Negative).toBool();
    bool        instant_3d =        thing->getComponentPropertyValue(Comps::Thing_Special_Effects, Props::Thing_Filter_Convert_3D).toBool();
    bool        wireframe =         thing->getComponentPropertyValue(Comps::Thing_Special_Effects, Props::Thing_Filter_Wireframe).toVector()[0].toBool();
    float       wireframe_width =   thing->getComponentPropertyValue(Comps::Thing_Special_Effects, Props::Thing_Filter_Wireframe).toVector()[1].toFloat();
    bool        cartoon =           thing->getComponentPropertyValue(Comps::Thing_Special_Effects, Props::Thing_Filter_Cartoon).toVector()[0].toBool();
    float       cartoon_width =     thing->getComponentPropertyValue(Comps::Thing_Special_Effects, Props::Thing_Filter_Cartoon).toVector()[1].toFloat();
    bool        cross_hatch =       thing->getComponentPropertyValue(Comps::Thing_Special_Effects, Props::Thing_Filter_Cross_Hatch).toVector()[0].toBool();
    float       cross_width =       thing->getComponentPropertyValue(Comps::Thing_Special_Effects, Props::Thing_Filter_Cross_Hatch).toVector()[1].toFloat();
    engine_thing->blend_type =          static_cast<Blend_Object>(blend_type);
    engine_thing->cast_shadows =        cast_shadows;
    engine_thing->pixel_texture =       static_cast<Pixel_Texture>(pixel_type);
    engine_thing->brightness =          brightness;
    engine_thing->contrast =            contrast;
    engine_thing->saturation =          saturation;
    engine_thing->hue =                 hue;
    engine_thing->grayscale =           grayscale;
    engine_thing->negative =            negative;
    engine_thing->extrude_3d =          instant_3d;
    engine_thing->wireframe =           wireframe;
    engine_thing->wireframe_width =     wireframe_width;
    engine_thing->cartoon =             cartoon;
    engine_thing->cartoon_width =       cartoon_width;
    engine_thing->cross_hatch =         cross_hatch;
    engine_thing->cross_hatch_width =   cross_width;
}


//####################################################################################
//##    Loads Health / Damage Settings from DrThing in DrProject to DrEngineObject
//####################################################################################
void DrEngineWorld::loadThingHealthSettings(DrAsset *asset, DrEngineThing *engine_thing) {
    if (engine_thing->compPhysics() == nullptr) return;
    double  max_health =        asset->getComponentPropertyValue(Comps::Asset_Health, Props::Asset_Health_Max_Health).toDouble();
    double  health =            asset->getComponentPropertyValue(Comps::Asset_Health, Props::Asset_Health_Health).toDouble();
    double  damage =            asset->getComponentPropertyValue(Comps::Asset_Health, Props::Asset_Health_Damage).toDouble();
    int     damage_delay =      asset->getComponentPropertyValue(Comps::Asset_Health, Props::Asset_Health_Damage_Delay).toInt();
    double  auto_damage =       asset->getComponentPropertyValue(Comps::Asset_Health, Props::Asset_Health_Auto_Damage).toDouble();
    int     death_delay =       asset->getComponentPropertyValue(Comps::Asset_Health, Props::Asset_Health_Death_Delay).toInt();
    int     death_animation =   asset->getComponentPropertyValue(Comps::Asset_Health, Props::Asset_Health_Death_Animation).toInt();
    int     death_duration =    asset->getComponentPropertyValue(Comps::Asset_Health, Props::Asset_Health_Death_Duration).toInt();
    double  damage_recoil =     asset->getComponentPropertyValue(Comps::Asset_Health, Props::Asset_Health_Damage_Recoil).toDouble();
    bool    invincible =        asset->getComponentPropertyValue(Comps::Asset_Health, Props::Asset_Health_Invincible).toBool();
    bool    death_touch =       asset->getComponentPropertyValue(Comps::Asset_Health, Props::Asset_Health_Death_Touch).toBool();
    engine_thing->compPhysics()->setMaxHealth(max_health);
    engine_thing->compPhysics()->setHealth(health);
    engine_thing->compPhysics()->setDamage(damage);
    engine_thing->compPhysics()->setDamageDelay(damage_delay);
    engine_thing->compPhysics()->setAutoDamage(auto_damage);
    engine_thing->compPhysics()->setDeathDelay(death_delay);
    engine_thing->compPhysics()->setDeathAnimation(static_cast<Death_Animation>(death_animation));
    engine_thing->compPhysics()->setDeathDuration(death_duration);
    engine_thing->compPhysics()->setDamageRecoil(damage_recoil);
    engine_thing->compPhysics()->setInvincible(invincible);
    engine_thing->compPhysics()->setDeathTouch(death_touch);
}


//####################################################################################
//##    Loads Health / Damage Settings from DrThing in DrProject to DrEngineObject
//####################################################################################
void DrEngineWorld::loadThingControlsSettings(DrAsset *asset, DrEngineThing *engine_thing) {
    if (engine_thing->compPhysics() == nullptr) return;
    double  rotate_speed =      asset->getComponentPropertyValue(Comps::Asset_Controls, Props::Asset_Controls_Rotate_Speed).toDouble();
    bool    touch_drag =        asset->getComponentPropertyValue(Comps::Asset_Controls, Props::Asset_Controls_Touch_Drag).toVector()[0].toBool();
    double  touch_drag_force =  asset->getComponentPropertyValue(Comps::Asset_Controls, Props::Asset_Controls_Touch_Drag).toVector()[1].toDouble();
    bool    touch_damage =      asset->getComponentPropertyValue(Comps::Asset_Controls, Props::Asset_Controls_Touch_Damage).toVector()[0].toBool();
    double  touch_damage_pts =  asset->getComponentPropertyValue(Comps::Asset_Controls, Props::Asset_Controls_Touch_Damage).toVector()[1].toDouble();
    engine_thing->compPhysics()->setRotateSpeedZ(      rotate_speed);
    engine_thing->compPhysics()->setTouchDrag(         touch_drag);
    engine_thing->compPhysics()->setTouchDragForce(    touch_drag_force);
    engine_thing->compPhysics()->setTouchDamage(       touch_damage);
    engine_thing->compPhysics()->setTouchDamagePoints( touch_damage_pts);
}


//####################################################################################
//##    Loads Collision Shape from DrThing in DrProject to DrEngineObject
//####################################################################################
void DrEngineWorld::loadThingCollisionShape(DrAsset *asset, DrEngineThing *engine_thing) {
    if (engine_thing->compPhysics() == nullptr) return;
    int shape_type =        asset->getComponentPropertyValue(Comps::Asset_Collision, Props::Asset_Collision_Shape).toInt();
    Collision_Shape shape = static_cast<Collision_Shape>(shape_type);
    if (shape == Collision_Shape::Image) {
        DrVariant shapes =  asset->getComponentPropertyValue(Comps::Asset_Collision, Props::Asset_Collision_Image_Shape);
        DrPropertyCollision shape = boost::any_cast<DrPropertyCollision>(shapes.value());
        for (auto poly : shape.getPolygons()) {
            engine_thing->compPhysics()->addShapePolygon(poly);
        }
    } else if (shape == Collision_Shape::Circle) {
        engine_thing->compPhysics()->addShapeCircleFromTexture( asset->getIdleAnimationFirstFrameImageKey() );
    } else if (shape == Collision_Shape::Square) {
        engine_thing->compPhysics()->addShapeBoxFromTexture( asset->getIdleAnimationFirstFrameImageKey() );
    } else if (shape == Collision_Shape::Triangle) {
        engine_thing->compPhysics()->addShapeTriangleFromTexture( asset->getIdleAnimationFirstFrameImageKey() );
    }
}


//####################################################################################
//##    Loads one DrProject DrThingType::Object (physics object) to World / Space
//####################################################################################
DrEngineThing* DrEngineWorld::loadPhysicsObjectToWorld(DrThing *thing,
                                                       double x, double y, double scale_x, double scale_y,
                                                       double angle, double x_velocity, double y_velocity,
                                                       double rotate_spawn) {
    if (thing == nullptr) return nullptr;

    // ***** Load Object Thing Properties
    long        asset_key = thing->getAssetKey();
    DrAsset    *asset =     m_project->findAssetFromKey(asset_key);
            if (asset == nullptr) return nullptr;
    Thing_Info  info =          loadThingBasicInfo( thing );
                info.scale.x *= scale_x;
                info.scale.y *= scale_y;
                info.angle   -= angle;

    bool        collide =       thing->getComponentPropertyValue(Comps::Thing_Settings_Object,  Props::Thing_Object_Collide).toBool();
    int         collide_with =  thing->getComponentPropertyValue(Comps::Thing_Settings_Object,  Props::Thing_Object_Collision_Group).toInt();
    int         physics =       thing->getComponentPropertyValue(Comps::Thing_Settings_Object,  Props::Thing_Object_Physics_Type).toInt();


    // ***** Adjust loading position from Spawn Offset
    DrPointF    spawn_off_x =   thing->getComponentPropertyValue(Comps::Thing_Spawn,   Props::Thing_Spawn_Offset_X).toPointF();
    DrPointF    spawn_off_y =   thing->getComponentPropertyValue(Comps::Thing_Spawn,   Props::Thing_Spawn_Offset_Y).toPointF();
    double spawn_x = spawn_off_x.x + (Dr::RandomDouble(0.0, spawn_off_x.y * 2.0) - spawn_off_x.y);
    double spawn_y = spawn_off_y.x + (Dr::RandomDouble(0.0, spawn_off_y.y * 2.0) - spawn_off_y.y);
    DrPointF spawn_rotate;
    DrPointF spawn_angle = Dr::RotatePointAroundOrigin( DrPointF(spawn_x * scale_x, spawn_y * scale_y), DrPointF(0, 0), rotate_spawn);
        spawn_rotate.x = spawn_angle.x;
        spawn_rotate.y = spawn_angle.y;
    double x_offset = spawn_rotate.x;
    double y_offset = spawn_rotate.y;

    // ***** Load Physics Properties
    int         physics_body_style =        asset->getComponentPropertyValue(Comps::Asset_Physics, Props::Asset_Physics_Body_Style).toInt();
    double      body_rigidness =            asset->getComponentPropertyValue(Comps::Asset_Physics, Props::Asset_Physics_Body_Rigidness).toDouble() / 100.0;
    DrPointF    gravity_scale =             asset->getComponentPropertyValue(Comps::Asset_Physics, Props::Asset_Physics_Gravity_Scale).toPointF();
    std::vector<DrVariant> friction =       asset->getComponentPropertyValue(Comps::Asset_Physics, Props::Asset_Physics_Custom_Friction).toVector();
    std::vector<DrVariant> bounce =         asset->getComponentPropertyValue(Comps::Asset_Physics, Props::Asset_Physics_Custom_Bounce).toVector();
    bool        can_rotate =                asset->getComponentPropertyValue(Comps::Asset_Physics, Props::Asset_Physics_Can_Rotate).toBool();
    Body_Style  body_style =        static_cast<Body_Style>(physics_body_style);
    double      use_friction =      (friction[0].toBool()) ? friction[1].toDouble() : c_friction;
    double      use_bounce =        (bounce[0].toBool())   ? bounce[1].toDouble()   : c_bounce;

    // ***** Set Chipmunk Body Type
    Body_Type body_type = Body_Type::Static;
    switch (physics) {
        case 0: body_type = Body_Type::Static;       break;
        case 1: body_type = Body_Type::Kinematic;    break;
        case 2: body_type = Body_Type::Dynamic;      break;
    }


    // ***** Add the block to the cpSpace
    DrEngineThing *object = nullptr;
    if (body_type != Body_Type::Dynamic && body_style != Body_Style::Rigid_Body) body_style = Body_Style::Rigid_Body;

    switch (body_style) {
        case Body_Style::Rigid_Body:
        case Body_Style::Foliage:
            object = new DrEngineThing(this, this->getNextKey(), thing->getKey());
            object->setComponentPhysics(new ThingCompPhysics(this, object, body_type, asset->getKey(),
                                                             x + x_offset, y + y_offset, info.z_order, info.scale,
                                                             use_friction, use_bounce, collide, can_rotate, info.angle, info.opacity));
            loadThingCollisionShape(asset, object);
            addThing(object);
            if (body_style == Body_Style::Foliage) object->setComponentFoliage(new ThingCompFoliage(this, object, body_rigidness));

            break;
        case Body_Style::Circular_Blob:
            object = addSoftBodyCircle( thing->getKey(), asset->getKey(), x + x_offset, y + y_offset, info.z_order,
                                        info.size, info.scale, body_rigidness, use_friction, use_bounce, can_rotate);
            break;
        case Body_Style::Square_Blob:
            object = addSoftBodySquare( thing->getKey(), asset->getKey(), x + x_offset, y + y_offset, info.z_order,
                                        info.size, info.scale, body_rigidness, use_friction, use_bounce, can_rotate);
            break;
        case Body_Style::Mesh_Blob:
            object = addSoftBodyMesh(   thing->getKey(), asset->getKey(), x + x_offset, y + y_offset, info.z_order,
                                        info.size, info.scale, body_rigidness, use_friction, use_bounce, can_rotate);
            break;
    }

    // Soft Body constructors dont set angle or opacity, do it now
    object->setOpacity(info.opacity);
    object->setAngle(-info.angle);


    // ***** Set collision type
    long     damage_type =      thing->getComponentPropertyValue(Comps::Thing_Settings_Object, Props::Thing_Object_Damage).toInt();
    object->compPhysics()->setCollidesWith(static_cast<Collision_Groups>(collide_with));
    object->compPhysics()->setCollisionType(static_cast<Collision_Type>(damage_type));
    object->compPhysics()->setGravityScale( DrPointF(gravity_scale.x, gravity_scale.y) );

    // ***** Velocity settings
    DrPointF vel_x =            thing->getComponentPropertyValue(Comps::Thing_Movement, Props::Thing_Velocity_X).toPointF();
    DrPointF vel_y =            thing->getComponentPropertyValue(Comps::Thing_Movement, Props::Thing_Velocity_Y).toPointF();
    DrPointF rotation_vel =     thing->getComponentPropertyValue(Comps::Thing_Movement, Props::Thing_Spin_Velocity).toPointF();
    bool     angle_velocity =   thing->getComponentPropertyValue(Comps::Thing_Movement, Props::Thing_Angle_Velocity).toBool();
    bool     angle_player =     thing->getComponentPropertyValue(Comps::Thing_Movement, Props::Thing_Angle_Player).toBool();

    cpVect velocity;
    velocity.x = vel_x.x + (Dr::RandomDouble(0.0, vel_x.y * 2.0) - vel_x.y);
    velocity.y = vel_y.x + (Dr::RandomDouble(0.0, vel_y.y * 2.0) - vel_y.y);
    double deg_angular = rotation_vel.x + (Dr::RandomDouble(0.0, rotation_vel.y * 2.0) - rotation_vel.y);
    double rad_angular = Dr::DegreesToRadians( deg_angular );

    // Attached to KinematicUpdateVelocity callback function in DrEngineObject Contructor
    if (body_type == Body_Type::Kinematic) {
        object->compPhysics()->setOriginalVelocityX( velocity.x + x_velocity );
        object->compPhysics()->setOriginalVelocityY( velocity.y + y_velocity );
        object->compPhysics()->setOriginalSpinVelocity( rad_angular );
        object->compPhysics()->setUseAngleVelocity( angle_velocity );
        object->compPhysics()->setRotateToPlayer( angle_player );
    }


    // ***** Collision Settings
    int         one_way_type =  asset->getComponentPropertyValue(Comps::Asset_Collision, Props::Asset_Collision_One_Way_Type).toInt();
    double      one_way_angle = asset->getComponentPropertyValue(Comps::Asset_Collision, Props::Asset_Collision_One_Way_Direction).toDouble();
    bool        drop_down =     asset->getComponentPropertyValue(Comps::Asset_Collision, Props::Asset_Collision_Drop_Down).toBool();
    double      gravity_multi = asset->getComponentPropertyValue(Comps::Asset_Collision, Props::Asset_Collision_Gravity_Multiplier).toDouble();
    double      repulse_force = asset->getComponentPropertyValue(Comps::Asset_Collision, Props::Asset_Collision_Repulse_Force).toDouble();
    DrPointF    surface_vel =   asset->getComponentPropertyValue(Comps::Asset_Collision, Props::Asset_Collision_Surface_Velocity).toPointF();

    DrPointF one_way_point = Dr::RotatePointAroundOrigin( DrPointF(0.0, 1.0), DrPointF(0, 0), one_way_angle - info.angle);
             one_way_point.x *= info.scale.x;
             one_way_point.y *= info.scale.y;
    object->compPhysics()->setOneWay(static_cast<One_Way>(one_way_type));
    object->compPhysics()->setOneWayDirection(one_way_point);
    object->compPhysics()->setDropDown(drop_down);
    object->compPhysics()->setGravityMultiplier(gravity_multi);
    object->compPhysics()->setRepulseForce(repulse_force);
    object->compPhysics()->setSurfaceVelocity(cpv(surface_vel.x, surface_vel.y));

    // ***** Additional settings
    loadThingAppearanceSettings(thing, object);                     // Appearance settings
    loadThingHealthSettings(asset, object);                         // Health / Damage Settings
    loadThing3DSettings(thing, object);                             // 3D Settings
    loadThingControlsSettings(asset, object);                       // Controls Settings

    // ***** Add Velocity, HAVE TO WAIT until is in world!!
    if (body_type != Body_Type::Static) {
        cpBodySetVelocity( object->compPhysics()->body, velocity + cpv(x_velocity, y_velocity) );
        cpBodySetAngularVelocity( object->compPhysics()->body, rad_angular );
    }

    return object;
}




























