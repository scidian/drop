//
//      Created by Stephens Nunnally on 12/7/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      DrThing Class Definitions
//
//
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
                 bool should_collide) {
    m_parent_project = parent_project;              // pointer to parent Project
    m_parent_world = parent_world;                  // pointer to parent World
    m_parent_stage = parent_stage;                  // pointer to parent Stage

    setKey(new_thing_key);                          // assign key passed in from key generator, this key matches key in parent Stage map container

    m_thing_type = new_thing_type;                  // assign thing type
    m_asset_key = from_asset_key;                   // associated asset key

    DrAsset *asset = m_parent_project->findAssetFromKey(from_asset_key);


    // Call to load in all the components / properties for this Stage thing
    switch (new_thing_type) {
        case DrThingType::Character:
            addComponentSettingsCharacter(new_thing_name);
            addComponentTransform(asset->getWidth(), asset->getHeight(), x, -y, DrThingType::Character);
            addComponentLayering(z);
            addComponentMovement();
            addComponentLighting();
            addComponentAppearance();
            break;

        case DrThingType::Object:
            addComponentSettingsObject(new_thing_name, should_collide);
            addComponentTransform(asset->getWidth(), asset->getHeight(), x, -y, DrThingType::Object);
            addComponentLayering(z);
            addComponentMovement();
            addComponentLighting();
            addComponentAppearance();
            break;

        case DrThingType::Text:
            addComponentSettingsText(new_thing_name);
            addComponentTransform(asset->getWidth(), asset->getHeight(), x, -y, DrThingType::Text);
            addComponentLayering(z);
            break;

        case DrThingType::Light:
            addComponentSettingsLight(Qt::white);
            addComponentTransform(400, 400, x, -y, DrThingType::Light);
            addComponentLayering(z);
            break;
    }

}

DrThing::~DrThing() { }



//####################################################################################
//##    Property loading
//####################################################################################

void DrThing::addComponentSettingsCharacter(QString new_name) {
    addComponent(Components::Entity_Name, "Name", "Name of selected item.", Component_Colors::Red_Tuscan, true);
    getComponent(Components::Entity_Name)->setIcon(Component_Icons::Name);
    addPropertyToComponent(Components::Entity_Name, Properties::Entity_Name, Property_Type::String, new_name,
                           "Character Name", "Name of the current Character.", false, false);

    addComponent(Components::Thing_Settings_Character, "Character Settings", "Settings for this Character.", Component_Colors::Mustard_Yellow, true);
    getComponent(Components::Thing_Settings_Character)->setIcon(Component_Icons::Character);
    addPropertyToComponent(Components::Thing_Settings_Character, Properties::Thing_Character_Jump_X, Property_Type::Double, 0,
                           "Jump Force X", "Force of jump button in x direction");
    addPropertyToComponent(Components::Thing_Settings_Character, Properties::Thing_Character_Jump_Y, Property_Type::Double, 0,
                           "Jump Force Y", "Force of jump button in y direction");
}

