//
//      Created by Stephens Nunnally on 12/7/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      DrProject Class Definitions
//
//

#include "project.h"
#include "project_world.h"
#include "project_world_scene.h"
#include "project_world_scene_object.h"

//####################################################################################
//##    Constructor, Destructor - addWorld
//####################################################################################
DrProject::DrProject(long key_generator_starting_number)
{
    m_key_generator = key_generator_starting_number;
    addWorld();
}

DrProject::~DrProject()
{
    for (auto i: m_worlds) { delete i.second; }
}

// Adds a World to the map container, finds next availbable "World xxx" name to assign to World
void DrProject::addWorld()
{
    int test_num = 0;
    std::string new_name;
    do {
        ++test_num;
        new_name = "World " + std::to_string(test_num);
    } while (getWorldWithName(new_name) != nullptr);

    long new_world_key = getNextKey();
    m_worlds[new_world_key] = new DrWorld(this, new_world_key, new_name);
}


//####################################################################################
//##    Getters - getFirstWorldKey, getWorld (from key), getWorldWithName (from name)
//####################################################################################

// Returns the map Key of the first World in the map container
long DrProject::getFirstWorldKey()
{
    return m_worlds.begin()->first;
}

// Returns a pointer to the World with the associated Key from the map container
DrWorld* DrProject::getWorld(long from_world_key)
{
    return m_worlds[from_world_key];
}

// Returns a pointer to the World with the mathcing name
DrWorld* DrProject::getWorldWithName(std::string world_name)
{
    std::string compare_name;
    for (auto i: m_worlds) {
        compare_name = i.second->getWorldName();
        if (compare_name == world_name) { return i.second; }
    }
    return nullptr;
}


//####################################################################################
//##
//##    Key Finding - These functions search the current project for the specified Key and return the requested info
//##                    (key's are generated from Project key generator upon object initialization)
//##
//##    findSettingsFromKey - Returns a pointer to the Base DrSettings class of the object with the specified key
//##
//##    findTypeFromKey - Searches all member variables / containers for the specified unique project key
//##
//####################################################################################
DrSettings* DrProject::findSettingsFromKey(long check_key)
{
    for (auto i: m_worlds) {
        if (i.second->getKey() == check_key) { return i.second->getSettings(); }

        for (auto j: i.second->getSceneMap()) {
            if (j.second->getKey() == check_key) { return j.second->getSettings(); }

            for (auto k: j.second->getObjectMap()) {
                if (k.second->getKey() == check_key) { return k.second->getSettings(); }

                //************* More types implemented

            }
        }
    }
    return nullptr;
}

DrTypes DrProject::findTypeFromKey(long check_key)
{
    return findSettingsFromKey(check_key)->getType();
}





