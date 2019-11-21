//
//      Created by Stephens Nunnally on 10/20/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "forms/form_main.h"
#include "imaging/imaging.h"
#include "opengl/opengl.h"
#include "project/project.h"
#include "project/project_asset.h"
#include "project/project_world.h"
#include "project/project_stage.h"
#include "project/project_thing.h"
#include "settings/settings.h"
#include "settings/settings_component.h"
#include "settings/settings_component_property.h"

#include "debug.h"
#include "globals.h"
#include "helper.h"
#include "helper_qt.h"


//####################################################################################
//##    Transform Components
//####################################################################################
void DrThing::addComponentTransform(double width, double height, double x, double y, DrThingType type) {
    addComponent(Components::Thing_Transform, "Transform", "Sets the physical size and angle of the " + Dr::StringFromThingType(type) + " "
                                                           "in the Stage.", Component_Colors::Green_SeaGrass, true);
    getComponent(Components::Thing_Transform)->setIcon(Component_Icons::Transform);

    QString position_text = "Location of the " + Dr::StringFromThingType(type) + " within the current Stage.";
    if (type == DrThingType::Camera) {
        position_text = "Point at which Camera is looking at.";
    }
    addPropertyToComponent(Components::Thing_Transform, Properties::Thing_Position, Property_Type::PositionF, QPointF(x, y), "Position", position_text);

    QString transform_title = "Rotation Z";
    QString transform_text =  "Starting rotation, in degrees, of item on the Z Axis. Does affect physics collision shape.";
    if (type == DrThingType::Camera) {
        transform_title = "Camera Tilt";
        transform_text =  "Starting tilt / rotation of Camera on the Z Axis.";
    }
    addPropertyToComponent(Components::Thing_Transform, Properties::Thing_Rotation, Property_Type::Angle, 0, transform_title, transform_text);

    QString size_text = "Width and Height of the " + Dr::StringFromThingType(type) + " in pixels, affected by Scale property.";
    if (type == DrThingType::Light) {
        size_text = "Width and Height of the Light in pixels, affected by Scale property. "
                    "<br><b>NOTE:</b> For best performace on older devices, Light size is best kept under 4096.";
    } else if (type == DrThingType::Camera) {
        size_text = "Width and Height settings have no effect on Cameras.";
    }
    addPropertyToComponent(Components::Thing_Transform, Properties::Thing_Size, Property_Type::SizeF, QPointF(width, height), "Size", size_text);

    QString scale_text = "X and Y scale of the " + Dr::StringFromThingType(type) + " within the Stage.";
    if (type== DrThingType::Camera) {
        scale_text = "X and Y scale settings have no effect on Cameras.";
    }
    addPropertyToComponent(Components::Thing_Transform, Properties::Thing_Scale, Property_Type::ScaleF, QPointF(1, 1), "Scale", scale_text);
}


//####################################################################################
//##    Layering Components
//####################################################################################
void DrThing::addComponentLayering(double z, double opacity, bool show_component) {
    addComponent(Components::Thing_Layering, "Layering", "Controls the order items are drawn onto the screen. For \"Z-Order\", lower numbers are "
                                                         "towards the back, higher towards the front.", Component_Colors::Blue_Yonder, show_component);
    getComponent(Components::Thing_Layering)->setIcon(Component_Icons::Layering);

    addPropertyToComponent(Components::Thing_Layering, Properties::Thing_Z_Order, Property_Type::Double, 0,
                           "Z-Order", "Arrangement of item along the z axis in the Stage. Should be between the near and far clipping planes (" +
                                      QString::number(double(c_near_plane)) + " and " + QString::number(double(c_far_plane)) + ") to be visible.");
    addPropertyToComponent(Components::Thing_Layering, Properties::Thing_Sub_Z_Order, Property_Type::Int, 0,
                           "Sub Order", "Arrangement of item compared to other items within the same Z-Order.", true, false);
    addPropertyToComponent(Components::Thing_Layering, Properties::Thing_Opacity, Property_Type::Percent, opacity,
                           "Opacity", "How transparent this item is, 0 (invisible) - 100 (solid)");
    setZOrderWithSub(z, Z_Insert::Front);
}


