//
//      Created by Stephens Nunnally on 12/7/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      DrStage Class Definitions
//
//
#include "core/dr_debug.h"
#include "project/constants_component_info.h"
#include "project/dr_project.h"
#include "project/entities/dr_stage.h"
#include "project/entities/dr_thing.h"
#include "project/entities/dr_world.h"
#include "project/entities_physics_2d/dr_asset.h"
#include "project/settings/settings.h"
#include "project/settings/settings_component.h"
#include "project/settings/settings_component_property.h"


//####################################################################################
//##    Constructor / Destructor
//####################################################################################
DrStage::DrStage(DrProject *parent_project, DrWorld *parent_world, long new_stage_key, std::string new_stage_name, bool is_start_stage)
    : DrSettings(parent_project) {

    this->setViewCenterPoint(DrPointF(0, 0));
    this->setParentWorld(parent_world);                 // Pointer to parent World
    this->setKey(new_stage_key);                        // assign key passed in from key generator, this key matches key in parent World std::map container (m_worlds)
    this->setIsStartStage(is_start_stage);              // Is this a "Start Stage" or not, can only be one "Start Stage" per World
    initializeStageSettings(new_stage_name);            // Call to load in all the components / properties for this Stage entity

    // If "Start Stage", make name hidden to stop user from changing it
    if (isStartStage()) {
        DrProperty *my_name = getComponentProperty(Comps::Entity_Settings, Props::Entity_Name);
        my_name->setEditable(false);
    }
}

DrStage::~DrStage() {
    for (auto &thing_pair: m_things) { delete thing_pair.second; }
}


//####################################################################################
//##
//##    Adds a new Thing to the Stage
//##
//##    ***** #NOTE: Y Axis is in Game Engine coordinates (i.e. positive is up, negative is down)
//##
//####################################################################################
DrThing* DrStage::addThing(DrThingType new_type, long from_asset_key, double x, double y, double z, bool should_collide, long key) {
    DrSettings *asset = getParentProject()->findSettingsFromKey(from_asset_key);
    if (asset == nullptr) {
        Dr::PrintDebug("Error in DrStage::addThing, Could not find underlying Entity to load from! \n "
                       "New Type: " + Dr::StringFromThingType(new_type) + " \n "
                       "Asset Key: " + std::to_string(from_asset_key) + " - End Error.....");
    }

    // Figure out name for Thing
    std::string new_name = asset->getComponentProperty(Comps::Entity_Settings, Props::Entity_Name)->getValue().toString();

    long new_thing_key = (key == c_no_key) ? getParentProject()->getNextKey() : key;
    m_things[new_thing_key] = new DrThing(getParentProject(), m_parent_world, this, new_thing_key,
                                          new_name, new_type, asset, x, y, z, should_collide);
    return m_things[new_thing_key];
}

// Removes a Thing from the project
void DrStage::deleteThing(DrThing *&thing) {
    thing->deleteAnimations();
    m_things.erase(thing->getKey());
    delete thing;
}


//####################################################################################
//##    Find Thing in Stage
//####################################################################################
DrThing* DrStage::findThing(long thing_key) {
    ThingMap::iterator thing_iter = m_things.find(thing_key);
    if (thing_iter != m_things.end()) return thing_iter->second; else return nullptr;
}


//####################################################################################
//##    Returns a list of Thing keys contained in Stage, sorted from high z value to low
//####################################################################################
std::vector<long> DrStage::thingKeysSortedByZOrder(Sort_Order sort_order) {
    std::vector<long> z_ordered_keys;
    for (auto thing : thingsSortedByZOrder(sort_order))
        z_ordered_keys.push_back(thing->getKey());
    return z_ordered_keys;
}

//####################################################################################
//##    Returns a list of Things contained in Stage, sorted from high z value to low
//####################################################################################
std::vector<DrThing*> DrStage::thingsSortedByZOrder(Sort_Order sort_order, bool all_things, std::list<DrThing*> just_these_things) {
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
    std::deque<DrThing*> z_ordered_things;
    for (auto one_pair : zorder_key_pair) {
        z_ordered_things.push_front(one_pair.second);
    }

    // Reverse if Ascending
    if (sort_order == Sort_Order::AscendingOrder) {
        std::reverse(z_ordered_things.begin(), z_ordered_things.end());
    }

    return { z_ordered_things.begin(), z_ordered_things.end() };
}


//####################################################################################
//##    Remove any references within the current Stage Things to any GraphicsScene Items
//####################################################################################
void DrStage::removeGraphicsItemReferences() {
    for (auto &thing_pair : getThingMap()) {
        thing_pair.second->setEditorItem(nullptr);
    }
}














