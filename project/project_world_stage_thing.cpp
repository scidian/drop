//
//      Created by Stephens Nunnally on 12/7/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      DrThing Class Definitions
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
#include "project_world_stage.h"
#include "project_world_stage_thing.h"
#include "settings/settings.h"
#include "settings/settings_component.h"
#include "settings/settings_component_property.h"

//####################################################################################
//##    Constructor, Destructor
//####################################################################################
DrThing::DrThing(DrProject *parent_project, DrWorld *parent_world, DrStage *parent_stage,
                 long new_thing_key, QString new_thing_name, DrThingType new_thing_type,
                 long from_asset_key,
                 double x, double y, double z,
                 bool should_collide) : DrSettings(parent_project) {
    m_parent_world = parent_world;                  // pointer to parent World
    m_parent_stage = parent_stage;                  // pointer to parent Stage

    this->setKey(new_thing_key);                    // assign key passed in from key generator, this key matches key in parent Stage map container

    m_thing_type = new_thing_type;                  // assign thing type
    m_asset_key =  from_asset_key;                  // associated asset key

    getComponentProperty(Components::Entity_Settings, Properties::Entity_Key)->setHidden(true);
    addPropertyToComponent(Components::Entity_Settings, Properties::Entity_Asset_Key, Property_Type::Int, QVariant::fromValue(from_asset_key),
                           "Asset Key", "ID Key of Asset this item represents.", false, false);

    DrAsset *asset = getParentProject()->findAssetFromKey(from_asset_key);


    // Call to load in all the components / properties for this Stage thing
    switch (new_thing_type) {
        case DrThingType::Character:
            addComponentSettingsCharacter(new_thing_name);
            addComponentTransform(asset->getWidth(), asset->getHeight(), x, -y, DrThingType::Character);
            addComponentLayering(z);
            addComponentLighting();
            addComponent3D();
            addComponentAppearance();
            addComponentSpecialEffects();
            break;

        case DrThingType::Object:
            addComponentSettingsObject(new_thing_name, should_collide);
            addComponentTransform(asset->getWidth(), asset->getHeight(), x, -y, DrThingType::Object);
            addComponentLayering(z);
            addComponentMovement();
            addComponentLighting();
            addComponent3D();
            addComponentAppearance();
            addComponentSpecialEffects();
            break;

        case DrThingType::Text:
            addComponentSettingsText(new_thing_name);
            addComponentTransform(asset->getWidth(), asset->getHeight(), x, -y, DrThingType::Text);
            addComponentLayering(z);
            break;

        // ***** DrEffectType DrThing's
        case DrThingType::Fire:
            addComponentSettingsFire();
            addComponentTransform(static_cast<int>(c_image_size * 0.625), c_image_size, x, -y, DrThingType::Fire);
            addComponentLayering(z, 100.0);
            addComponentAppearance(true);
            break;
        case DrThingType::Fisheye:
            addComponentSettingsFisheye();
            addComponentTransform(c_image_size, c_image_size, x, -y, DrThingType::Fisheye);
            addComponentLayering(z, 100.0);
            addComponentAppearance(true);
            break;
        case DrThingType::Light:
            addComponentSettingsLight(Qt::white);
            addComponentTransform(c_image_size, c_image_size, x, -y, DrThingType::Light);
            addComponentLayering(z);
            break;
        case DrThingType::Mirror:
            addComponentSettingsMirror();
            addComponentTransform(c_image_size, c_image_size, x, -y, DrThingType::Mirror);
            addComponentLayering(z);
            addComponentAppearance(true);
            break;
        case DrThingType::Swirl:
            addComponentSettingsSwirl();
            addComponentTransform(c_image_size, c_image_size, x, -y, DrThingType::Swirl);
            addComponentLayering(z);
            addComponentAppearance(true);
            break;
        case DrThingType::Water:
            addComponentSettingsWater();
            addComponentTransform(c_image_size, c_image_size, x, -y, DrThingType::Water);
            addComponentLayering(z, 75.0);
            addComponentAppearance(true);
            break;

        default:
            Dr::ShowMessageBox("Error in DrThing Constructor, DrThingType not handled! Type: " + Dr::StringFromThingType(new_thing_type),
                               QMessageBox::Icon::Critical, "Error!", Dr::GetActiveFormMain());
    }

}

DrThing::~DrThing() { }



