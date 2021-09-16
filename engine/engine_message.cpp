//
//      Created by Stephens Nunnally on 5/15/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "engine/engine_message.h"
#include "engine/thing/engine_thing.h"
#include "engine/world/engine_world.h"
#include "project/dr_project.h"
#include "project/entities/dr_thing.h"


//####################################################################################
//##    Constructor / Destructor
//####################################################################################
DrEngineMessage::DrEngineMessage(std::string name, DrVariant value, DrEngineThing *thing_a, DrEngineThing *thing_b) {
    m_name =        name;
    m_value =       value;
    m_life =        Message_Life::Born;

    m_thing_a =     thing_a;
    m_thing_b =     thing_b;
}

DrEngineMessage::~DrEngineMessage() {

}


//####################################################################################
//##    Underlying Thing Info
//####################################################################################
std::string DrEngineMessage::thingNameA() {
    if (m_thing_a != nullptr) return m_thing_a->name();
    return "Thing A Not Found";
}

long DrEngineMessage::thingAssetKeyA() {
    if (m_thing_a != nullptr) {
        DrThing *thing = m_thing_a->world()->getProject()->findThingFromKey(m_thing_a->getOriginalKey());
        if (thing != nullptr) return thing->getAssetKey();
    }
    return c_no_key;
}

std::string DrEngineMessage::thingNameB() {
    if (m_thing_b != nullptr) return m_thing_b->name();
    return "Thing A Not Found";
}

long DrEngineMessage::thingAssetKeyB() {
    if (m_thing_b != nullptr) {
        DrThing *thing = m_thing_b->world()->getProject()->findThingFromKey(m_thing_b->getOriginalKey());
        if (thing != nullptr) return thing->getAssetKey();
    }
    return c_no_key;
}











