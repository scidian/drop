//
//      Created by Stephens Nunnally on 12/7/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      DrWorld Class Definitions
//
//
#include "model/project/project.h"
#include "model/project/project_world.h"
#include "model/project/project_stage.h"
#include "model/project/project_thing.h"
#include "model/settings/settings.h"
#include "model/settings/settings_component.h"
#include "model/settings/settings_component_property.h"


//####################################################################################
//##    Constructor, Destructor
//####################################################################################
DrWorld::DrWorld(DrProject *parent_project, long key, QString new_world_name, bool add_start_stage) : DrSettings(parent_project) {
    this->setKey(key);

    initializeWorldSettings(new_world_name);

    // Adds the initial "Start Stage"
    if (add_start_stage) {
        m_start_stage_key = addStage()->getKey();
    }

    m_last_stage_shown_in_editor = m_start_stage_key;
}

DrWorld::~DrWorld() {
    for (auto i: m_stages) { delete i.second; }
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
DrStage* DrWorld::addStage(QString new_stage_name) {
    bool need_start_stage = false;
    if (m_stages.size() < 1) {
        need_start_stage = true;
        new_stage_name = "Start Stage";
    } else if (new_stage_name == "") {
        new_stage_name = QString::number(static_cast<long>(m_stages.size() + 1));
    }

    long new_stage_key = getParentProject()->getNextKey();

    m_stages[new_stage_key] = new DrStage(getParentProject(), this, new_stage_key, new_stage_name, need_start_stage);
    return m_stages[new_stage_key];
}

DrStage* DrWorld::addStage(long stage_key, bool is_start_stage, QPointF center_point, double zoom_scale) {
    m_stages[stage_key] = new DrStage(getParentProject(), this, stage_key, "TEMP", false);
    m_stages[stage_key]->setIsStartStage(is_start_stage);
    m_stages[stage_key]->setViewCenterPoint(center_point);
    m_stages[stage_key]->setViewZoomLevel(zoom_scale);
    return m_stages[stage_key];
}

// Adds a Stage as a copy from another Stage
DrStage* DrWorld::addStageCopyFromStage(DrStage *from_stage, bool copy_into_start_stage) {
    DrStage *copy_stage = (copy_into_start_stage) ? getStageMap()[m_start_stage_key] : addStage();
    copy_stage->copyEntitySettings(from_stage);

    // No longer a Start Stage, can edit name
    if (copy_stage->isStartStage() == false) {
        DrProperty *my_name = copy_stage->getComponentProperty(Components::Entity_Settings, Properties::Entity_Name);
        my_name->setEditable(true);
    }

    // Find a new name for Stage
    QString new_name;
    bool    has_name;
    int     name_count = 1;
    do {
        has_name = false;
        new_name = (name_count == 1) ? from_stage->getName() + " copy" : from_stage->getName() + " copy (" + QString::number(name_count) + ")";
        for (auto &stage_pair : getStageMap()) {
            if (stage_pair.second->getName() == new_name) has_name = true;
        }
        name_count++;
    } while (has_name != false);
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
DrStage* DrWorld::getStageWithName(QString stage_name) {
    QString compare_name;
    for (auto i: m_stages) {
        compare_name = i.second->getName();
        if (compare_name == stage_name) { return i.second; }
    }
    return nullptr;
}

