//####################################################################################
//####################################################################################
//##
//##    Shared Components
//##
//####################################################################################
//####################################################################################
void DrThing::addComponentTransform(double width, double height, double x, double y, DrThingType type) {
    addComponent(Components::Thing_Transform, "Transform", "Sets the physical size and angle of the item in the stage.", Component_Colors::Green_SeaGrass, true);
    getComponent(Components::Thing_Transform)->setIcon(Component_Icons::Transform);
    addPropertyToComponent(Components::Thing_Transform, Properties::Thing_Position, Property_Type::PositionF, QPointF(x, y),
                           "Position", "Location of item within the current stage.");
    addPropertyToComponent(Components::Thing_Transform, Properties::Thing_Rotation, Property_Type::Angle, 0,
                           "Rotation", "Angle of item within the stage.");
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

void DrThing::addComponentLayering(double z, double opacity) {
    addComponent(Components::Thing_Layering, "Layering", "Controls the order items are drawn onto the screen. For \"Z Order\", lower numbers are "
                                                           "towards the back, higher towards the front.", Component_Colors::Blue_Yonder, true);
    getComponent(Components::Thing_Layering)->setIcon(Component_Icons::Layering);
    addPropertyToComponent(Components::Thing_Layering, Properties::Thing_Z_Order, Property_Type::Double, QVariant::fromValue(z),
                           "Z Order", "Arrangement of item along the z axis in the stage. Should be between " +
                                      QString::number(double(c_near_plane)) + " and " + QString::number(double(c_far_plane)) + " to be visible.");
    addPropertyToComponent(Components::Thing_Layering, Properties::Thing_Opacity, Property_Type::Percent, opacity,
                           "Opacity", "How transparent this item is, 0 (invisible) - 100 (solid)");
}

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
                           "Rotation", "Initial rotational speed Object on the Z Axis, +/- variable amount. <br><br> "
                                       "<b>NOTE:</b> Object Type must be <b>Kinematic</b> or <b>Dynamic</b> to use this setting!", false, false);
    addPropertyToComponent(Components::Thing_Movement, Properties::Thing_Angle_Velocity, Property_Type::Bool, true,
                           "Relative?", "Should the Rotation of this object affect it's Movement direction? (allows Object to move "
                                        "along an arc instead of spinning in place) <br> "
                                        "<b>NOTE:</b> Object Type must be <b>Kinematic</b> to use this setting!", false, false);
}

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
                           "Rotation X", "Starting rotation of object on the X Axis, +/- variable amount. Does not affect physics.");
    addPropertyToComponent(Components::Thing_3D, Properties::Thing_3D_Y_Axis_Rotation, Property_Type::Variable, QPointF(0, 0),
                           "Rotation Y", "Starting rotation of object on the Y Axis, +/- variable amount. Does not affect physics.");
    addPropertyToComponent(Components::Thing_3D, Properties::Thing_3D_X_Axis_Speed, Property_Type::Variable, QPointF(0, 0),
                           "Speed X", "Rotational speed of object on the X Axis, +/- variable amount. Does not affect physics.");
    addPropertyToComponent(Components::Thing_3D, Properties::Thing_3D_Y_Axis_Speed, Property_Type::Variable, QPointF(0, 0),
                           "Speed Y", "Rotational speed of object on the Y Axis, +/- variable amount. Does not affect physics.");
    addPropertyToComponent(Components::Thing_3D, Properties::Thing_3D_Billboard, Property_Type::Bool, false,
                           "Face Camera?", "Keeps this object facing toward camera no matter the camera position or camera angle.");
}

void DrThing::addComponentLighting() {
    addComponent(Components::Thing_Lighting, "Lighting", "Lighting settings for this item.", Component_Colors::Mustard_Yellow, true);
    getComponent(Components::Thing_Lighting)->setIcon(Component_Icons::Light);
    addPropertyToComponent(Components::Thing_Lighting, Properties::Thing_Lighting_Cast_Shadows, Property_Type::Bool, true,
                           "Cast Shadows", "This item will cast shadows when appearing higher on the z axis than a Light.");
}

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

void DrThing::addComponentSpecialEffects() {
    addComponent(Components::Thing_Special_Effects, "Special Effects", "Special effects that affect this item.", Component_Colors::Purple_Royal, true);
    getComponent(Components::Thing_Special_Effects)->setIcon(Component_Icons::Effects);
    addPropertyToComponent(Components::Thing_Special_Effects, Properties::Thing_Filter_Wireframe, Property_Type::Bool, false,
                           "Wireframe", "Renders this item as an 3d outline, item 3D Depth should be greater than 0 for best effect.");
}

