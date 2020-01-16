//
//      Created by Stephens Nunnally on 10/20/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "engine/opengl/opengl.h"
#include "project/constants_components.h"
#include "project/dr_project.h"
#include "project/entities/dr_world.h"
#include "project/entities/dr_stage.h"
#include "project/entities/dr_thing.h"
#include "project/settings/settings.h"
#include "project/settings/settings_component.h"
#include "project/settings/settings_component_property.h"


//####################################################################################
//##    Property loading - initializeWorldSettings
//####################################################################################
void DrWorld::initializeWorldSettings(std::string new_name) {
    DrProperty *property_name = getComponentProperty(Components::Entity_Settings, Properties::Entity_Name);
    property_name->setDisplayName("World Name");
    property_name->setDescription("Name of this World.");
    property_name->setValue(new_name);


    addComponent(Components::World_Settings, "World Settings", "Settings for this World.", Component_Colors::White_Snow, true);
    getComponent(Components::World_Settings)->setIcon(Component_Icons::Settings);
    addPropertyToComponent(Components::World_Settings, Properties::World_Game_Direction, Property_Type::Angle, 0.0,
                           "Game Direction", "Default direction, in degrees, to load new stages, 0 - right, 90 - down, 180 - left, 270 - up, etc.");
    addPropertyToComponent(Components::World_Settings, Properties::World_Score_Multiplier, Property_Type::Double, 1.0,
                           "Score Multiplier", "Value used as multiplier to adjust speed at which distance scoring is calculated.");


    std::vector<DrVariant> background_color_properties { static_cast<int>(Properties::World_Background_Color) };
    addPropertyToComponent(Components::World_Settings, Properties::World_Use_Background_Color, Property_Type::BoolEnabled,
                           std::vector<DrVariant>({false, background_color_properties }),
                           "Background Color?", "Check this box to use a custom Background Color for this world, otherwise"
                                                "background defaults to transparent.");
    addPropertyToComponent(Components::World_Settings, Properties::World_Background_Color, Property_Type::Color, DrColor(16, 16, 16, 255).rgba(),
                           "Color", "This is the base Background Color for this world.");
    addPropertyToComponent(Components::World_Settings, Properties::World_Deletion_Threshold, Property_Type::Positive, 10000,
                           "Deletion Threshold", "Distance (in World units) away from active camera that objects are removed from the World. Keeps game "
                                                 "running smoothly.");


    addComponent(Components::World_Camera, "Camera", "Camera settings for this World.", Component_Colors::Pink_Pearl, true);
    getComponent(Components::World_Camera)->setIcon(Component_Icons::Camera);
    addPropertyToComponent(Components::World_Camera, Properties::World_Camera_Type, Property_Type::List, static_cast<int>(Render_Type::Orthographic),
                           "Projection Type", "Sets the projection type of Camera's for this World. ");
    addPropertyToComponent(Components::World_Camera, Properties::World_Camera_Switch_Speed, Property_Type::PositiveDouble, 1.0,
                           "Switching Speed", "Multiplier to affect how fast Camera switching happens. A value lower than 1.0 will decrease switching "
                                              "speed, higher than 1.0 will increase switching speed (i.e. 0.1 will be slow, 10.0 is very fast).");

    std::vector<DrVariant> auto_zoom_properties { static_cast<int>(Properties::World_Camera_Zoom_In_Or_Out),
                                                  static_cast<int>(Properties::World_Camera_Zoom_Multiplier),
                                                  static_cast<int>(Properties::World_Camera_Zoom_Damping) };
    addPropertyToComponent(Components::World_Camera, Properties::World_Camera_Zoom_Enabled, Property_Type::BoolEnabled,
                           std::vector<DrVariant>({false, auto_zoom_properties }),
                           "Auto Zoom?", "Check this box to have Character Cameras change zoom automatically based on Characters speed.");
    addPropertyToComponent(Components::World_Camera, Properties::World_Camera_Zoom_In_Or_Out, Property_Type::List, static_cast<int>(Auto_Zoom::Zoom_Out),
                           "Zoom Type", "Select whether auto zoom should zoom in or zoom out.");
    addPropertyToComponent(Components::World_Camera, Properties::World_Camera_Zoom_Multiplier, Property_Type::RangedDouble, std::vector<DrVariant>({2.0, 1.0, 100.0, 0.1}),
                           "Zoom Multiplier", "This multiplier controls how much of a potential zoom can occur. Range: 1.0 (1 x current zoom level) to "
                                              "maximum of 100.0 (100 x current zoom level)");
    addPropertyToComponent(Components::World_Camera, Properties::World_Camera_Zoom_Damping, Property_Type::RangedDouble, std::vector<DrVariant>({5.0, 1.0, 10.0, 0.1}),
                           "Zoom Damping", "The higher this number, the slower the camera performs Auto Zoom. Range: 1.0 (fastest) to 10.0 (slowest)");

    addComponent(Components::World_Physics, "Physics", "Starting physics settings for this world, this affects all Objects that have \"Physics?\" "
                                                       "enabled.", Component_Colors::Orange_Pastel, true);
    getComponent(Components::World_Physics)->setIcon(Component_Icons::Physics);
    addPropertyToComponent(Components::World_Physics, Properties::World_Time_Warp, Property_Type::PositiveDouble, 1.0,
                           "Time Warp", "Physics update time multiplier, affects the speed the world changes. Default value of 1.0 is Normal speed. A value of 0.5 "
                                        "will halve the speed, a value of 2.0 will double it. Generally keep this less than 10.");
    addPropertyToComponent(Components::World_Physics, Properties::World_Gravity, Property_Type::PointF, DrPointF(0.0, -1000.0),
                           "Gravity", "Amount of gravity in X and Y directions, can be negative or positive. For example, a value of -1000 for Y and Objects will "
                                      "fall down; a value of 1000 for Y and Objects will fall up.");
    addPropertyToComponent(Components::World_Physics, Properties::World_Drag, Property_Type::PositiveDouble, 0.9,
                           "Damping", "Defauly value of 0.9 means objects will lose 10% of their velocity per second. Value of 1.0 means objects will not lose "
                                      "velocity, can be greater than 1.0 (objects will gain velocity). A value of 0.0 causes objects to lose all "
                                      "velocity immediately.");
    addPropertyToComponent(Components::World_Physics, Properties::World_Friction, Property_Type::PositiveDouble, 0.5,
                           "Friction", "Global surface friction setting, greater than or equal to 0.0 (no limit, but generally less than 10.0). Friction can be "
                                       "overriden on a per item basis.");
    addPropertyToComponent(Components::World_Physics, Properties::World_Bounce, Property_Type::PositiveDouble, 0.1,
                           "Bounce", "Global bounce setting, greater than or equal to 0.0 (no limit, but generally less than or equal to 1.0). Bounce can be "
                                     "overriden on a per item basis.");


    addComponent(Components::World_Lighting, "Lighting", "Lighting settings for this World.", Component_Colors::Mustard_Yellow, true);
    getComponent(Components::World_Lighting)->setIcon(Component_Icons::Light);
    addPropertyToComponent(Components::World_Lighting, Properties::World_Light_Ambient, Property_Type::Percent, 50,
                           "Ambient Light", "Used to darken World, mostly for the purpose of lighting it with Glow Lights.");
    addPropertyToComponent(Components::World_Lighting, Properties::World_Light_Layer, Property_Type::Double, 50.0,
                           "Light Layer", "Location along the z axis (Z-Order) to draw Glow Lights. Should be between " +
                                          std::to_string(double(c_near_plane)) + " and " + std::to_string(double(c_far_plane)) + " to be visible.");
    addPropertyToComponent(Components::World_Lighting, Properties::World_Light_Blend, Property_Type::List, static_cast<int>(Blend_Mode::Standard),
                           "Blend Mode", "This is the blend mode used to add Glow Lights and Object Emitted Light to the Stage. Different blend modes can effect "
                                         "oversaturation or be better for dark scenes.");


    addComponent(Components::World_Appearance, "Appearance", "These filters affect the entire world after it has been rendered.",
                                               Component_Colors::Purple_Royal, true);
    getComponent(Components::World_Appearance)->setIcon(Component_Icons::Appearance);
    addPropertyToComponent(Components::World_Appearance, Properties::World_Filter_Bitrate, Property_Type::Slider, std::vector<DrVariant>({256, 0, 256, 8, ""}),
                           "Bit Depth", "Standard output has color channel depth of 256, you can use this value to limit the number of available colors. "
                                        "Combining this with Pixelation gives a great retro look.");
    addPropertyToComponent(Components::World_Appearance, Properties::World_Filter_Pixelation, Property_Type::OneSizeF, DrPointF(1.0, 1.0),
                           "Pixelation", "Size of x and y pixels, larger numbers provide more pixelation.");
    addPropertyToComponent(Components::World_Appearance, Properties::World_Filter_Brightness, Property_Type::Slider, std::vector<DrVariant>({0, -255, 255, 5, ""}),
                           "Brightness", "How light / dark this world should appear. \nDefault: \t0 \nRange: \t-255 to 255");
    addPropertyToComponent(Components::World_Appearance, Properties::World_Filter_Contrast, Property_Type::Slider, std::vector<DrVariant>({0, -255, 255, 5, ""}),
                           "Contrast", "Amount of distinguishable difference of colors. \nDefault: \t0 \nRange: \t-255 to 255");
    addPropertyToComponent(Components::World_Appearance, Properties::World_Filter_Saturation, Property_Type::Slider, std::vector<DrVariant>({0, -255, 255, 5, ""}),
                           "Saturation", "How colorful the colors appear. \nDefault: \t0 \nRange: \t-255 to 255");
    addPropertyToComponent(Components::World_Appearance, Properties::World_Filter_Hue, Property_Type::Slider, std::vector<DrVariant>({0, 0, 360, 5, ""}),
                           "Hue", "Rotate color values. \nDefault: \t0 \nRange: \t0 to 360");
    addPropertyToComponent(Components::World_Appearance, Properties::World_Filter_Grayscale, Property_Type::Bool, false,
                           "Grayscale", "Should this world be shown grayscale?");
    addPropertyToComponent(Components::World_Appearance, Properties::World_Filter_Negative, Property_Type::Bool, false,
                           "Negative", "Should this world's colors be inverted?");


    addComponent(Components::World_Special_Effects, "Special Effects", "Special effects that affect the entire world.", Component_Colors::Blue_Royal, true);
    getComponent(Components::World_Special_Effects)->setIcon(Component_Icons::Effects);
    addPropertyToComponent(Components::World_Special_Effects, Properties::World_Filter_Convert_3D, Property_Type::Bool, false,
                           "Instant 3D", "Auto convert all 2D graphics into 3D. Depth of each item is customizable utilizing each "
                                         "item's \"3D Properties - Depth\" property.");
    addPropertyToComponent(Components::World_Special_Effects, Properties::World_Filter_Wireframe,
                           Property_Type::BoolDouble, std::vector<DrVariant>({false, 1.0, 0.0, 1000.0, 1.0, "Edge Width: "}),
                           "Wireframe", "Renders all the world's objects as outlines.");
    addPropertyToComponent(Components::World_Special_Effects, Properties::World_Filter_Cartoon,
                           Property_Type::BoolDouble, std::vector<DrVariant>({false, 5.0, 0.0, 10.0, 1.0,  "Edge Width: "}),
                           "Cartoon", "Gives the world a comic book look.");
    addPropertyToComponent(Components::World_Special_Effects, Properties::World_Filter_Cross_Hatch,
                           Property_Type::BoolDouble, std::vector<DrVariant>({false, 10.0, 1.0, 100.0, 5.0, "Hatch Width: "}),
                           "Cross Hatch", "Gives the world a sketched look.");
    addPropertyToComponent(Components::World_Special_Effects, Properties::World_Filter_Wavy, Property_Type::Bool, false,
                           "Wavy", "Gives the world a trippy / drunken look.");

}
















