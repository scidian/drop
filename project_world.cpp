//
//      Created by Stephens Nunnally on 12/7/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      DrWorld Class Definitions
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
//##    Constructor, Destructor - addScene
//####################################################################################
DrWorld::DrWorld(DrProject *parent_project, long new_world_key, QString new_world_name)
{
    m_parent_project = parent_project;
    setKey(new_world_key);

    initializeWorldSettings(new_world_name);

    addScene();
}

DrWorld::~DrWorld()
{
    for (auto i: m_scenes) { delete i.second; }
}

// Adds a Scene to the map container, assins name based on current scene count
void DrWorld::addScene(QString new_scene_name)
{
    if (new_scene_name == "") {
        new_scene_name = QString::number(static_cast<long>(m_scenes.size() + 1));
    }

    long new_scene_key = m_parent_project->getNextKey();
    bool need_start_scene = false;
    if (m_scenes.size() < 1) {
        need_start_scene = true;
        new_scene_name = "Start Scene";
    }

    m_scenes[new_scene_key] = new DrScene(m_parent_project, this, new_scene_key, new_scene_name, need_start_scene);
}


//####################################################################################
//##    Getters - getFirstWorldKey, getWorld (from key), getWorldWithName (from name)
//####################################################################################

// Returns the map Key of the first World in the map container
long DrWorld::getFirstSceneKey()
{
    return m_scenes.begin()->first;
}

// Returns a pointer to the World with the associated Key from the map container
DrScene* DrWorld::getScene(long from_scene_key)
{
    return m_scenes[from_scene_key];
}

// Returns a pointer to the World with the mathcing name
DrScene* DrWorld::getSceneWithName(QString scene_name)
{
    QString compare_name;
    for (auto i: m_scenes) {
        compare_name = i.second->getSceneName();
        if (compare_name == scene_name) { return i.second; }
    }
    return nullptr;
}


//####################################################################################
//##    Property loading - initializeWorldSettings
//####################################################################################

void DrWorld::initializeWorldSettings(QString new_name)
{
    addComponent(World_Components::settings, "Settings", "Basic settings for current world.", Component_Colors::White_Snow, true);
    getComponent(Object_Components::settings)->setIcon(Component_Icons::Settings);
    addPropertyToComponent(World_Components::settings, World_Properties::name, Property_Type::String, new_name,
                           "World Name", "Name of the current world.");
    addPropertyToComponent(World_Components::settings, World_Properties::game_direction, Property_Type::Double, 0.0,
                           "Game Direction", "Default direction, in degrees, to load new scenes, 0 - right, 90 - up, 180 - left, 270 - down, etc.");
    addPropertyToComponent(World_Components::settings, World_Properties::score_multiplier, Property_Type::Double, 1.0,
                           "Score Multiplier", "Value used as multiplier to adjust speed at which distance scoring is calculated.");

    addComponent(World_Components::physics, "Physics", "Starting physics settings for current world.", Component_Colors::Orange_Medium, true);
    getComponent(World_Components::physics)->setIcon(Component_Icons::Physics);
    addPropertyToComponent(World_Components::physics, World_Properties::use_physics, Property_Type::Bool, true,
                           "Use Physics?", "Whether or not physics is turned on in current world.");
    addPropertyToComponent(World_Components::physics, World_Properties::gravity, Property_Type::PointF, QPointF(0.0, 10.0),
                           "Gravity", "Amount of gravity in x and y directions, can be negative.");
    addPropertyToComponent(World_Components::physics, World_Properties::time_warp, Property_Type::Double, 60.0,
                           "Time Warp", "Update calculations per second.");
    addPropertyToComponent(World_Components::physics, World_Properties::friction, Property_Type::Double, 10.0,
                           "Friction", "Global friction setting.");
    addPropertyToComponent(World_Components::physics, World_Properties::drag, Property_Type::Double, 5.0,
                           "Velocity Drag", "Velocity drag.");
    addPropertyToComponent(World_Components::physics, World_Properties::bounce, Property_Type::Double, 5.0,
                           "Bounce", "Global bounce setting.");

}








