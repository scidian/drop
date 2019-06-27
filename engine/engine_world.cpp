//
//      Created by Stephens Nunnally on 6/12/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "engine_thing_light.h"
#include "engine_world.h"


//####################################################################################
//##    Constructor / Destructor
//####################################################################################
DrEngineWorld::DrEngineWorld(DrEngine *engine, DrProject *project, long world_key) : m_engine(engine), m_project(project) {
    m_world = world_key;
}

DrEngineWorld::~DrEngineWorld() {
    clearWorld();
}


//####################################################################################
//##    Finds a Thing by Unique Key
//####################################################################################
DrEngineThing* DrEngineWorld::findThingByKey(long key) {
    for (auto thing : m_things) {
        if (thing->getKey() == key) return thing;
    }
    return nullptr;
}


//####################################################################################
//##    Adds Things to the World
//####################################################################################
void DrEngineWorld::addThings(QList<DrEngineThing*> things) {
    for (auto thing : things) {
        thing->addToWorld();
        m_things.append(thing);
    }
}
void DrEngineWorld::addThing( DrEngineThing *thing) {
    thing->addToWorld();
    m_things.append(thing);
}
