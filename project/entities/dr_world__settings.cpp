//
//      Created by Stephens Nunnally on 10/20/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "engine/opengl/opengl.h"
#include "project/constants_component_info.h"
#include "project/dr_project.h"
#include "project/entities/dr_stage.h"
#include "project/entities/dr_thing.h"
#include "project/entities/dr_world.h"
#include "project/settings/settings.h"
#include "project/settings/settings_component.h"
#include "project/settings/settings_component_property.h"


//####################################################################################
//##    Property loading - initializeWorldSettings
//####################################################################################
void DrWorld::initializeWorldSettings(std::string new_name) {
    DrProperty *property_name = getComponentProperty(Comps::Entity_Settings, Props::Entity_Name);
    property_name->setDisplayName("World Name");
    property_name->setDescription("Name of this World.");
    property_name->setValue(new_name);


    addComponent(Comps::World_Settings, "World Settings", "Settings for this World.", Component_Colors::RGB_18_Gray, true);
    getComponent(Comps::World_Settings)->setIcon(Component_Icons::Settings);
    addPropertyToComponent(Comps::World_Settings, Props::World_Game_Direction, Property_Type::Angle, 0.0,
                           "Game Direction", "Default direction, in degrees, to load new stages, 0 - right, 90 - down, 180 - left, 270 - up, etc.");
    addPropertyToComponent(Comps::World_Settings, Props::World_Score_Multiplier, Property_Type::Double, 1.0,
                           "Score Multiplier", "Value used as multiplier to adjust speed at which distance scoring is calculated.");


    std::vector<DrVariant> background_color_properties { Props::World_Background_Color };
    addPropertyToComponent(Comps::World_Settings, Props::World_Use_Background_Color, Property_Type::BoolEnabled,
                           std::vector<DrVariant>({false, background_color_properties }),
                           "Background Color?", "Check this box to use a custom Background Color for this world, otherwise"
                                                "background defaults to transparent.");
    addPropertyToComponent(Comps::World_Settings, Props::World_Background_Color, Property_Type::Color, DrColor(16, 16, 16, 255).rgba(),
                           "Color", "This is the base Background Color for this world.");
    addPropertyToComponent(Comps::World_Settings, Props::World_Deletion_Threshold, Property_Type::Positive, 10000,
                           "Deletion Threshold", "Distance (in World units) away from active camera that Things are removed from the World. Keeps game "
                                                 "running smoothly.");


    addComponent(Comps::World_Camera, "Camera", "Camera settings for this World.", Component_Colors::RGB_08_SeaFoam, true);
    getComponent(Comps::World_Camera)->setIcon(Component_Icons::Camera);
    addPropertyToComponent(Comps::World_Camera, Props::World_Camera_Type, Property_Type::List, static_cast<int>(Render_Type::Orthographic),
                           "Projection Type", "Sets the projection type of Camera's for this World. ");
    addPropertyToComponent(Comps::World_Camera, Props::World_Camera_Switch_Speed, Property_Type::PositiveDouble, 1.0,
                           "Switching Speed", "Multiplier to affect how fast Camera switching happens. A value lower than 1.0 will decrease switching "
                                              "speed, higher than 1.0 will increase switching speed (i.e. 0.1 will be slow, 10.0 is very fast).");

    std::vector<DrVariant> auto_zoom_properties { Props::World_Camera_Zoom_In_Or_Out,
                                                  Props::World_Camera_Zoom_Multiplier,
                                                  Props::World_Camera_Zoom_Damping };
    addPropertyToComponent(Comps::World_Camera, Props::World_Camera_Zoom_Enabled, Property_Type::BoolEnabled,
                           std::vector<DrVariant>({false, auto_zoom_properties }),
                           "Auto Zoom?", "Check this box to have Character Cameras change zoom automatically based on Characters speed.");
    addPropertyToComponent(Comps::World_Camera, Props::World_Camera_Zoom_In_Or_Out, Property_Type::List, static_cast<int>(Auto_Zoom::Zoom_Out),
                           "Zoom Type", "Select whether auto zoom should zoom in or zoom out.");
    addPropertyToComponent(Comps::World_Camera, Props::World_Camera_Zoom_Multiplier, Property_Type::RangedDouble, std::vector<DrVariant>({2.0, 1.0, 100.0, 0.1}),
                           "Zoom Multiplier", "This multiplier controls how much of a potential zoom can occur. Range: 1.0 (1 x current zoom level) to "
                                              "maximum of 100.0 (100 x current zoom level)");
    addPropertyToComponent(Comps::World_Camera, Props::World_Camera_Zoom_Damping, Property_Type::RangedDouble, std::vector<DrVariant>({5.0, 1.0, 10.0, 0.1}),
                           "Zoom Damping", "The higher this number, the slower the camera performs Auto Zoom. Range: 1.0 (fastest) to 10.0 (slowest)");


    addComponent(Comps::World_Physics, "Physics", "Starting physics settings for this world, this affects all Things that have \"Physics?\" "
                                                  "enabled.", Component_Colors::RGB_15_Sunset, true);
    getComponent(Comps::World_Physics)->setIcon(Component_Icons::Physics);
    addPropertyToComponent(Comps::World_Physics, Props::World_Time_Warp, Property_Type::PositiveDouble, 1.0,
                           "Time Warp", "Physics update time multiplier, affects the speed the world changes. Default value of 1.0 is Normal speed. A value of 0.5 "
                                        "will halve the speed, a value of 2.0 will double it. Generally keep this less than 10.");
    addPropertyToComponent(Comps::World_Physics, Props::World_Gravity, Property_Type::PointF, DrPointF(0.0, -1000.0),
                           "Gravity", "Amount of gravity in X and Y directions, can be negative or positive. For example, a value of -1000 for Y and Things will "
                                      "fall down; a value of 1000 for Y and Things will fall up.");
    addPropertyToComponent(Comps::World_Physics, Props::World_Drag, Property_Type::PositiveDouble, 0.9,
                           "Damping", "Defauly value of 0.9 means Things will lose 10% of their velocity per second. Value of 1.0 means Things will not lose "
                                      "velocity, can be greater than 1.0 (Things will gain velocity). A value of 0.0 will cause Things to lose all "
                                      "velocity immediately.");
    addPropertyToComponent(Comps::World_Physics, Props::World_Friction, Property_Type::PositiveDouble, 0.5,
                           "Friction", "Global surface friction setting, greater than or equal to 0.0 (no limit, but generally less than 10.0). Friction can be "
                                       "overriden on a per Thing basis.");
    addPropertyToComponent(Comps::World_Physics, Props::World_Bounce, Property_Type::PositiveDouble, 0.1,
                           "Bounce", "Global bounce setting, greater than or equal to 0.0 (no limit, but generally less than or equal to 1.0). Bounce can be "
                                     "overriden on a per Thing basis.");


    addComponent(Comps::World_Lighting, "Lighting", "Lighting settings for this World.", Component_Colors::RGB_13_Yellow, true);
    getComponent(Comps::World_Lighting)->setIcon(Component_Icons::Light);
    addPropertyToComponent(Comps::World_Lighting, Props::World_Light_Ambient, Property_Type::Percent, 50,
                           "Ambient Light", "Used to darken World, mostly for the purpose of lighting it with Glow Lights.");
    addPropertyToComponent(Comps::World_Lighting, Props::World_Light_Layer, Property_Type::Double, 50.0,
                           "Light Layer", "Location along the z axis (Z-Order) to draw Glow Lights. Should be between " +
                                          Dr::RoundToDecimalPlace(c_near_plane, 1) + " and " + Dr::RoundToDecimalPlace(c_far_plane, 1) + " to be visible.");
    addPropertyToComponent(Comps::World_Lighting, Props::World_Light_Blend, Property_Type::List, static_cast<int>(Blend_Mode::Standard),
                           "Blend Mode", "This is the blend mode used to add Glow Lights and Thing Emitted Light to the Stage. Different blend modes can effect "
                                         "oversaturation or be better for dark scenes.");


    addComponent(Comps::World_Appearance, "Appearance", "These filters affect the entire world after it has been rendered.", Component_Colors::RGB_01_Red, true);
    getComponent(Comps::World_Appearance)->setIcon(Component_Icons::Appearance);
    addPropertyToComponent(Comps::World_Appearance, Props::World_Filter_PixelType, Property_Type::List, static_cast<int>(Pixel_Texture::None),
                           "Pixel Texture", "Design of pixelation. For plain blocky pixels leave as \"None\".");
    addPropertyToComponent(Comps::World_Appearance, Props::World_Filter_Pixelation, Property_Type::OneSizeF, DrPointF(1.0, 1.0),
                           "Pixelation", "Size of x and y pixels, larger numbers provide more pixelation.");
    addPropertyToComponent(Comps::World_Appearance, Props::World_Filter_Bitrate, Property_Type::Slider, std::vector<DrVariant>({256, 0, 256, 8, ""}),
                           "Bit Depth", "Standard output has color channel depth of 256, you can use this value to limit the number of available colors. "
                                        "Combining this with Pixelation gives a great retro look.");
    addPropertyToComponent(Comps::World_Appearance, Props::World_Filter_Brightness, Property_Type::Slider, std::vector<DrVariant>({0, -255, 255, 5, ""}),
                           "Brightness", "How light / dark this world should appear. \nDefault: \t0 \nRange: \t-255 to 255");
    addPropertyToComponent(Comps::World_Appearance, Props::World_Filter_Contrast, Property_Type::Slider, std::vector<DrVariant>({0, -255, 255, 5, ""}),
                           "Contrast", "Amount of distinguishable difference of colors. \nDefault: \t0 \nRange: \t-255 to 255");
    addPropertyToComponent(Comps::World_Appearance, Props::World_Filter_Saturation, Property_Type::Slider, std::vector<DrVariant>({0, -255, 255, 5, ""}),
                           "Saturation", "How colorful the colors appear. \nDefault: \t0 \nRange: \t-255 to 255");
    addPropertyToComponent(Comps::World_Appearance, Props::World_Filter_Hue, Property_Type::Slider, std::vector<DrVariant>({0, 0, 360, 5, ""}),
                           "Hue", "Rotate color values. \nDefault: \t0 \nRange: \t0 to 360");
    addPropertyToComponent(Comps::World_Appearance, Props::World_Filter_Grayscale, Property_Type::Bool, false,
                           "Grayscale", "Should this world be shown grayscale?");
    addPropertyToComponent(Comps::World_Appearance, Props::World_Filter_Negative, Property_Type::Bool, false,
                           "Negative", "Should this world's colors be inverted?");


    addComponent(Comps::World_Special_Effects, "Special Effects", "Special effects that affect the entire world.", Component_Colors::RGB_03_Violet, true);
    getComponent(Comps::World_Special_Effects)->setIcon(Component_Icons::Effects);
    addPropertyToComponent(Comps::World_Special_Effects, Props::World_Filter_Convert_3D, Property_Type::Bool, false,
                           "Instant 3D", "Auto convert all 2D Things into 3D. Depth of each Thing is customizable utilizing each "
                                         "Thing's \"3D Properties - Depth\" property.");
    addPropertyToComponent(Comps::World_Special_Effects, Props::World_Filter_Wireframe,
                           Property_Type::BoolDouble, std::vector<DrVariant>({false, 1.0, 0.0, 1000.0, 1.0, "Edge Width: "}),
                           "Wireframe", "Renders all the World's Things as outlines.");
    addPropertyToComponent(Comps::World_Special_Effects, Props::World_Filter_Cartoon,
                           Property_Type::BoolDouble, std::vector<DrVariant>({false, 5.0, 0.0, 10.0, 1.0,  "Edge Width: "}),
                           "Cartoon", "Gives the world a comic book look.");
    addPropertyToComponent(Comps::World_Special_Effects, Props::World_Filter_Cross_Hatch,
                           Property_Type::BoolDouble, std::vector<DrVariant>({false, 10.0, 1.0, 100.0, 5.0, "Hatch Width: "}),
                           "Cross Hatch", "Gives the world a sketched look.");
    addPropertyToComponent(Comps::World_Special_Effects, Props::World_Filter_Wavy, Property_Type::Bool, false,
                           "Wavy", "Gives the world a trippy / drunken look.");

}
