//####################################################################################
//##    Movement Components
//####################################################################################
void DrThing::addComponentMovement() {
    addComponent(Components::Thing_Movement, "Movement", "Initial starting velocities of Object in Stage. <br><br> "
                                                          "<b>NOTE:</b> Object Type must be <b>Kinematic</b> or <b>Dynamic</b> to use these settings!",
                                                          Component_Colors::Red_Faded, true);
    getComponent(Components::Thing_Movement)->setIcon(Component_Icons::Movement);

    addPropertyToComponent(Components::Thing_Movement, Properties::Thing_Velocity_X, Property_Type::Variable, QPointF(0, 0),
                           "Horizontal", "Initial horizontal movement speed of Object, +/- variable amount.  <br><br> "
                                         "<b>NOTE:</b> Object Type must be <b>Kinematic</b> or <b>Dynamic</b> to use this setting!", false, false);
    addPropertyToComponent(Components::Thing_Movement, Properties::Thing_Velocity_Y, Property_Type::Variable, QPointF(0, 0),
                           "Vertical",  "Initial vertical movement speed of Object, +/- variable amount.  <br><br> "
                                        "<b>NOTE:</b> Object Type must be <b>Kinematic</b> or <b>Dynamic</b> to use this setting!", false, false);
    addPropertyToComponent(Components::Thing_Movement, Properties::Thing_Spin_Velocity, Property_Type::Variable, QPointF(0, 0),
                           "Rotate Speed Z", "Initial rotational speed Object on the Z Axis, +/- variable amount. <br><br> "
                                             "<b>NOTE:</b> Object Type must be <b>Kinematic</b> or <b>Dynamic</b> to use this setting!", false, false);
    addPropertyToComponent(Components::Thing_Movement, Properties::Thing_Angle_Velocity, Property_Type::Bool, true,
                           "Relative Angle?", "Should the Rotation and Scale of this Object affect it's Movement direction? (allows Object to move "
                                              "along an arc instead of spinning in place) <br> "
                                              "<b>NOTE:</b> Object Type must be <b>Kinematic</b> to use this setting!", false, false);
    addPropertyToComponent(Components::Thing_Movement, Properties::Thing_Angle_Player, Property_Type::Bool, false,
                           "Rotate To Player?", "Should the Rotation of this Object try and point towards (Y Axis Up) the active player? Rotates this"
                                                "direction at Rotate Speed Z speed. <br> "
                                                "<b>NOTE:</b> Object Type must be <b>Kinematic</b> to use this setting!", false, false);
}


//####################################################################################
//##    Spawn Components
//####################################################################################
void DrThing::addComponentSpawn() {
    addComponent(Components::Thing_Spawn, "Spawning", "Initial starting velocities of Object in stage. <br><br> "
                                                      "<b>NOTE:</b> Object Type must be <b>Kinematic</b> or <b>Dynamic</b> to use these settings!",
                                                      Component_Colors::Orange_Pastel, true);
    getComponent(Components::Thing_Spawn)->setIcon(Component_Icons::Spawn);

    addPropertyToComponent(Components::Thing_Spawn, Properties::Thing_Spawn_Count, Property_Type::Int, 1,
                           "Spawn Count", "How many instances of this item will spawn. A value of -1 will allow unlimited spawning.");
    addPropertyToComponent(Components::Thing_Spawn, Properties::Thing_Spawn_Rate, Property_Type::Variable, QPointF(1, 0),
                           "Spawn Rate", "Rate, in seconds, this item will spawn, +/- variable amount.");
    addPropertyToComponent(Components::Thing_Spawn, Properties::Thing_Spawn_Instantly, Property_Type::Bool, true,
                           "Spawn Immediately", "If this option is not selected, item will wait the initial Spawn Rate amount of time before spawning "
                                                "the first time. When selected, item will spawn as soon as the event occurs and then wait Spawn Rate in between "
                                                "the remaining spawms.");
    addPropertyToComponent(Components::Thing_Spawn, Properties::Thing_Spawn_Type, Property_Type::List, 0,
                           "Spawn Type", "What causes this item to spawn. Permanent is the default. Selecting Shoot or Jump will cause item to "
                                         "spawn when corresponding buttons are pressed. ");
    // QList<QVariant> of 6 values: bool, int value,    min, max, int step size,    string spinText
    addPropertyToComponent(Components::Thing_Spawn, Properties::Thing_Spawn_At_Object, Property_Type::BoolInt,
                           QList<QVariant>({false, 1001, 0, 1000000000, 1, "Object ID: "}),
                           "Spawn At Object", "This will cause this item to spawn at the location of the Object with the designated ID Key. <br>"
                                              "<b>NOTE:</b> Enter instance <i>Object</i> or <i>Character</i> ID Keys here, "
                                              "<b>NOT</b> Asset ID Keys. Enter <b>\"0\"</b> here to stay attached to active character.");
    addPropertyToComponent(Components::Thing_Spawn, Properties::Thing_Spawn_Offset_X, Property_Type::Variable, QPointF(0, 0),
                           "Spawn Offset X", "This is a X offset for use with spawning, +/- variable amount. "
                                             "Useful for some randomess to object location.");
    addPropertyToComponent(Components::Thing_Spawn, Properties::Thing_Spawn_Offset_Y, Property_Type::Variable, QPointF(0, 0),
                           "Spawn Offset Y", "This is a Y offset for use with spawning, +/- variable amount. "
                                             "Useful for some randomess to object location.");
}


