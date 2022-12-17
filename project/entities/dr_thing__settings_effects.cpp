//
//      Created by Stephens Nunnally on 10/19/2019, (c) 2019 Scidian Studios, All Rights Reserved
//
//  File:
//      Effect Specific Components / Properties
//
//
#include "engine/enums_engine.h"
#include "project/constants_component_info.h"
#include "project/dr_project.h"
#include "project/entities/dr_stage.h"
#include "project/entities/dr_thing.h"
#include "project/entities/dr_world.h"
#include "project/entities_physics_2d/dr_asset.h"
#include "project/settings/settings.h"
#include "project/settings/settings_component.h"
#include "project/settings/settings_component_property.h"


//####################################################################################
//##    Fire Components
//####################################################################################
void DrThing::addComponentSettingsFire() {
    DrProperty *property_name = getComponentProperty(Comps::Entity_Settings, Props::Entity_Name);
    property_name->setDisplayName("Effect Name");
    property_name->setDescription("Name of this Effect.");
    property_name->setValue("Fire");

    addComponent(Comps::Thing_Settings_Fire, "Fire Settings", "Settings for this Fire.", Component_Colors::RGB_16_Orange, false);
    getComponent(Comps::Thing_Settings_Fire)->setIcon(Component_Icons::Fire);

    addPropertyToComponent(Comps::Thing_Settings_Fire, Props::Thing_Fire_Shape, Property_Type::List, static_cast<int>(Fire_Mask::Torch),
                           "Shape", "Defines a shape mask to use for the Fire.");
    addPropertyToComponent(Comps::Thing_Settings_Fire, Props::Thing_Fire_Color_1, Property_Type::Color, DrColor(255,   0, 0, 255).rgba(),
                           "Top Color", "Top color of this Fire.");
    addPropertyToComponent(Comps::Thing_Settings_Fire, Props::Thing_Fire_Color_2, Property_Type::Color, DrColor(255, 255, 0, 255).rgba(),
                           "Bottom Color", "Bottom color of this Fire.");
    addPropertyToComponent(Comps::Thing_Settings_Fire, Props::Thing_Fire_Color_Smoke, Property_Type::Color, DrColor(0, 0, 0, 255).rgba(),
                           "Smoke Color", "Background color of this Fire.");
    addPropertyToComponent(Comps::Thing_Settings_Fire, Props::Thing_Fire_Intensity, Property_Type::Percent, 50.0,
                           "Intensity", "How intense colors should appear in this Fire.");
    addPropertyToComponent(Comps::Thing_Settings_Fire, Props::Thing_Fire_Smoothness, Property_Type::Percent, 25.0,
                           "Smoothness", "How smooth to make the flames.");
    addPropertyToComponent(Comps::Thing_Settings_Fire, Props::Thing_Fire_Wavy, Property_Type::Percent, 50.0,
                           "Waviness", "How wavy to make the flames.");
    addPropertyToComponent(Comps::Thing_Settings_Fire, Props::Thing_Fire_Speed, Property_Type::Percent, 80.0,
                           "Speed", "How fast flames will move.");
}


//####################################################################################
//##    Fisheye Components
//####################################################################################
void DrThing::addComponentSettingsFisheye() {
    DrProperty *property_name = getComponentProperty(Comps::Entity_Settings, Props::Entity_Name);
    property_name->setDisplayName("Effect Name");
    property_name->setDescription("Name of this Effect.");
    property_name->setValue("Fisheye");

    addComponent(Comps::Thing_Settings_Fisheye, "Fisheye Lens Settings", "Settings for this Fisheye Lens.", Component_Colors::RGB_02_Pink, false);
    getComponent(Comps::Thing_Settings_Fisheye)->setIcon(Component_Icons::Fisheye);

    addPropertyToComponent(Comps::Thing_Settings_Fisheye, Props::Thing_Fisheye_Color, Property_Type::Color, DrColor(128, 128, 128, 255).rgba(),
                           "Color", "Color tint for this Lens.");
    addPropertyToComponent(Comps::Thing_Settings_Fisheye, Props::Thing_Fisheye_Color_Tint, Property_Type::Percent, 25.0,
                           "Tint Percent", "How much color to tint the Lens, 0 (none) - 100 (all the way)");
    addPropertyToComponent(Comps::Thing_Settings_Fisheye, Props::Thing_Fisheye_Lens_Zoom, Property_Type::RangedDouble, std::vector<DrVariant>({ 2.25, 0.0, 10.0, 0.25 }),
                           "Lens Zoom", "How much to zoom the Fisheye Lens. Default is 2.25. Ranged from 0 to 10.");
}

