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

#include "engine.h"
#include "engine_things/engine_thing_fire.h"
#include "engine_things/engine_thing_fisheye.h"
#include "engine_things/engine_thing_light.h"
#include "engine_things/engine_thing_mirror.h"
#include "engine_things/engine_thing_object.h"
#include "engine_things/engine_thing_swirl.h"
#include "engine_things/engine_thing_water.h"
#include "engine_world.h"
#include "project/project.h"
#include "project/project_asset.h"
#include "project/project_world_stage.h"
#include "project/project_world_stage_thing.h"
#include "project/thing_shape_list.h"

// Internal Constants
const double    c_light_size_adjuster = 1.15;           // Multiplier to equal out the slight rendering difference between shader and pixmap drawing functions


//####################################################################################
//##    Load DrProject Stage to World / Space
//####################################################################################
void DrEngineWorld::loadStageToWorld(DrStage *stage, double offset_x, double offset_y) {

    // ***** Load Things
    for (auto thing_pair : stage->getThingMap()) {

        // Grab current Thing
        DrThing *thing = thing_pair.second;

        // Check if Thing type, if it is, call appropriate Thing Loader
        if (thing->getType() != DrType::Thing) continue;

        switch (thing->getThingType()) {
            // Load Thing
            case DrThingType::Object:   loadObjectToWorld(thing, offset_x, offset_y);       break;

            // Load Thing Effect
            case DrThingType::Fire:     loadFireToWorld(thing, offset_x, offset_y);         break;
            case DrThingType::Fisheye:  loadFisheyeToWorld(thing, offset_x, offset_y);      break;
            case DrThingType::Light:    loadLightToWorld(thing, offset_x, offset_y);        break;
            case DrThingType::Mirror:   loadMirrorToWorld(thing, offset_x, offset_y);       break;
            case DrThingType::Swirl:    loadSwirlToWorld(thing, offset_x, offset_y);        break;
            case DrThingType::Water:    loadWaterToWorld(thing, offset_x, offset_y);        break;

        default:
            break;
        }

    }

    // ***** Update distance we've loaded scenes to
    m_loaded_to += stage->getComponentPropertyValue(Components::Stage_Settings, Properties::Stage_Size).toInt();
}


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
    info.z_order =  thing->getComponentPropertyValue(Components::Thing_Layering,  Properties::Thing_Z_Order).toDouble();
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

    // ***** Appearance settings
    loadThingAppearanceSettings(thing, block);

    // ***** 3D Settings
    loadThing3DSettings(thing, block);
}


//####################################################################################
//##    Loads one DrProject DrThingType::Fisheye to World / Space
//####################################################################################
void DrEngineWorld::loadFisheyeToWorld(DrThing *thing, double offset_x, double offset_y) {
    ThingInfo   info =              loadThingBasicInfo( thing );

    QColor      start_color =       QColor::fromRgba(thing->getComponentPropertyValue(Components::Thing_Settings_Fisheye, Properties::Thing_Fisheye_Color).toUInt());
    float       tint =              thing->getComponentPropertyValue(Components::Thing_Settings_Fisheye, Properties::Thing_Fisheye_Color_Tint).toFloat() / 100.0f;
    float       zoom =              thing->getComponentPropertyValue(Components::Thing_Settings_Fisheye, Properties::Thing_Fisheye_Lens_Zoom).toList().first().toFloat();

    float       bit_rate =          thing->getComponentPropertyValue(Components::Thing_Appearance, Properties::Thing_Filter_Bitrate).toList().first().toInt();
    QPointF     pixelation =        thing->getComponentPropertyValue(Components::Thing_Appearance, Properties::Thing_Filter_Pixelation).toPointF();

    DrEngineFisheye *lens = new DrEngineFisheye(this, getNextKey(), info.position.x + offset_x, -info.position.y + offset_y,
                                                info.z_order, info.angle, info.opacity, info.size,
                                                start_color, tint, zoom );
    addThing( lens );
    lens->bitrate = bit_rate;
    lens->pixel_x = static_cast<float>(pixelation.x());
    lens->pixel_y = static_cast<float>(pixelation.y());
}


