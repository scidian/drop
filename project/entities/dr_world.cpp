//
//      Created by Stephens Nunnally on 12/7/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      DrWorld Class Definitions
//
//
#include "core/dr_random.h"
#include "project/dr_project.h"
#include "project/entities/dr_stage.h"
#include "project/entities/dr_thing.h"
#include "project/entities/dr_world.h"
#include "project/settings/settings.h"
#include "project/settings/settings_component.h"
#include "project/settings/settings_component_property.h"


//####################################################################################
//##    Constructor / Destructor
//####################################################################################
DrWorld::DrWorld(DrProject *parent_project, long key, DrWorldType world_type, std::string new_world_name, bool add_start_stage)
    : DrSettings(parent_project), DrNode(DrNodeType::World) {

    this->setKey(key);
    this->initializeSettings(world_type, new_world_name);
    this->addWorldVariables();

    // Add initial Node Slots
    switch(world_type) {
        case DrWorldType::Physics_2D:
            addInputSlot(Input_Slots::Start);
            addOutputSlot(Output_Slots::UI);
            break;
        case DrWorldType::UI:
            addInputSlot(Input_Slots::Load);
            break;
    }


    // !!!!! #TEMP: Testing node building
    int add_input =  Dr::RandomInt(0, 3);
    int add_output = Dr::RandomInt(0, 4);
    for (int i = 0; i < add_input;  i++) { addInputSlot( "Test In "  + std::to_string(i)); }
    for (int i = 0; i < add_output; i++) { addOutputSlot("Test Out " + std::to_string(i)); }
    // !!!!!


    // Adds the initial "Start Stage"
    if (add_start_stage) {
        m_start_stage_key = addStage()->getKey();
    }

    m_last_stage_shown_in_editor = m_start_stage_key;
}

DrWorld::~DrWorld() {
    for (auto &stage_pair: m_stages) { delete stage_pair.second; }
}

// Removes a Stage from the Project
void DrWorld::deleteStage(DrStage *stage) {
    m_stages.erase(stage->getKey());
    delete stage;
}


//####################################################################################
//##    Adds a Stage
//####################################################################################
// Adds a Stage to the map container, assigns name based on current stage count
DrStage* DrWorld::addStage(std::string new_stage_name) {
    bool need_start_stage = false;
    if (m_stages.size() < 1) {
        need_start_stage = true;
        new_stage_name = "Start Stage";
    } else if (new_stage_name.empty()) {
        new_stage_name = std::to_string(static_cast<long>(m_stages.size() + 1));
    }

    long new_stage_key = getParentProject()->getNextKey();

    m_stages[new_stage_key] = new DrStage(getParentProject(), this, new_stage_key, new_stage_name, need_start_stage);
    return m_stages[new_stage_key];
}

DrStage* DrWorld::addStage(long stage_key, bool is_start_stage, DrPointF center_point, double zoom_scale) {
    m_stages[stage_key] = new DrStage(getParentProject(), this, stage_key, "TEMP", false);
    m_stages[stage_key]->setIsStartStage(is_start_stage);
    m_stages[stage_key]->setViewCenterPoint(center_point);
    m_stages[stage_key]->setViewZoomLevel(zoom_scale);
    return m_stages[stage_key];
}

// Adds a Stage as a copy from another Stage
DrStage* DrWorld::addStageCopyFromStage(DrStage *from_stage, std::string new_name, bool copy_into_start_stage) {
    DrStage *copy_stage = (copy_into_start_stage) ? getStageMap()[m_start_stage_key] : addStage();
    copy_stage->copyEntitySettings(from_stage);

    // No longer a Start Stage, can edit name
    if (copy_stage->isStartStage() == false) {
        DrProperty *my_name = copy_stage->getComponentProperty(Comps::Entity_Settings, Props::Entity_Name);
        my_name->setEditable(true);
    }

    // Set new name for Stage
    copy_stage->setName( new_name );

    // Copy all Things from From_Stage
    for (auto &thing_pair : from_stage->getThingMap()) {
        DrThing *thing = thing_pair.second;
        DrThing *copy_thing = copy_stage->addThing(thing->getThingType(), thing->getAssetKey(), 0, 0, 0);
        copy_thing->copyEntitySettings(thing);
    }
    return copy_stage;
}


//####################################################################################
//##    Stages / Stage Keys
//####################################################################################
// Returns the map Key of the first World in the map container
long DrWorld::getFirstStageKey() {
    return m_stages.begin()->first;
}

// Returns a pointer to the World with the associated Key from the map container
DrStage* DrWorld::getStageFromKey(long from_stage_key) {
    return m_stages[from_stage_key];
}

// Returns a pointer to the World with the mathcing name
DrStage* DrWorld::getStageWithName(std::string stage_name) {
    std::string compare_name;
    for (auto &stage_pair: m_stages) {
        compare_name = stage_pair.second->getName();
        if (compare_name == stage_name) { return stage_pair.second; }
    }
    return nullptr;
}

















