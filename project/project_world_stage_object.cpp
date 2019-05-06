//
//      Created by Stephens Nunnally on 12/7/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      DrObject Class Definitions
//
//
#include "project.h"
#include "project_asset.h"
#include "project_world.h"
#include "project_world_stage.h"
#include "project_world_stage_object.h"
#include "settings/settings.h"
#include "settings/settings_component.h"
#include "settings/settings_component_property.h"

//####################################################################################
//##    Constructor, Destructor
//####################################################################################
DrObject::DrObject(DrProject *parent_project, DrWorld *parent_world, DrStage *parent_stage,
                   long new_object_key, QString new_object_name, DrObjectType new_object_type,
                   long from_asset_key, double x, double y, long z)
{
    m_parent_project = parent_project;              // pointer to parent Project
    m_parent_world = parent_world;                  // pointer to parent World
    m_parent_stage = parent_stage;                  // pointer to parent Stage

    setKey(new_object_key);                         // assign key passed in from key generator, this key matches key in parent Stage map container

    m_object_type = new_object_type;                // assign object type
    m_asset_key = from_asset_key;                   // associated asset key

    DrAsset *asset = m_parent_project->findAssetFromKey(from_asset_key);


    // Call to load in all the components / properties for this Stage object
    switch (new_object_type) {
    case DrObjectType::Object:
        addComponentSettingsObject(new_object_name);
        addComponentTransform(asset->getWidth(), asset->getHeight(), x, y, z);
        addComponentMovement();
        addComponentAppearance();
        break;

    case DrObjectType::Text:
        addComponentSettingsText(new_object_name);
        addComponentTransform(asset->getWidth(), asset->getHeight(), x, y, z);
        break;
    case DrObjectType::Character:
        addComponentSettingsCharacter(new_object_name);
        addComponentTransform(asset->getWidth(), asset->getHeight(), x, y, z);
        addComponentMovement();
        addComponentAppearance();
        break;

    //case DrObjectType::Camera:
    //    addComponentSettingsCamera(new_object_name);
    //    break;
    }

}

DrObject::~DrObject()
{

}



//####################################################################################
//##    Property loading
//####################################################################################

void DrObject::addComponentSettingsObject(QString new_name)
{
    addComponent(Components::Object_Settings, "Settings", "Basic settings for current object.", Component_Colors::White_Snow, true);
    getComponent(Components::Object_Settings)->setIcon(Component_Icons::Settings);
    addPropertyToComponent(Components::Object_Settings, Properties::Object_Name, Property_Type::String, new_name,
                           "Object Name", "Name of the current object.", false, false);
    addPropertyToComponent(Components::Object_Settings, Properties::Object_Physics, Property_Type::Bool, false,
                           "Physics?", "Should this item be effected by physics?");
    addPropertyToComponent(Components::Object_Settings, Properties::Object_Collide, Property_Type::Bool, true,
                           "Collide?", "Should this item collide with other items?");
    addPropertyToComponent(Components::Object_Settings, Properties::Object_Damage, Property_Type::List, 0,
                           "Damage", "What should this item damage when it collides with something else.");
}


void DrObject::addComponentSettingsText(QString new_name)
{
    addComponent(Components::Object_Settings, "Settings", "Basic settings for current object.", Component_Colors::White_Snow, true);
    getComponent(Components::Object_Settings)->setIcon(Component_Icons::Settings);
    addPropertyToComponent(Components::Object_Settings, Properties::Object_Name, Property_Type::String, new_name,
                           "Object Name", "Name of the current object.", false, false);

    addComponent(Components::Object_Settings_Text, "Text Settings", "Settings for this text", Component_Colors::Orange_Medium, true);
    getComponent(Components::Object_Settings_Text)->setIcon(Component_Icons::Font);
    addPropertyToComponent(Components::Object_Settings_Text, Properties::Object_Text_User_Text, Property_Type::String, "Text",
                           "User Text", "Custom text value to be shown in this text object.");
}


void DrObject::addComponentSettingsCharacter(QString new_name)
{
    addComponent(Components::Object_Settings, "Settings", "Basic settings for current object.", Component_Colors::White_Snow, true);
    getComponent(Components::Object_Settings)->setIcon(Component_Icons::Settings);
    addPropertyToComponent(Components::Object_Settings, Properties::Object_Name, Property_Type::String, new_name,
                           "Object Name", "Name of the current object.", false, false);

    addComponent(Components::Object_Settings_Character, "Character Settings", "Settings for this character", Component_Colors::Mustard_Yellow, true);
    getComponent(Components::Object_Settings_Character)->setIcon(Component_Icons::Character);
    addPropertyToComponent(Components::Object_Settings_Character, Properties::Object_Character_Jump_X, Property_Type::Double, 0,
                           "Jump Force X", "Force of jump button in x direction");
    addPropertyToComponent(Components::Object_Settings_Character, Properties::Object_Character_Jump_Y, Property_Type::Double, 0,
                           "Jump Force Y", "Force of jump button in y direction");
}

