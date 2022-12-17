//
//      Created by Stephens Nunnally on 10/20/2019, (c) 2019 Scidian Studios, All Rights Reserved
//
//  File:
//
//
//
#include "engine/engine.h"
#include "engine/thing/components/thing_comp_3d.h"
#include "engine/thing/components_effects/thing_comp_fire.h"
#include "engine/thing/components_effects/thing_comp_fisheye.h"
#include "engine/thing/components_effects/thing_comp_light.h"
#include "engine/thing/components_effects/thing_comp_mirror.h"
#include "engine/thing/components_effects/thing_comp_swirl.h"
#include "engine/thing/components_effects/thing_comp_water.h"
#include "engine/thing/engine_thing.h"
#include "engine/world/engine_world.h"
#include "project/dr_project.h"
#include "project/entities/dr_stage.h"
#include "project/entities/dr_thing.h"
#include "project/entities_physics_2d/dr_asset.h"
#include "project/properties/property_collision.h"


// Internal Constants
const double    c_light_size_adjuster = 1.15;           // Multiplier to equal out the slight rendering difference between shader and pixmap drawing functions



//####################################################################################
//##    Loads one DrProject DrThingType::Fisheye to World / Space
//####################################################################################
void DrEngineWorld::loadFisheyeToWorld(DrThing *thing, double offset_x, double offset_y) {
    Thing_Info  info =              loadThingBasicInfo( thing );

    DrColor     start_color =       DrColor(thing->getComponentPropertyValue(Comps::Thing_Settings_Fisheye, Props::Thing_Fisheye_Color).toUInt());
    float       tint =              thing->getComponentPropertyValue(Comps::Thing_Settings_Fisheye, Props::Thing_Fisheye_Color_Tint).toFloat() / 100.0f;
    float       zoom =              thing->getComponentPropertyValue(Comps::Thing_Settings_Fisheye, Props::Thing_Fisheye_Lens_Zoom).toVector()[0].toFloat();

    DrEngineThing *fisheye = new DrEngineThing(this, getNextKey(), thing->getKey(), info.position.x + offset_x, -info.position.y + offset_y,
                                               info.z_order, info.scale, info.angle, info.opacity, info.size);
    fisheye->setComponent(Comps::Thing_Settings_Fisheye, new ThingCompFisheye(this, fisheye, start_color, tint, zoom));
    addThing( fisheye );

    // ***** Apply additional settings
    loadThingAppearanceSettings(thing, fisheye, true);          // Appearance settings
}


//####################################################################################
//##    Loads one DrProject DrThingType::Fire to World / Space
//####################################################################################
void DrEngineWorld::loadFireToWorld(DrThing *thing, double offset_x, double offset_y) {
    Thing_Info  info =              loadThingBasicInfo( thing );

    int         mask =              thing->getComponentPropertyValue(Comps::Thing_Settings_Fire, Props::Thing_Fire_Shape).toInt();
    DrColor     color_1 =           DrColor(thing->getComponentPropertyValue(Comps::Thing_Settings_Fire, Props::Thing_Fire_Color_1).toUInt());
    DrColor     color_2 =           DrColor(thing->getComponentPropertyValue(Comps::Thing_Settings_Fire, Props::Thing_Fire_Color_2).toUInt());
    DrColor     smoke =             DrColor(thing->getComponentPropertyValue(Comps::Thing_Settings_Fire, Props::Thing_Fire_Color_Smoke).toUInt());
    float       intensity =         thing->getComponentPropertyValue(Comps::Thing_Settings_Fire, Props::Thing_Fire_Intensity).toFloat() / 100.f;
    float       smooth =            thing->getComponentPropertyValue(Comps::Thing_Settings_Fire, Props::Thing_Fire_Smoothness).toFloat() / 100.f;
    float       wave =              thing->getComponentPropertyValue(Comps::Thing_Settings_Fire, Props::Thing_Fire_Wavy).toFloat() / 100.f;
    float       speed =             thing->getComponentPropertyValue(Comps::Thing_Settings_Fire, Props::Thing_Fire_Speed).toFloat();

    // Calculate original size used in editor
    info.size.x = info.size.x / info.scale.x;
    info.size.y = info.size.y / info.scale.y;

    DrEngineThing *fire = new DrEngineThing(this, getNextKey(), thing->getKey(), info.position.x + offset_x, -info.position.y + offset_y,
                                            info.z_order, info.scale, info.angle, info.opacity, info.size);
    fire->setComponent3D(new ThingComp3D(this, fire));
    fire->setComponent(Comps::Thing_Settings_Fire, new ThingCompFire(this, fire, static_cast<Fire_Mask>(mask), color_1, color_2, smoke, intensity, smooth, wave, speed));
    addThing( fire );

    // ***** Apply additional settings
    loadThingAppearanceSettings(thing, fire, true);             // Appearance settings
    loadThing3DSettings(thing, fire);                           // 3D Settings
}