//####################################################################################
//##    3D Components
//####################################################################################
void DrThing::addComponent3D() {
    addComponent(Components::Thing_3D, "3D Properties", "Turns 2D Objects in 3D, rotates 3D Objects. Extrusion works best for images "
                                                        "whose pixels are mostly opaque. The World Setting \"Instant 3D\" setting under "
                                                        "the \"Appearance\" category must be checked to see 3D.",
                                                        Component_Colors::Brown_Sugar, true);
    getComponent(Components::Thing_3D)->setIcon(Component_Icons::Object);

    addPropertyToComponent(Components::Thing_3D, Properties::Thing_3D_Type, Property_Type::List, 0,
                           "Type", "How this object is represented in 3D. The texture can be Extruded (pulled) into the 3rd dimension. Or it can "
                                   "be Wrapped onto a Cube.");
    addPropertyToComponent(Components::Thing_3D, Properties::Thing_3D_Depth, Property_Type::PositiveDouble, 50.0,
                           "Depth", "Depth of Object in pixels when appearing in 3D.");
    addPropertyToComponent(Components::Thing_3D, Properties::Thing_3D_X_Axis_Rotation, Property_Type::Variable, QPointF(0, 0),
                           "Rotation X", "Starting rotation of object on the X Axis, +/- variable amount. Does not affect physics collision shape.");
    addPropertyToComponent(Components::Thing_3D, Properties::Thing_3D_Y_Axis_Rotation, Property_Type::Variable, QPointF(0, 0),
                           "Rotation Y", "Starting rotation of object on the Y Axis, +/- variable amount. Does not affect physics collision shape.");
    addPropertyToComponent(Components::Thing_3D, Properties::Thing_3D_X_Axis_Speed, Property_Type::Variable, QPointF(0, 0),
                           "Rotate Speed X", "Rotational speed of object on the X Axis, +/- variable amount. Does not affect physics collision shape.");
    addPropertyToComponent(Components::Thing_3D, Properties::Thing_3D_Y_Axis_Speed, Property_Type::Variable, QPointF(0, 0),
                           "Rotate Speed Y", "Rotational speed of object on the Y Axis, +/- variable amount. Does not affect physics collision shape.");
    addPropertyToComponent(Components::Thing_3D, Properties::Thing_3D_Billboard, Property_Type::Bool, false,
                           "Face Camera?", "Keeps this object facing toward camera no matter the camera position or camera angle.");
}


//####################################################################################
//##    Lighting Components
//####################################################################################
void DrThing::addComponentLighting() {
    addComponent(Components::Thing_Lighting, "Lighting", "Lighting settings for this item.", Component_Colors::Mustard_Yellow, true);
    getComponent(Components::Thing_Lighting)->setIcon(Component_Icons::Light);

    addPropertyToComponent(Components::Thing_Lighting, Properties::Thing_Lighting_Cast_Shadows, Property_Type::Bool, true,
                           "Cast Shadows?", "This item will cast shadows when appearing higher on the z axis than a Light.");
}


