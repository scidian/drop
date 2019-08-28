//
//      Created by Stephens Nunnally on 5/15/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QtMath>
#include <QRandomGenerator>

#include "engine.h"
#include "engine_thing_fire.h"
#include "engine_thing_fisheye.h"
#include "engine_thing_light.h"
#include "engine_thing_mirror.h"
#include "engine_thing_object.h"
#include "engine_thing_swirl.h"
#include "engine_thing_water.h"
#include "engine_world.h"
#include "project/project_world_stage.h"
#include "project/project_world_stage_thing.h"

// Local Constants
const double    c_light_size_adjuster = 1.15;           // Multiplier to equal out the slight rendering difference between shader and pixmap drawing functions


//####################################################################################
//##    Loads Basic DrThing Info from LAyering / Transform components
//####################################################################################
ThingInfo DrEngineWorld::getThingBasicInfo(DrThing *thing) {
    ThingInfo info;
    info.angle =    thing->getComponentPropertyValue(Components::Thing_Transform, Properties::Thing_Rotation).toDouble();
    info.position = thing->getComponentPropertyValue(Components::Thing_Transform, Properties::Thing_Position).toPointF();
    info.scale =    thing->getComponentPropertyValue(Components::Thing_Transform, Properties::Thing_Scale).toPointF();
    info.size =     thing->getComponentPropertyValue(Components::Thing_Transform, Properties::Thing_Size).toPointF();
    info.opacity =  thing->getComponentPropertyValue(Components::Thing_Layering,  Properties::Thing_Opacity).toFloat() / 100.0f;
    info.z_order =  thing->getComponentPropertyValue(Components::Thing_Layering,  Properties::Thing_Z_Order).toDouble();
    return info;
}


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
//##    Loads one DrProject DrThingType::Object to World / Space
//####################################################################################
void DrEngineWorld::loadObjectToWorld(DrThing *thing, double offset_x, double offset_y) {

    // ***** Load Object Thing Properties
    long        asset_key = thing->getAssetKey();
    ThingInfo   info =      getThingBasicInfo( thing );
    bool        collide =   thing->getComponentPropertyValue(Components::Thing_Settings_Object,  Properties::Thing_Object_Collide).toBool();
    int         physics =   thing->getComponentPropertyValue(Components::Thing_Settings_Object,  Properties::Thing_Object_Physics_Type).toInt();

    Body_Type body_type = Body_Type::Static;
    switch (physics) {
        case 0: body_type = Body_Type::Static;       break;
        case 1: body_type = Body_Type::Kinematic;    break;
        case 2: body_type = Body_Type::Dynamic;      break;
    }

    // ***** Add the block to the cpSpace
    DrEngineObject *block = new DrEngineObject(this, getNextKey(), body_type, asset_key, info.position.x() + offset_x, -info.position.y() + offset_y,
                                               info.z_order, info.scale, c_friction, c_bounce, collide, true, info.angle, info.opacity);
    block->addShapeBoxFromTexture(asset_key);
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
    bool    cast_shadows =  thing->getComponentPropertyValue(Components::Thing_Lighting,   Properties::Thing_Lighting_Cast_Shadows).toBool();
    int     bit_rate =      thing->getComponentPropertyValue(Components::Thing_Appearance, Properties::Thing_Filter_Bitrate).toList().first().toInt();
    QPointF pixelation =    thing->getComponentPropertyValue(Components::Thing_Appearance, Properties::Thing_Filter_Pixelation).toPointF();
    float   brightness =    thing->getComponentPropertyValue(Components::Thing_Appearance, Properties::Thing_Filter_Brightness).toList().first().toInt() / 255.f;
    float   contrast =      thing->getComponentPropertyValue(Components::Thing_Appearance, Properties::Thing_Filter_Contrast).toList().first().toInt() / 255.f;
    float   saturation =    thing->getComponentPropertyValue(Components::Thing_Appearance, Properties::Thing_Filter_Saturation).toList().first().toInt() / 255.f;
    float   hue =           thing->getComponentPropertyValue(Components::Thing_Appearance, Properties::Thing_Filter_Hue).toList().first().toInt() / 360.f;
    bool    grayscale =     thing->getComponentPropertyValue(Components::Thing_Appearance, Properties::Thing_Filter_Grayscale).toBool();
    bool    negative =      thing->getComponentPropertyValue(Components::Thing_Appearance, Properties::Thing_Filter_Negative).toBool();
    block->cast_shadows =   cast_shadows;
    block->bitrate =        bit_rate;
    block->pixel_x =        static_cast<float>(pixelation.x());
    block->pixel_y =        static_cast<float>(pixelation.y());
    block->brightness =     brightness;
    block->contrast =       contrast;
    block->saturation =     saturation;
    block->hue =            hue;
    block->grayscale =      grayscale;
    block->negative =       negative;
}


