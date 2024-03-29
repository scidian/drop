//
//      Created by Stephens Nunnally on 10/20/2019, (c) 2019 Scidian Studios, All Rights Reserved
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
#include "project/entities_physics_2d/dr_asset.h"
#include "project/settings/settings.h"
#include "project/settings/settings_component.h"
#include "project/settings/settings_component_property.h"


//####################################################################################
//##    Transform Components
//####################################################################################
void DrThing::addComponentTransform(double width, double height, double x, double y, DrThingType type) {
    std::string type_string = Dr::StringFromThingType(type);

    addComponent(Comps::Thing_Transform, "Transform", "Sets the physical size and angle of the " + type_string + " "
                                                           "in the Stage.", Component_Colors::RGB_19_Silver, false);
    getComponent(Comps::Thing_Transform)->setIcon(Component_Icons::Transform);

    std::string position_text = "Location of the " + type_string + " within the current Stage.";
    if (type == DrThingType::Camera) {
        position_text = "Point at which Camera is looking at.";
    }
    addPropertyToComponent(Comps::Thing_Transform, Props::Thing_Position, Property_Type::PositionF, DrPointF(x, y), "Position", position_text);

    std::string transform_title = "Rotation Z";
    std::string transform_text =  "Starting rotation, in degrees, of Thing on the Z Axis. Does affect physics collision shape.";
    if (type == DrThingType::Camera) {
        transform_title = "Camera Tilt";
        transform_text =  "Starting tilt / rotation of Camera on the Z Axis.";
    }
    addPropertyToComponent(Comps::Thing_Transform, Props::Thing_Rotation, Property_Type::Angle, 0, transform_title, transform_text);

    std::string size_text = "Width and Height of the " + type_string + " in pixels, affected by Scale property.";
    if (type == DrThingType::Light) {
        size_text = "Width and Height of the Light in pixels, affected by Scale property. "
                    "<br><b>NOTE:</b> For best performace on older devices, Light size is best kept under 4096.";
    } else if (type == DrThingType::Camera) {
        size_text = "Width and Height settings have no effect on Cameras.";
    }
    addPropertyToComponent(Comps::Thing_Transform, Props::Thing_Size, Property_Type::SizeF, DrPointF(width, height), "Size", size_text);

    std::string scale_text = "X and Y scale of the " + type_string + " within the Stage.";
    if (type== DrThingType::Camera) {
        scale_text = "X and Y scale settings have no effect on Cameras.";
    }
    addPropertyToComponent(Comps::Thing_Transform, Props::Thing_Scale, Property_Type::ScaleF, DrPointF(1, 1), "Scale", scale_text);
}


//####################################################################################
//##    Layering Components
//####################################################################################
void DrThing::addComponentLayering(double z, double opacity, bool show_component) {
    addComponent(Comps::Thing_Layering, "Layering", "Controls the order Things are drawn onto the screen. For \"Z-Order\", lower numbers are "
                                                    "towards the back, higher towards the front.", Component_Colors::RGB_07_LightBlue, !(show_component));
    getComponent(Comps::Thing_Layering)->setIcon(Component_Icons::Layering);

    addPropertyToComponent(Comps::Thing_Layering, Props::Thing_Z_Order, Property_Type::Double, 0.0,
                           "Z-Order", "Arrangement of Thing along the z axis in the Stage. Should be between the near and far clipping planes (" +
                                      std::to_string(double(c_near_plane)) + " and " + std::to_string(double(c_far_plane)) + ") to be visible.");
    // !!!!! Hidden Property
    addPropertyToComponent(Comps::Thing_Layering, Props::Thing_Sub_Z_Order, Property_Type::Int, 0,
                           "Sub Order", "Arrangement of Thing compared to other Things within the same Z-Order.", true, false);
    addPropertyToComponent(Comps::Thing_Layering, Props::Thing_Opacity, Property_Type::Percent, opacity,
                           "Opacity", "How transparent this Thing is, 0 (invisible) - 100 (solid)");
    setZOrderWithSub(z, Z_Insert::Front);
}