//####################################################################################
//##    Loads one DrProject DrThingType::Light to World / Space
//####################################################################################
void DrEngineWorld::loadLightToWorld(DrThing *thing, double offset_x, double offset_y) {
    Thing_Info  info =              loadThingBasicInfo( thing );
    DrColor     light_color =       DrColor(thing->getComponentPropertyValue(Comps::Thing_Settings_Light, Props::Thing_Light_Color).toUInt());
    int         light_type =        thing->getComponentPropertyValue(Comps::Thing_Settings_Light, Props::Thing_Light_Type).toInt();

    double      cone_start =        thing->getComponentPropertyValue(Comps::Thing_Settings_Light, Props::Thing_Light_Cone_Start).toVector()[0].toDouble();
    double      cone_end =          thing->getComponentPropertyValue(Comps::Thing_Settings_Light, Props::Thing_Light_Cone_End).toVector()[0].toDouble();
    float       intensity =         thing->getComponentPropertyValue(Comps::Thing_Settings_Light, Props::Thing_Light_Intensity).toFloat();
    float       blur =              thing->getComponentPropertyValue(Comps::Thing_Settings_Light, Props::Thing_Light_Blur).toFloat();
    float       shadows =           thing->getComponentPropertyValue(Comps::Thing_Settings_Light, Props::Thing_Light_Shadows).toFloat();
    bool        draw_shadows =      thing->getComponentPropertyValue(Comps::Thing_Settings_Light, Props::Thing_Light_Draw_Shadows).toBool();

    float       pulse =             thing->getComponentPropertyValue(Comps::Thing_Settings_Light, Props::Thing_Light_Pulse).toFloat();
    float       pulse_speed =       thing->getComponentPropertyValue(Comps::Thing_Settings_Light, Props::Thing_Light_Pulse_Speed).toFloat();

    info.size.x = info.size.x * c_light_size_adjuster;

    DrEngineThing *light = new DrEngineThing(this, getNextKey(), thing->getKey(), info.position.x + offset_x, -info.position.y + offset_y,
                                             info.z_order, info.scale, info.angle, info.opacity, info.size);
    light->setComponent(Comps::Thing_Settings_Light, new ThingCompLight(this, light, static_cast<Light_Type>(light_type),
                                                                        light_color, static_cast<float>(info.size.x),
                                                                        DrPointF(cone_start, cone_end), intensity, shadows,
                                                                        draw_shadows, blur, pulse, pulse_speed));
    addThing( light );

}