//####################################################################################
//##    Loads one DrProject DrThingType::Fire to World / Space
//####################################################################################
void DrEngineWorld::loadFireToWorld(DrThing *thing, double offset_x, double offset_y) {
    ThingInfo   info =              loadThingBasicInfo( thing );

    int         mask =              thing->getComponentPropertyValue(Components::Thing_Settings_Fire, Properties::Thing_Fire_Shape).toInt();
    QColor      color_1 =           QColor::fromRgba(thing->getComponentPropertyValue(Components::Thing_Settings_Fire, Properties::Thing_Fire_Color_1).toUInt());
    QColor      color_2 =           QColor::fromRgba(thing->getComponentPropertyValue(Components::Thing_Settings_Fire, Properties::Thing_Fire_Color_2).toUInt());
    QColor      smoke =             QColor::fromRgba(thing->getComponentPropertyValue(Components::Thing_Settings_Fire, Properties::Thing_Fire_Color_Smoke).toUInt());
    float       intensity =         thing->getComponentPropertyValue(Components::Thing_Settings_Fire, Properties::Thing_Fire_Intensity).toFloat() / 100.f;
    float       smooth =            thing->getComponentPropertyValue(Components::Thing_Settings_Fire, Properties::Thing_Fire_Smoothness).toFloat() / 100.f;
    float       wave =              thing->getComponentPropertyValue(Components::Thing_Settings_Fire, Properties::Thing_Fire_Wavy).toFloat() / 100.f;
    float       speed =             thing->getComponentPropertyValue(Components::Thing_Settings_Fire, Properties::Thing_Fire_Speed).toFloat();

    float       bit_rate =          thing->getComponentPropertyValue(Components::Thing_Appearance, Properties::Thing_Filter_Bitrate).toList().first().toInt();
    QPointF     pixelation =        thing->getComponentPropertyValue(Components::Thing_Appearance, Properties::Thing_Filter_Pixelation).toPointF();

    // Find out original QPixmap size used in editor
    info.size.x = info.size.x / info.scale.x;
    info.size.y = info.size.y / info.scale.y;
    DrEngineFire *fire = new DrEngineFire(this, getNextKey(), info.position.x + offset_x, -info.position.y + offset_y,
                                          info.z_order, info.scale, info.angle, info.opacity, info.size,
                                          static_cast<Fire_Mask>(mask), color_1, color_2, smoke, intensity, smooth, wave, speed);
    addThing( fire );
    fire->bitrate = bit_rate;
    fire->pixel_x = static_cast<float>(pixelation.x());
    fire->pixel_y = static_cast<float>(pixelation.y());
}


//####################################################################################
//##    Loads one DrProject DrThingType::Light to World / Space
//####################################################################################
void DrEngineWorld::loadLightToWorld(DrThing *thing, double offset_x, double offset_y) {
    ThingInfo   info =          loadThingBasicInfo( thing );
    QColor      light_color =   QColor::fromRgba(thing->getComponentPropertyValue(Components::Thing_Settings_Light, Properties::Thing_Light_Color).toUInt());
    int         light_type =    thing->getComponentPropertyValue(Components::Thing_Settings_Light, Properties::Thing_Light_Type).toInt();

    double      cone_start =    thing->getComponentPropertyValue(Components::Thing_Settings_Light, Properties::Thing_Light_Cone_Start).toList().first().toDouble();
    double      cone_end =      thing->getComponentPropertyValue(Components::Thing_Settings_Light, Properties::Thing_Light_Cone_End).toList().first().toDouble();
    float       intensity =     thing->getComponentPropertyValue(Components::Thing_Settings_Light, Properties::Thing_Light_Intensity).toFloat();
    float       blur =          thing->getComponentPropertyValue(Components::Thing_Settings_Light, Properties::Thing_Light_Blur).toFloat();
    float       shadows =       thing->getComponentPropertyValue(Components::Thing_Settings_Light, Properties::Thing_Light_Shadows).toFloat();
    bool        draw_shadows =  thing->getComponentPropertyValue(Components::Thing_Settings_Light, Properties::Thing_Light_Draw_Shadows).toBool();

    float       pulse =         thing->getComponentPropertyValue(Components::Thing_Settings_Light, Properties::Thing_Light_Pulse).toFloat();
    float       pulse_speed =   thing->getComponentPropertyValue(Components::Thing_Settings_Light, Properties::Thing_Light_Pulse_Speed).toFloat();

    info.size.x = info.size.x * c_light_size_adjuster;

    addThing( new DrEngineLight(this, getNextKey(), info.position.x + offset_x, -info.position.y + offset_y, info.z_order, info.opacity,
                                static_cast<Light_Type>(light_type),
                                light_color, static_cast<float>(info.size.x),
                                DrPointF(cone_start, cone_end), intensity, shadows, draw_shadows, blur, pulse, pulse_speed));
}


