//
//      Created by Stephens Nunnally on 12/7/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      DrObject Class Definitions
//
//

#include "project.h"
#include "project_world.h"
#include "project_world_scene.h"
#include "project_world_scene_object.h"

#include "settings.h"
#include "settings_component.h"
#include "settings_component_property.h"

//####################################################################################
//##    Constructor, Destructor
//####################################################################################
DrObject::DrObject(DrProject *parent_project, DrWorld *parent_world, DrScene *parent_scene,
                   long new_object_key, std::string new_object_name, DrTypes new_object_type)
{
    m_parent_project = parent_project;              // pointer to parent Project
    m_parent_world = parent_world;                  // pointer to parent World
    m_parent_scene = parent_scene;                  // pointer to parent Scene

    setKey(new_object_key);                         // assign key passed in from key generator, this key matches key in parent Scene map container

    m_object_type = new_object_type;                // assign object type

    initializeObjectSettings(new_object_name);      // call to load in all the components / properties for this Scene object

    switch (new_object_type)
    {
    case DrTypes::Camera:       initializeCameraSettings();     break;
    case DrTypes::Character:    initializeCharacterSettings();  break;
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

//    // movement
//    velocity_x,             //variable
//    velocity_y,             //variable
//    angular_velocity,       //variable


void DrObject::initializeObjectSettings(std::string new_name)
{
    addComponent(Object_Components::settings, "Settings", "Basic settings for current object.", Component_Colors::White_Snow, true);
    getComponent(Object_Components::settings)->setIcon(Component_Icons::Settings);

    addPropertyToComponent(Object_Components::settings, Object_Properties::name, Property_Type::String, QString::fromStdString(new_name),
                           "Object Name", "Name of the current object.");
    addPropertyToComponent(Object_Components::settings, Object_Properties::physics, Property_Type::Bool, false,
                           "Physics?", "Should this item be effected by physics?");
    addPropertyToComponent(Object_Components::settings, Object_Properties::collide, Property_Type::Bool, true,
                           "Collide?", "Should this item collide with other items?");
    addPropertyToComponent(Object_Components::settings, Object_Properties::damage, Property_Type::List, 0,
                           "Damage", "What should this item damage when it collides with something else.");

    addComponent(Object_Components::transform, "Transform", "Sets the physical size and angle of the item in the scene.", Component_Colors::Green_SeaGrass, true);
    getComponent(Object_Components::transform)->setIcon(Component_Icons::Transform);

    addPropertyToComponent(Object_Components::transform, Object_Properties::position, Property_Type::PointF, QPointF(0, 0),
                           "Position", "Location of item within the current scene.");
    addPropertyToComponent(Object_Components::transform, Object_Properties::rotation, Property_Type::Double, 0,
                           "Rotation", "Angle of item within the scene.");
    addPropertyToComponent(Object_Components::transform, Object_Properties::scale, Property_Type::PointF, QPointF(1, 1),
                           "Scale", "X and Y scale of item within the scene.");
    addPropertyToComponent(Object_Components::transform, Object_Properties::opacity, Property_Type::Percent, 100,
                           "Opacity", "How see through is the item, 0 (invisible) - 100 (solid)");

    addComponent(Object_Components::movement, "Movement", "Initial starting velocities of item in scene.", Component_Colors::Red_Faded, true);
    getComponent(Object_Components::movement)->setIcon(Component_Icons::Movement);

    addPropertyToComponent(Object_Components::movement, Object_Properties::velocity_x, Property_Type::Variable, QPointF(0, 0),
                           "Velocity X", "Initial horizontal movement speed of item +/- variable amount.");
    addPropertyToComponent(Object_Components::movement, Object_Properties::velocity_y, Property_Type::Variable, QPointF(0, 0),
                           "Velocity Y", "Initial vertical movement speed of item +/- variable amount.");
    addPropertyToComponent(Object_Components::movement, Object_Properties::angular_velocity, Property_Type::Variable, QPointF(0, 0),
                           "Angular Velocity", "Rotational movement speed of item.");
}

void DrObject::initializeCameraSettings()
{
    addComponent(Object_Components::camera_settings, "Camera Settings", "Settings for this camera", Component_Colors::Beige_Apricot, true);
    getComponent(Object_Components::camera_settings)->setIcon(Component_Icons::Camera);

    addPropertyToComponent(Object_Components::camera_settings, Object_Properties::camera_zoom, Property_Type::Double, 10,
                           "Zoom Level", "Sets distance away from scene (0 to 1000)");

}

void DrObject::initializeCharacterSettings()
{
    addComponent(Object_Components::character_settings, "Character Settings", "Settings for this character", Component_Colors::Blue_Yonder, true);
    getComponent(Object_Components::character_settings)->setIcon(Component_Icons::Character);

    addPropertyToComponent(Object_Components::character_settings, Object_Properties::character_jump_x, Property_Type::Double, 0,
                           "Jump Force X", "Force of jump button in x direction");
    addPropertyToComponent(Object_Components::character_settings, Object_Properties::character_jump_y, Property_Type::Double, 0,
                           "Jump Force Y", "Force of jump button in y direction");

}







