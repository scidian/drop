//
//      Created by Stephens Nunnally on 12/7/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      DrStage Class Definitions
//
//
#include "colors/colors.h"
#include "debug.h"
#include "helper.h"
#include "helper_qt.h"
#include "project.h"
#include "project_asset.h"
#include "project_world.h"
#include "project_world_stage.h"
#include "project_world_stage_thing.h"
#include "settings/settings.h"
#include "settings/settings_component.h"
#include "settings/settings_component_property.h"

//####################################################################################
//##    Constructor, Destructor
//####################################################################################
DrStage::DrStage(DrProject *parent_project, DrWorld *parent_world, long new_stage_key, QString new_stage_name, bool is_start_stage) {
    m_parent_project = parent_project;              // pointer to parent Project
    m_parent_world = parent_world;                  // pointer to parent World

    this->setKey(new_stage_key);                    // assign key passed in from key generator, this key matches key in parent World map container

    m_is_start_stage = is_start_stage;              // is this a start stage or not, can only be one start stage per World

    initializeStageSettings(new_stage_name);        // call to load in all the components / properties for this Stage thing

    if (m_is_start_stage) {
        // If start stage, make name hidden to stop user from changing it
        DrProperty *my_name = getComponentProperty(Components::Entity_Settings, Properties::Entity_Name);
        my_name->setEditable(false);
    }
}

DrStage::~DrStage() {
    for (auto i: m_things) { delete i.second; }
}

//####################################################################################
//##
//##    Adds a new Thing to the Stage
//##
//##    ***** #NOTE: Y Axis is in Game Engine coordinates (i.e. positive is up, negative is down)
//##
//####################################################################################
DrThing* DrStage::addThing(DrThingType new_type, long from_asset_key, double x, double y, double z, bool should_collide, long key) {
    DrAsset *asset = m_parent_project->getAsset(from_asset_key);

    // Figure out name for Thing
    QString new_name;
    switch (new_type) {
        // Thing Types
        case DrThingType::Character:
        case DrThingType::Object:
        case DrThingType::Text:

        // Effect Types
        case DrThingType::Fisheye:
        case DrThingType::Fire:
        case DrThingType::Light:
        case DrThingType::Mirror:
        case DrThingType::Swirl:
        case DrThingType::Water:
            new_name = asset->getComponentProperty(Components::Entity_Settings, Properties::Entity_Name)->getValue().toString();
            break;

        ///case DrThingType::Camera:
        ///    "Camera " + QString::number(static_cast<long>(m_things.size() + 1));
        ///    break;

        default:
            Dr::ShowMessageBox("Error in DrStage::addThing, DrThingType not handled! Type: " + Dr::StringFromThingType(new_type));
    }

    long new_thing_key = (key == c_no_key) ? m_parent_project->getNextKey() : key;
    m_things[new_thing_key] = new DrThing(m_parent_project, m_parent_world, this, new_thing_key,
                                          new_name, new_type, from_asset_key, x, y, z, should_collide);
    return m_things[new_thing_key];
}

// Copies all component / property settings from one Thing to another Thing of the same type
void DrStage::copyThingSettings(DrThing *from_thing, DrThing *to_thing) {
    if (from_thing->getThingType() != to_thing->getThingType()) return;

    for (auto component_pair : from_thing->getComponentMap()) {
        for (auto property_pair : component_pair.second->getPropertyMap()) {
            DrProperty *from_property = property_pair.second;
            DrProperty *to_property =   to_thing->getComponentProperty(component_pair.first, property_pair.first);

            // Don't copy key
            if (to_property->getPropertyKey() == static_cast<int>(Properties::Entity_Key)) continue;

            // Copy all other properties
            to_property->setValue(      from_property->getValue());
            to_property->setEditable(   from_property->isEditable());
            to_property->setHidden(     from_property->isHidden());
            to_property->setDisplayName(from_property->getDisplayName());
            to_property->setDescription(from_property->getDescription());
        }
    }
}