void DrObject::addComponentSettingsCamera(QString new_name)
{
    addComponent(Components::Object_Settings, "Settings", "Basic settings for current object.", Component_Colors::White_Snow, true);
    getComponent(Components::Object_Settings)->setIcon(Component_Icons::Settings);
    addPropertyToComponent(Components::Object_Settings, Properties::Object_Name, Property_Type::String, new_name,
                           "Object Name", "Name of the current object.", false, false);

    addComponent(Components::Object_Settings_Camera, "Camera Settings", "Settings for this camera", Component_Colors::Beige_Apricot, true);
    getComponent(Components::Object_Settings_Camera)->setIcon(Component_Icons::Camera);
    addPropertyToComponent(Components::Object_Settings_Camera, Properties::Object_Camera_Zoom, Property_Type::Double, 10,
                           "Zoom Level", "Sets distance away from stage (0 to 1000)");
}




//####################################################################################
//##    Shared Components
//####################################################################################
void DrObject::addComponentTransform(double width, double height, double x, double y, long z)
{
    addComponent(Components::Object_Transform, "Transform", "Sets the physical size and angle of the item in the stage.", Component_Colors::Green_SeaGrass, true);
    getComponent(Components::Object_Transform)->setIcon(Component_Icons::Transform);
    addPropertyToComponent(Components::Object_Transform, Properties::Object_Position, Property_Type::PositionF, QPointF(x, y),
                           "Position", "Location of item within the current stage.");
    addPropertyToComponent(Components::Object_Transform, Properties::Object_Rotation, Property_Type::Angle, 0,
                           "Rotation", "Angle of item within the stage.");
    addPropertyToComponent(Components::Object_Transform, Properties::Object_Size, Property_Type::SizeF, QPointF(width, height),
                           "Size", "Width and Height of object in pixels, affected by Scale property.");
    addPropertyToComponent(Components::Object_Transform, Properties::Object_Scale, Property_Type::ScaleF, QPointF(1, 1),
                           "Scale", "X and Y scale of item within the stage.");

    addComponent(Components::Object_Layering, "Layering", "Controls the order items are drawn onto the screen. For \"Z Order\", lower numbers are "
                                                           "towards the back, higher towards the front.", Component_Colors::Blue_Yonder, true);
    getComponent(Components::Object_Layering)->setIcon(Component_Icons::Layering);
    addPropertyToComponent(Components::Object_Layering, Properties::Object_Z_Order, Property_Type::Int, QVariant::fromValue(z),
                           "Z Order", "Arrangement of item along the z axis in the stage");
    addPropertyToComponent(Components::Object_Layering, Properties::Object_Opacity, Property_Type::Percent, 100,
                           "Opacity", "How transparent an item is, 0 (invisible) - 100 (solid)");
}

void DrObject::addComponentMovement()
{
    addComponent(Components::Object_Movement, "Movement", "Initial starting velocities of item in stage.", Component_Colors::Red_Faded, true);
    getComponent(Components::Object_Movement)->setIcon(Component_Icons::Movement);
    addPropertyToComponent(Components::Object_Movement, Properties::Object_Velocity_X, Property_Type::Variable, QPointF(0, 0),
                           "Velocity X", "Initial horizontal movement speed of item +/- variable amount.");
    addPropertyToComponent(Components::Object_Movement, Properties::Object_Velocity_Y, Property_Type::Variable, QPointF(0, 0),
                           "Velocity Y", "Initial vertical movement speed of item +/- variable amount.");
    addPropertyToComponent(Components::Object_Movement, Properties::Object_Angular_Velocity, Property_Type::Variable, QPointF(0, 0),
                           "Angular Velocity", "Rotational movement speed of item +/- variable amount.");
}

void DrObject::addComponentAppearance()
{
    addComponent(Components::Object_Appearance, "Appearance", "Filters for objects as they appear in the Stage. ", Component_Colors::Mellow_Yellow, true);
    getComponent(Components::Object_Appearance)->setIcon(Component_Icons::Appearance);
    addPropertyToComponent(Components::Object_Appearance, Properties::Object_Filter_Brightness, Property_Type::Filter, 0,
                           "Brightness", "How light / dark this object should appear. \nDefault: \t0 \nRange: \t-255 to 255");
    addPropertyToComponent(Components::Object_Appearance, Properties::Object_Filter_Contrast, Property_Type::Filter, 0,
                           "Contrast", "Amount of distinguishable difference of colors. \nDefault: \t0 \nRange: \t-255 to 255");
    addPropertyToComponent(Components::Object_Appearance, Properties::Object_Filter_Saturation, Property_Type::Filter, 0,
                           "Saturation", "How colorful the colors appear. \nDefault: \t0 \nRange: \t-255 to 255");
    addPropertyToComponent(Components::Object_Appearance, Properties::Object_Filter_Hue, Property_Type::FilterAngle, 0,
                           "Hue", "Rotate color values. \nDefault: \t0 \nRange: \t0 to 360");
    addPropertyToComponent(Components::Object_Appearance, Properties::Object_Filter_Grayscale, Property_Type::Bool, false,
                           "Grayscale", "Should this object be shown grayscale?");
    addPropertyToComponent(Components::Object_Appearance, Properties::Object_Filter_Negative, Property_Type::Bool, false,
                           "Negative", "Should this objects colors be inverted?");
}