//####################################################################################
//##    Light Components
//####################################################################################
void DrThing::addComponentSettingsLight(DrColor color) {
    DrProperty *property_name = getComponentProperty(Comps::Entity_Settings, Props::Entity_Name);
    property_name->setDisplayName("Effect Name");
    property_name->setDescription("Name of this Effect.");
    property_name->setValue("Light");

    addComponent(Comps::Thing_Settings_Light, "Light Settings", "Settings for this Light.", Component_Colors::RGB_13_Yellow, false);
    getComponent(Comps::Thing_Settings_Light)->setIcon(Component_Icons::Light);

    addPropertyToComponent(Comps::Thing_Settings_Light, Props::Thing_Light_Type, Property_Type::List, static_cast<int>(Light_Type::Opaque),
                           "Light Type", "<b>Opaque</b> - Solid texture, does not provide lighting. <br> "
                                         "<b>Glow</b> - Provides diffuse lighting, no z-ordering available. ");
    addPropertyToComponent(Comps::Thing_Settings_Light, Props::Thing_Light_Color, Property_Type::Color, color.rgba(),
                           "Light Color", "The Color for this Light.");
    addPropertyToComponent(Comps::Thing_Settings_Light, Props::Thing_Light_Cone_Start, Property_Type::Slider, std::vector<DrVariant>({0, 0, 360, 5, "°", 0}),
                           "Cone Start", "Starting angle of light, Cone Angles travel in counter-clockwise direction.");
    addPropertyToComponent(Comps::Thing_Settings_Light, Props::Thing_Light_Cone_End, Property_Type::Slider, std::vector<DrVariant>({360, 0, 360, 5, "°", 0}),
                           "Cone End", "Ending angle of light, Cone Angles travel in counter-clockwise direction.");
    addPropertyToComponent(Comps::Thing_Settings_Light, Props::Thing_Light_Intensity, Property_Type::Percent, 50.0,
                           "Intensity", "How intense the light is starting from the middle out.");
    addPropertyToComponent(Comps::Thing_Settings_Light, Props::Thing_Light_Blur, Property_Type::Percent, 50.0,
                           "Blur", "How much to blur the light's edges and shadows.");
    addPropertyToComponent(Comps::Thing_Settings_Light, Props::Thing_Light_Draw_Shadows, Property_Type::Bool, 50.0,
                           "Cast Shadows?", "Should this light cast shadows? <b>NOTE:</b> The use of many shadow casting lights on the screen at one "
                                            "time can slow down older devices.");
    addPropertyToComponent(Comps::Thing_Settings_Light, Props::Thing_Light_Shadows, Property_Type::Percent, 50.0,
                           "Shadow Amount", "How much light should shine through the shadows. 0 is No Light, 100 is Most Light.");
    addPropertyToComponent(Comps::Thing_Settings_Light, Props::Thing_Light_Pulse, Property_Type::Double, 0.0,
                           "Pulse Amount", "This value will cause light to pulse back and forth +/- the Pulse Amount from the lights Intensity.");
    addPropertyToComponent(Comps::Thing_Settings_Light, Props::Thing_Light_Pulse_Speed, Property_Type::Double, 1.0,
                           "Pulse Speed", "How fast the light Pulses over time. This value is the total change desired over the course of one second.");
}


//####################################################################################
//##    Mirror Components
//####################################################################################
void DrThing::addComponentSettingsMirror() {
    DrProperty *property_name = getComponentProperty(Comps::Entity_Settings, Props::Entity_Name);
    property_name->setDisplayName("Effect Name");
    property_name->setDescription("Name of this Effect.");
    property_name->setValue("Mirror");

    addComponent(Comps::Thing_Settings_Mirror, "Mirror Settings", "Settings for this Mirror.", Component_Colors::RGB_18_Gray, false);
    getComponent(Comps::Thing_Settings_Mirror)->setIcon(Component_Icons::Mirror);

    addPropertyToComponent(Comps::Thing_Settings_Mirror, Props::Thing_Mirror_Start_Color, Property_Type::Color, DrColor(255, 255, 255, 255).rgba(),
                           "Start Color", "Color tint for the top of this Mirror.");
    addPropertyToComponent(Comps::Thing_Settings_Mirror, Props::Thing_Mirror_End_Color, Property_Type::Color, DrColor(170, 170, 170, 255).rgba(),
                           "End Color", "Color tint to fade to toward the bottom of this Mirror.");
    addPropertyToComponent(Comps::Thing_Settings_Mirror, Props::Thing_Mirror_Color_Tint, Property_Type::Percent, 50.0,
                           "Tint Percent", "How much color to tint the Mirror, 0 (none) - 100 (all the way)");
    addPropertyToComponent(Comps::Thing_Settings_Mirror, Props::Thing_Mirror_Blur, Property_Type::Double, 5.0,
                           "Scatter", "Amount to scatter reflection.");
    addPropertyToComponent(Comps::Thing_Settings_Mirror, Props::Thing_Mirror_Blur_Stretch, Property_Type::Double, 0.0,
                           "Scatter Stretch", "Amount to increase scatter away from start of mirror.");
    addPropertyToComponent(Comps::Thing_Settings_Mirror, Props::Thing_Mirror_Scale, Property_Type::RangedDouble, std::vector<DrVariant>({ 1.0, 0.1, 100000.0, 0.1 }),
                           "Scatter Scale", "How much to scale the scatter noise. Default is 1.0. Must be greater than 0.");
}