// Removes an object from the project
void DrStage::deleteThing(DrThing *thing) {
    m_things.erase(thing->getKey());
    delete thing;
}


// Returns a list of Thing keys contained in stage, sorted from high z value to low
QList<long> DrStage::thingKeysSortedByZOrder() {
    std::vector<std::pair<long, long>> zorder_key_pair;

    for (auto thing : m_things) {
        long z_order = thing.second->getComponentProperty(Components::Thing_Layering, Properties::Thing_Z_Order)->getValue().toInt();
        zorder_key_pair.push_back(std::make_pair(z_order, thing.first));
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

void DrStage::initializeStageSettings(QString new_name) {
    DrProperty *property_name = getComponentProperty(Components::Entity_Settings, Properties::Entity_Name);
    property_name->setDisplayName("Stage Name");
    property_name->setDescription("Name of the current Stage.");
    property_name->setValue(new_name);

    addComponent(Components::Stage_Settings, "Stage Settings", "Settings for current stage.", Component_Colors::White_Snow, true);
    getComponent(Components::Stage_Settings)->setIcon(Component_Icons::Settings);
    addPropertyToComponent(Components::Stage_Settings, Properties::Stage_Start, Property_Type::Positive, 0,
                           "Start", "Start showing stage at this distance.");
    addPropertyToComponent(Components::Stage_Settings, Properties::Stage_End, Property_Type::Positive, 1200,
                           "End", "Stop showing stage at this distance, -1 for always show.");
    addPropertyToComponent(Components::Stage_Settings, Properties::Stage_Size, Property_Type::Positive, 1200,
                           "Size", "Length of stage.");
    addPropertyToComponent(Components::Stage_Settings, Properties::Stage_Cooldown, Property_Type::Positive, 0,
                           "Cooldown", "Distance to wait after stage plays before it is possible to be shown again.");


    addComponent(Components::Stage_Grid, "Grid", "Settings for the alignment grid within the editor. For an Isometric Grid: set \"Grid Rotation\" "
                                                 "to 45 degrees, and set \"Grid Scale\" X value twice the size \"Grid Scale\" Y value (i.e. X: 2, Y: 1).",
                                                 Component_Colors::Pink_Pearl, true);
    getComponent(Components::Stage_Grid)->setIcon(Component_Icons::Transform);

    addPropertyToComponent(Components::Stage_Grid, Properties::Stage_Grid_Style, Property_Type::List, 0,
                           "Grid Style", "Visual style of alignment grid.");
    addPropertyToComponent(Components::Stage_Grid, Properties::Stage_Grid_Origin_Point, Property_Type::PointF, QPointF(0, 0),
                           "Grid Origin Point", "Origin point in stage the grid begins at. Allows for small adjustments of enitre grid.");
    addPropertyToComponent(Components::Stage_Grid, Properties::Stage_Grid_Size, Property_Type::GridF, QPointF(50, 50),
                           "Grid Cell Size", "Width and height of the cells in the grid.");
    addPropertyToComponent(Components::Stage_Grid, Properties::Stage_Grid_Scale, Property_Type::GridScaleF, QPointF(1, 1),
                           "Grid Scale", "X and Y stretch factor after grid has been rotated. For Isometric Grids, set X value twice that of Y value.");
    addPropertyToComponent(Components::Stage_Grid, Properties::Stage_Grid_Rotation, Property_Type::Angle, 0,
                           "Grid Rotation", "Rotation of the grid lines. For Isometric Grids, set \"Grid Rotation\" to 45 degrees.");
    addPropertyToComponent(Components::Stage_Grid, Properties::Stage_Grid_Color, Property_Type::Color, Dr::GetColor(Window_Colors::Background_Light).rgba(),
                           "Grid Color", "Color of grid lines.");
}


//####################################################################################
//##    Remove any references within the current Stage Things to any GraphicsScene Items
//####################################################################################
void DrStage::removeGraphicsItemReferences() {
    for (auto thing_pair : getThingMap()) {
        thing_pair.second->setDrItem(nullptr);
    }
}