//####################################################################################
//##    Camera Components
//####################################################################################
void DrThing::addComponentSettingsCamera(QString new_name) {
    DrProperty *property_name = getComponentProperty(Components::Entity_Settings, Properties::Entity_Name);
    property_name->setDisplayName("Camera Name");
    property_name->setDescription("Name of the current Camera.");
    property_name->setValue(new_name);

    addComponent(Components::Thing_Settings_Camera, "Camera Settings", "Settings for this Camera.", Component_Colors::Beige_Apricot, true);
    getComponent(Components::Thing_Settings_Camera)->setIcon(Component_Icons::Camera);
    addPropertyToComponent(Components::Thing_Settings_Camera, Properties::Thing_Camera_Zoom, Property_Type::Double, 10,
                           "Zoom Level", "Sets distance away from stage (0 to 1000)");
}

//####################################################################################
//##    Character Components
//####################################################################################
void DrThing::addComponentSettingsCharacter(QString new_name) {
    DrProperty *property_name = getComponentProperty(Components::Entity_Settings, Properties::Entity_Name);
    property_name->setDisplayName("Character Name");
    property_name->setDescription("Name of the current Character.");
    property_name->setValue(new_name);
}

//####################################################################################
//##    Object Components
//####################################################################################
void DrThing::addComponentSettingsObject(QString new_name, bool should_collide) {
    DrProperty *property_name = getComponentProperty(Components::Entity_Settings, Properties::Entity_Name);
    property_name->setDisplayName("Object Name");
    property_name->setDescription("Name of the current Object.");
    property_name->setValue(new_name);

    addComponent(Components::Thing_Settings_Object, "Object Settings", "Settings for current Object.", Component_Colors::White_Snow, true);
    getComponent(Components::Thing_Settings_Object)->setIcon(Component_Icons::Object);
    addPropertyToComponent(Components::Thing_Settings_Object, Properties::Thing_Object_Physics_Type, Property_Type::List, 0,
                           "Object Type", "<b>Static</b> - Can not move. <br> "
                                          "<b>Kinematic</b> - Moves at fixed speed. <br> "
                                          "<b>Dynamic</b> - Physics object.");
    addPropertyToComponent(Components::Thing_Settings_Object, Properties::Thing_Object_Collide, Property_Type::Bool, should_collide,
                           "Collide?", "Should this Object collide with Dynamic Objects? Objects not marked to collide "
                                       "still provide damage and sound reponses when coming into contact with other Objects.");
    addPropertyToComponent(Components::Thing_Settings_Object, Properties::Thing_Object_Damage, Property_Type::List, 0,
                           "Damage", "Choose the type of Object this will damage when coming into contact. By choosing \"Damage Player\" this "
                                     "Object will be treated as an enemy and vice versa.");
}

//####################################################################################
//##    Text Components
//####################################################################################
void DrThing::addComponentSettingsText(QString new_name) {
    DrProperty *property_name = getComponentProperty(Components::Entity_Settings, Properties::Entity_Name);
    property_name->setDisplayName("Text Name");
    property_name->setDescription("Name of the current Text Box.");
    property_name->setValue(new_name);

    addComponent(Components::Thing_Settings_Text, "Text Settings", "Settings for current Text Box.", Component_Colors::Orange_Medium, true);
    getComponent(Components::Thing_Settings_Text)->setIcon(Component_Icons::Font);
    addPropertyToComponent(Components::Thing_Settings_Text, Properties::Thing_Text_User_Text, Property_Type::String, "Text",
                           "User Text", "Custom text value to be shown in this Text Box.");
}


//####################################################################################
//####################################################################################
//##
//##    Effect Specific
//##
//####################################################################################
//####################################################################################

