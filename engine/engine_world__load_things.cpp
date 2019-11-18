//
//      Created by Stephens Nunnally on 5/15/2019, (c) 2019 Scidian Software, All Rights Reserved
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
//##    Loads Basic DrThing Info from Layering / Transform components
//####################################################################################
ThingInfo DrEngineWorld::loadThingBasicInfo(DrThing *thing) {
    ThingInfo info;
    info.angle =    thing->getComponentPropertyValue(Components::Thing_Transform, Properties::Thing_Rotation).toDouble();
    QPointF pos =   thing->getComponentPropertyValue(Components::Thing_Transform, Properties::Thing_Position).toPointF();
    QPointF scale = thing->getComponentPropertyValue(Components::Thing_Transform, Properties::Thing_Scale).toPointF();
    QPointF size =  thing->getComponentPropertyValue(Components::Thing_Transform, Properties::Thing_Size).toPointF();
    info.position = DrPointF(pos.x(), pos.y());
    info.scale =    DrPointF(scale.x(), scale.y());
    info.size =     DrPointF(size.x(), size.y());
    info.opacity =  thing->getComponentPropertyValue(Components::Thing_Layering,  Properties::Thing_Opacity).toFloat() / 100.0f;
    info.z_order =  thing->getZOrderWithSub();
    return info;
}


//####################################################################################
//##    Loads 3D Settings from DrThing in DrProject to DrEngineObject
//####################################################################################
void DrEngineWorld::loadThing3DSettings(DrThing *thing, DrEngineThing *object) {
    int    convert_type =   thing->getComponentPropertyValue(Components::Thing_3D, Properties::Thing_3D_Type).toInt();
    double depth =          thing->getComponentPropertyValue(Components::Thing_3D, Properties::Thing_3D_Depth).toDouble();
    QPointF x_axis_rotate = thing->getComponentPropertyValue(Components::Thing_3D, Properties::Thing_3D_X_Axis_Rotation).toPointF();
    QPointF y_axis_rotate = thing->getComponentPropertyValue(Components::Thing_3D, Properties::Thing_3D_Y_Axis_Rotation).toPointF();
    QPointF x_axis_speed =  thing->getComponentPropertyValue(Components::Thing_3D, Properties::Thing_3D_X_Axis_Speed).toPointF();
    QPointF y_axis_speed =  thing->getComponentPropertyValue(Components::Thing_3D, Properties::Thing_3D_Y_Axis_Speed).toPointF();
    bool   billboard =      thing->getComponentPropertyValue(Components::Thing_3D, Properties::Thing_3D_Billboard).toBool();
    object->set3DType(static_cast<Convert_3D_Type>(convert_type));
    object->setAngleX( x_axis_rotate.x() + (QRandomGenerator::global()->bounded(x_axis_rotate.y() * 2.0) - x_axis_rotate.y()) );
    object->setAngleY( y_axis_rotate.x() + (QRandomGenerator::global()->bounded(y_axis_rotate.y() * 2.0) - y_axis_rotate.y()) );
    object->setBillboard( billboard );
    object->setDepth(depth);
    object->setRotateSpeedX( (x_axis_speed.x() + (QRandomGenerator::global()->bounded(x_axis_speed.y() * 2.0) - x_axis_speed.y())) / 100.0 );
    object->setRotateSpeedY( (y_axis_speed.x() + (QRandomGenerator::global()->bounded(y_axis_speed.y() * 2.0) - y_axis_speed.y())) / 100.0 );
}