//####################################################################################
//##    Loads one DrProject DrThingType::Mirror to World / Space
//####################################################################################
void DrEngineWorld::loadMirrorToWorld(DrThing *thing, double offset_x, double offset_y) {
    Thing_Info  info =              loadThingBasicInfo( thing );

    DrColor color_1 =       DrColor(thing->getComponentPropertyValue(Comps::Thing_Settings_Mirror, Props::Thing_Mirror_Start_Color).toUInt());
    DrColor color_2 =       DrColor(thing->getComponentPropertyValue(Comps::Thing_Settings_Mirror, Props::Thing_Mirror_End_Color).toUInt());
    float       color_tint =        thing->getComponentPropertyValue(Comps::Thing_Settings_Mirror, Props::Thing_Mirror_Color_Tint).toFloat() / 100.0f;
    float       blur =              thing->getComponentPropertyValue(Comps::Thing_Settings_Mirror, Props::Thing_Mirror_Blur).toFloat();
    float       blur_stretch =      thing->getComponentPropertyValue(Comps::Thing_Settings_Mirror, Props::Thing_Mirror_Blur_Stretch).toFloat();
    float       scale =             thing->getComponentPropertyValue(Comps::Thing_Settings_Mirror, Props::Thing_Mirror_Scale).toVector()[0].toFloat();

    DrEngineThing *mirror = new DrEngineThing(this, getNextKey(), thing->getKey(), info.position.x + offset_x, -info.position.y + offset_y,
                                              info.z_order, info.scale, info.angle, info.opacity, info.size);
    mirror->setComponent(Comps::Thing_Settings_Mirror, new ThingCompMirror(this, mirror, color_1, color_2, color_tint, blur, blur_stretch, scale));
    addThing( mirror );

    // ***** Apply additional settings
    loadThingAppearanceSettings(thing, mirror, true);           // Appearance settings
    loadThing3DSettings(thing, mirror);                         // 3D Settings
}


//####################################################################################
//##    Loads one DrProject DrThingType::Swirl to World / Space
//####################################################################################
void DrEngineWorld::loadSwirlToWorld(DrThing *thing, double offset_x, double offset_y) {
    Thing_Info  info =              loadThingBasicInfo( thing );

    DrColor     start_color =       DrColor(thing->getComponentPropertyValue(Comps::Thing_Settings_Swirl, Props::Thing_Swirl_Start_Color).toUInt());
    float       tint =              thing->getComponentPropertyValue(Comps::Thing_Settings_Swirl, Props::Thing_Swirl_Color_Tint).toFloat() / 100.0f;
    float       rotation =          thing->getComponentPropertyValue(Comps::Thing_Settings_Swirl, Props::Thing_Swirl_Angle).toFloat();

    DrEngineThing *swirl = new DrEngineThing(this, getNextKey(), thing->getKey(), info.position.x + offset_x, -info.position.y + offset_y,
                                             info.z_order, info.scale, info.angle, info.opacity, info.size);
    swirl->setComponent(Comps::Thing_Settings_Swirl, new ThingCompSwirl(this, swirl, start_color, tint, rotation));
    addThing( swirl );

    // ***** Apply additional settings
    loadThingAppearanceSettings(thing, swirl, true);            // Appearance settings
    loadThing3DSettings(thing, swirl);                          // 3D Settings
}


