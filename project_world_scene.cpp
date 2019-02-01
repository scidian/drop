//
//      Created by Stephens Nunnally on 12/7/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      DrScene Class Definitions
//
//

#include "project.h"
#include "project_asset.h"
#include "project_world.h"
#include "project_world_scene.h"
#include "project_world_scene_object.h"

#include "settings.h"
#include "settings_component.h"
#include "settings_component_property.h"

//####################################################################################
//##    Constructor, Destructor, addObject
//####################################################################################
DrScene::DrScene(DrProject *parent_project, DrWorld *parent_world, long new_scene_key, QString new_scene_name, bool is_start_scene)
{
    m_parent_project = parent_project;              // pointer to parent Project
    m_parent_world = parent_world;                  // pointer to parent World

    setKey(new_scene_key);                          // assign key passed in from key generator, this key matches key in parent World map container

    m_is_start_scene = is_start_scene;              // is this a start scene or not, can only be one start scene per World

    initializeSceneSettings(new_scene_name);        // call to load in all the components / properties for this Scene object

    if (m_is_start_scene) {
        ///addObject(DrType::Camera, new DrAsset(), 0, 0);
        ///addObject(DrType::Character, new DrAsset(), 0, 0);
    }
}

DrScene::~DrScene()
{
    for (auto i: m_objects) { delete i.second; }
}

void DrScene::addObject(DrType new_type, long from_asset_key, double x, double y, long z)
{
    QString new_name;
    switch (new_type) {
    case DrType::Camera:        "Camera " + QString::number(static_cast<long>(m_objects.size() + 1));       break;
    case DrType::Action:
    case DrType::Character:
    case DrType::Object:
        new_name = m_parent_project->getAsset(from_asset_key)->getComponentProperty(Asset_Components::settings, Asset_Properties::name)->getValue().toString();
        break;
    default:
        new_name = "Fix me";
    }

    long new_object_key = m_parent_project->getNextKey();
    m_objects[new_object_key] = new DrObject(m_parent_project, m_parent_world, this, new_object_key,
                                             new_name, new_type, from_asset_key, x, y, z);
}

// Returns a list of object keys contained in scene, sorted from high z value to low
QList<long> DrScene::objectKeysSortedByZOrder()
{
    std::vector<std::pair<long, long>> zorder_key_pair;

    for (auto object : m_objects) {
        long z_order = object.second->getComponentProperty(Object_Components::layering, Object_Properties::z_order)->getValue().toInt();
        zorder_key_pair.push_back(std::make_pair(z_order, object.first));
    }

    std::sort(zorder_key_pair.begin(), zorder_key_pair.end());

    QList<long> z_ordered_keys;

    for (auto one_pair : zorder_key_pair)
        z_ordered_keys.push_front(one_pair.second);

    return z_ordered_keys;
}


//####################################################################################
//##    Property loading - initializeSceneSettings
//####################################################################################

void DrScene::initializeSceneSettings(QString new_name)
{
    addComponent(Scene_Components::settings, "Settings", "Basic settings for current scene.", Component_Colors::White_Snow, true);
    getComponent(Object_Components::settings)->setIcon(Component_Icons::Settings);

    addPropertyToComponent(Scene_Components::settings, Scene_Properties::name, Property_Type::String, new_name,
                           "Scene Name", "Name of the current scene.");
    addPropertyToComponent(Scene_Components::settings, Scene_Properties::start, Property_Type::Positive, 0,
                           "Start", "Start showing scene at this distance.");
    addPropertyToComponent(Scene_Components::settings, Scene_Properties::end, Property_Type::Positive, 1200,
                           "End", "Stop showing scene at this distance, -1 for always show.");
    addPropertyToComponent(Scene_Components::settings, Scene_Properties::size, Property_Type::Positive, 1200,
                           "Size", "Length of scene.");
    addPropertyToComponent(Scene_Components::settings, Scene_Properties::cooldown, Property_Type::Positive, 0,
                           "Cooldown", "Distance to wait after scene plays before it is possible to be shown again.");

}




