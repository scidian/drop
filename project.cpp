//
//      Created by Stephens Nunnally on 12/7/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      DrProject Class Definitions
//
//

#include "project.h"
#include "project_asset.h"
#include "project_world.h"
#include "project_world_scene.h"
#include "project_world_scene_object.h"

//####################################################################################
//##    Constructor, Destructor
//####################################################################################
DrProject::DrProject(long child_key_generator_starting_number, long asset_key_generator_starting_number)
{
    m_child_key_generator = child_key_generator_starting_number;
    m_asset_key_generator = asset_key_generator_starting_number;
}

DrProject::~DrProject()
{
    for (auto i: m_worlds) { delete i.second; }
}



//####################################################################################
//##    Project Children Functions
//####################################################################################

long DrProject::addAsset(QString new_asset_name, DrAsset_Type new_asset_type, QPixmap pixmap)
{
    long new_asset_key = getNextAssetKey();
    m_assets[new_asset_key] = new DrAsset(this, new_asset_key, new_asset_name, new_asset_type, pixmap);
    return new_asset_key;
}

// Adds a World to the map container, finds next availbable "World xxx" name to assign to World
void DrProject::addWorld()
{
    int test_num = 0;
    QString new_name;
    do {
        ++test_num;
        new_name = "World " + QString::number(test_num);
    } while (getWorldWithName(new_name) != nullptr);

    long new_world_key = getNextChildKey();
    m_worlds[new_world_key] = new DrWorld(this, new_world_key, new_name);
}

// Returns a pointer to the World with the mathcing name
DrWorld* DrProject::getWorldWithName(QString world_name)
{
    QString compare_name;
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
//####################################################################################

DrScene* DrProject::findSceneFromKey(long key)
{
    for (auto i: m_worlds) {
        for (auto j: i.second->getSceneMap()) {
            if (j.second->getKey() == key) { return j.second; }
        }
    }
    return nullptr;
}

// Returns a pointer to the Base DrSettings class of the object with the specified key
DrSettings* DrProject::findChildSettingsFromKey(long check_key)
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

// Searches all member variables / containers for the specified unique project key
DrType DrProject::findChildTypeFromKey(long check_key)
{
    return findChildSettingsFromKey(check_key)->getType();
}

DrSettings* DrProject::findAssetSettingsFromKey(long check_key)
{
    return m_assets[check_key]->getSettings();
}

DrAsset_Type DrProject::findAssetTypeFromKey(long check_key)
{
    return m_assets[check_key]->getAssetType();
}














