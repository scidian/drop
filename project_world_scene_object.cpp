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


void DrObject::initializeObjectSettings(std::string new_name)
{
    addComponent(Object_Components::settings, "Settings", "Basic settings for current object", Component_Colors::White_Snow, true);
    getComponent(Object_Components::settings)->setIcon(Component_Icons::Settings);

    addPropertyToComponent(Object_Components::settings, Object_Properties::name, Property_Type::String, QString::fromStdString(new_name),
                           "Object Name", "Name of the current object");
    addPropertyToComponent(Object_Components::settings, Object_Properties::x, Property_Type::Float, 0,
                           "X", "X Location");
    addPropertyToComponent(Object_Components::settings, Object_Properties::y, Property_Type::Float, 0,
                           "Y", "Y Location");

}

void DrObject::initializeCameraSettings()
{
    addComponent(Object_Components::camera_settings, "Camera Settings", "Settings for this camera", Component_Colors::Beige_Apricot, true);
    getComponent(Object_Components::camera_settings)->setIcon(Component_Icons::Camera);

    addPropertyToComponent(Object_Components::camera_settings, Object_Properties::camera_zoom, Property_Type::Float, 10,
                           "Zoom Level", "Sets distance away from scene (0 to 1000)");

}

void DrObject::initializeCharacterSettings()
{
    addComponent(Object_Components::character_settings, "Character Settings", "Settings for this character", Component_Colors::Blue_Yonder, true);
    getComponent(Object_Components::character_settings)->setIcon(Component_Icons::Character);

    addPropertyToComponent(Object_Components::character_settings, Object_Properties::character_jump_x, Property_Type::Float, 0,
                           "Jump Force X", "Force of jump button in x direction");
    addPropertyToComponent(Object_Components::character_settings, Object_Properties::character_jump_y, Property_Type::Float, 0,
                           "Jump Force Y", "Force of jump button in y direction");

}