//####################################################################################
//##    Appearance Components
//####################################################################################
void DrThing::addComponentAppearance(bool bitrate_and_pixel_only, bool show_component) {
    addComponent(Components::Thing_Appearance, "Appearance", "Filters for items as they appear in the Stage.", Component_Colors::Purple_Royal, show_component);
    getComponent(Components::Thing_Appearance)->setIcon(Component_Icons::Appearance);

    addPropertyToComponent(Components::Thing_Appearance, Properties::Thing_Filter_Bitrate, Property_Type::Slider, QList<QVariant>({256, 0, 256, 8, ""}),
                           "Bit Depth", "Standard output has color channel depth of 256, you can use this value to limit the number of available colors. "
                                        "Combining this with Pixelation gives a great retro look.");
    addPropertyToComponent(Components::Thing_Appearance, Properties::Thing_Filter_Pixelation, Property_Type::PositiveSizeF, QPointF(1.0, 1.0),
                           "Pixelation", "Size of x and y pixels, larger numbers provide more pixelation.");
    bool hidden = bitrate_and_pixel_only;
    addPropertyToComponent(Components::Thing_Appearance, Properties::Thing_Filter_Brightness, Property_Type::Slider, QList<QVariant>({0, -255, 255, 5, ""}),
                           "Brightness", "How light / dark this item should appear. \nDefault: \t0 \nRange: \t-255 to 255", hidden);
    addPropertyToComponent(Components::Thing_Appearance, Properties::Thing_Filter_Contrast, Property_Type::Slider, QList<QVariant>({0, -255, 255, 5, ""}),
                           "Contrast", "Amount of distinguishable difference of colors. \nDefault: \t0 \nRange: \t-255 to 255", hidden);
    addPropertyToComponent(Components::Thing_Appearance, Properties::Thing_Filter_Saturation, Property_Type::Slider, QList<QVariant>({0, -255, 255, 5, ""}),
                           "Saturation", "How colorful the colors appear. \nDefault: \t0 \nRange: \t-255 to 255", hidden);
    addPropertyToComponent(Components::Thing_Appearance, Properties::Thing_Filter_Hue, Property_Type::Slider, QList<QVariant>({0, 0, 360, 5, ""}),
                           "Hue", "Rotate color values. \nDefault: \t0 \nRange: \t0 to 360", hidden);
    addPropertyToComponent(Components::Thing_Appearance, Properties::Thing_Filter_Grayscale, Property_Type::Bool, false,
                           "Grayscale", "Should this item be shown grayscale?", hidden);
    addPropertyToComponent(Components::Thing_Appearance, Properties::Thing_Filter_Negative, Property_Type::Bool, false,
                           "Negative", "Should this item's colors be inverted?", hidden);
}


//####################################################################################
//##    Special Effects Components
//####################################################################################
void DrThing::addComponentSpecialEffects() {
    addComponent(Components::Thing_Special_Effects, "Special Effects", "Special effects that affect this item.", Component_Colors::Blue_Royal, true);
    getComponent(Components::Thing_Special_Effects)->setIcon(Component_Icons::Effects);

    addPropertyToComponent(Components::Thing_Special_Effects, Properties::Thing_Filter_Convert_3D, Property_Type::Bool, false,
                           "Instant 3D", "Auto convert 2D graphics into 3D. Depth of item is customizable utilizing the "
                                         "\"3D Properties - Depth\" property.");
    addPropertyToComponent(Components::Thing_Special_Effects, Properties::Thing_Filter_Wireframe, Property_Type::Bool, false,
                           "Wireframe", "Renders this item as an 3d outline, item 3D Depth should be greater than 0 for best effect.");
    addPropertyToComponent(Components::Thing_Special_Effects, Properties::Thing_Filter_Cartoon,
                           Property_Type::BoolDouble, QList<QVariant>({false, 5.0, 0.0, 10, 1.0, "Edge Width: "}),
                           "Cartoon", "Gives this item a comic book look.");
    addPropertyToComponent(Components::Thing_Special_Effects, Properties::Thing_Filter_Cross_Hatch,
                           Property_Type::BoolDouble, QList<QVariant>({false, 10.0, 1.0, 100, 5.0, "Hatch Width: "}),
                           "Cross Hatch", "Gives this item a sketched look.");
}

















