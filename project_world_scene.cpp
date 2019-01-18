//
//      Created by Stephens Nunnally on 12/7/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      DrScene Class Definitions
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
//##    Constructor, Destructor, addObject
//####################################################################################
DrScene::DrScene(DrProject *parent_project, DrWorld *parent_world, long new_scene_key, std::string new_scene_name, bool is_start_scene)
{
    m_parent_project = parent_project;              // pointer to parent Project
    m_parent_world = parent_world;                  // pointer to parent World

    setKey(new_scene_key);                          // assign key passed in from key generator, this key matches key in parent World map container

    m_is_start_scene = is_start_scene;              // is this a start scene or not, can only be one start scene per World

    initializeSceneSettings(new_scene_name);        // call to load in all the components / properties for this Scene object

    if (m_is_start_scene) {
        addObject(DrTypes::Camera);
        addObject(DrTypes::Character);
    }
}

DrScene::~DrScene()
{
    for (auto i: m_objects) { delete i.second; }
}

void DrScene::addObject(DrTypes new_type)
{
    std::string new_name;
    new_name = std::to_string(static_cast<long>(m_objects.size() + 1));

    long new_object_key = m_parent_project->getNextKey();
    m_objects[new_object_key] = new DrObject(m_parent_project, m_parent_world, this, new_object_key, new_name, new_type);
}


//####################################################################################
//##    Property loading - initializeSceneSettings
//####################################################################################

void DrScene::initializeSceneSettings(std::string new_name)
{
    addComponent(Scene_Components::settings, "Settings", "Basic settings for current scene", Component_Colors::White_Snow, true);
    getComponent(Object_Components::settings)->setIcon(Component_Icons::Settings);

    addPropertyToComponent(Scene_Components::settings, Scene_Properties::name, Property_Type::String, QString::fromStdString(new_name),
                           "Scene Name", "Name of the current scene");
    addPropertyToComponent(Scene_Components::settings, Scene_Properties::start, Property_Type::Int, 0,
                           "Start", "Start showing scene at this distance");
    addPropertyToComponent(Scene_Components::settings, Scene_Properties::end, Property_Type::Int, 1200,
                           "End", "Stop showing scene at this distance, -1 for always show");
    addPropertyToComponent(Scene_Components::settings, Scene_Properties::size, Property_Type::Int, 1200,
                           "Size", "Length of scene");
    addPropertyToComponent(Scene_Components::settings, Scene_Properties::cooldown, Property_Type::Int, 0,
                           "Cooldown", "Distance to wait after scene plays before it is possible to be shown again");

}