//####################################################################################
//##    Movement Components
//####################################################################################
void DrThing::addComponentMovement() {
    addComponent(Comps::Thing_Movement, "Movement", "Initial starting velocities of Object in Stage. <br><br> "
                                                    "<b>NOTE:</b> Object Type must be <b>Kinematic</b> or <b>Dynamic</b> to use these settings!",
                                                    Component_Colors::RGB_10_Green, false);
    getComponent(Comps::Thing_Movement)->setIcon(Component_Icons::Movement);

    addPropertyToComponent(Comps::Thing_Movement, Props::Thing_Velocity_X, Property_Type::Variable, DrPointF(0, 0),
                           "Horizontal", "Initial horizontal movement speed of Object, +/- variable amount.  <br><br> "
                                         "<b>NOTE:</b> Object Type must be <b>Kinematic</b> or <b>Dynamic</b> to use this setting!", false, false);
    addPropertyToComponent(Comps::Thing_Movement, Props::Thing_Velocity_Y, Property_Type::Variable, DrPointF(0, 0),
                           "Vertical",  "Initial vertical movement speed of Object, +/- variable amount.  <br><br> "
                                        "<b>NOTE:</b> Object Type must be <b>Kinematic</b> or <b>Dynamic</b> to use this setting!", false, false);
    addPropertyToComponent(Comps::Thing_Movement, Props::Thing_Spin_Velocity, Property_Type::Variable, DrPointF(0, 0),
                           "Rotate Speed Z", "Initial rotational speed Object on the Z Axis, +/- variable amount. <br><br> "
                                             "<b>NOTE:</b> Object Type must be <b>Kinematic</b> or <b>Dynamic</b> to use this setting!", false, false);
    addPropertyToComponent(Comps::Thing_Movement, Props::Thing_Angle_Velocity, Property_Type::Bool, true,
                           "Relative Angle?", "Should the Rotation and Scale of this Object affect it's Movement direction? (allows Object to move "
                                              "along an arc instead of spinning in place) <br> "
                                              "<b>NOTE:</b> Object Type must be <b>Kinematic</b> to use this setting!", false, false);
    addPropertyToComponent(Comps::Thing_Movement, Props::Thing_Angle_Player, Property_Type::Bool, false,
                           "Rotate To Player?", "Should the Rotation of this Object try and point towards (Y Axis Up) the active player? Rotates this"
                                                "direction at Rotate Speed Z speed. <br> "
                                                "<b>NOTE:</b> Object Type must be <b>Kinematic</b> to use this setting!", false, false);
}



//####################################################################################
//##    Lighting Components
//####################################################################################
void DrThing::addComponentLighting() {
    addComponent(Comps::Thing_Lighting, "Lighting", "Lighting settings for this Thing.", Component_Colors::RGB_13_Yellow, false);
    getComponent(Comps::Thing_Lighting)->setIcon(Component_Icons::Light);

    addPropertyToComponent(Comps::Thing_Lighting, Props::Thing_Lighting_Blend_Object, Property_Type::List, static_cast<int>(Blend_Object::Standard),
                           "Blend Mode", "Blend mode for rendering this Thing.");
    addPropertyToComponent(Comps::Thing_Lighting, Props::Thing_Lighting_Cast_Shadows, Property_Type::Bool, true,
                           "Cast Shadows?", "This Thing will cast shadows when appearing higher on the z axis than a Light.");
}


