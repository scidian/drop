//
//      Created by Stephens Nunnally on 6/19/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Abstract Item representing a thing that has a body in the Physics Engine (object, character, light, etc)
//
//
#include "engine/engine_component.h"
#include "engine/thing/engine_thing.h"
#include "engine/thing/engine_thing_object.h"
#include "engine/thing_component/thing_comp_3d.h"
#include "engine/world/engine_world.h"
#include "project/entities/dr_asset.h"
#include "project/dr_project.h"


//####################################################################################
//##    Constructor / Destructor
//####################################################################################
DrEngineThing::DrEngineThing(DrEngineWorld* world, long unique_key, long original_key) {
    m_world = world;
    m_key = unique_key;
    m_original_project_key = original_key;

    init();
}

DrEngineThing::~DrEngineThing() {
    destroy();

    for (auto component_pair : m_components) {
        delete component_pair.second;
    }
}


//####################################################################################
//####################################################################################
//##
//##    EXPOSED API FUNCTIONS
//##
//####################################################################################
//####################################################################################

//####################################################################################
//##    Basic Virtual Component Functions
//####################################################################################
// Called when Thing is first created
void DrEngineThing::init() {

}

// Called when Thing is added to world
void DrEngineThing::addToWorld() {

}

// Update Function, Called every physics frame
bool DrEngineThing::update(double time_passed, double time_warp, DrRectF &area) {
    (void)time_passed;
    (void)time_warp;

    bool remove = false;

    for (auto component : m_components) {
        component.second->update(time_passed, time_warp);
    }

    // ***** Delete object if ends up outside the deletion threshold
    if (area.contains(getPosition()) == false) remove = true;
    return (remove || m_remove_me);
}

// Call during updateWorld(), process all signals then delete them
void DrEngineThing::signal(std::string name, DrVariant value) {
    for (auto component : m_components) {
        component.second->signal(name, value);
    }
}

// Called when Thing is being destroyed
void DrEngineThing::destroy() {

}


//####################################################################################
//##    Info Functions
//####################################################################################
// Returns name of this Entity (Thing)
std::string DrEngineThing::name() {
    if (m_world == nullptr) return "";
    DrSettings *entity = m_world->getProject()->findSettingsFromKey(m_original_project_key);
    if (entity == nullptr)   return "";
    return (entity->getName());
}


//####################################################################################
//##    Components Functions
//####################################################################################
// Returns component with matching component_name
DrEngineComponent* DrEngineThing::component(std::string component_name) {
    if (component_name == "") return nullptr;
    auto it = m_components.find(component_name);
    if (it == m_components.end()) { return nullptr; }
    return (*it).second;
}

// Removes component with component_name
void DrEngineThing::removeComponent(std::string component_name) {
    if (component_name == "") return;
    for (auto it = m_components.begin(); it != m_components.end(); ) {
        if ((*it).first == component_name) {
            it = m_components.erase(it);
            continue;
        }
        it++;
    }
}

// Sets Component to 'component', if nullptr is passed in, removes component
void DrEngineThing::setComponent(std::string component_name, DrEngineComponent *component) {
    if (component_name == "") return;
    if (component == nullptr) {
        removeComponent(component_name);
    } else {
        m_components[component_name] = component;
    }
}



//####################################################################################
//####################################################################################
//##
//##    INTERNAL FUNCTIONS
//##
//####################################################################################
//####################################################################################

//####################################################################################
//##    Components Functions
//####################################################################################
void DrEngineThing::setComponent3D(ThingComp3D *component)          { m_comp_3d = component;        setComponent(component->name(), component); }
void DrEngineThing::setComponentCamera(ThingCompCamera *component)  { m_comp_camera = component;    setComponent(component->name(), component); }
void DrEngineThing::setComponentPlayer(ThingCompPlayer *component)  { m_comp_player = component;    setComponent(component->name(), component); }


//####################################################################################
//##    Get time since last update
//####################################################################################
void DrEngineThing::calculateTimeSinceLastUpdate() {
    time_since_last_update = Dr::MillisecondsElapsed( update_timer );
    Dr::ResetTimer( update_timer );
}