//####################################################################################
//##    Loads one DrProject DrThingType::Mirror to World / Space
//####################################################################################
void DrEngineWorld::loadMirrorToWorld(DrThing *thing, double offset_x, double offset_y) {
    ThingInfo   info =              loadThingBasicInfo( thing );

    QColor color_1 = QColor::fromRgba(thing->getComponentPropertyValue(Components::Thing_Settings_Mirror, Properties::Thing_Mirror_Start_Color).toUInt());
    QColor color_2 = QColor::fromRgba(thing->getComponentPropertyValue(Components::Thing_Settings_Mirror, Properties::Thing_Mirror_End_Color).toUInt());
    float       color_tint =        thing->getComponentPropertyValue(Components::Thing_Settings_Mirror, Properties::Thing_Mirror_Color_Tint).toFloat() / 100.0f;
    float       blur =              thing->getComponentPropertyValue(Components::Thing_Settings_Mirror, Properties::Thing_Mirror_Blur).toFloat();
    float       blur_stretch =      thing->getComponentPropertyValue(Components::Thing_Settings_Mirror, Properties::Thing_Mirror_Blur_Stretch).toFloat();
    float       scale =             thing->getComponentPropertyValue(Components::Thing_Settings_Mirror, Properties::Thing_Mirror_Scale).toList().first().toFloat();

    float       bit_rate =          thing->getComponentPropertyValue(Components::Thing_Appearance, Properties::Thing_Filter_Bitrate).toList().first().toInt();
    QPointF     pixelation =        thing->getComponentPropertyValue(Components::Thing_Appearance, Properties::Thing_Filter_Pixelation).toPointF();

    DrEngineMirror *mirror = new DrEngineMirror(this, getNextKey(), info.position.x + offset_x, -info.position.y + offset_y,
                                                info.z_order, info.angle, info.opacity, info.size,
                                                color_1, color_2, color_tint, blur, blur_stretch, scale );
    addThing( mirror );
    mirror->bitrate = bit_rate;
    mirror->pixel_x = static_cast<float>(pixelation.x());
    mirror->pixel_y = static_cast<float>(pixelation.y());
}


//####################################################################################
//##    Loads one DrProject DrThingType::Swirl to World / Space
//####################################################################################
void DrEngineWorld::loadSwirlToWorld(DrThing *thing, double offset_x, double offset_y) {
    ThingInfo   info =              loadThingBasicInfo( thing );

    QColor      start_color =       QColor::fromRgba(thing->getComponentPropertyValue(Components::Thing_Settings_Swirl, Properties::Thing_Swirl_Start_Color).toUInt());
    float       tint =              thing->getComponentPropertyValue(Components::Thing_Settings_Swirl, Properties::Thing_Swirl_Color_Tint).toFloat() / 100.0f;
    float       rotation =          thing->getComponentPropertyValue(Components::Thing_Settings_Swirl, Properties::Thing_Swirl_Angle).toFloat();

    float       bit_rate =          thing->getComponentPropertyValue(Components::Thing_Appearance, Properties::Thing_Filter_Bitrate).toList().first().toInt();
    QPointF     pixelation =        thing->getComponentPropertyValue(Components::Thing_Appearance, Properties::Thing_Filter_Pixelation).toPointF();

    DrEngineSwirl *swirl = new DrEngineSwirl(this, getNextKey(), info.position.x + offset_x, -info.position.y + offset_y,
                                             info.z_order, info.angle, info.opacity, info.size,
                                             start_color, tint, rotation );
    addThing( swirl );
    swirl->bitrate = bit_rate;
    swirl->pixel_x = static_cast<float>(pixelation.x());
    swirl->pixel_y = static_cast<float>(pixelation.y());
}