//####################################################################################
//##    Spawn Components
//####################################################################################
void DrThing::addComponentSpawn() {
    addComponent(Comps::Thing_Spawn, "Spawning", "Spawning allows for a copy of a Thing to continue to appear over and over again.",
                                                 Component_Colors::RGB_15_Sunset, false);
    getComponent(Comps::Thing_Spawn)->setIcon(Component_Icons::Spawn);

    addPropertyToComponent(Comps::Thing_Spawn, Props::Thing_Spawn_Count, Property_Type::Int, 1,
                           "Spawn Count", "How many instances of this Thing will spawn. A value of -1 will allow unlimited spawning.");
    addPropertyToComponent(Comps::Thing_Spawn, Props::Thing_Spawn_Rate, Property_Type::Variable, DrPointF(1, 0),
                           "Spawn Rate", "Rate, in seconds, this Thing will spawn, +/- variable amount.");
    addPropertyToComponent(Comps::Thing_Spawn, Props::Thing_Spawn_Instantly, Property_Type::Bool, true,
                           "Spawn Immediately", "If this option is not selected, Thing will wait the initial Spawn Rate amount of time before spawning "
                                                "the first time. When selected, Thing will spawn as soon as the event occurs and then wait Spawn Rate in between "
                                                "the remaining spawms.");
    addPropertyToComponent(Comps::Thing_Spawn, Props::Thing_Spawn_Chance, Property_Type::Percent, 100.0,
                           "Appeance Chance", "When it is time for this Thing to spawn, this is the likelihood it will spawn.");
    addPropertyToComponent(Comps::Thing_Spawn, Props::Thing_Spawn_Type, Property_Type::List, static_cast<int>(Spawn_Type::Permanent),
                           "Spawn Type", "What causes this Thing to spawn. Permanent is the default. Selecting Shoot or Jump will cause Thing to "
                                         "spawn when corresponding buttons are pressed. ");
    // std::vector<DrVariant> of 6 values: bool, int value,    min, max, int step size,    string spinText
    addPropertyToComponent(Comps::Thing_Spawn, Props::Thing_Spawn_At_Object, Property_Type::BoolInt,
                           std::vector<DrVariant>({false, 1001, 0, 1000000000, 1, "Object ID: "}),
                           "Spawn At Thing", "This item will spawn in relation to the location of the Thing with the designated ID Key. <br>"
                                              "<b>NOTE:</b> Enter instance Thing (i.e. <i>Object</i> or <i>Character</i>) ID Keys here, "
                                              "<b>NOT</b> Asset ID Keys. Enter <b>\"0\"</b> here to stay attached to active character.");
    addPropertyToComponent(Comps::Thing_Spawn, Props::Thing_Spawn_Offset_X, Property_Type::Variable, DrPointF(0, 0),
                           "Spawn Offset X", "This is a X offset for use with spawning, +/- variable amount. "
                                             "Useful for some randomess to Thing location.");
    addPropertyToComponent(Comps::Thing_Spawn, Props::Thing_Spawn_Offset_Y, Property_Type::Variable, DrPointF(0, 0),
                           "Spawn Offset Y", "This is a Y offset for use with spawning, +/- variable amount. "
                                             "Useful for some randomess to Thing location.");
}


//####################################################################################
//##    3D Components
//####################################################################################
void DrThing::addComponent3D() {
    addComponent(Comps::Thing_3D, "3D Properties", "Turns 2D Things in 3D, rotates 3D Things. Extrusion works best for images "
                                                   "whose pixels are mostly opaque. The World Setting \"Instant 3D\" setting under "
                                                   "the \"Appearance\" category must be checked to see 3D.",
                                                   Component_Colors::RGB_17_Brown, false);
    getComponent(Comps::Thing_3D)->setIcon(Component_Icons::Object);

    addPropertyToComponent(Comps::Thing_3D, Props::Thing_3D_Type, Property_Type::List, static_cast<int>(Convert_3D_Type::Extrusion),
                           "Type", "How this Thing is represented in 3D. The texture can be Extruded (pulled) into the 3rd dimension. Or it can "
                                   "be Wrapped onto a Cube.");
    addPropertyToComponent(Comps::Thing_3D, Props::Thing_3D_Depth, Property_Type::PositiveDouble, 50.0,
                           "Depth", "Depth of Thing in pixels when appearing in 3D.");
    addPropertyToComponent(Comps::Thing_3D, Props::Thing_3D_X_Axis_Rotation, Property_Type::Variable, DrPointF(0, 0),
                           "Rotation X", "Starting rotation of Thing on the X Axis, +/- variable amount. Does not affect physics collision shape.");
    addPropertyToComponent(Comps::Thing_3D, Props::Thing_3D_Y_Axis_Rotation, Property_Type::Variable, DrPointF(0, 0),
                           "Rotation Y", "Starting rotation of Thing on the Y Axis, +/- variable amount. Does not affect physics collision shape.");
    addPropertyToComponent(Comps::Thing_3D, Props::Thing_3D_X_Axis_Speed, Property_Type::Variable, DrPointF(0, 0),
                           "Rotate Speed X", "Rotational speed of Thing on the X Axis, +/- variable amount. Does not affect physics collision shape.");
    addPropertyToComponent(Comps::Thing_3D, Props::Thing_3D_Y_Axis_Speed, Property_Type::Variable, DrPointF(0, 0),
                           "Rotate Speed Y", "Rotational speed of Thing on the Y Axis, +/- variable amount. Does not affect physics collision shape.");
    addPropertyToComponent(Comps::Thing_3D, Props::Thing_3D_Billboard, Property_Type::Bool, false,
                           "Face Camera?", "Keeps this Thing facing toward camera no matter the camera position or camera angle.");
}


