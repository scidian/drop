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

#include "settings.h"
#include "settings_component.h"
#include "settings_component_property.h"

//####################################################################################
//##    Constructor, Destructor
//####################################################################################
DrObject::DrObject(DrProject *parent_project, DrWorld *parent_world, DrStage *parent_stage,
                   long new_object_key, QString new_object_name, DrType new_object_type,
                   long from_asset_key, double x, double y, long z)
{
    m_parent_project = parent_project;              // pointer to parent Project
    m_parent_world = parent_world;                  // pointer to parent World
    m_parent_stage = parent_stage;                  // pointer to parent Stage

    setKey(new_object_key);                         // assign key passed in from key generator, this key matches key in parent Stage map container

    m_object_type = new_object_type;                // assign object type
    m_asset_key = from_asset_key;                   // associated asset key

    DrSettings *asset_settings = m_parent_project->findSettingsFromKey(from_asset_key);
    DrAsset *asset = dynamic_cast<DrAsset*>(asset_settings);

    // Call to load in all the components / properties for this Stage object
    initializeObjectSettings(new_object_name, asset->getWidth(), asset->getHeight(), x, y, z);

    switch (new_object_type)
    {
    case DrType::Camera:       initializeCameraSettings();     break;
    case DrType::Character:    initializeCharacterSettings();  break;
    default: break;
    }

}

DrObject::~DrObject()
{

}



//####################################################################################
//##    Property loading - initializeObjectSettings
//##                       initializeCameraSettings
//##                       initializeCharacterSettings
//####################################################################################

void DrObject::initializeObjectSettings(QString new_name, double width, double height, double x, double y, long z)
{
    addComponent(Object_Components::settings, "Settings", "Basic settings for current object.", Component_Colors::White_Snow, true);
    getComponent(Object_Components::settings)->setIcon(Component_Icons::Settings);

    addPropertyToComponent(Object_Components::settings, Object_Properties::name, Property_Type::String, new_name,
                           "Object Name", "Name of the current object.", true);
    addPropertyToComponent(Object_Components::settings, Object_Properties::physics, Property_Type::Bool, false,
                           "Physics?", "Should this item be effected by physics?");
    addPropertyToComponent(Object_Components::settings, Object_Properties::collide, Property_Type::Bool, true,
                           "Collide?", "Should this item collide with other items?");
    addPropertyToComponent(Object_Components::settings, Object_Properties::damage, Property_Type::List, 0,
                           "Damage", "What should this item damage when it collides with something else.");


    addPropertyToComponent(Object_Components::settings, Object_Properties::test, Property_Type::List2, 0,
                           "Test List", "A test of a new widget");


    addComponent(Object_Components::transform, "Transform", "Sets the physical size and angle of the item in the stage.", Component_Colors::Green_SeaGrass, true);
    getComponent(Object_Components::transform)->setIcon(Component_Icons::Transform);

    addPropertyToComponent(Object_Components::transform, Object_Properties::position, Property_Type::PointF, QPointF(x, y),
                           "Position", "Location of item within the current stage.");
    addPropertyToComponent(Object_Components::transform, Object_Properties::rotation, Property_Type::Angle, 0,
                           "Rotation", "Angle of item within the stage.");
    addPropertyToComponent(Object_Components::transform, Object_Properties::size, Property_Type::SizeF, QPointF(width, height),
                           "Size", "Width and Height of object in pixels, affected by Scale property.");
    addPropertyToComponent(Object_Components::transform, Object_Properties::scale, Property_Type::PointF, QPointF(1, 1),
                           "Scale", "X and Y scale of item within the stage.");

    addComponent(Object_Components::layering, "Layering", "Controls the order items are drawn onto the screen. Lower numbers are "
                                                           "towards the back, higher towards the front.", Component_Colors::Blue_Yonder, true);
    getComponent(Object_Components::layering)->setIcon(Component_Icons::Layering);

    addPropertyToComponent(Object_Components::layering, Object_Properties::z_order, Property_Type::Int, QVariant::fromValue(z),
                           "Z Order", "Arrangement of item along the z axis in the stage");
    addPropertyToComponent(Object_Components::layering, Object_Properties::opacity, Property_Type::Percent, 100,
                           "Opacity", "How see transparent an item is, 0 (invisible) - 100 (solid)");

    addComponent(Object_Components::movement, "Movement", "Initial starting velocities of item in stage.", Component_Colors::Red_Faded, true);
    getComponent(Object_Components::movement)->setIcon(Component_Icons::Movement);

    addPropertyToComponent(Object_Components::movement, Object_Properties::velocity_x, Property_Type::Variable, QPointF(0, 0),
                           "Velocity X", "Initial horizontal movement speed of item +/- variable amount.");
    addPropertyToComponent(Object_Components::movement, Object_Properties::velocity_y, Property_Type::Variable, QPointF(0, 0),
                           "Velocity Y", "Initial vertical movement speed of item +/- variable amount.");
    addPropertyToComponent(Object_Components::movement, Object_Properties::angular_velocity, Property_Type::Variable, QPointF(0, 0),
                           "Angular Velocity", "Rotational movement speed of item +/- variable amount.");
}

void DrObject::initializeCameraSettings()
{
    addComponent(Object_Components::camera_settings, "Camera Settings", "Settings for this camera", Component_Colors::Beige_Apricot, true);
    getComponent(Object_Components::camera_settings)->setIcon(Component_Icons::Camera);

    addPropertyToComponent(Object_Components::camera_settings, Object_Properties::camera_zoom, Property_Type::Float, 10,
                           "Zoom Level", "Sets distance away from stage (0 to 1000)");

    getComponentProperty(Object_Components::settings, Object_Properties::name)->setHidden(false);
}

void DrObject::initializeCharacterSettings()
{
    addComponent(Object_Components::character_settings, "Character Settings", "Settings for this character", Component_Colors::Silver_Sonic, true);
    getComponent(Object_Components::character_settings)->setIcon(Component_Icons::Character);

    addPropertyToComponent(Object_Components::character_settings, Object_Properties::character_jump_x, Property_Type::Float, 0,
                           "Jump Force X", "Force of jump button in x direction");
    addPropertyToComponent(Object_Components::character_settings, Object_Properties::character_jump_y, Property_Type::Float, 0,
                           "Jump Force Y", "Force of jump button in y direction");
}







