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
void DrEngineWorld::loadThing3DSettings(DrThing *thing, DrEngineObject *object) {
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
    bool    wireframe =     thing->getComponentPropertyValue(Components::Thing_Special_Effects, Properties::Thing_Filter_Wireframe).toBool();
    object->cast_shadows =  cast_shadows;
    object->bitrate =       bit_rate;
    object->pixel_x =       static_cast<float>(pixelation.x());
    object->pixel_y =       static_cast<float>(pixelation.y());
    object->brightness =    brightness;
    object->contrast =      contrast;
    object->saturation =    saturation;
    object->hue =           hue;
    object->grayscale =     grayscale;
    object->negative =      negative;
    object->wireframe =     wireframe;
}

//####################################################################################
//##    Loads Collision Shape from DrThing in DrProject to DrEngineObject
//####################################################################################
void DrEngineWorld::loadThingCollisionShape(DrEngineObject *object) {
    ///block->addShapeBoxFromTexture(asset_key);
    QVariant shapes = m_project->getAsset(object->getAssetKey())->getComponentPropertyValue(Components::Asset_Collision, Properties::Asset_Collision_Shape);
    DrShapeList shape = shapes.value<DrShapeList>();
    for (auto poly : shape.getPolygons()) {
        QVector<DrPointF> points = QVector<DrPointF>::fromStdVector(poly);
        object->addShapePolygon(points);
    }
}


//####################################################################################
//##    Loads one DrProject DrThingType::Object to World / Space
//####################################################################################
void DrEngineWorld::loadCharacterToWorld(DrThing *thing) {

    // ***** Load Character Thing Properties
    long        asset_key = thing->getAssetKey();
    DrAsset    *asset = m_project->getAsset(asset_key);
    ThingInfo   info =      loadThingBasicInfo( thing );

    // ***** Load Character Settings
    QPointF max_speed =     asset->getComponentPropertyValue(Components::Asset_Settings_Character, Properties::Asset_Character_Max_Speed).toPointF();
    QPointF forced_speed =  asset->getComponentPropertyValue(Components::Asset_Settings_Character, Properties::Asset_Character_Forced_Speed).toPointF();
    QPointF move_speed =    asset->getComponentPropertyValue(Components::Asset_Settings_Character, Properties::Asset_Character_Move_Speed).toPointF();

    QPointF jump_force =    asset->getComponentPropertyValue(Components::Asset_Settings_Character, Properties::Asset_Character_Jump_Force).toPointF();
    int     jump_timeout =  asset->getComponentPropertyValue(Components::Asset_Settings_Character, Properties::Asset_Character_Jump_Timeout).toInt();
    int     jump_count =    asset->getComponentPropertyValue(Components::Asset_Settings_Character, Properties::Asset_Character_Jump_Counter).toInt();
    bool    jump_air =      asset->getComponentPropertyValue(Components::Asset_Settings_Character, Properties::Asset_Character_Jump_Air).toBool();
    bool    jump_wall =     asset->getComponentPropertyValue(Components::Asset_Settings_Character, Properties::Asset_Character_Jump_Wall).toBool();

    double  air_drag =      asset->getComponentPropertyValue(Components::Asset_Settings_Character, Properties::Asset_Character_Air_Drag).toDouble();
    double  ground_drag =   asset->getComponentPropertyValue(Components::Asset_Settings_Character, Properties::Asset_Character_Ground_Drag).toDouble();
    double  rotate_drag =   asset->getComponentPropertyValue(Components::Asset_Settings_Character, Properties::Asset_Character_Rotation_Drag).toDouble();

    bool    can_rotate =    asset->getComponentPropertyValue(Components::Asset_Settings_Character, Properties::Asset_Character_Can_Rotate).toBool();
    bool    feels_gravity = asset->getComponentPropertyValue(Components::Asset_Settings_Character, Properties::Asset_Character_Feels_Gravity).toBool();

    // ***** Add the player to the cpSpace
    DrEngineObject *player = new DrEngineObject(this, getNextKey(), Body_Type::Dynamic, asset_key,
                                                info.position.x, -info.position.y, info.z_order,
                                                info.scale, c_friction, c_bounce,
                                                c_collide_true, can_rotate, info.angle, info.opacity);
    loadThingCollisionShape(player);                                    // Load collision shape(s)
    addThing(player);                                                   // Add to world
    assignPlayerControls(player, true, true, true);

    // ***** Apply Character Settings
    player->setMaxSpeedX( max_speed.x() );
    player->setMaxSpeedY( max_speed.y() );
    player->setForcedSpeedX( forced_speed.x() );
    player->setForcedSpeedY( forced_speed.y() );
    player->setMoveSpeedX( move_speed.x() );
    player->setMoveSpeedY( move_speed.y() );

    player->setJumpForceX( jump_force.x() );
    player->setJumpForceY( jump_force.y() );
    player->setJumpTimeout( jump_timeout );
    player->setJumpCount( jump_count );
    player->setCanAirJump( jump_air );
    player->setCanWallJump( jump_wall );

    player->setAirDrag( air_drag );
    player->setGroundDrag( ground_drag );
    player->setRotateDrag( rotate_drag );

    player->setIgnoreGravity( !feels_gravity );

    // ***** Appearance settings
    loadThingAppearanceSettings(thing, player);

    // ***** 3D Settings
    loadThing3DSettings(thing, player);
}


