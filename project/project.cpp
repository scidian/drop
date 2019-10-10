//
//      Created by Stephens Nunnally on 12/7/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      DrProject Class Definitions
//
//
#include <QDebug>
#include <QRandomGenerator>
#include <QTime>

#include "helper.h"
#include "helper_qt.h"
#include "project.h"
#include "project_asset.h"
#include "project_effect.h"
#include "project_font.h"
#include "project_image.h"
#include "project_world.h"
#include "project_world_stage.h"
#include "project_world_stage_thing.h"


//####################################################################################
//##    Constructor, Destructor
//####################################################################################
DrProject::DrProject(long key_generator_starting_number) {
    // #NOTE: Don't alllow key to start at less than 1, having an item with key 0 could conflict with nullptr results
    if (key_generator_starting_number <= 1) key_generator_starting_number = 1;
    m_key_generator = key_generator_starting_number;
}

DrProject::~DrProject() {
    clearProject();
}

void DrProject::clearProject() {
    for (auto it = m_worlds.begin();    it != m_worlds.end(); )     {   delete it->second; it = m_worlds.erase(it);     }
    for (auto it = m_assets.begin();    it != m_assets.end(); )     {   delete it->second; it = m_assets.erase(it);     }
    for (auto it = m_fonts.begin();     it != m_fonts.end(); )      {   delete it->second; it = m_fonts.erase(it);      }
    for (auto it = m_images.begin();    it != m_images.end(); )     {   delete it->second; it = m_images.erase(it);     }
    for (auto it = m_effects.begin();   it != m_effects.end(); )    {   delete it->second; it = m_effects.erase(it);    }
    m_key_generator = 1;
}


//####################################################################################
//##
//##    Functions to add different item types into project
//##
//####################################################################################
long DrProject::addAsset(DrAssetType new_asset_type, long image_key) {
    long new_asset_key = getNextKey();
    m_assets[new_asset_key] = new DrAsset(this, new_asset_key, new_asset_type, image_key);
    return new_asset_key;
}

long DrProject::addEffect(QString effect_name, DrEffectType effect_type) {
    long new_effect_key = getNextKey();
    m_effects[new_effect_key] = new DrEffect(this, new_effect_key, effect_name, effect_type);
    return new_effect_key;
}

long DrProject::addFont(QString font_name, QPixmap font_pixmap, QString font_family, int font_size, bool use_test_rects) {
    long new_font_key = getNextKey();
    m_fonts[new_font_key] = new DrFont(this, new_font_key, font_name, font_pixmap, font_family, font_size, use_test_rects);
    return new_font_key;
}

long DrProject::addImage(QString image_path) {
    long new_image_key = getNextKey();
    m_images[new_image_key] = new DrImage(this, new_image_key, image_path);
    return new_image_key;
}


// Adds a World to the map container, finds next availbable "World xxx" name to assign to World
long DrProject::addWorld() {
    int test_num = 0;
    QString new_name;
    do {
        ++test_num;
        new_name = "World " + QString::number(test_num);
    } while (findWorldWithName(new_name) != nullptr);

    long new_world_key = getNextKey();
    m_worlds[new_world_key] = new DrWorld(this, new_world_key, new_name);
    return new_world_key;
}


//####################################################################################
//##
//##    Key Finding - These functions search the current project for the specified Key and return the requested info
//##                  (key's are generated from Project key generator upon item initialization)
//##
//####################################################################################