//####################################################################################
//##    Loads one DrProject DrThingType::Water to World / Space
//####################################################################################
void DrEngineWorld::loadWaterToWorld(DrThing *thing, double offset_x, double offset_y) {
    Thing_Info  info =              loadThingBasicInfo( thing );

    int         texture =           thing->getComponentPropertyValue(Comps::Thing_Settings_Water, Props::Thing_Water_Texture).toInt();
    DrColor     start_color =       DrColor(thing->getComponentPropertyValue(Comps::Thing_Settings_Water, Props::Thing_Water_Start_Color).toUInt());
    DrColor     end_color =         DrColor(thing->getComponentPropertyValue(Comps::Thing_Settings_Water, Props::Thing_Water_End_Color).toUInt());
    float       water_tint =        thing->getComponentPropertyValue(Comps::Thing_Settings_Water, Props::Thing_Water_Color_Tint).toFloat() / 100.0f;
    float       reflection =        thing->getComponentPropertyValue(Comps::Thing_Settings_Water, Props::Thing_Water_Reflection).toFloat() / 100.0f;
    float       move_speed =        thing->getComponentPropertyValue(Comps::Thing_Settings_Water, Props::Thing_Water_Movement_Speed).toFloat();

    float       ripple_freq =       thing->getComponentPropertyValue(Comps::Thing_Settings_Water_Ripple, Props::Thing_Water_Ripple_Frequency).toFloat();
    float       ripple_speed =      thing->getComponentPropertyValue(Comps::Thing_Settings_Water_Ripple, Props::Thing_Water_Ripple_Speed).toFloat();
    float       ripple_amplitude =  thing->getComponentPropertyValue(Comps::Thing_Settings_Water_Ripple, Props::Thing_Water_Ripple_Amplitude).toFloat();
    float       ripple_stretch =    thing->getComponentPropertyValue(Comps::Thing_Settings_Water_Ripple, Props::Thing_Water_Ripple_Stretch).toFloat();

    float       wave_freq =         thing->getComponentPropertyValue(Comps::Thing_Settings_Water_Wave, Props::Thing_Water_Wave_Frequency).toFloat();
    float       wave_speed =        thing->getComponentPropertyValue(Comps::Thing_Settings_Water_Wave, Props::Thing_Water_Wave_Speed).toFloat();
    float       wave_amplitude =    thing->getComponentPropertyValue(Comps::Thing_Settings_Water_Wave, Props::Thing_Water_Wave_Amplitude).toFloat();

    float       refract_1 =         thing->getComponentPropertyValue(Comps::Thing_Settings_Water_Refract, Props::Thing_Water_Refract_Reflection).toFloat();
    float       refract_2 =         thing->getComponentPropertyValue(Comps::Thing_Settings_Water_Refract, Props::Thing_Water_Refract_Underwater).toFloat();
    float       refract_3 =         thing->getComponentPropertyValue(Comps::Thing_Settings_Water_Refract, Props::Thing_Water_Refract_Texture).toFloat();
    float       foam_refract =      thing->getComponentPropertyValue(Comps::Thing_Settings_Water_Refract, Props::Thing_Water_Refract_Foam).toFloat();

    DrColor     foam_color =         DrColor(thing->getComponentPropertyValue(Comps::Thing_Settings_Water_Foam, Props::Thing_Water_Surface_Color).toUInt());
    float       foam_tint =         thing->getComponentPropertyValue(Comps::Thing_Settings_Water_Foam, Props::Thing_Water_Surface_Tint).toFloat() / 100.0f;
    float       foam_height =       thing->getComponentPropertyValue(Comps::Thing_Settings_Water_Foam, Props::Thing_Water_Surface_Height).toFloat();
    bool        foam_flat =         thing->getComponentPropertyValue(Comps::Thing_Settings_Water_Foam, Props::Thing_Water_Surface_Is_Flat).toBool();

    DrEngineThing *water = new DrEngineThing(this, getNextKey(), thing->getKey(), info.position.x + offset_x, -info.position.y + offset_y,
                                             info.z_order, info.scale, info.angle, info.opacity, info.size);
    ThingCompWater *w = new ThingCompWater(this, water, static_cast<Water_Texture>(texture), start_color, end_color, water_tint, reflection,
                                           ripple_freq, ripple_speed, ripple_amplitude, ripple_stretch,
                                           wave_freq, wave_speed, wave_amplitude,
                                           foam_color, foam_tint, foam_height, foam_flat,
                                           refract_1, refract_2, refract_3, foam_refract, move_speed);
    water->setComponent(Comps::Thing_Settings_Water, w);
    addThing( water );

    // ***** Apply additional settings
    loadThingAppearanceSettings(thing, water, true);            // Appearance settings
    loadThing3DSettings(thing, water);                          // 3D Settings
}












