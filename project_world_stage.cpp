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
        // If start stage, make name hidden to stop user from changing it
        DrProperty *my_name = getComponentProperty(Components::Stage_Settings, Properties::Stage_Name);
        my_name->setHidden(true);

        ///addObject(DrType::Camera, new DrAsset(), 0, 0);
        ///addObject(DrType::Character, new DrAsset(), 0, 0);
    }
}

DrStage::~DrStage()
{
    for (auto i: m_objects) { delete i.second; }
}

void DrStage::addObject(DrObjectType new_type, long from_asset_key, double x, double y, long z)
{
    QString new_name;
    switch (new_type) {
    case DrObjectType::Camera:        "Camera " + QString::number(static_cast<long>(m_objects.size() + 1));       break;
    case DrObjectType::Action:
    case DrObjectType::Character:
    case DrObjectType::Object:
        new_name = m_parent_project->getAsset(from_asset_key)->getComponentProperty(Components::Asset_Settings, Properties::Asset_Name)->getValue().toString();
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
        long z_order = object.second->getComponentProperty(Components::Object_Layering, Properties::Object_Z_Order)->getValue().toInt();
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
    addComponent(Components::Stage_Settings, "Settings", "Basic settings for current stage.", Component_Colors::White_Snow, true);
    getComponent(Components::Stage_Settings)->setIcon(Component_Icons::Settings);

    addPropertyToComponent(Components::Stage_Settings, Properties::Stage_Name, Property_Type::String, new_name,
                           "Stage Name", "Name of the current stage.");
    addPropertyToComponent(Components::Stage_Settings, Properties::Stage_Start, Property_Type::Positive, 0,
                           "Start", "Start showing stage at this distance.");
    addPropertyToComponent(Components::Stage_Settings, Properties::Stage_End, Property_Type::Positive, 1200,
                           "End", "Stop showing stage at this distance, -1 for always show.");
    addPropertyToComponent(Components::Stage_Settings, Properties::Stage_Size, Property_Type::Positive, 1200,
                           "Size", "Length of stage.");
    addPropertyToComponent(Components::Stage_Settings, Properties::Stage_Cooldown, Property_Type::Positive, 0,
                           "Cooldown", "Distance to wait after stage plays before it is possible to be shown again.");


    addComponent(Components::Stage_Grid, "Grid", "Settings for the alignment grid within the editor. For an isometric grid, set 'Grid Rotation' "
                                                 "to 30 degrees, and set a grid size width twice the value of the grid size height (i.e. w: 50, h: 25).",
                                                 Component_Colors::Pink_Pearl, true);
    getComponent(Components::Stage_Grid)->setIcon(Component_Icons::Transform);

    addPropertyToComponent(Components::Stage_Grid, Properties::Stage_Grid_Style, Property_Type::List, 0,
                           "Grid Style", "Visual style of alignment grid.");
    addPropertyToComponent(Components::Stage_Grid, Properties::Stage_Grid_Origin_Point, Property_Type::PointF, QPointF(0, 0),
                           "Grid Origin Point", "Origin point in stage the grid begins at.");
    addPropertyToComponent(Components::Stage_Grid, Properties::Stage_Grid_Size, Property_Type::GridF, QPointF(50, 50),
                           "Grid Cell Size", "Width and height of the cells in the grid.");
    addPropertyToComponent(Components::Stage_Grid, Properties::Stage_Grid_Scale, Property_Type::Scale, QPointF(1, 1),
                           "Grid Cell Scale", "X and Y stretch factor after grid has been rotated.");
    addPropertyToComponent(Components::Stage_Grid, Properties::Stage_Grid_Rotation, Property_Type::Angle, 0,
                           "Grid Rotation", "Rotation of the grid lines.");
    addPropertyToComponent(Components::Stage_Grid, Properties::Stage_Grid_Should_Snap, Property_Type::Bool, true,
                           "Snap to Grid?", "Snaps objects to grid lines when moving objects around with the mouse.");
    addPropertyToComponent(Components::Stage_Grid, Properties::Stage_Grid_Show_On_Top, Property_Type::Bool, false,
                           "Grid on Top?", "Draws grid lines on top of objects in editor.");
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














