//
//      Created by Stephens Nunnally on 10/19/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Effect Specific Components / Properties
//
//
#include "engine/enums_engine.h"
#include "project/constants_components.h"
#include "project/dr_project.h"
#include "project/entities/dr_asset.h"
#include "project/entities/dr_world.h"
#include "project/entities/dr_stage.h"
#include "project/entities/dr_thing.h"
#include "project/settings/settings.h"
#include "project/settings/settings_component.h"
#include "project/settings/settings_component_property.h"


//####################################################################################
//##    Fire Components
//####################################################################################
void DrThing::addComponentSettingsFire() {
    DrProperty *property_name = getComponentProperty(Components::Entity_Settings, Properties::Entity_Name);
    property_name->setDisplayName("Effect Name");
    property_name->setDescription("Name of this Effect.");
    property_name->setValue("Fire");

    addComponent(Components::Thing_Settings_Fire, "Fire Settings", "Settings for this Fire.", Component_Colors::Red_Faded, true);
    getComponent(Components::Thing_Settings_Fire)->setIcon(Component_Icons::Fire);

    addPropertyToComponent(Components::Thing_Settings_Fire, Properties::Thing_Fire_Shape, Property_Type::List, static_cast<int>(Fire_Mask::Torch),
                           "Shape", "Defines a shape mask to use for the Fire.");
    addPropertyToComponent(Components::Thing_Settings_Fire, Properties::Thing_Fire_Color_1, Property_Type::Color, DrColor(255,   0, 0, 255).rgba(),
                           "Top Color", "Top color of this Fire.");
    addPropertyToComponent(Components::Thing_Settings_Fire, Properties::Thing_Fire_Color_2, Property_Type::Color, DrColor(255, 255, 0, 255).rgba(),
                           "Bottom Color", "Bottom color of this Fire.");
    addPropertyToComponent(Components::Thing_Settings_Fire, Properties::Thing_Fire_Color_Smoke, Property_Type::Color, DrColor(0, 0, 0, 255).rgba(),
                           "Smoke Color", "Background color of this Fire.");
    addPropertyToComponent(Components::Thing_Settings_Fire, Properties::Thing_Fire_Intensity, Property_Type::Percent, 50.0,
                           "Intensity", "How intense colors should appear in this Fire.");
    addPropertyToComponent(Components::Thing_Settings_Fire, Properties::Thing_Fire_Smoothness, Property_Type::Percent, 25.0,
                           "Smoothness", "How smooth to make the flames.");
    addPropertyToComponent(Components::Thing_Settings_Fire, Properties::Thing_Fire_Wavy, Property_Type::Percent, 50.0,
                           "Waviness", "How wavy to make the flames.");
    addPropertyToComponent(Components::Thing_Settings_Fire, Properties::Thing_Fire_Speed, Property_Type::Percent, 80.0,
                           "Speed", "How fast flames will move.");
}


//####################################################################################
//##    Fisheye Components
//####################################################################################
void DrThing::addComponentSettingsFisheye() {
    DrProperty *property_name = getComponentProperty(Components::Entity_Settings, Properties::Entity_Name);
    property_name->setDisplayName("Effect Name");
    property_name->setDescription("Name of this Effect.");
    property_name->setValue("Fisheye");

    addComponent(Components::Thing_Settings_Fisheye, "Fisheye Lens Settings", "Settings for this Fisheye Lens.", Component_Colors::Orange_Pastel, true);
    getComponent(Components::Thing_Settings_Fisheye)->setIcon(Component_Icons::Fisheye);

    addPropertyToComponent(Components::Thing_Settings_Fisheye, Properties::Thing_Fisheye_Color, Property_Type::Color, DrColor(128, 128, 128, 255).rgba(),
                           "Color", "Color tint for this Lens.");
    addPropertyToComponent(Components::Thing_Settings_Fisheye, Properties::Thing_Fisheye_Color_Tint, Property_Type::Percent, 25.0,
                           "Tint Percent", "How much color to tint the Lens, 0 (none) - 100 (all the way)");
    addPropertyToComponent(Components::Thing_Settings_Fisheye, Properties::Thing_Fisheye_Lens_Zoom, Property_Type::RangedDouble, std::vector<DrVariant>({ 2.25, 0.0, 10.0, 0.25 }),
                           "Lens Zoom", "How much to zoom the Fisheye Lens. Default is 2.25. Ranged from 0 to 10.");
}

