//
//      Created by Stephens Nunnally on 3/10/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "engine/engine.h"
#include "engine/thing/engine_thing_component.h"
#include "engine/thing/engine_thing.h"
#include "engine/world/engine_world.h"


//####################################################################################
//##    Constructor / Destructor
//####################################################################################
DrThingComponent::DrThingComponent(DrEngineWorld *engine_world, DrEngineThing *parent_thing, std::string component_name) {
    m_world = engine_world;
    m_thing = parent_thing;
    m_name =  component_name;

    init();
}

DrThingComponent::~DrThingComponent() {
    destroy();
}


//####################################################################################
//##    Basic Virtual Component Functions
//####################################################################################
// Called when component is first created
void DrThingComponent::init() {

}

// Called when Thing is added to m_things DrEngineWorld vector
void DrThingComponent::addToWorld() {

}

// Called when it is time to Render Thing
void DrThingComponent::draw() {

}

// Called during DrEngineWorld->updateWorld() step
void DrThingComponent::update(double time_passed, double time_warp) {
    (void) time_passed;
    (void) time_warp;

}

// Called when component is destroyed
void DrThingComponent::destroy() {

}


//####################################################################################
//##    Component Functions
//####################################################################################
// Adds signal to stack
void DrThingComponent::emitSignal(std::string name, DrVariant value, long thing_b) {
    m_world->getEngine()->pushSignal(name, value, m_thing->getKey(), thing_b);
}

// Returns list of signals with name
EngineSignals DrThingComponent::signalList(std::string name) {
    return m_world->getEngine()->signalList(name);
}


//####################################################################################
//##    Attributes
//####################################################################################
// Returns attribute by name
DrVariant DrThingComponent::attribute(std::string attribute_name) {
    auto it = m_attributes.find(attribute_name);
    if (it == m_attributes.end()) { return NULL; }
    return (*it).second;
}

// Sets attribute by name
void DrThingComponent::setAttribute(std::string attribute_name, DrVariant value) {
    m_attributes[attribute_name] = value;
}


















