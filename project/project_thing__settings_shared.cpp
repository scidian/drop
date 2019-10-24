//
//      Created by Stephens Nunnally on 10/20/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "debug.h"
#include "forms/form_main.h"
#include "globals.h"
#include "helper.h"
#include "helper_qt.h"
#include "imaging/imaging.h"
#include "opengl/opengl.h"
#include "project.h"
#include "project_asset.h"
#include "project_world.h"
#include "project_stage.h"
#include "project_thing.h"
#include "settings/settings.h"
#include "settings/settings_component.h"
#include "settings/settings_component_property.h"


//####################################################################################
//##    Transform Components
//####################################################################################
void DrThing::addComponentTransform(double width, double height, double x, double y, DrThingType type) {
    addComponent(Components::Thing_Transform, "Transform", "Sets the physical size and angle of the item in the stage.", Component_Colors::Green_SeaGrass, true);
    getComponent(Components::Thing_Transform)->setIcon(Component_Icons::Transform);

    addPropertyToComponent(Components::Thing_Transform, Properties::Thing_Position, Property_Type::PositionF, QPointF(x, y),
                           "Position", "Location of item within the current stage.");
    addPropertyToComponent(Components::Thing_Transform, Properties::Thing_Rotation, Property_Type::Angle, 0,
                           "Rotation Z", "Starting rotation of object on the Z Axis. Does affect physics collision shape.");
    QString size_text = "Width and Height of item in pixels, affected by Scale property.";
    if (type == DrThingType::Light) {
            size_text = "Width and Height of item in pixels, affected by Scale property. "
                        "<br><b>NOTE:</b> For best performace on older devices, Light size is best kept under 4096.";
    }
    addPropertyToComponent(Components::Thing_Transform, Properties::Thing_Size, Property_Type::SizeF, QPointF(width, height),
                           "Size", size_text);
    addPropertyToComponent(Components::Thing_Transform, Properties::Thing_Scale, Property_Type::ScaleF, QPointF(1, 1),
                           "Scale", "X and Y scale of item within the stage.");
}


//####################################################################################
//##    Layering Components
//####################################################################################
void DrThing::addComponentLayering(double z, double opacity) {
    addComponent(Components::Thing_Layering, "Layering", "Controls the order items are drawn onto the screen. For \"Z Order\", lower numbers are "
                                                         "towards the back, higher towards the front.", Component_Colors::Blue_Yonder, true);
    getComponent(Components::Thing_Layering)->setIcon(Component_Icons::Layering);

    addPropertyToComponent(Components::Thing_Layering, Properties::Thing_Z_Order, Property_Type::Double, 0,
                           "Z Order", "Arrangement of item along the z axis in the stage. Should be between the near and far clipping planes (" +
                                      QString::number(double(c_near_plane)) + " and " + QString::number(double(c_far_plane)) + ") to be visible.");
    addPropertyToComponent(Components::Thing_Layering, Properties::Thing_Sub_Z_Order, Property_Type::Int, 0,
                           "Sub Order", "Arrangement of item compared to other items within the same Z Order.", true, false);
    addPropertyToComponent(Components::Thing_Layering, Properties::Thing_Opacity, Property_Type::Percent, opacity,
                           "Opacity", "How transparent this item is, 0 (invisible) - 100 (solid)");
    setZOrderWithSub(z, Z_Insert::Front);
}


//####################################################################################
//##    Movement Components
//####################################################################################
void DrThing::addComponentMovement() {
    addComponent(Components::Thing_Movement, "Movement", "Initial starting velocities of Object in stage. <br><br> "
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
                           "Rotation Z", "Initial rotational speed Object on the Z Axis, +/- variable amount. <br><br> "
                                         "<b>NOTE:</b> Object Type must be <b>Kinematic</b> or <b>Dynamic</b> to use this setting!", false, false);
    addPropertyToComponent(Components::Thing_Movement, Properties::Thing_Angle_Velocity, Property_Type::Bool, true,
                           "Relative?", "Should the Rotation of this object affect it's Movement direction? (allows Object to move "
                                        "along an arc instead of spinning in place) <br> "
                                        "<b>NOTE:</b> Object Type must be <b>Kinematic</b> to use this setting!", false, false);
}


//####################################################################################
//##    3D Components
//####################################################################################
void DrThing::addComponent3D() {
    addComponent(Components::Thing_3D, "3D Properties", "Turns 2D Objects in 3D, rotates 3D Objects. Extrusion works best for images whose pixels are "
                                       "mostly opaque.", Component_Colors::Orange_Pastel, true);
    getComponent(Components::Thing_3D)->setIcon(Component_Icons::Object);

    addPropertyToComponent(Components::Thing_3D, Properties::Thing_3D_Type, Property_Type::List, 0,
                           "Type", "How this object is represented in 3D. The texture can be Extruded (pulled) into the 3rd dimension. Or it can be Wrapped "
                                   "onto a Cube.");
    addPropertyToComponent(Components::Thing_3D, Properties::Thing_3D_Depth, Property_Type::PositiveDouble, 50.0,
                           "Depth", "Depth of Object in pixels when appearing in 3D.");
    addPropertyToComponent(Components::Thing_3D, Properties::Thing_3D_X_Axis_Rotation, Property_Type::Variable, QPointF(0, 0),
                           "Rotation X", "Starting rotation of object on the X Axis, +/- variable amount. Does not affect physics collision shape.");
    addPropertyToComponent(Components::Thing_3D, Properties::Thing_3D_Y_Axis_Rotation, Property_Type::Variable, QPointF(0, 0),
                           "Rotation Y", "Starting rotation of object on the Y Axis, +/- variable amount. Does not affect physics collision shape.");
    addPropertyToComponent(Components::Thing_3D, Properties::Thing_3D_X_Axis_Speed, Property_Type::Variable, QPointF(0, 0),
                           "Speed X", "Rotational speed of object on the X Axis, +/- variable amount. Does not affect physics collision shape.");
    addPropertyToComponent(Components::Thing_3D, Properties::Thing_3D_Y_Axis_Speed, Property_Type::Variable, QPointF(0, 0),
                           "Speed Y", "Rotational speed of object on the Y Axis, +/- variable amount. Does not affect physics collision shape.");
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
                           "Cast Shadows", "This item will cast shadows when appearing higher on the z axis than a Light.");
}


//####################################################################################
//##    Appearance Components
//####################################################################################
void DrThing::addComponentAppearance(bool bitrate_and_pixel_only) {
    addComponent(Components::Thing_Appearance, "Appearance", "Filters for items as they appear in the Stage.", Component_Colors::Brown_Sugar, true);
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
    addComponent(Components::Thing_Special_Effects, "Special Effects", "Special effects that affect this item.", Component_Colors::Purple_Royal, true);
    getComponent(Components::Thing_Special_Effects)->setIcon(Component_Icons::Effects);

    addPropertyToComponent(Components::Thing_Special_Effects, Properties::Thing_Filter_Wireframe, Property_Type::Bool, false,
                           "Wireframe", "Renders this item as an 3d outline, item 3D Depth should be greater than 0 for best effect.");
}
