//####################################################################################
//##    Appearance Components
//####################################################################################
void DrThing::addComponentAppearance(bool bitrate_and_pixel_only, bool show_component) {
    addComponent(Comps::Thing_Appearance, "Appearance", "Filters for Things as they appear in the Stage.", Component_Colors::RGB_01_Red, !(show_component));
    getComponent(Comps::Thing_Appearance)->setIcon(Component_Icons::Appearance);

    bool hidden = bitrate_and_pixel_only;
    addPropertyToComponent(Comps::Thing_Appearance, Props::Thing_Filter_PixelType, Property_Type::List, static_cast<int>(Pixel_Texture::None),
                           "Pixel Texture", "Design of pixelation. For plain blocky pixels leave as \"None\".", hidden);
    addPropertyToComponent(Comps::Thing_Appearance, Props::Thing_Filter_Pixelation, Property_Type::OneSizeF, DrPointF(1.0, 1.0),
                           "Pixelation", "Size of x and y pixels, larger numbers provide more pixelation.");
    addPropertyToComponent(Comps::Thing_Appearance, Props::Thing_Filter_Bitrate, Property_Type::Slider, std::vector<DrVariant>({256, 0, 256, 8, "", 0}),
                           "Bit Depth", "Standard output has color channel depth of 256, you can use this value to limit the number of available colors. "
                                        "Combining this with Pixelation gives a great retro look.");
    addPropertyToComponent(Comps::Thing_Appearance, Props::Thing_Filter_Brightness, Property_Type::Slider, std::vector<DrVariant>({0, -255, 255, 5, "", 0}),
                           "Brightness", "How light / dark this Thing should appear. \nDefault: \t0 \nRange: \t-255 to 255", hidden);
    addPropertyToComponent(Comps::Thing_Appearance, Props::Thing_Filter_Contrast, Property_Type::Slider, std::vector<DrVariant>({0, -255, 255, 5, "", 0}),
                           "Contrast", "Amount of distinguishable difference of colors. \nDefault: \t0 \nRange: \t-255 to 255", hidden);
    addPropertyToComponent(Comps::Thing_Appearance, Props::Thing_Filter_Saturation, Property_Type::Slider, std::vector<DrVariant>({0, -255, 255, 5, "", 0}),
                           "Saturation", "How colorful the colors appear. \nDefault: \t0 \nRange: \t-255 to 255", hidden);
    addPropertyToComponent(Comps::Thing_Appearance, Props::Thing_Filter_Hue, Property_Type::Slider, std::vector<DrVariant>({0, 0, 360, 5, "", 0}),
                           "Hue", "Rotate color values. \nDefault: \t0 \nRange: \t0 to 360", hidden);
    addPropertyToComponent(Comps::Thing_Appearance, Props::Thing_Filter_Grayscale, Property_Type::Bool, false,
                           "Grayscale", "Should this Thing be shown grayscale?", hidden);
    addPropertyToComponent(Comps::Thing_Appearance, Props::Thing_Filter_Negative, Property_Type::Bool, false,
                           "Negative", "Should this Thing's colors be inverted?", hidden);
}


//####################################################################################
//##    Special Effects Components
//####################################################################################
void DrThing::addComponentSpecialEffects() {
    addComponent(Comps::Thing_Special_Effects, "Special Effects", "Special effects that affect this Thing.", Component_Colors::RGB_03_Violet, false);
    getComponent(Comps::Thing_Special_Effects)->setIcon(Component_Icons::Effects);

    addPropertyToComponent(Comps::Thing_Special_Effects, Props::Thing_Filter_Convert_3D, Property_Type::Bool, false,
                           "Instant 3D", "Auto convert 2D graphics into 3D. Depth of Thing is customizable utilizing the "
                                         "\"3D Properties - Depth\" property.");
    addPropertyToComponent(Comps::Thing_Special_Effects, Props::Thing_Filter_Wireframe,
                           Property_Type::BoolDouble, std::vector<DrVariant>({false, 1.0, 0.0, 1000.0, 1.0, "Edge Width: "}),
                           "Wireframe", "Renders this Thing as an 3D outline, Thing 3D Depth should be greater than 0 for best effect.");
    addPropertyToComponent(Comps::Thing_Special_Effects, Props::Thing_Filter_Cartoon,
                           Property_Type::BoolDouble, std::vector<DrVariant>({false, 5.0, 0.0, 10, 1.0, "Edge Width: "}),
                           "Cartoon", "Gives this Thing a comic book look.");
    addPropertyToComponent(Comps::Thing_Special_Effects, Props::Thing_Filter_Cross_Hatch,
                           Property_Type::BoolDouble, std::vector<DrVariant>({false, 10.0, 1.0, 100, 5.0, "Hatch Width: "}),
                           "Cross Hatch", "Gives this Thing a sketched look.");
}

















