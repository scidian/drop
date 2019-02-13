//
//      Created by Stephens Nunnally on 12/7/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      DrStage Class Definitions
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
//##    Constructor, Destructor, addObject
//####################################################################################
DrStage::DrStage(DrProject *parent_project, DrWorld *parent_world, long new_stage_key, QString new_stage_name, bool is_start_stage)
{
    m_parent_project = parent_project;              // pointer to parent Project
    m_parent_world = parent_world;                  // pointer to parent World

    setKey(new_stage_key);                          // assign key passed in from key generator, this key matches key in parent World map container

    m_is_start_stage = is_start_stage;              // is this a start stage or not, can only be one start stage per World

    initializeStageSettings(new_stage_name);        // call to load in all the components / properties for this Stage object

    if (m_is_start_stage) {
        ///addObject(DrType::Camera, new DrAsset(), 0, 0);
        ///addObject(DrType::Character, new DrAsset(), 0, 0);
    }
}

DrStage::~DrStage()
{
    for (auto i: m_objects) { delete i.second; }
}

void DrStage::addObject(DrType new_type, long from_asset_key, double x, double y, long z)
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

// Returns a list of object keys contained in stage, sorted from high z value to low
QList<long> DrStage::objectKeysSortedByZOrder()
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
//##    Property loading - initializeStageSettings
//####################################################################################

void DrStage::initializeStageSettings(QString new_name)
{
    addComponent(Stage_Components::settings, "Settings", "Basic settings for current stage.", Component_Colors::White_Snow, true);
    getComponent(Stage_Components::settings)->setIcon(Component_Icons::Settings);

    addPropertyToComponent(Stage_Components::settings, Stage_Properties::name, Property_Type::String, new_name,
                           "Stage Name", "Name of the current stage.");
    addPropertyToComponent(Stage_Components::settings, Stage_Properties::start, Property_Type::Positive, 0,
                           "Start", "Start showing stage at this distance.");
    addPropertyToComponent(Stage_Components::settings, Stage_Properties::end, Property_Type::Positive, 1200,
                           "End", "Stop showing stage at this distance, -1 for always show.");
    addPropertyToComponent(Stage_Components::settings, Stage_Properties::size, Property_Type::Positive, 1200,
                           "Size", "Length of stage.");
    addPropertyToComponent(Stage_Components::settings, Stage_Properties::cooldown, Property_Type::Positive, 0,
                           "Cooldown", "Distance to wait after stage plays before it is possible to be shown again.");


    addComponent(Stage_Components::grid, "Grid", "Settings for the alignment grid within the editor. For an isometric grid, set 'Grid Rotation' "
                                                 "to 30 degrees, and set a grid size width twice the value of the grid size height (i.e. w: 50, h: 25).",
                                                 Component_Colors::Pink_Pearl, true);
    getComponent(Stage_Components::grid)->setIcon(Component_Icons::Transform);

    addPropertyToComponent(Stage_Components::grid, Stage_Properties::grid_style, Property_Type::List, 0,
                           "Grid Style", "Visual style of alignment grid.");
    addPropertyToComponent(Stage_Components::grid, Stage_Properties::grid_origin_point, Property_Type::PointF, QPointF(0, 0),
                           "Grid Origin Point", "Origin point in stage the grid begins at.");
    addPropertyToComponent(Stage_Components::grid, Stage_Properties::grid_size, Property_Type::SizeF, QPointF(50, 50),
                           "Grid Cell Size", "Width and height of the cells in the grid.");
    addPropertyToComponent(Stage_Components::grid, Stage_Properties::grid_rotation, Property_Type::Angle, 0,
                           "Grid Rotation", "Rotation of the grid lines.");

}


//####################################################################################
//##    Remove any references within the current Stage Objects to any GraphicsScene Items
//####################################################################################
void DrStage::removeGraphicsItemReferences()
{
    for (auto object_pair : getObjectMap()) {
        object_pair.second->setDrItem(nullptr);
    }
}














