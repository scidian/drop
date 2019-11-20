//
//      Created by Stephens Nunnally on 10/20/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QtMath>
#include <QRandomGenerator>

#include "engine/engine.h"
#include "engine/engine_world.h"
#include "engine_things/engine_thing_fire.h"
#include "engine_things/engine_thing_fisheye.h"
#include "engine_things/engine_thing_light.h"
#include "engine_things/engine_thing_mirror.h"
#include "engine_things/engine_thing_object.h"
#include "engine_things/engine_thing_swirl.h"
#include "engine_things/engine_thing_water.h"
#include "project/project.h"
#include "project/project_asset.h"
#include "project/project_stage.h"
#include "project/project_thing.h"
#include "properties/property_collision.h"


// Internal Constants
const double    c_light_size_adjuster = 1.15;           // Multiplier to equal out the slight rendering difference between shader and pixmap drawing functions



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

    DrEngineFisheye *lens = new DrEngineFisheye(this, getNextKey(), thing->getKey(), info.position.x + offset_x, -info.position.y + offset_y,
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
    DrEngineFire *fire = new DrEngineFire(this, getNextKey(), thing->getKey(), info.position.x + offset_x, -info.position.y + offset_y,
                                          info.z_order, info.scale, info.angle, info.opacity, info.size,
                                          static_cast<Fire_Mask>(mask), color_1, color_2, smoke, intensity, smooth, wave, speed);
    addThing( fire );
    fire->bitrate = bit_rate;
    fire->pixel_x = static_cast<float>(pixelation.x());
    fire->pixel_y = static_cast<float>(pixelation.y());

    // ***** 3D Settings
    loadThing3DSettings(thing, fire);
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

    addThing( new DrEngineLight(this, getNextKey(), thing->getKey(), info.position.x + offset_x, -info.position.y + offset_y, info.z_order, info.opacity,
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

    DrEngineMirror *mirror = new DrEngineMirror(this, getNextKey(), thing->getKey(), info.position.x + offset_x, -info.position.y + offset_y,
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

    DrEngineSwirl *swirl = new DrEngineSwirl(this, getNextKey(), thing->getKey(), info.position.x + offset_x, -info.position.y + offset_y,
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

    DrEngineWater *water = new DrEngineWater(this, getNextKey(), thing->getKey(), info.position.x + offset_x, -info.position.y + offset_y,
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












