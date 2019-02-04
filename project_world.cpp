//
//      Created by Stephens Nunnally on 12/7/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      DrWorld Class Definitions
//
//

#include "project.h"
#include "project_world.h"
#include "project_world_stage.h"
#include "project_world_stage_object.h"

#include "settings.h"
#include "settings_component.h"
#include "settings_component_property.h"

//####################################################################################
//##    Constructor, Destructor - addStage
//####################################################################################
DrWorld::DrWorld(DrProject *parent_project, long new_world_key, QString new_world_name)
{
    m_parent_project = parent_project;
    setKey(new_world_key);

    initializeWorldSettings(new_world_name);

    addStage();
}

DrWorld::~DrWorld()
{
    for (auto i: m_stages) { delete i.second; }
}

// Adds a Stage to the map container, assins name based on current stage count
void DrWorld::addStage(QString new_stage_name)
{
    if (new_stage_name == "") {
        new_stage_name = QString::number(static_cast<long>(m_stages.size() + 1));
    }

    long new_stage_key = m_parent_project->getNextKey();
    bool need_start_stage = false;
    if (m_stages.size() < 1) {
        need_start_stage = true;
        new_stage_name = "Start Stage";
    }

    m_stages[new_stage_key] = new DrStage(m_parent_project, this, new_stage_key, new_stage_name, need_start_stage);
}


//####################################################################################
//##    Getters - getFirstWorldKey, getWorld (from key), getWorldWithName (from name)
//####################################################################################

// Returns the map Key of the first World in the map container
long DrWorld::getFirstStageKey()
{
    return m_stages.begin()->first;
}

// Returns a pointer to the World with the associated Key from the map container
DrStage* DrWorld::getStage(long from_stage_key)
{
    return m_stages[from_stage_key];
}

// Returns a pointer to the World with the mathcing name
DrStage* DrWorld::getStageWithName(QString stage_name)
{
    QString compare_name;
    for (auto i: m_stages) {
        compare_name = i.second->getStageName();
        if (compare_name == stage_name) { return i.second; }
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
    addPropertyToComponent(World_Components::settings, World_Properties::game_direction, Property_Type::Float, 0.0,
                           "Game Direction", "Default direction, in degrees, to load new stages, 0 - right, 90 - up, 180 - left, 270 - down, etc.");
    addPropertyToComponent(World_Components::settings, World_Properties::score_multiplier, Property_Type::Float, 1.0,
                           "Score Multiplier", "Value used as multiplier to adjust speed at which distance scoring is calculated.");

    addComponent(World_Components::physics, "Physics", "Starting physics settings for current world.", Component_Colors::Orange_Medium, true);
    getComponent(World_Components::physics)->setIcon(Component_Icons::Physics);
    addPropertyToComponent(World_Components::physics, World_Properties::use_physics, Property_Type::Bool, true,
                           "Use Physics?", "Whether or not physics is turned on in current world.");
    addPropertyToComponent(World_Components::physics, World_Properties::gravity, Property_Type::PointF, QPointF(0.0, 10.0),
                           "Gravity", "Amount of gravity in x and y directions, can be negative.");
    addPropertyToComponent(World_Components::physics, World_Properties::time_warp, Property_Type::Float, 60.0,
                           "Time Warp", "Update calculations per second.");
    addPropertyToComponent(World_Components::physics, World_Properties::friction, Property_Type::Float, 10.0,
                           "Friction", "Global friction setting.");
    addPropertyToComponent(World_Components::physics, World_Properties::drag, Property_Type::Float, 5.0,
                           "Velocity Drag", "Velocity drag.");
    addPropertyToComponent(World_Components::physics, World_Properties::bounce, Property_Type::Float, 5.0,
                           "Bounce", "Global bounce setting.");

}








