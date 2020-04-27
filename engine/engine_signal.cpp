//
//      Created by Stephens Nunnally on 2/3/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "engine/engine_signal.h"
#include "engine/thing/engine_thing.h"
#include "engine/world/engine_world.h"
#include "project/dr_project.h"
#include "project/entities/dr_thing.h"


//####################################################################################
//##    Constructor / Destructor
//####################################################################################
DrEngineSignal::DrEngineSignal(std::string name, DrVariant value, DrEngineThing *thing_a, DrEngineThing *thing_b) {
    m_name =        name;
    m_value =       value;
    m_life =        Signal_Life::Born;

    m_thing_a =     thing_a;
    m_thing_b =     thing_b;
}

DrEngineSignal::~DrEngineSignal() {

}


//####################################################################################
//##    Underlying Thing Info
//####################################################################################
std::string DrEngineSignal::thingNameA() {
    if (m_thing_a != nullptr) return m_thing_a->name();
    return "Thing A Not Found";
}

long DrEngineSignal::thingAssetKeyA() {
    if (m_thing_a != nullptr) {
        DrThing *thing = m_thing_a->world()->getProject()->findThingFromKey(m_thing_a->getOriginalKey());
        if (thing != nullptr) return thing->getAssetKey();
    }
    return c_no_key;
}

std::string DrEngineSignal::thingNameB() {
    if (m_thing_b != nullptr) return m_thing_b->name();
    return "Thing A Not Found";
}

long DrEngineSignal::thingAssetKeyB() {
    if (m_thing_b != nullptr) {
        DrThing *thing = m_thing_b->world()->getProject()->findThingFromKey(m_thing_b->getOriginalKey());
        if (thing != nullptr) return thing->getAssetKey();
    }
    return c_no_key;
}




