//####################################################################################
//##    Fire Components
//####################################################################################
void DrThing::addComponentSettingsFire() {
    DrProperty *property_name = getComponentProperty(Components::Entity_Settings, Properties::Entity_Name);
    property_name->setDisplayName("Effect Name");
    property_name->setDescription("Name of the current Effect.");
    property_name->setValue("Fire");

    addComponent(Components::Thing_Settings_Fire, "Fire Settings", "Settings for current Fire.", Component_Colors::Red_Faded, true);
    getComponent(Components::Thing_Settings_Fire)->setIcon(Component_Icons::Fire);

    addPropertyToComponent(Components::Thing_Settings_Fire, Properties::Thing_Fire_Shape, Property_Type::List, 0,
                           "Shape", "Defines a shape mask to use for the Fire.");
    addPropertyToComponent(Components::Thing_Settings_Fire, Properties::Thing_Fire_Color_1, Property_Type::Color, QColor(255,   0, 0, 255).rgba(),
                           "Top Color", "Top color of this Fire.");
    addPropertyToComponent(Components::Thing_Settings_Fire, Properties::Thing_Fire_Color_2, Property_Type::Color, QColor(255, 255, 0, 255).rgba(),
                           "Bottom Color", "Bottom color of this Fire.");
    addPropertyToComponent(Components::Thing_Settings_Fire, Properties::Thing_Fire_Color_Smoke, Property_Type::Color, QColor(0, 0, 0, 255).rgba(),
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
    property_name->setDescription("Name of the current Effect.");
    property_name->setValue("Fisheye");

    addComponent(Components::Thing_Settings_Fisheye, "Fisheye Lens Settings", "Settings for current Fisheye Lens.", Component_Colors::Orange_Medium, true);
    getComponent(Components::Thing_Settings_Fisheye)->setIcon(Component_Icons::Fisheye);
    addPropertyToComponent(Components::Thing_Settings_Fisheye, Properties::Thing_Fisheye_Color, Property_Type::Color, QColor(128, 128, 128, 255).rgba(),
                           "Color", "Color tint for this Lens.");
    addPropertyToComponent(Components::Thing_Settings_Fisheye, Properties::Thing_Fisheye_Color_Tint, Property_Type::Percent, 25.0,
                           "Tint Percent", "How much color to tint the Lens, 0 (none) - 100 (all the way)");
    addPropertyToComponent(Components::Thing_Settings_Fisheye, Properties::Thing_Fisheye_Lens_Zoom, Property_Type::RangedDouble, QList<QVariant>({ 2.25, 0.0, 10.0, 0.25 }),
                           "Lens Zoom", "How much to zoom the Fisheye Lens. Default is 2.25. Ranged from 0 to 10.");
}

//####################################################################################
//##    Light Components
//####################################################################################
void DrThing::addComponentSettingsLight(QColor color) {
    DrProperty *property_name = getComponentProperty(Components::Entity_Settings, Properties::Entity_Name);
    property_name->setDisplayName("Effect Name");
    property_name->setDescription("Name of the current Effect.");
    property_name->setValue("Light");

    addComponent(Components::Thing_Settings_Light, "Light Settings", "Settings for current Light.", Component_Colors::Mellow_Yellow, true);
    getComponent(Components::Thing_Settings_Light)->setIcon(Component_Icons::Light);
    addPropertyToComponent(Components::Thing_Settings_Light, Properties::Thing_Light_Type, Property_Type::List, 0,
                           "Light Type", "<b>Opaque</b> - Solid texture, does not provide lighting. <br> "
                                         "<b>Glow</b> - Provides diffuse lighting, no z ordering available. ");
    addPropertyToComponent(Components::Thing_Settings_Light, Properties::Thing_Light_Color, Property_Type::Color, color.rgba(),
                           "Light Color", "The Color for this Light.");
    addPropertyToComponent(Components::Thing_Settings_Light, Properties::Thing_Light_Cone_Start, Property_Type::Slider, QList<QVariant>({0, 0, 360, 5, "°"}),
                           "Cone Start", "Starting angle of light, Cone Angles travel in counter-clockwise direction.");
    addPropertyToComponent(Components::Thing_Settings_Light, Properties::Thing_Light_Cone_End, Property_Type::Slider, QList<QVariant>({360, 0, 360, 5, "°"}),
                           "Cone End", "Ending angle of light, Cone Angles travel in counter-clockwise direction.");
    addPropertyToComponent(Components::Thing_Settings_Light, Properties::Thing_Light_Intensity, Property_Type::Percent, 50.0,
                           "Intensity", "How intense the light is starting from the middle out.");
    addPropertyToComponent(Components::Thing_Settings_Light, Properties::Thing_Light_Blur, Property_Type::Percent, 50.0,
                           "Blur", "How much to blur the light's edges and shadows.");
    addPropertyToComponent(Components::Thing_Settings_Light, Properties::Thing_Light_Draw_Shadows, Property_Type::Bool, 50.0,
                           "Cast Shadows", "Should this light cast shadows? <b>NOTE:</b> The use of many shadow casting lights on the screen at one "
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
    property_name->setDescription("Name of the current Effect.");
    property_name->setValue("Mirror");

    addComponent(Components::Thing_Settings_Mirror, "Mirror Settings", "Settings for current Mirror.", Component_Colors::Silver_Snow, true);
    getComponent(Components::Thing_Settings_Mirror)->setIcon(Component_Icons::Mirror);
    addPropertyToComponent(Components::Thing_Settings_Mirror, Properties::Thing_Mirror_Start_Color, Property_Type::Color, QColor(255, 255, 255, 255).rgba(),
                           "Start Color", "Color tint for the top of this Mirror.");
    addPropertyToComponent(Components::Thing_Settings_Mirror, Properties::Thing_Mirror_End_Color, Property_Type::Color, QColor(170, 170, 170, 255).rgba(),
                           "End Color", "Color tint to fade to toward the bottom of this Mirror.");
    addPropertyToComponent(Components::Thing_Settings_Mirror, Properties::Thing_Mirror_Color_Tint, Property_Type::Percent, 50.0,
                           "Tint Percent", "How much color to tint the Mirror, 0 (none) - 100 (all the way)");
    addPropertyToComponent(Components::Thing_Settings_Mirror, Properties::Thing_Mirror_Blur, Property_Type::Double, 5.0,
                           "Scatter", "Amount to scatter reflection.");
    addPropertyToComponent(Components::Thing_Settings_Mirror, Properties::Thing_Mirror_Blur_Stretch, Property_Type::Double, 0.0,
                           "Scatter Stretch", "Amount to increase scatter away from start of mirror.");
    addPropertyToComponent(Components::Thing_Settings_Mirror, Properties::Thing_Mirror_Scale, Property_Type::RangedDouble, QList<QVariant>({ 1.0, 0.1, 100000.0, 0.1 }),
                           "Scatter Scale", "How much to scale the scatter noise. Default is 1.0. Must be greater than 0.");
}

//####################################################################################
//##    Swirl Components
//####################################################################################
void DrThing::addComponentSettingsSwirl() {
    DrProperty *property_name = getComponentProperty(Components::Entity_Settings, Properties::Entity_Name);
    property_name->setDisplayName("Effect Name");
    property_name->setDescription("Name of the current Effect.");
    property_name->setValue("Swirl");

    addComponent(Components::Thing_Settings_Swirl, "Swirl Settings", "Settings for current Swirl.", Component_Colors::Purple_Pastel, true);
    getComponent(Components::Thing_Settings_Swirl)->setIcon(Component_Icons::Swirl);
    addPropertyToComponent(Components::Thing_Settings_Swirl, Properties::Thing_Swirl_Start_Color, Property_Type::Color, QColor(255, 255, 255, 255).rgba(),
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
    property_name->setDescription("Name of the current Effect.");
    property_name->setValue("Water");

    addComponent(Components::Thing_Settings_Water, "Water Settings", "Settings for current Water.", Component_Colors::Blue_Drop_1, true);
    getComponent(Components::Thing_Settings_Water)->setIcon(Component_Icons::Water);
    addPropertyToComponent(Components::Thing_Settings_Water, Properties::Thing_Water_Texture, Property_Type::List, 1,
                           "Texture", "Defines a texture to use for the water surface, can give the Water several different looks.");
    addPropertyToComponent(Components::Thing_Settings_Water, Properties::Thing_Water_Start_Color, Property_Type::Color, QColor(41, 182, 246, 255).rgba(),
                           "Start Color", "Color tint for the top of this Water.");
    addPropertyToComponent(Components::Thing_Settings_Water, Properties::Thing_Water_End_Color, Property_Type::Color, QColor(0, 58, 103, 255).rgba(),
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

    addComponent(Components::Thing_Settings_Water_Foam, "Foam Settings", "Settings for top of the current Water.", Component_Colors::Blue_Drop_5, true);
    getComponent(Components::Thing_Settings_Water_Foam)->setIcon(Component_Icons::Water_Foam);
    addPropertyToComponent(Components::Thing_Settings_Water_Foam, Properties::Thing_Water_Surface_Color, Property_Type::Color, QColor(255, 255, 255, 255).rgba(),
                           "Color", "Color tint for the foam on the top of the Water.");
    addPropertyToComponent(Components::Thing_Settings_Water_Foam, Properties::Thing_Water_Surface_Tint, Property_Type::Percent, 75.0,
                           "Tint", "How much color to tint the foam on top of the Water, 0 (none) - 100 (all the way)");
    addPropertyToComponent(Components::Thing_Settings_Water_Foam, Properties::Thing_Water_Surface_Height, Property_Type::Double, 5.0,
                           "Height", "Thickness of the foam on top of the Water.");
    addPropertyToComponent(Components::Thing_Settings_Water_Foam, Properties::Thing_Water_Surface_Is_Flat, Property_Type::Bool, false,
                           "Keep Flat?", "Should the top of the water stay completely flat, even with waves and refraction?");

}