//####################################################################################
//##    Swirl Components
//####################################################################################
void DrThing::addComponentSettingsSwirl() {
    DrProperty *property_name = getComponentProperty(Comps::Entity_Settings, Props::Entity_Name);
    property_name->setDisplayName("Effect Name");
    property_name->setDescription("Name of this Effect.");
    property_name->setValue("Swirl");

    addComponent(Comps::Thing_Settings_Swirl, "Swirl Settings", "Settings for this Swirl.", Component_Colors::RGB_03_Violet, false);
    getComponent(Comps::Thing_Settings_Swirl)->setIcon(Component_Icons::Swirl);

    addPropertyToComponent(Comps::Thing_Settings_Swirl, Props::Thing_Swirl_Start_Color, Property_Type::Color, DrColor(255, 255, 255, 255).rgba(),
                           "Color", "Color tint for this Lens.");
    addPropertyToComponent(Comps::Thing_Settings_Swirl, Props::Thing_Swirl_Color_Tint, Property_Type::Percent, 0.0,
                           "Tint Percent", "How much color to tint the Lens, 0 (none) - 100 (all the way)");
    addPropertyToComponent(Comps::Thing_Settings_Swirl, Props::Thing_Swirl_Angle, Property_Type::Double, 100.0,
                           "Intensity", "Amount to swirl images under the Swirl effect.");
}


