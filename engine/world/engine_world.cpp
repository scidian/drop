//
//      Created by Stephens Nunnally on 6/12/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "engine/thing/components/thing_comp_physics.h"
#include "engine/thing/engine_thing.h"
#include "engine/world/engine_world.h"


//####################################################################################
//##    Constructor / Destructor
//####################################################################################
DrEngineWorld::DrEngineWorld(DrEngine *engine, DrProject *project, EngineTextureMap &textures, long world_key) : m_textures(textures) {
    m_engine =      engine;
    m_project =     project;
    m_world_key =   world_key;
}

DrEngineWorld::~DrEngineWorld() {
    clearWorld();
}


//####################################################################################
//####################################################################################
//##
//##    EXPOSED API FUNCTIONS
//##
//####################################################################################
//####################################################################################
// Returns a list of Things (entities) with designated name
EngineThings DrEngineWorld::find(std::string name) {
    std::vector<DrEngineThing*> thing_list;
    thing_list.clear();
    for (auto thing : m_things) {
        if (thing->name() == name) {
            thing_list.push_back(thing);
        }
    }
    return thing_list;
}




//####################################################################################
//####################################################################################
//##
//##    INTERNAL FUNCTIONS
//##
//####################################################################################
//####################################################################################

//####################################################################################
//##    Finds a Thing by Unique Key
//####################################################################################
DrEngineThing* DrEngineWorld::findThingByKey(long key) {
    for (auto thing : m_things) {
        if (thing == nullptr) continue;
        if (thing->getKey() == key) return thing;
    }
    return nullptr;
}


//####################################################################################
//##    Adds Things to the World
//####################################################################################
void DrEngineWorld::addThings(std::list<DrEngineThing*> things_to_add) {
    for (auto thing : things_to_add) {
        addThing(thing);
    }
}
void DrEngineWorld::addThing(DrEngineThing *thing) {
    thing->addToWorld();
    m_things.push_back(thing);
}

int DrEngineWorld::countCharacters() {
    int character_count = 0;
    for (auto thing : m_things) {
        if (thing != nullptr) {
            if (thing->compPhysics() != nullptr) {
                if (thing->compPhysics()->hasKeyControls()) character_count++;
            }
        }
    }
    return character_count;
}
