void DrThing::addComponentSettingsObject(QString new_name, bool should_collide) {
    addComponent(Components::Entity_Name, "Name", "Name of selected item.", Component_Colors::Red_Tuscan, true);
    getComponent(Components::Entity_Name)->setIcon(Component_Icons::Name);
    addPropertyToComponent(Components::Entity_Name, Properties::Entity_Name, Property_Type::String, new_name,
                           "Object Name", "Name of the current Object.", false, false);

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

void DrThing::addComponentSettingsLight(QColor color) {
    addComponent(Components::Entity_Name, "Name", "Name of selected item.", Component_Colors::Red_Tuscan, true);
    getComponent(Components::Entity_Name)->setIcon(Component_Icons::Name);
    addPropertyToComponent(Components::Entity_Name, Properties::Entity_Name, Property_Type::String, "Light",
                           "Effect Name", "Name of the current Effect.", false, false);

    addComponent(Components::Thing_Settings_Light, "Light Settings", "Settings for current Light.", Component_Colors::Mellow_Yellow, true);
    getComponent(Components::Thing_Settings_Light)->setIcon(Component_Icons::Light);
    addPropertyToComponent(Components::Thing_Settings_Light, Properties::Thing_Light_Type, Property_Type::List, 0,
                           "Light Type", "<b>Opaque</b> - Solid texture, does not provide lighting. <br> "
                                         "<b>Glow</b> - Provides diffuse lighting, no z ordering available. ");
    addPropertyToComponent(Components::Thing_Settings_Light, Properties::Thing_Light_Color, Property_Type::Color, color.rgba(),
                           "Light Color", "The Color for this Light.");
    addPropertyToComponent(Components::Thing_Settings_Light, Properties::Thing_Light_Cone_Start, Property_Type::FilterAngle, 0,
                           "Cone Start", "Starting angle of light, Cone Angles travel in counter-clockwise direction.");
    addPropertyToComponent(Components::Thing_Settings_Light, Properties::Thing_Light_Cone_End, Property_Type::FilterAngle, 360,
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

void DrThing::addComponentSettingsText(QString new_name) {
    addComponent(Components::Entity_Name, "Name", "Name of selected item.", Component_Colors::Red_Tuscan, true);
    getComponent(Components::Entity_Name)->setIcon(Component_Icons::Name);
    addPropertyToComponent(Components::Entity_Name, Properties::Entity_Name, Property_Type::String, new_name,
                           "Text Name", "Name of the current Text Box.", false, false);

    addComponent(Components::Thing_Settings_Text, "Text Settings", "Settings for current Text Box.", Component_Colors::Orange_Medium, true);
    getComponent(Components::Thing_Settings_Text)->setIcon(Component_Icons::Font);
    addPropertyToComponent(Components::Thing_Settings_Text, Properties::Thing_Text_User_Text, Property_Type::String, "Text",
                           "User Text", "Custom text value to be shown in this Text Box.");
}


void DrThing::addComponentSettingsCamera(QString new_name) {
    addComponent(Components::Entity_Name, "Name", "Name of selected item.", Component_Colors::Red_Tuscan, true);
    getComponent(Components::Entity_Name)->setIcon(Component_Icons::Name);
    addPropertyToComponent(Components::Entity_Name, Properties::Entity_Name, Property_Type::String, new_name,
                           "Camera Name", "Name of the current Camera.", false, false);

    addComponent(Components::Thing_Settings_Camera, "Camera Settings", "Settings for this Camera.", Component_Colors::Beige_Apricot, true);
    getComponent(Components::Thing_Settings_Camera)->setIcon(Component_Icons::Camera);
    addPropertyToComponent(Components::Thing_Settings_Camera, Properties::Thing_Camera_Zoom, Property_Type::Double, 10,
                           "Zoom Level", "Sets distance away from stage (0 to 1000)");
}




//####################################################################################
//##    Shared Components
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

void DrThing::addComponentLayering(double z) {
    addComponent(Components::Thing_Layering, "Layering", "Controls the order items are drawn onto the screen. For \"Z Order\", lower numbers are "
                                                           "towards the back, higher towards the front.", Component_Colors::Blue_Yonder, true);
    getComponent(Components::Thing_Layering)->setIcon(Component_Icons::Layering);
    addPropertyToComponent(Components::Thing_Layering, Properties::Thing_Z_Order, Property_Type::Double, QVariant::fromValue(z),
                           "Z Order", "Arrangement of item along the z axis in the stage. Should be between -5000 and 5000 to be visible.");
    addPropertyToComponent(Components::Thing_Layering, Properties::Thing_Opacity, Property_Type::Percent, 100,
                           "Opacity", "How transparent this item is, 0 (invisible) - 100 (solid)");
}

void DrThing::addComponentMovement() {
    addComponent(Components::Thing_Movement, "Movement", "Initial starting velocities of Object in stage. <br><br> "
                                                          "<b>NOTE:</b> Object Type must be <b>Kinematic</b> or <b>Dynamic</b> to use these settings!",
                                                          Component_Colors::Red_Faded, true);
    getComponent(Components::Thing_Movement)->setIcon(Component_Icons::Movement);
    addPropertyToComponent(Components::Thing_Movement, Properties::Thing_Velocity_X, Property_Type::Variable, QPointF(0, 0),
                           "Velocity X", "Initial horizontal movement speed of Object, +/- variable amount.  <br><br> "
                                         "<b>NOTE:</b> Object Type must be <b>Kinematic</b> or <b>Dynamic</b> to use this setting!", false, false);
    addPropertyToComponent(Components::Thing_Movement, Properties::Thing_Velocity_Y, Property_Type::Variable, QPointF(0, 0),
                           "Velocity Y", "Initial vertical movement speed of Object, +/- variable amount.  <br><br> "
                                         "<b>NOTE:</b> Object Type must be <b>Kinematic</b> or <b>Dynamic</b> to use this setting!", false, false);
    addPropertyToComponent(Components::Thing_Movement, Properties::Thing_Spin_Velocity, Property_Type::Variable, QPointF(0, 0),
                           "Spin Velocity", "Initial rotational speed Object, +/- variable amount. <br><br> "
                                               "<b>NOTE:</b> Object Type must be <b>Kinematic</b> or <b>Dynamic</b> to use this setting!", false, false);
    addPropertyToComponent(Components::Thing_Movement, Properties::Thing_Angle_Velocity, Property_Type::Bool, true,
                           "Angle Velocity?", "Should the angle of this Object affect it's velocity? (allows Object to move "
                                              "along an arc instead of spinning in place) <br> "
                                               "<b>NOTE:</b> Object Type must be <b>Kinematic</b> to use this setting!", false, false);
}

void DrThing::addComponentLighting() {
    addComponent(Components::Thing_Lighting, "Lighting", "Lighting settings for this item.", Component_Colors::Mustard_Yellow, true);
    getComponent(Components::Thing_Lighting)->setIcon(Component_Icons::Light);
    addPropertyToComponent(Components::Thing_Lighting, Properties::Thing_Lighting_Cast_Shadows, Property_Type::Bool, true,
                           "Cast Shadows", "This object will cast shadows when appearing higher on the z axis than a Light.");
}

void DrThing::addComponentAppearance() {
    addComponent(Components::Thing_Appearance, "Appearance", "Filters for items as they appear in the Stage. ", Component_Colors::Mellow_Yellow, true);
    getComponent(Components::Thing_Appearance)->setIcon(Component_Icons::Appearance);

    addPropertyToComponent(Components::Thing_Appearance, Properties::Thing_Filter_Pixelation, Property_Type::PositiveSizeF, QPointF(1.0, 1.0),
                           "Pixelation", "Size of x and y pixels, larger numbers provide more pixelation.");
    addPropertyToComponent(Components::Thing_Appearance, Properties::Thing_Filter_Brightness, Property_Type::Filter, 0,
                           "Brightness", "How light / dark this item should appear. \nDefault: \t0 \nRange: \t-255 to 255");
    addPropertyToComponent(Components::Thing_Appearance, Properties::Thing_Filter_Contrast, Property_Type::Filter, 0,
                           "Contrast", "Amount of distinguishable difference of colors. \nDefault: \t0 \nRange: \t-255 to 255");
    addPropertyToComponent(Components::Thing_Appearance, Properties::Thing_Filter_Saturation, Property_Type::Filter, 0,
                           "Saturation", "How colorful the colors appear. \nDefault: \t0 \nRange: \t-255 to 255");
    addPropertyToComponent(Components::Thing_Appearance, Properties::Thing_Filter_Hue, Property_Type::FilterAngle, 0,
                           "Hue", "Rotate color values. \nDefault: \t0 \nRange: \t0 to 360");
    addPropertyToComponent(Components::Thing_Appearance, Properties::Thing_Filter_Grayscale, Property_Type::Bool, false,
                           "Grayscale", "Should this item be shown grayscale?");
    addPropertyToComponent(Components::Thing_Appearance, Properties::Thing_Filter_Negative, Property_Type::Bool, false,
                           "Negative", "Should this item's colors be inverted?");
}