//####################################################################################
//##    Water Components
//####################################################################################
void DrThing::addComponentSettingsWater() {
    DrProperty *property_name = getComponentProperty(Comps::Entity_Settings, Props::Entity_Name);
    property_name->setDisplayName("Effect Name");
    property_name->setDescription("Name of this Effect.");
    property_name->setValue("Water");


    addComponent(Comps::Thing_Settings_Water, "Water Settings", "Settings for this Water.", Component_Colors::Blue_Drop_1, false);
    getComponent(Comps::Thing_Settings_Water)->setIcon(Component_Icons::Water);

    addPropertyToComponent(Comps::Thing_Settings_Water, Props::Thing_Water_Texture, Property_Type::List, static_cast<int>(Water_Texture::Ripples),
                           "Texture", "Defines a texture to use for the water surface, can give the Water several different looks.");
    addPropertyToComponent(Comps::Thing_Settings_Water, Props::Thing_Water_Start_Color, Property_Type::Color, DrColor(41, 182, 246, 255).rgba(),
                           "Start Color", "Color tint for the top of this Water.");
    addPropertyToComponent(Comps::Thing_Settings_Water, Props::Thing_Water_End_Color, Property_Type::Color, DrColor(0, 58, 103, 255).rgba(),
                           "End Color", "Color tint to fade to toward the bottom of this Water.");
    addPropertyToComponent(Comps::Thing_Settings_Water, Props::Thing_Water_Color_Tint, Property_Type::Percent, 75.0,
                           "Tint Percent", "How much color to tint the Water, 0 (none) - 100 (all the way)");
    addPropertyToComponent(Comps::Thing_Settings_Water, Props::Thing_Water_Reflection, Property_Type::Percent, 20.0,
                           "Reflection", "Reflection opacity.");
    addPropertyToComponent(Comps::Thing_Settings_Water, Props::Thing_Water_Movement_Speed, Property_Type::Double, 0.0,
                           "Movement", "This value will cause the Water Texture to move left / right.");


    addComponent(Comps::Thing_Settings_Water_Ripple, "Ripple Settings", "Settings for this Water's Ripple effect.", Component_Colors::Blue_Drop_2, false);
    getComponent(Comps::Thing_Settings_Water_Ripple)->setIcon(Component_Icons::Water_Ripple);

    addPropertyToComponent(Comps::Thing_Settings_Water_Ripple, Props::Thing_Water_Ripple_Frequency, Property_Type::Double, 100.0,
                           "Frequency", "Length of Ripples appearing in Water, lower numbers produce longer period Ripples. "
                                        "Generally, ranging around 0 to 100 will produce nice results.");
    addPropertyToComponent(Comps::Thing_Settings_Water_Ripple, Props::Thing_Water_Ripple_Speed, Property_Type::Double, 50.0,
                           "Speed", "Rate Ripples move through Water. Generally, ranging around 0 to 100 will produce nice results.");
    addPropertyToComponent(Comps::Thing_Settings_Water_Ripple, Props::Thing_Water_Ripple_Amplitude, Property_Type::Double, 20.0,
                           "Amplitude", "Size of Ripples appearing in Water. Generally, ranging around 0 to 100 will produce nice results.");
    addPropertyToComponent(Comps::Thing_Settings_Water_Ripple, Props::Thing_Water_Ripple_Stretch, Property_Type::Double, 10.0,
                           "Stretch", "Rate the Ripples' Amplitude increases as Ripples move away from start of water. A setting of 0 will keep Amplitude "
                                      "the same, a negative value will decrease Ripple Amplitude.");


    addComponent(Comps::Thing_Settings_Water_Wave, "Wave Settings", "Settings that this Water's Wave effect.", Component_Colors::Blue_Drop_3, false);
    getComponent(Comps::Thing_Settings_Water_Wave)->setIcon(Component_Icons::Water_Wave);

    addPropertyToComponent(Comps::Thing_Settings_Water_Wave, Props::Thing_Water_Wave_Frequency, Property_Type::Double, 10.0,
                           "Frequency", "Length of Waves appearing in Water, lower numbers produce longer period waves. "
                                        "Generally, ranging around 0 to 100 will produce nice results.");
    addPropertyToComponent(Comps::Thing_Settings_Water_Wave, Props::Thing_Water_Wave_Speed, Property_Type::Double, 20.0,
                           "Speed", "Rate Waves move up and down. Generally, ranging around 0 to 100 will produce nice results.");
    addPropertyToComponent(Comps::Thing_Settings_Water_Wave, Props::Thing_Water_Wave_Amplitude, Property_Type::Double, 10.0,
                           "Amplitude", "Size of Waves as they move up and down in Water. Generally, ranging around 0 to 100 will produce nice results.");


    addComponent(Comps::Thing_Settings_Water_Refract, "Refraction Settings", "Settings that effect refraction (randomness) of different parts of "
                                                                                  "this Water.", Component_Colors::Blue_Drop_4, false);
    getComponent(Comps::Thing_Settings_Water_Refract)->setIcon(Component_Icons::Water_Refract);

    addPropertyToComponent(Comps::Thing_Settings_Water_Refract, Props::Thing_Water_Refract_Reflection, Property_Type::Percent, 20.0,
                           "Reflection", "How much refraction to apply to the Reflection shown on the Water surface.");
    addPropertyToComponent(Comps::Thing_Settings_Water_Refract, Props::Thing_Water_Refract_Underwater, Property_Type::Percent, 20.0,
                           "Underwater", "How much refraction to apply to the Things under the Water.");
    addPropertyToComponent(Comps::Thing_Settings_Water_Refract, Props::Thing_Water_Refract_Texture, Property_Type::Percent, 20.0,
                           "Texture", "How much refraction to apply to the Water Texture.");
    addPropertyToComponent(Comps::Thing_Settings_Water_Refract, Props::Thing_Water_Refract_Foam, Property_Type::Percent, 20.0,
                           "Surface", "How much refraction to apply to the top of the Water.");


    addComponent(Comps::Thing_Settings_Water_Foam, "Foam Settings", "Settings for top of this Water.", Component_Colors::Blue_Drop_5, false);
    getComponent(Comps::Thing_Settings_Water_Foam)->setIcon(Component_Icons::Water_Foam);

    addPropertyToComponent(Comps::Thing_Settings_Water_Foam, Props::Thing_Water_Surface_Color, Property_Type::Color, DrColor(255, 255, 255, 255).rgba(),
                           "Color", "Color tint for the foam on the top of the Water.");
    addPropertyToComponent(Comps::Thing_Settings_Water_Foam, Props::Thing_Water_Surface_Tint, Property_Type::Percent, 75.0,
                           "Tint", "How much color to tint the foam on top of the Water, 0 (none) - 100 (all the way)");
    addPropertyToComponent(Comps::Thing_Settings_Water_Foam, Props::Thing_Water_Surface_Height, Property_Type::Double, 5.0,
                           "Height", "Thickness of the foam on top of the Water.");
    addPropertyToComponent(Comps::Thing_Settings_Water_Foam, Props::Thing_Water_Surface_Is_Flat, Property_Type::Bool, false,
                           "Keep Flat?", "Should the top of the water stay completely flat, even with waves and refraction?");

}