//####################################################################################
//##    Loads Appearance Settings from DrThing in DrProject to DrEngineObject
//####################################################################################
void DrEngineWorld::loadThingAppearanceSettings(DrThing *thing, DrEngineObject *object) {
    bool    cast_shadows =  thing->getComponentPropertyValue(Components::Thing_Lighting,   Properties::Thing_Lighting_Cast_Shadows).toBool();
    int     bit_rate =      thing->getComponentPropertyValue(Components::Thing_Appearance, Properties::Thing_Filter_Bitrate).toList().first().toInt();
    QPointF pixelation =    thing->getComponentPropertyValue(Components::Thing_Appearance, Properties::Thing_Filter_Pixelation).toPointF();
    float   brightness =    thing->getComponentPropertyValue(Components::Thing_Appearance, Properties::Thing_Filter_Brightness).toList().first().toInt() / 255.f;
    float   contrast =      thing->getComponentPropertyValue(Components::Thing_Appearance, Properties::Thing_Filter_Contrast).toList().first().toInt() / 255.f;
    float   saturation =    thing->getComponentPropertyValue(Components::Thing_Appearance, Properties::Thing_Filter_Saturation).toList().first().toInt() / 255.f;
    float   hue =           thing->getComponentPropertyValue(Components::Thing_Appearance, Properties::Thing_Filter_Hue).toList().first().toInt() / 360.f;
    bool    grayscale =     thing->getComponentPropertyValue(Components::Thing_Appearance, Properties::Thing_Filter_Grayscale).toBool();
    bool    negative =      thing->getComponentPropertyValue(Components::Thing_Appearance, Properties::Thing_Filter_Negative).toBool();
    bool    instant_3d =    thing->getComponentPropertyValue(Components::Thing_Special_Effects, Properties::Thing_Filter_Convert_3D).toBool();
    bool    wireframe =     thing->getComponentPropertyValue(Components::Thing_Special_Effects, Properties::Thing_Filter_Wireframe).toBool();
    bool    cartoon =       thing->getComponentPropertyValue(Components::Thing_Special_Effects, Properties::Thing_Filter_Cartoon).toList()[0].toBool();
    float   cartoon_width = thing->getComponentPropertyValue(Components::Thing_Special_Effects, Properties::Thing_Filter_Cartoon).toList()[1].toFloat();
    bool    cross_hatch =   thing->getComponentPropertyValue(Components::Thing_Special_Effects, Properties::Thing_Filter_Cross_Hatch).toList()[0].toBool();
    float   cross_width =   thing->getComponentPropertyValue(Components::Thing_Special_Effects, Properties::Thing_Filter_Cross_Hatch).toList()[1].toFloat();
    object->cast_shadows =      cast_shadows;
    object->bitrate =           bit_rate;
    object->pixel_x =           static_cast<float>(pixelation.x());
    object->pixel_y =           static_cast<float>(pixelation.y());
    object->brightness =        brightness;
    object->contrast =          contrast;
    object->saturation =        saturation;
    object->hue =               hue;
    object->grayscale =         grayscale;
    object->negative =          negative;
    object->extrude_3d =        instant_3d;
    object->wireframe =         wireframe;
    object->cartoon =           cartoon;
    object->cartoon_width =     cartoon_width;
    object->cross_hatch =       cross_hatch;
    object->cross_hatch_width = cross_width;
}


//####################################################################################
//##    Loads Health / Damage Settings from DrThing in DrProject to DrEngineObject
//####################################################################################
void DrEngineWorld::loadThingHealthSettings(DrAsset *asset, DrEngineObject *object) {
    double  max_health =        asset->getComponentPropertyValue(Components::Asset_Health, Properties::Asset_Health_Max_Health).toDouble();
    double  health =            asset->getComponentPropertyValue(Components::Asset_Health, Properties::Asset_Health_Health).toDouble();
    double  damage =            asset->getComponentPropertyValue(Components::Asset_Health, Properties::Asset_Health_Damage).toDouble();
    int     damage_delay =      asset->getComponentPropertyValue(Components::Asset_Health, Properties::Asset_Health_Damage_Delay).toInt();
    double  auto_damage =       asset->getComponentPropertyValue(Components::Asset_Health, Properties::Asset_Health_Auto_Damage).toDouble();
    int     death_delay =       asset->getComponentPropertyValue(Components::Asset_Health, Properties::Asset_Health_Death_Delay).toInt();
    int     death_animation =   asset->getComponentPropertyValue(Components::Asset_Health, Properties::Asset_Health_Death_Animation).toInt();
    int     death_duration =    asset->getComponentPropertyValue(Components::Asset_Health, Properties::Asset_Health_Death_Duration).toInt();
    double  damage_recoil =     asset->getComponentPropertyValue(Components::Asset_Health, Properties::Asset_Health_Damage_Recoil).toDouble();
    bool    invincible =        asset->getComponentPropertyValue(Components::Asset_Health, Properties::Asset_Health_Invincible).toBool();
    bool    death_touch =       asset->getComponentPropertyValue(Components::Asset_Health, Properties::Asset_Health_Death_Touch).toBool();
    object->setMaxHealth(max_health);
    object->setHealth(health);
    object->setDamage(damage);
    object->setDamageDelay(damage_delay);
    object->setAutoDamage(auto_damage);
    object->setDeathDelay(death_delay);
    object->setDeathAnimation(static_cast<Death_Animation>(death_animation));
    object->setDeathDuration(death_duration);
    object->setDamageRecoil(damage_recoil);
    object->setInvincible(invincible);
    object->setDeathTouch(death_touch);
}