//####################################################################################
//##    Light Components
//####################################################################################
void DrThing::addComponentSettingsLight(DrColor color) {
    DrProperty *property_name = getComponentProperty(Components::Entity_Settings, Properties::Entity_Name);
    property_name->setDisplayName("Effect Name");
    property_name->setDescription("Name of this Effect.");
    property_name->setValue("Light");

    addComponent(Components::Thing_Settings_Light, "Light Settings", "Settings for this Light.", Component_Colors::Mellow_Yellow, true);
    getComponent(Components::Thing_Settings_Light)->setIcon(Component_Icons::Light);

    addPropertyToComponent(Components::Thing_Settings_Light, Properties::Thing_Light_Type, Property_Type::List, static_cast<int>(Light_Type::Opaque),
                           "Light Type", "<b>Opaque</b> - Solid texture, does not provide lighting. <br> "
                                         "<b>Glow</b> - Provides diffuse lighting, no z-ordering available. ");
    addPropertyToComponent(Components::Thing_Settings_Light, Properties::Thing_Light_Color, Property_Type::Color, color.rgba(),
                           "Light Color", "The Color for this Light.");
    addPropertyToComponent(Components::Thing_Settings_Light, Properties::Thing_Light_Cone_Start, Property_Type::Slider, std::vector<DrVariant>({0, 0, 360, 5, "°"}),
                           "Cone Start", "Starting angle of light, Cone Angles travel in counter-clockwise direction.");
    addPropertyToComponent(Components::Thing_Settings_Light, Properties::Thing_Light_Cone_End, Property_Type::Slider, std::vector<DrVariant>({360, 0, 360, 5, "°"}),
                           "Cone End", "Ending angle of light, Cone Angles travel in counter-clockwise direction.");
    addPropertyToComponent(Components::Thing_Settings_Light, Properties::Thing_Light_Intensity, Property_Type::Percent, 50.0,
                           "Intensity", "How intense the light is starting from the middle out.");
    addPropertyToComponent(Components::Thing_Settings_Light, Properties::Thing_Light_Blur, Property_Type::Percent, 50.0,
                           "Blur", "How much to blur the light's edges and shadows.");
    addPropertyToComponent(Components::Thing_Settings_Light, Properties::Thing_Light_Draw_Shadows, Property_Type::Bool, 50.0,
                           "Cast Shadows?", "Should this light cast shadows? <b>NOTE:</b> The use of many shadow casting lights on the screen at one "
                                            "time can slow down older devices.");
    addPropertyToComponent(Components::Thing_Settings_Light, Properties::Thing_Light_Shadows, Property_Type::Percent, 50.0,
                           "Shadow Amount", "How much light should shine through the shadows. 0 is No Light, 100 is Most Light.");
    addPropertyToComponent(Components::Thing_Settings_Light, Properties::Thing_Light_Pulse, Property_Type::Double, 0.0,
                           "Pulse Amount", "This value will cause light to pulse back and forth +/- the Pulse Amount from the lights Intensity.");
    addPropertyToComponent(Components::Thing_Settings_Light, Properties::Thing_Light_Pulse_Speed, Property_Type::Double, 1.0,
                           "Pulse Speed", "How fast the light Pulses over time. This value is the total change desired over the course of one second.");
}


//####################################################################################
//##    Mirror Components
//####################################################################################
void DrThing::addComponentSettingsMirror() {
    DrProperty *property_name = getComponentProperty(Components::Entity_Settings, Properties::Entity_Name);
    property_name->setDisplayName("Effect Name");
    property_name->setDescription("Name of this Effect.");
    property_name->setValue("Mirror");

    addComponent(Components::Thing_Settings_Mirror, "Mirror Settings", "Settings for this Mirror.", Component_Colors::Silver_Snow, true);
    getComponent(Components::Thing_Settings_Mirror)->setIcon(Component_Icons::Mirror);

    addPropertyToComponent(Components::Thing_Settings_Mirror, Properties::Thing_Mirror_Start_Color, Property_Type::Color, DrColor(255, 255, 255, 255).rgba(),
                           "Start Color", "Color tint for the top of this Mirror.");
    addPropertyToComponent(Components::Thing_Settings_Mirror, Properties::Thing_Mirror_End_Color, Property_Type::Color, DrColor(170, 170, 170, 255).rgba(),
                           "End Color", "Color tint to fade to toward the bottom of this Mirror.");
    addPropertyToComponent(Components::Thing_Settings_Mirror, Properties::Thing_Mirror_Color_Tint, Property_Type::Percent, 50.0,
                           "Tint Percent", "How much color to tint the Mirror, 0 (none) - 100 (all the way)");
    addPropertyToComponent(Components::Thing_Settings_Mirror, Properties::Thing_Mirror_Blur, Property_Type::Double, 5.0,
                           "Scatter", "Amount to scatter reflection.");
    addPropertyToComponent(Components::Thing_Settings_Mirror, Properties::Thing_Mirror_Blur_Stretch, Property_Type::Double, 0.0,
                           "Scatter Stretch", "Amount to increase scatter away from start of mirror.");
    addPropertyToComponent(Components::Thing_Settings_Mirror, Properties::Thing_Mirror_Scale, Property_Type::RangedDouble, std::vector<DrVariant>({ 1.0, 0.1, 100000.0, 0.1 }),
                           "Scatter Scale", "How much to scale the scatter noise. Default is 1.0. Must be greater than 0.");
}


