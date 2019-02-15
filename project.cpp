//
//      Created by Stephens Nunnally on 12/7/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      DrProject Class Definitions
//
//

#include <stdexcept>

#include "library.h"

#include "project.h"
#include "project_asset.h"
#include "project_image.h"
#include "project_world.h"
#include "project_world_stage.h"
#include "project_world_stage_object.h"

//####################################################################################
//##    Constructor, Destructor
//####################################################################################
DrProject::DrProject(long key_generator_starting_number)
{
    // Don't alllow key to start at less than 1, having an item with key 0 could conflict with nullptr results
    if (key_generator_starting_number <= 1) key_generator_starting_number = 1;
    m_key_generator = key_generator_starting_number;
}

DrProject::~DrProject()
{
    for (auto i: m_worlds) { delete i.second; }
}



//####################################################################################
//##    Project Children Functions
//####################################################################################

long DrProject::addAsset(DrAssetType new_asset_type, long image_key)
{
    long new_asset_key = getNextKey();
    m_assets[new_asset_key] = new DrAsset(this, new_asset_key, new_asset_type, image_key);
    return new_asset_key;
}

long DrProject::addImage(QString image_path)
{
    long new_image_key = getNextKey();
    m_images[new_image_key] = new DrImage(this, new_image_key, image_path);
    return new_image_key;
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

    long new_world_key = getNextKey();
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

DrStage* DrProject::findStageFromKey(long check_key)
{
    for (auto world_pair : m_worlds) {
        try {  return world_pair.second->getStageMap().at(check_key);  }
        catch (const std::out_of_range&) {  }               // Not Found
    }
    return nullptr;
}

// Returns a pointer to the Base DrSettings class of the object with the specified key
DrSettings* DrProject::findSettingsFromKey(long check_key)
{
    try {  return m_assets.at(check_key);  }
    catch (const std::out_of_range&) {  }                   // Not Found

    for (auto i : m_worlds) {
        if (i.second->getKey() == check_key) { return i.second->getSettings(); }

        for (auto j : i.second->getStageMap()) {
            if (j.second->getKey() == check_key) { return j.second->getSettings(); }

            for (auto k : j.second->getObjectMap()) {
                if (k.second->getKey() == check_key) { return k.second->getSettings(); }

                //************* More types implemented
            }
        }
    }

    Dr::ShowMessageBox("WARNING: Did not find key in project (from 'DrProject::findChildSettingsFromKey')...");
    return nullptr;
}

// Searches all member variables / containers for the specified unique project key
DrType DrProject::findChildTypeFromKey(long check_key)
{
    return findSettingsFromKey(check_key)->getType();
}

























