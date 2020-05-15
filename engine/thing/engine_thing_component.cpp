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
//##    Virtual Component Event Functions
//####################################################################################
// Called when component is first created
void DrThingComponent::init() {

}

// Called when Thing is added to m_things DrEngineWorld vector
void DrThingComponent::addToWorld() {

}

// Called when it is time to Render Thing
void DrThingComponent::draw() {
    m_call_draw = false;
}

// Called during DrEngineWorld->updateWorld() step, returns true if parent DrEngineThing should be removed
bool DrThingComponent::update(double, double) {
    m_call_update = false;
    return false;
}

// Called when component is destroyed
void DrThingComponent::destroy() {

}


//####################################################################################
//##    Component Functions
//####################################################################################
// Adds message to stack
void DrThingComponent::emitMessage(std::string name, DrVariant value, DrEngineThing *thing_b = nullptr) {
    m_world->getEngine()->pushMessage(name, value, m_thing, thing_b);
}

// Returns list of messages with name
EngineMessages DrThingComponent::messageList(std::string name, long thing_key) {
    return m_world->engine()->messageList(name, thing_key);
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


