//####################################################################################
//##    Loads one DrProject DrThingType::Object to World / Space
//####################################################################################
void DrEngineWorld::loadObjectToWorld(DrThing *thing, double offset_x, double offset_y) {

    // ***** Load Object Thing Properties
    long        asset_key = thing->getAssetKey();
    DrAsset    *asset =     m_project->getAsset(asset_key);
    ThingInfo   info =      loadThingBasicInfo( thing );
    bool        collide =   thing->getComponentPropertyValue(Components::Thing_Settings_Object,  Properties::Thing_Object_Collide).toBool();
    int         physics =   thing->getComponentPropertyValue(Components::Thing_Settings_Object,  Properties::Thing_Object_Physics_Type).toInt();

    Body_Type body_type = Body_Type::Static;
    switch (physics) {
        case 0: body_type = Body_Type::Static;       break;
        case 1: body_type = Body_Type::Kinematic;    break;
        case 2: body_type = Body_Type::Dynamic;      break;
    }

    // ***** Add the block to the cpSpace
    DrEngineObject *block = new DrEngineObject(this, getNextKey(), body_type, asset_key, info.position.x + offset_x, -info.position.y + offset_y,
                                               info.z_order, info.scale, c_friction, c_bounce, collide, true, info.angle, info.opacity);
    loadThingCollisionShape(block);
    addThing(block);

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

    // ***** Velocity settings
    QPointF vel_x = thing->getComponentPropertyValue(Components::Thing_Movement, Properties::Thing_Velocity_X).toPointF();
    QPointF vel_y = thing->getComponentPropertyValue(Components::Thing_Movement, Properties::Thing_Velocity_Y).toPointF();
    QPointF rotation_vel =  thing->getComponentPropertyValue(Components::Thing_Movement, Properties::Thing_Spin_Velocity).toPointF();
    bool    angle_velocty = thing->getComponentPropertyValue(Components::Thing_Movement, Properties::Thing_Angle_Velocity).toBool();

    cpVect velocity;
    velocity.x = vel_x.x() + (QRandomGenerator::global()->bounded(vel_x.y() * 2.0) - vel_x.y());
    velocity.y = vel_y.x() + (QRandomGenerator::global()->bounded(vel_y.y() * 2.0) - vel_y.y());
    double deg_angular = rotation_vel.x() + (QRandomGenerator::global()->bounded(rotation_vel.y() * 2.0) - rotation_vel.y());
    double rad_angular = qDegreesToRadians( deg_angular );
    if (body_type != Body_Type::Static) {
        cpBodySetVelocity( block->body, velocity );
        cpBodySetAngularVelocity( block->body, rad_angular );

        // Attach KinematicUpdateVelocity callback function
        if (body_type == Body_Type::Kinematic) {
            block->setOriginalVelocityX( velocity.x );
            block->setOriginalVelocityY( velocity.y );
            block->setOriginalSpinVelocity( rad_angular );
            block->setUseAngleVelocity( angle_velocty );
            cpBodySetVelocityUpdateFunc(block->body, KinematicUpdateVelocity);
        }
    }

    // ***** Additional Collision Settings
    int    one_way_type =  asset->getComponentPropertyValue(Components::Asset_Collision, Properties::Asset_Collision_One_Way_Type).toInt();
    double one_way_angle = asset->getComponentPropertyValue(Components::Asset_Collision, Properties::Asset_Collision_One_Way_Direction).toDouble();
    double gravity_multi = asset->getComponentPropertyValue(Components::Asset_Collision, Properties::Asset_Collision_Gravity_Multiplier).toDouble();
    QTransform t; t.rotate(one_way_angle);
    QPointF one_way_point = t.map(QPoint(0.0, 1.0));
    block->setOneWay(static_cast<One_Way>(one_way_type));
    block->setOneWayDirection(DrPointF(one_way_point.x(), one_way_point.y()));
    block->setGravityMultiplier(gravity_multi);


    // ***** Appearance settings
    loadThingAppearanceSettings(thing, block);

    // ***** 3D Settings
    loadThing3DSettings(thing, block);
}




























