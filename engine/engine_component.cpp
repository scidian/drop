//
//      Created by Stephens Nunnally on 2/2/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "engine/engine.h"
#include "engine/engine_component.h"
#include "engine/thing/engine_thing.h"
#include "engine/world/engine_world.h"


//####################################################################################
//##    Constructor / Destructor
//####################################################################################
DrEngineComponent::DrEngineComponent(DrEngineWorld *engine_world, DrEngineThing *parent_thing, std::string component_name) {
    m_world = engine_world;
    m_thing = parent_thing;
    m_name =  component_name;

    init();
}

DrEngineComponent::~DrEngineComponent() {
    destroy();
}


//####################################################################################
//##    Basic Virtual Component Functions
//####################################################################################
// Called when component is first created
void DrEngineComponent::init() {

}

// Called during DrEngineWorld->updateWorld() step
void DrEngineComponent::update(double time_passed, double time_warp) {

}

// Called when component is destroyed
void DrEngineComponent::destroy() {

}


//####################################################################################
//##    Component Functions
//####################################################################################
// Adds signal to stack
void DrEngineComponent::emitSignal(std::string name, DrVariant value) {
    m_world->getEngine()->pushSignal(name, value, m_thing->getKey());
}

// Returns list of signals with name
EngineSignals DrEngineComponent::signalList(std::string name) {
    return m_world->getEngine()->signalList(name);
}

//####################################################################################
//##    Attributes
//####################################################################################
// Returns attribute by name
DrVariant DrEngineComponent::attribute(std::string attribute_name) {
    auto it = m_attributes.find(attribute_name);
    if (it == m_attributes.end()) { return NULL; }
    return (*it).second;
}

// Sets attribute by name
void DrEngineComponent::setAttribute(std::string attribute_name, DrVariant value) {
    m_attributes[attribute_name] = value;
}
