//####################################################################################
//##    Loads Collision Shape from DrThing in DrProject to DrEngineObject
//####################################################################################
void DrEngineWorld::loadThingCollisionShape(DrAsset *asset, DrEngineObject *object) {
    int shape_type =        asset->getComponentPropertyValue(Components::Asset_Collision, Properties::Asset_Collision_Shape).toInt();
    Collision_Shape shape = static_cast<Collision_Shape>(shape_type);
    if (shape == Collision_Shape::Image) {
        QVariant shapes =   asset->getComponentPropertyValue(Components::Asset_Collision, Properties::Asset_Collision_Image_Shape);
        DrShapeList shape = shapes.value<DrShapeList>();
        for (auto poly : shape.getPolygons()) {
            QVector<DrPointF> points = QVector<DrPointF>::fromStdVector(poly);
            object->addShapePolygon(points);
        }
    } else if (shape == Collision_Shape::Circle) {
        object->addShapeCircleFromTexture( asset->getKey() );
    } else if (shape == Collision_Shape::Square) {
        object->addShapeBoxFromTexture( asset->getKey() );
    } else if (shape == Collision_Shape::Triangle) {
        object->addShapeTriangleFromTexture( asset->getKey() );
    }
}


//####################################################################################
//##    Loads one DrProject DrThingType::Object to World / Space
//####################################################################################
DrEngineObject* DrEngineWorld::loadObjectToWorld(DrThing *thing,
                                                 double x, double y, double scale_x, double scale_y,
                                                 double angle, double x_velocity, double y_velocity,
                                                 double rotate_spawn) {
            if (thing == nullptr) return nullptr;

    // ***** Load Object Thing Properties
    long        asset_key = thing->getAssetKey();
    DrAsset    *asset =     m_project->getAsset(asset_key);
            if (asset == nullptr) return nullptr;
    ThingInfo   info =          loadThingBasicInfo( thing );
                info.scale.x *= scale_x;
                info.scale.y *= scale_y;
                info.angle   -= angle;

    bool        collide =       thing->getComponentPropertyValue(Components::Thing_Settings_Object,  Properties::Thing_Object_Collide).toBool();
    int         collide_with =  thing->getComponentPropertyValue(Components::Thing_Settings_Object,  Properties::Thing_Object_Collision_Group).toInt();
    int         physics =       thing->getComponentPropertyValue(Components::Thing_Settings_Object,  Properties::Thing_Object_Physics_Type).toInt();

    // ***** Adjust loading position from Spawn Offset
    QPointF     spawn_off_x =   thing->getComponentPropertyValue(Components::Thing_Spawn,   Properties::Thing_Spawn_Offset_X).toPointF();
    QPointF     spawn_off_y =   thing->getComponentPropertyValue(Components::Thing_Spawn,   Properties::Thing_Spawn_Offset_Y).toPointF();
    double spawn_x = spawn_off_x.x() + (QRandomGenerator::global()->bounded(spawn_off_x.y() * 2.0) - spawn_off_x.y());
    double spawn_y = spawn_off_y.x() + (QRandomGenerator::global()->bounded(spawn_off_y.y() * 2.0) - spawn_off_y.y());
    DrPointF spawn_rotate;
    QTransform t = QTransform().rotate(rotate_spawn);
    QPointF spawn_angle = t.map( QPointF(spawn_x * scale_x, spawn_y * scale_y) );
        spawn_rotate.x = spawn_angle.x();
        spawn_rotate.y = spawn_angle.y();
    double x_offset = spawn_rotate.x;
    double y_offset = spawn_rotate.y;

    // ***** Load Physics Properties
    bool    feels_gravity =     asset->getComponentPropertyValue(Components::Asset_Physics, Properties::Asset_Physics_Feels_Gravity).toBool();
    QList<QVariant> friction =  asset->getComponentPropertyValue(Components::Asset_Physics, Properties::Asset_Physics_Custom_Friction).toList();
    QList<QVariant> bounce =    asset->getComponentPropertyValue(Components::Asset_Physics, Properties::Asset_Physics_Custom_Bounce).toList();
    bool    can_rotate =        asset->getComponentPropertyValue(Components::Asset_Physics, Properties::Asset_Physics_Can_Rotate).toBool();
    double  rotate_speed =      asset->getComponentPropertyValue(Components::Asset_Physics, Properties::Asset_Physics_Rotate_Speed).toDouble();
    double  use_friction = (friction[0].toBool()) ? friction[1].toDouble() : c_friction;
    double  use_bounce =   (bounce[0].toBool())   ? bounce[1].toDouble()   : c_bounce;

    // ***** Set Chipmunk Body Type
    Body_Type body_type = Body_Type::Static;
    switch (physics) {
        case 0: body_type = Body_Type::Static;       break;
        case 1: body_type = Body_Type::Kinematic;    break;
        case 2: body_type = Body_Type::Dynamic;      break;
    }

    // ***** Add the block to the cpSpace
    DrEngineObject *block = new DrEngineObject(this, getNextKey(), thing->getKey(), body_type, asset_key,
                                               x + x_offset, y + y_offset, info.z_order, info.scale,
                                               use_friction, use_bounce, collide, can_rotate, info.angle, info.opacity);
    loadThingCollisionShape(asset, block);
    block->setCollidesWith(static_cast<Collision_Groups>(collide_with));

    // ***** Set collision type
    Collision_Type collision_type = Collision_Type::Damage_None;
    long           damage_type = thing->getComponentPropertyValue(Components::Thing_Settings_Object, Properties::Thing_Object_Damage).toInt();
    switch (damage_type) {
        case 0: collision_type = Collision_Type::Damage_None;   break;
        case 1: collision_type = Collision_Type::Damage_Player; break;
        case 2: collision_type = Collision_Type::Damage_Enemy;  break;
        case 3: collision_type = Collision_Type::Damage_All;    break;
    }
    block->setCollisionType(collision_type);

    // ***** Physics settings
    if (Dr::FuzzyCompare(rotate_speed, 0.0) == false) {
        block->setCanRotate( true );
        block->setRotateSpeedZ(rotate_speed);
    }
    block->setIgnoreGravity( !feels_gravity );

    // ***** Velocity settings
    QPointF vel_x = thing->getComponentPropertyValue(Components::Thing_Movement, Properties::Thing_Velocity_X).toPointF();
    QPointF vel_y = thing->getComponentPropertyValue(Components::Thing_Movement, Properties::Thing_Velocity_Y).toPointF();
    QPointF rotation_vel =      thing->getComponentPropertyValue(Components::Thing_Movement, Properties::Thing_Spin_Velocity).toPointF();
    bool    angle_velocity =    thing->getComponentPropertyValue(Components::Thing_Movement, Properties::Thing_Angle_Velocity).toBool();
    bool    angle_player =      thing->getComponentPropertyValue(Components::Thing_Movement, Properties::Thing_Angle_Player).toBool();

    cpVect velocity;
    velocity.x = vel_x.x() + (QRandomGenerator::global()->bounded(vel_x.y() * 2.0) - vel_x.y());
    velocity.y = vel_y.x() + (QRandomGenerator::global()->bounded(vel_y.y() * 2.0) - vel_y.y());
    double deg_angular = rotation_vel.x() + (QRandomGenerator::global()->bounded(rotation_vel.y() * 2.0) - rotation_vel.y());
    double rad_angular = qDegreesToRadians( deg_angular );

    // Attach KinematicUpdateVelocity callback function
    if (body_type == Body_Type::Kinematic) {
        block->setOriginalVelocityX( velocity.x + x_velocity );
        block->setOriginalVelocityY( velocity.y + y_velocity );
        block->setOriginalSpinVelocity( rad_angular );
        block->setUseAngleVelocity( angle_velocity );
        block->setRotateToPlayer( angle_player );
    }


    // ***** Additional Collision Settings
    int     one_way_type =  asset->getComponentPropertyValue(Components::Asset_Collision, Properties::Asset_Collision_One_Way_Type).toInt();
    double  one_way_angle = asset->getComponentPropertyValue(Components::Asset_Collision, Properties::Asset_Collision_One_Way_Direction).toDouble();
    double  gravity_multi = asset->getComponentPropertyValue(Components::Asset_Collision, Properties::Asset_Collision_Gravity_Multiplier).toDouble();
    QPointF surface_vel =   asset->getComponentPropertyValue(Components::Asset_Collision, Properties::Asset_Collision_Surface_Velocity).toPointF();

    t.reset();
    t.rotate(one_way_angle);
    QPointF one_way_point = t.map(QPoint(0.0, 1.0));
    block->setOneWay(static_cast<One_Way>(one_way_type));
    block->setOneWayDirection(DrPointF(one_way_point.x(), one_way_point.y()));
    block->setGravityMultiplier(gravity_multi);
    block->setSurfaceVelocity(cpv(surface_vel.x(), surface_vel.y()));

    // ***** Appearance settings
    loadThingAppearanceSettings(thing, block);

    // ***** Health / Damage Settings
    loadThingHealthSettings(asset, block);

    // ***** 3D Settings
    loadThing3DSettings(thing, block);


    // ********** Add to world
    addThing(block);

    // ***** Add Velocity, HAVE TO WAIT until is in world!!
    if (body_type != Body_Type::Static) {
        cpBodySetVelocity( block->body, velocity + cpv(x_velocity, y_velocity) );
        cpBodySetAngularVelocity( block->body, rad_angular );
    }

    return block;
}




