//####################################################################################
//##    Loads one DrProject DrThingType::Fisheye to World / Space
//####################################################################################
void DrEngineWorld::loadFisheyeToWorld(DrThing *thing, double offset_x, double offset_y) {
    ThingInfo   info =              getThingBasicInfo( thing );

    QColor      start_color =       QColor::fromRgba(thing->getComponentPropertyValue(Components::Thing_Settings_Fisheye, Properties::Thing_Fisheye_Color).toUInt());
    float       tint =              thing->getComponentPropertyValue(Components::Thing_Settings_Fisheye, Properties::Thing_Fisheye_Color_Tint).toFloat() / 100.0f;
    float       zoom =              thing->getComponentPropertyValue(Components::Thing_Settings_Fisheye, Properties::Thing_Fisheye_Lens_Zoom).toList().first().toFloat();

    float       bit_rate =          thing->getComponentPropertyValue(Components::Thing_Appearance, Properties::Thing_Filter_Bitrate).toList().first().toInt();
    QPointF     pixelation =        thing->getComponentPropertyValue(Components::Thing_Appearance, Properties::Thing_Filter_Pixelation).toPointF();

    DrEngineFisheye *lens = new DrEngineFisheye(this, getNextKey(), info.position.x() + offset_x, -info.position.y() + offset_y,
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
    ThingInfo   info =              getThingBasicInfo( thing );

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
    info.size.setX( info.size.x() / info.scale.x() );
    info.size.setY( info.size.y() / info.scale.y() );
    DrEngineFire *fire = new DrEngineFire(this, getNextKey(), info.position.x() + offset_x, -info.position.y() + offset_y,
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
    ThingInfo   info =          getThingBasicInfo( thing );
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

    info.size.setX( info.size.x() * c_light_size_adjuster);

    addThing( new DrEngineLight(this, getNextKey(), info.position.x() + offset_x, -info.position.y() + offset_y, info.z_order, info.opacity,
                                static_cast<Light_Type>(light_type),
                                light_color, static_cast<float>(info.size.x()),
                                QPointF(cone_start, cone_end), intensity, shadows, draw_shadows, blur, pulse, pulse_speed));
}


//####################################################################################
//##    Loads one DrProject DrThingType::Mirror to World / Space
//####################################################################################
void DrEngineWorld::loadMirrorToWorld(DrThing *thing, double offset_x, double offset_y) {
    ThingInfo   info =              getThingBasicInfo( thing );

    QColor color_1 = QColor::fromRgba(thing->getComponentPropertyValue(Components::Thing_Settings_Mirror, Properties::Thing_Mirror_Start_Color).toUInt());
    QColor color_2 = QColor::fromRgba(thing->getComponentPropertyValue(Components::Thing_Settings_Mirror, Properties::Thing_Mirror_End_Color).toUInt());
    float       color_tint =        thing->getComponentPropertyValue(Components::Thing_Settings_Mirror, Properties::Thing_Mirror_Color_Tint).toFloat() / 100.0f;
    float       blur =              thing->getComponentPropertyValue(Components::Thing_Settings_Mirror, Properties::Thing_Mirror_Blur).toFloat();
    float       blur_stretch =      thing->getComponentPropertyValue(Components::Thing_Settings_Mirror, Properties::Thing_Mirror_Blur_Stretch).toFloat();
    float       scale =             thing->getComponentPropertyValue(Components::Thing_Settings_Mirror, Properties::Thing_Mirror_Scale).toList().first().toFloat();

    float       bit_rate =          thing->getComponentPropertyValue(Components::Thing_Appearance, Properties::Thing_Filter_Bitrate).toList().first().toInt();
    QPointF     pixelation =        thing->getComponentPropertyValue(Components::Thing_Appearance, Properties::Thing_Filter_Pixelation).toPointF();

    DrEngineMirror *mirror = new DrEngineMirror(this, getNextKey(), info.position.x() + offset_x, -info.position.y() + offset_y,
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
    ThingInfo   info =              getThingBasicInfo( thing );

    QColor      start_color =       QColor::fromRgba(thing->getComponentPropertyValue(Components::Thing_Settings_Swirl, Properties::Thing_Swirl_Start_Color).toUInt());
    float       tint =              thing->getComponentPropertyValue(Components::Thing_Settings_Swirl, Properties::Thing_Swirl_Color_Tint).toFloat() / 100.0f;
    float       rotation =          thing->getComponentPropertyValue(Components::Thing_Settings_Swirl, Properties::Thing_Swirl_Angle).toFloat();

    float       bit_rate =          thing->getComponentPropertyValue(Components::Thing_Appearance, Properties::Thing_Filter_Bitrate).toList().first().toInt();
    QPointF     pixelation =        thing->getComponentPropertyValue(Components::Thing_Appearance, Properties::Thing_Filter_Pixelation).toPointF();

    DrEngineSwirl *swirl = new DrEngineSwirl(this, getNextKey(), info.position.x() + offset_x, -info.position.y() + offset_y,
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
    ThingInfo   info =              getThingBasicInfo( thing );

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

    DrEngineWater *water = new DrEngineWater(this, getNextKey(), info.position.x() + offset_x, -info.position.y() + offset_y,
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