//####################################################################################
//##    Swirl Components
//####################################################################################
void DrThing::addComponentSettingsSwirl() {
    DrProperty *property_name = getComponentProperty(Components::Entity_Settings, Properties::Entity_Name);
    property_name->setDisplayName("Effect Name");
    property_name->setDescription("Name of this Effect.");
    property_name->setValue("Swirl");

    addComponent(Components::Thing_Settings_Swirl, "Swirl Settings", "Settings for this Swirl.", Component_Colors::Purple_Pastel, true);
    getComponent(Components::Thing_Settings_Swirl)->setIcon(Component_Icons::Swirl);

    addPropertyToComponent(Components::Thing_Settings_Swirl, Properties::Thing_Swirl_Start_Color, Property_Type::Color, DrColor(255, 255, 255, 255).rgba(),
                           "Color", "Color tint for this Lens.");
    addPropertyToComponent(Components::Thing_Settings_Swirl, Properties::Thing_Swirl_Color_Tint, Property_Type::Percent, 0.0,
                           "Tint Percent", "How much color to tint the Lens, 0 (none) - 100 (all the way)");
    addPropertyToComponent(Components::Thing_Settings_Swirl, Properties::Thing_Swirl_Angle, Property_Type::Double, 100.0,
                           "Intensity", "Amount to swirl images under the Swirl effect.");
}


