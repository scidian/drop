//
//      Created by Stephens Nunnally on 12/7/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      DrStage Class Definitions
//
//
#include "colors/colors.h"
#include "debug.h"
#include "forms/form_main.h"
#include "globals.h"
#include "helper.h"
#include "helper_qt.h"
#include "project.h"
#include "project_asset.h"
#include "project_world.h"
#include "project_stage.h"
#include "project_thing.h"
#include "settings/settings.h"
#include "settings/settings_component.h"
#include "settings/settings_component_property.h"


//####################################################################################
//##    Constructor, Destructor
//####################################################################################
DrStage::DrStage(DrProject *parent_project, DrWorld *parent_world,
                 long new_stage_key, QString new_stage_name, bool is_start_stage) : DrSettings(parent_project) {
    m_parent_world = parent_world;                  // pointer to parent World

    this->setKey(new_stage_key);                    // assign key passed in from key generator, this key matches key in parent World map container

    m_is_start_stage = is_start_stage;              // is this a start stage or not, can only be one start stage per World

    initializeStageSettings(new_stage_name);        // call to load in all the components / properties for this Stage thing

    // If start stage, make name hidden to stop user from changing it
    if (m_is_start_stage) {
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
    DrAsset *asset = getParentProject()->getAsset(from_asset_key);

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
            Dr::ShowMessageBox("Error in DrStage::addThing, DrThingType not handled! Type: " + Dr::StringFromThingType(new_type),
                               QMessageBox::Icon::Critical, "Error!", Dr::GetActiveFormMain());
    }

    long new_thing_key = (key == c_no_key) ? getParentProject()->getNextKey() : key;
    m_things[new_thing_key] = new DrThing(getParentProject(), m_parent_world, this, new_thing_key,
                                          new_name, new_type, from_asset_key, x, y, z, should_collide);
    return m_things[new_thing_key];
}

// Removes an object from the project
void DrStage::deleteThing(DrThing *thing) {
    m_things.erase(thing->getKey());
    delete thing;
}


//####################################################################################
//##    Returns a list of Thing keys contained in stage, sorted from high z value to low
//####################################################################################
QList<long> DrStage::thingKeysSortedByZOrder(Qt::SortOrder sort_order) {
    QList<long> z_ordered_keys;
    for (auto thing : thingsSortedByZOrder(sort_order))
        z_ordered_keys.push_back(thing->getKey());
    return z_ordered_keys;
}

//####################################################################################
//##    Returns a list of Things contained in stage, sorted from high z value to low
//####################################################################################
QList<DrThing*> DrStage::thingsSortedByZOrder(Qt::SortOrder sort_order, bool all_things, QList<DrThing*> just_these_things) {
    // Make a Vector of pairs for sorting
    std::vector<std::pair<double, DrThing*>> zorder_key_pair;
    if (all_things) {
        for (auto &thing_pair : m_things)
            zorder_key_pair.push_back(std::make_pair(thing_pair.second->getZOrderWithSub(), thing_pair.second));
    } else {
        for (auto &thing : just_these_things)
            zorder_key_pair.push_back(std::make_pair(thing->getZOrderWithSub(), thing));
    }

    // Sort!
    std::sort(zorder_key_pair.begin(), zorder_key_pair.end());

    // Copy sorted Map into List
    QList<DrThing*> z_ordered_things;
    for (auto one_pair : zorder_key_pair)
        z_ordered_things.push_front(one_pair.second);

    // Reverse if Ascending
    if (sort_order == Qt::SortOrder::AscendingOrder)
        std::reverse(z_ordered_things.begin(), z_ordered_things.end());

    return z_ordered_things;
}


//####################################################################################
//##    Remove any references within the current Stage Things to any GraphicsScene Items
//####################################################################################
void DrStage::removeGraphicsItemReferences() {
    for (auto thing_pair : getThingMap()) {
        thing_pair.second->setDrItem(nullptr);
    }
}