//####################################################################################
//##    Loads one DrProject DrThingType::Water to World / Space
//####################################################################################
void DrEngineWorld::loadWaterToWorld(DrThing *thing, double offset_x, double offset_y) {   
    ThingInfo   info =              loadThingBasicInfo( thing );

    int         texture =           thing->getComponentPropertyValue(Components::Thing_Settings_Water, Properties::Thing_Water_Texture).toInt();
    QColor      start_color =       QColor::fromRgba(thing->getComponentPropertyValue(Components::Thing_Settings_Water, Properties::Thing_Water_Start_Color).toUInt());
    QColor      end_color =         QColor::fromRgba(thing->getComponentPropertyValue(Components::Thing_Settings_Water, Properties::Thing_Water_End_Color).toUInt());
    float       water_tint =        thing->getComponentPropertyValue(Components::Thing_Settings_Water, Properties::Thing_Water_Color_Tint).toFloat() / 100.0f;
    float       reflection =        thing->getComponentPropertyValue(Components::Thing_Settings_Water, Properties::Thing_Water_Reflection).toFloat() / 100.0f;
    float       move_speed =        thing->getComponentPropertyValue(Components::Thing_Settings_Water, Properties::Thing_Water_Movement_Speed).toFloat();

    float       ripple_freq =       thing->getComponentPropertyValue(Components::Thing_Settings_Water_Ripple, Properties::Thing_Water_Ripple_Frequency).toFloat();
    float       ripple_speed =      thing->getComponentPropertyValue(Components::Thing_Settings_Water_Ripple, Properties::Thing_Water_Ripple_Speed).toFloat();
    float       ripple_amplitude =  thing->getComponentPropertyValue(Components::Thing_Settings_Water_Ripple, Properties::Thing_Water_Ripple_Amplitude).toFloat();
    float       ripple_stretch =    thing->getComponentPropertyValue(Components::Thing_Settings_Water_Ripple, Properties::Thing_Water_Ripple_Stretch).toFloat();

    float       wave_freq =         thing->getComponentPropertyValue(Components::Thing_Settings_Water_Wave, Properties::Thing_Water_Wave_Frequency).toFloat();
    float       wave_speed =        thing->getComponentPropertyValue(Components::Thing_Settings_Water_Wave, Properties::Thing_Water_Wave_Speed).toFloat();
    float       wave_amplitude =    thing->getComponentPropertyValue(Components::Thing_Settings_Water_Wave, Properties::Thing_Water_Wave_Amplitude).toFloat();

    float       refract_1 =         thing->getComponentPropertyValue(Components::Thing_Settings_Water_Refract, Properties::Thing_Water_Refract_Reflection).toFloat();
    float       refract_2 =         thing->getComponentPropertyValue(Components::Thing_Settings_Water_Refract, Properties::Thing_Water_Refract_Underwater).toFloat();
    float       refract_3 =         thing->getComponentPropertyValue(Components::Thing_Settings_Water_Refract, Properties::Thing_Water_Refract_Texture).toFloat();
    float       foam_refract =      thing->getComponentPropertyValue(Components::Thing_Settings_Water_Refract, Properties::Thing_Water_Refract_Foam).toFloat();

    QColor      foam_color =    QColor::fromRgba(thing->getComponentPropertyValue(Components::Thing_Settings_Water_Foam, Properties::Thing_Water_Surface_Color).toUInt());
    float       foam_tint =     thing->getComponentPropertyValue(Components::Thing_Settings_Water_Foam, Properties::Thing_Water_Surface_Tint).toFloat() / 100.0f;
    float       foam_height =   thing->getComponentPropertyValue(Components::Thing_Settings_Water_Foam, Properties::Thing_Water_Surface_Height).toFloat();
    bool        foam_flat =     thing->getComponentPropertyValue(Components::Thing_Settings_Water_Foam, Properties::Thing_Water_Surface_Is_Flat).toBool();

    float       bit_rate =          thing->getComponentPropertyValue(Components::Thing_Appearance, Properties::Thing_Filter_Bitrate).toList().first().toInt();
    QPointF     pixelation =        thing->getComponentPropertyValue(Components::Thing_Appearance, Properties::Thing_Filter_Pixelation).toPointF();

    DrEngineWater *water = new DrEngineWater(this, getNextKey(), info.position.x + offset_x, -info.position.y + offset_y,
                                             info.z_order, info.angle, info.opacity, info.size,
                                             static_cast<Water_Texture>(texture), start_color, end_color,
                                             water_tint, reflection,
                                             ripple_freq, ripple_speed, ripple_amplitude, ripple_stretch,
                                             wave_freq, wave_speed, wave_amplitude,
                                             foam_color, foam_tint, foam_height, foam_flat,
                                             refract_1, refract_2, refract_3, foam_refract, move_speed );
    addThing( water );
    water->bitrate = bit_rate;
    water->pixel_x = static_cast<float>(pixelation.x());
    water->pixel_y = static_cast<float>(pixelation.y());
}