// Returns a pointer to the Base DrSettings class of the item with the specified key
DrSettings* DrProject::findSettingsFromKey(long check_key, bool show_warning) {
    AssetMap::iterator asset_iter = m_assets.find(check_key);
    if (asset_iter != m_assets.end())   return asset_iter->second;

    ImageMap::iterator image_iter = m_images.find(check_key);
    if (image_iter != m_images.end())   return image_iter->second;

    EffectMap::iterator effect_iter = m_effects.find(check_key);
    if (effect_iter != m_effects.end()) return effect_iter->second;

    FontMap::iterator font_iter = m_fonts.find(check_key);
    if (font_iter != m_fonts.end())     return font_iter->second;

    WorldMap &worlds = m_worlds;
    WorldMap::iterator world_iter = worlds.find(check_key);
    if (world_iter != worlds.end())   return world_iter->second;

    for (auto world_pair : worlds) {
        StageMap &stages = world_pair.second->getStageMap();
        StageMap::iterator stage_iter = stages.find(check_key);
        if (stage_iter != stages.end())     return stage_iter->second;

        for (auto stage_pair : stages) {
            ThingMap &things = stage_pair.second->getThingMap();
            ThingMap::iterator thing_iter = things.find(check_key);
            if (thing_iter != things.end()) return thing_iter->second;
        }
    }

    if (show_warning) {
        Dr::ShowMessageBox("WARNING: Did not find key (" + QString::number(check_key) +
                           ") in project! \n"
                           "Last key used in project: " + QString::number(m_key_generator - 1) + "!\n\n"
                           "This warning called from \"DrProject::findChildSettingsFromKey\"");
    }
    return nullptr;
}

// Searches all member variables / containers for the specified unique project key
DrType DrProject::findChildTypeFromKey(long check_key) {
    DrSettings *settings = findSettingsFromKey(check_key);
    if (settings == nullptr) {
        return DrType::NotFound;
    } else {
        return settings->getType();
    }
}


DrAsset* DrProject::findAssetFromKey(long check_key) {
    AssetMap::iterator asset_iter = m_assets.find(check_key);
    if (asset_iter != m_assets.end())
        return asset_iter->second;
    else
        return nullptr;
}

DrStage* DrProject::findStageFromKey(long check_key) {
    for (auto world_pair : m_worlds) {
        StageMap &stages = world_pair.second->getStageMap();
        StageMap::iterator stage_iter = stages.find(check_key);

        if (stage_iter != stages.end())
            return stage_iter->second;
    }
    return nullptr;
}

DrThing* DrProject::findThingFromKey(long check_key) {
    for (auto world : m_worlds) {
        for (auto stage : world.second->getStageMap()) {
            ThingMap &things = stage.second->getThingMap();
            ThingMap::iterator thing_iter = things.find(check_key);
            if (thing_iter != things.end())
                return thing_iter->second;
        }
    }
    return nullptr;
}

DrWorld* DrProject::findWorldFromKey(long check_key) {
    WorldMap::iterator world_iter = m_worlds.find(check_key);
    if (world_iter != m_worlds.end())
        return world_iter->second;
    else
        return nullptr;
}

// Returns a pointer to the World with the mathcing name
DrWorld* DrProject::findWorldWithName(QString world_name) {
    QString compare_name;
    for (auto world_pair: m_worlds) {
        compare_name = world_pair.second->getName();
        if (compare_name == world_name) { return world_pair.second; }
    }
    return nullptr;
}


//####################################################################################
//##    Tests find function for speed so we can test speed of std::map vs std::unordered_map, etc
//####################################################################################
QString DrProject::testSpeedFindSettings(int test_size) {
    // Show some initial data
    int     number_of_assets = int(m_key_generator) - 1;
    QString results =   "Find Speed Test \n---------------\n"
                        "Number of Assets in Project: " + QString::number(number_of_assets) + "\n"
                        "Newest Asset Name: " + findSettingsFromKey(number_of_assets)->getName() + "\n"
                        "Newest Asset Type: " + Dr::StringFromType(findSettingsFromKey(number_of_assets)->getType()) + "\n"
                        "First  Asset Name: " + findSettingsFromKey(c_key_starting_number)->getName() + "\n"
                        "First  Asset Type: " + Dr::StringFromType(findSettingsFromKey(c_key_starting_number)->getType()) + "\n\n"
                        "Number of iterations tested: " + QString::number(test_size) + "\n";

    // Create a vector of random keys to find
    QVector<int> keys;
    for (int i = 0; i < test_size; i++)
        keys.append( QRandomGenerator::global()->bounded(c_key_starting_number, number_of_assets) );

    // Find the random keys within the project
    QTime timer;
    timer.restart();
    for (auto key : keys)
        findSettingsFromKey(key);
    results += "Total milliseconds taken to find all settings in project: " + QString::number(timer.elapsed()) + "\n\n";

    return results;
}
