//####################################################################################
//##    Water Components
//####################################################################################
void DrThing::addComponentSettingsWater() {
    DrProperty *property_name = getComponentProperty(Components::Entity_Settings, Properties::Entity_Name);
    property_name->setDisplayName("Effect Name");
    property_name->setDescription("Name of this Effect.");
    property_name->setValue("Water");

    addComponent(Components::Thing_Settings_Water, "Water Settings", "Settings for this Water.", Component_Colors::Blue_Drop_1, true);
    getComponent(Components::Thing_Settings_Water)->setIcon(Component_Icons::Water);

    addPropertyToComponent(Components::Thing_Settings_Water, Properties::Thing_Water_Texture, Property_Type::List, static_cast<int>(Water_Texture::Ripples),
                           "Texture", "Defines a texture to use for the water surface, can give the Water several different looks.");
    addPropertyToComponent(Components::Thing_Settings_Water, Properties::Thing_Water_Start_Color, Property_Type::Color, DrColor(41, 182, 246, 255).rgba(),
                           "Start Color", "Color tint for the top of this Water.");
    addPropertyToComponent(Components::Thing_Settings_Water, Properties::Thing_Water_End_Color, Property_Type::Color, DrColor(0, 58, 103, 255).rgba(),
                           "End Color", "Color tint to fade to toward the bottom of this Water.");
    addPropertyToComponent(Components::Thing_Settings_Water, Properties::Thing_Water_Color_Tint, Property_Type::Percent, 75.0,
                           "Tint Percent", "How much color to tint the Water, 0 (none) - 100 (all the way)");
    addPropertyToComponent(Components::Thing_Settings_Water, Properties::Thing_Water_Reflection, Property_Type::Percent, 20.0,
                           "Reflection", "Reflection opacity.");
    addPropertyToComponent(Components::Thing_Settings_Water, Properties::Thing_Water_Movement_Speed, Property_Type::Double, 0.0,
                           "Movement", "This value will cause the Water Texture to move left / right.");

    addComponent(Components::Thing_Settings_Water_Ripple, "Ripple Settings", "Settings for this Water's Ripple effect.", Component_Colors::Blue_Drop_2, true);
    getComponent(Components::Thing_Settings_Water_Ripple)->setIcon(Component_Icons::Water_Ripple);

    addPropertyToComponent(Components::Thing_Settings_Water_Ripple, Properties::Thing_Water_Ripple_Frequency, Property_Type::Double, 100.0,
                           "Frequency", "Length of Ripples appearing in Water, lower numbers produce longer period Ripples. "
                                        "Generally, ranging around 0 to 100 will produce nice results.");
    addPropertyToComponent(Components::Thing_Settings_Water_Ripple, Properties::Thing_Water_Ripple_Speed, Property_Type::Double, 50.0,
                           "Speed", "Rate Ripples move through Water. Generally, ranging around 0 to 100 will produce nice results.");
    addPropertyToComponent(Components::Thing_Settings_Water_Ripple, Properties::Thing_Water_Ripple_Amplitude, Property_Type::Double, 20.0,
                           "Amplitude", "Size of Ripples appearing in Water. Generally, ranging around 0 to 100 will produce nice results.");
    addPropertyToComponent(Components::Thing_Settings_Water_Ripple, Properties::Thing_Water_Ripple_Stretch, Property_Type::Double, 10.0,
                           "Stretch", "Rate the Ripples' Amplitude increases as Ripples move away from start of water. A setting of 0 will keep Amplitude "
                                      "the same, a negative value will decrease Ripple Amplitude.");

    addComponent(Components::Thing_Settings_Water_Wave, "Wave Settings", "Settings that this Water's Wave effect.", Component_Colors::Blue_Drop_3, true);
    getComponent(Components::Thing_Settings_Water_Wave)->setIcon(Component_Icons::Water_Wave);

    addPropertyToComponent(Components::Thing_Settings_Water_Wave, Properties::Thing_Water_Wave_Frequency, Property_Type::Double, 10.0,
                           "Frequency", "Length of Waves appearing in Water, lower numbers produce longer period waves. "
                                        "Generally, ranging around 0 to 100 will produce nice results.");
    addPropertyToComponent(Components::Thing_Settings_Water_Wave, Properties::Thing_Water_Wave_Speed, Property_Type::Double, 20.0,
                           "Speed", "Rate Waves move up and down. Generally, ranging around 0 to 100 will produce nice results.");
    addPropertyToComponent(Components::Thing_Settings_Water_Wave, Properties::Thing_Water_Wave_Amplitude, Property_Type::Double, 10.0,
                           "Amplitude", "Size of Waves as they move up and down in Water. Generally, ranging around 0 to 100 will produce nice results.");

    addComponent(Components::Thing_Settings_Water_Refract, "Refraction Settings", "Settings that effect refraction (randomness) of different parts of "
                                                                                  "this Water.", Component_Colors::Blue_Drop_4, true);
    getComponent(Components::Thing_Settings_Water_Refract)->setIcon(Component_Icons::Water_Refract);

    addPropertyToComponent(Components::Thing_Settings_Water_Refract, Properties::Thing_Water_Refract_Reflection, Property_Type::Percent, 20.0,
                           "Reflection", "How much refraction to apply to the Reflection shown on the Water surface.");
    addPropertyToComponent(Components::Thing_Settings_Water_Refract, Properties::Thing_Water_Refract_Underwater, Property_Type::Percent, 20.0,
                           "Underwater", "How much refraction to apply to the objects under the Water.");
    addPropertyToComponent(Components::Thing_Settings_Water_Refract, Properties::Thing_Water_Refract_Texture, Property_Type::Percent, 20.0,
                           "Texture", "How much refraction to apply to the Water Texture.");
    addPropertyToComponent(Components::Thing_Settings_Water_Refract, Properties::Thing_Water_Refract_Foam, Property_Type::Percent, 20.0,
                           "Surface", "How much refraction to apply to the top of the Water.");

    addComponent(Components::Thing_Settings_Water_Foam, "Foam Settings", "Settings for top of this Water.", Component_Colors::Blue_Drop_5, true);
    getComponent(Components::Thing_Settings_Water_Foam)->setIcon(Component_Icons::Water_Foam);

    addPropertyToComponent(Components::Thing_Settings_Water_Foam, Properties::Thing_Water_Surface_Color, Property_Type::Color, DrColor(255, 255, 255, 255).rgba(),
                           "Color", "Color tint for the foam on the top of the Water.");
    addPropertyToComponent(Components::Thing_Settings_Water_Foam, Properties::Thing_Water_Surface_Tint, Property_Type::Percent, 75.0,
                           "Tint", "How much color to tint the foam on top of the Water, 0 (none) - 100 (all the way)");
    addPropertyToComponent(Components::Thing_Settings_Water_Foam, Properties::Thing_Water_Surface_Height, Property_Type::Double, 5.0,
                           "Height", "Thickness of the foam on top of the Water.");
    addPropertyToComponent(Components::Thing_Settings_Water_Foam, Properties::Thing_Water_Surface_Is_Flat, Property_Type::Bool, false,
                           "Keep Flat?", "Should the top of the water stay completely flat, even with waves and refraction?");

}












