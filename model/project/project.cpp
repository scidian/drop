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

#include "editor/forms/form_main.h"
#include "editor/globals_editor.h"
#include "editor/helper_editor.h"
#include "helper.h"
#include "model/project/project.h"
#include "model/project/project_animation.h"
#include "model/project/project_asset.h"
#include "model/project/project_device.h"
#include "model/project/project_effect.h"
#include "model/project/project_font.h"
#include "model/project/project_image.h"
#include "model/project/project_world.h"
#include "model/project/project_stage.h"
#include "model/project/project_thing.h"


//####################################################################################
//##    Constructor, Destructor
//####################################################################################
DrProject::DrProject() {
    clearProject();
}

DrProject::~DrProject() {
    clearProject(false);
}

void DrProject::clearProject(bool add_built_in_items) {
    m_test_only = false;

    for (auto it = m_animations.begin();    it != m_animations.end(); ) {   delete it->second; it = m_animations.erase(it); }
    for (auto it = m_assets.begin();        it != m_assets.end(); )     {   delete it->second; it = m_assets.erase(it);     }
    for (auto it = m_devices.begin();       it != m_devices.end(); )    {   delete it->second; it = m_devices.erase(it);    }
    for (auto it = m_effects.begin();       it != m_effects.end(); )    {   delete it->second; it = m_effects.erase(it);    }
    for (auto it = m_fonts.begin();         it != m_fonts.end(); )      {   delete it->second; it = m_fonts.erase(it);      }
    for (auto it = m_images.begin();        it != m_images.end(); )     {   delete it->second; it = m_images.erase(it);     }
    for (auto it = m_worlds.begin();        it != m_worlds.end(); )     {   delete it->second; it = m_worlds.erase(it);     }

    // Add these Images to every project for use with New Assets
    if (add_built_in_items) {
        addBuiltInImages();
        addDefaultAssets();
    }

    // !!!!! #NOTE: Don't allow key to start at less than 1, having an item with key 0 could conflict with nullptr results
    //              (starts at 1001)
    m_key_generator = c_key_starting_number;
}


//####################################################################################
//##    Delete
//##        Functions to Delete different item types into project
//####################################################################################
// Removes an Animation from the Project if not used by another Asset
void DrProject::deleteAnimation(long animation_key, long ignore_asset_key) {
    if (animation_key < c_key_starting_number) return;
    DrAnimation *animation = findAnimationFromKey(animation_key);
    if (animation == nullptr) return;

    // See if Animations are used by any other Asset
    for (auto &asset_pair : getAssetMap()) {
        if (asset_pair.first == ignore_asset_key) continue;
        if (asset_pair.second == nullptr) continue;

        // If key is used by another Asset, don't delete
        for (auto &check_key : asset_pair.second->animationsUsedByAsset()) {
            if (check_key == animation_key) return;
        }
    }

    // Delete all Images in Animation
    for (auto frame : animation->getFrames()) {
        long image_key = frame->getKey();
        if  (image_key < c_key_starting_number) continue;

        // See if Image is used by any other Animaiton
        for (auto &animation_pair : getAnimationMap()) {
            if (animation_pair.first != animation_key) {
                bool another_is_using_image = false;

                for (auto check_frame : animation_pair.second->getFrames()) {
                    if (check_frame->getKey() == image_key) {
                        another_is_using_image = true;
                        break;
                    }
                }
                if (another_is_using_image) break;
            }

            deleteImage( image_key );
        }
    }

    // Delete Animation
    m_animations.erase(animation_key);
    delete animation;
}

// Removes an Asset from the Project
void DrProject::deleteAsset(long asset_key) {
    DrAsset *asset = findAssetFromKey(asset_key);
    if (asset == nullptr) return;
    asset->deleteAnimations();
    m_assets.erase(asset_key);
    delete asset;
}

// Attempts to remove an Entity from the project
void DrProject::deleteEntity(long entity_key) {
    DrSettings *entity = findSettingsFromKey(entity_key);
    if (entity == nullptr) return;
    switch (entity->getType()) {
        case DrType::Asset:     deleteAsset(entity_key);    break;
        case DrType::Font:      deleteFont(entity_key);     break;
        default: return;
    }
}

// Removes a Font from the Project
void DrProject::deleteFont(long font_key) {
    DrFont *font = findFontFromKey(font_key);
    if (font == nullptr) return;
    m_fonts.erase(font_key);
    delete font;
}

// Removes an Image from the Project
void DrProject::deleteImage(long image_key) {
    DrImage *image = findImageFromKey(image_key);
    if (image == nullptr) return;
    m_images.erase(image_key);
    delete image;
}

// Removes a World from the Project
void DrProject::deleteWorld(long world_key) {
    DrWorld *world = findWorldFromKey(world_key);
    if (world == nullptr) return;
    m_worlds.erase(world->getKey());
    delete world;
}


//####################################################################################
//##    Add
//##        Functions to Add different item types into project
//####################################################################################
DrAnimation* DrProject::addAnimation(QList<long> source_image_keys, long key) {
    long new_animation_key = (key == c_no_key) ? getNextKey() : key;
    m_animations[new_animation_key] = new DrAnimation(this, new_animation_key, source_image_keys);
    return m_animations[new_animation_key];
}

DrAsset* DrProject::addAsset(DrAssetType new_asset_type, long source_image_key, long key) {
    long new_asset_key = (key == c_no_key) ? getNextKey() : key;
    m_assets[new_asset_key] = new DrAsset(this, new_asset_key, new_asset_type, source_image_key);
    if (new_asset_key <= 0) {
        qDebug() << "DrProject::addAsset() strange number!!! Key: " << new_asset_key << ", Type: " << Dr::StringFromAssetType(new_asset_type);
    }
    return m_assets[new_asset_key];
}

long DrProject::addDevice(DrDeviceType device_type, long key) {
    long new_device_key = (key == c_no_key) ? getNextKey() : key;
    m_devices[new_device_key] = new DrDevice(this, new_device_key, device_type);
    return new_device_key;
}

long DrProject::addEffect(DrEffectType effect_type, long key) {
    long new_effect_key = (key == c_no_key) ? getNextKey() : key;
    m_effects[new_effect_key] = new DrEffect(this, new_effect_key, effect_type);
    return new_effect_key;
}

long DrProject::addFont(QString font_name, QPixmap font_pixmap, QString font_family, int font_size, bool use_test_rects, long key) {
    long new_font_key = (key == c_no_key) ? getNextKey() : key;
    m_fonts[new_font_key] = new DrFont(this, new_font_key, font_name, font_pixmap, font_family, font_size, use_test_rects);
    return new_font_key;
}

DrImage* DrProject::addImage(QString image_path, long key, Asset_Category category) {
    long new_image_key = (key == c_no_key) ? getNextKey() : key;
    m_images[new_image_key] = new DrImage(this, new_image_key, image_path, category);
    return m_images[new_image_key];
}
long DrProject::addImage(long key, QString full_path, QString filename, QString simple_name, QImage &image, Asset_Category category) {
    m_images[key] = new DrImage(this, key, full_path, filename, simple_name, image, category);
    return key;
}

// Adds a World to the map container, finds next available "World xxx" name to assign to World
DrWorld* DrProject::addWorld() {
    int test_num = static_cast<int>(m_worlds.size());
    QString new_name;
    do {
        ++test_num;
        new_name = "World " + QString::number(test_num);
    } while (findWorldWithName(new_name) != nullptr);

    long new_world_key = getNextKey();
    m_worlds[new_world_key] = new DrWorld(this, new_world_key, new_name);
    return m_worlds[new_world_key];
}

// Adds a World to the map container, !!!!! DOES NOT CREATE Start Stage
DrWorld* DrProject::addWorld(long key, long start_stage_key, long last_stage_in_editor_key) {
    m_worlds[key] = new DrWorld(this, key, "TEMP", false);
    m_worlds[key]->setStartStageKey(start_stage_key);
    m_worlds[key]->setLastStageShownKey(last_stage_in_editor_key);
    return m_worlds[key];
}

// Adds a new World, copied from another World
DrWorld* DrProject::addWorldCopyFromWorld(DrWorld* from_world) {
    DrWorld *copy_world = addWorld();
    copy_world->copyEntitySettings(from_world);

    // Find name for Copy World
    QString new_name;
    bool    has_name;
    int     i = 1;
    do {
        has_name = false;
        new_name = (i == 1) ? from_world->getName() + " copy" : from_world->getName() + " copy (" + QString::number(i) + ")";
        for (auto &world_pair : getWorldMap()) {
            if (world_pair.second->getName() == new_name) has_name = true;
        }
        i++;
    } while (has_name != false);
    copy_world->setName( new_name );

    // Copy all Stages from World
    int stage_count = 0;
    for (auto &stage_pair : from_world->getStageMap()) {
        DrStage *stage = stage_pair.second;
        DrStage *copy_stage = copy_world->addStageCopyFromStage(stage, (stage_count == 0));
        copy_stage->copyEntitySettings(stage);
        stage_count++;
    }
    return copy_world;
}


//####################################################################################
//##
//##    Key Finding - These functions search the current project for the specified Key and return the requested info
//##                  (key's are generated from Project key generator upon item initialization)
//##
//####################################################################################

// Returns a pointer to the Base DrSettings class of the item with the specified key
DrSettings* DrProject::findSettingsFromKey(long check_key, bool show_warning, QString custom_error) {
    AnimationMap::iterator animation_iter = m_animations.find(check_key);
    if (animation_iter != m_animations.end())   return animation_iter->second;

    AssetMap::iterator asset_iter = m_assets.find(check_key);
    if (asset_iter != m_assets.end())           return asset_iter->second;

    ImageMap::iterator image_iter = m_images.find(check_key);
    if (image_iter != m_images.end())           return image_iter->second;

    DeviceMap::iterator device_iter = m_devices.find(check_key);
    if (device_iter != m_devices.end())         return device_iter->second;

    EffectMap::iterator effect_iter = m_effects.find(check_key);
    if (effect_iter != m_effects.end())         return effect_iter->second;

    FontMap::iterator font_iter = m_fonts.find(check_key);
    if (font_iter != m_fonts.end())             return font_iter->second;

    WorldMap &worlds = m_worlds;
    WorldMap::iterator world_iter = worlds.find(check_key);
    if (world_iter != worlds.end())             return world_iter->second;

    for (auto world_pair : worlds) {
        StageMap &stages = world_pair.second->getStageMap();
        StageMap::iterator stage_iter = stages.find(check_key);
        if (stage_iter != stages.end())         return stage_iter->second;

        for (auto stage_pair : stages) {
            ThingMap &things = stage_pair.second->getThingMap();
            ThingMap::iterator thing_iter = things.find(check_key);
            if (thing_iter != things.end())     return thing_iter->second;
        }
    }

    if (show_warning) {
        if (custom_error == "") custom_error = "No more info available...";
        Dr::ShowMessageBox("WARNING: Did not find key (" + QString::number(check_key) +
                           ") in project! \n"
                           "Last key used in project: " + QString::number(m_key_generator - 1) + "!\n\n"
                           "This warning called from \"DrProject::findChildSettingsFromKey\"" + "\n\n" + custom_error,
                           QMessageBox::Icon::Warning, "Warning!", Dr::GetActiveFormMain());
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

DrAnimation* DrProject::findAnimationFromKey(long check_key) {
    AnimationMap::iterator animation_iter = m_animations.find(check_key);
    if (animation_iter != m_animations.end())
        return animation_iter->second;
    else
        return nullptr;
}

DrAsset* DrProject::findAssetFromKey(long check_key) {
    AssetMap::iterator asset_iter = m_assets.find(check_key);
    if (asset_iter != m_assets.end())
        return asset_iter->second;
    else
        return nullptr;
}

DrDevice* DrProject::findDeviceFromKey(long check_key) {
    DeviceMap::iterator device_iter = m_devices.find(check_key);
    if (device_iter != m_devices.end())
        return device_iter->second;
    else
        return nullptr;
}

DrDevice* DrProject::findDeviceFromType(DrDeviceType type) {
    for (auto device_pair : m_devices) {
        if (device_pair.second->getDeviceType() == type) {
            return device_pair.second;
        }
    }
    return nullptr;
}

DrEffect* DrProject::findEffectFromKey(long check_key) {
    EffectMap::iterator effect_iter = m_effects.find(check_key);
    if (effect_iter != m_effects.end())
        return effect_iter->second;
    else
        return nullptr;
}

DrEffect* DrProject::findEffectFromType(DrEffectType type) {
    for (auto effect_pair : m_effects) {
        if (effect_pair.second->getEffectType() == type) {
            return effect_pair.second;
        }
    }
    return nullptr;
}

DrFont* DrProject::findFontFromKey(long check_key) {
    FontMap::iterator font_iter = m_fonts.find(check_key);
    if (font_iter != m_fonts.end())
        return font_iter->second;
    else
        return nullptr;
}

DrImage* DrProject::findImageFromKey(long check_key) {
    ImageMap::iterator image_iter = m_images.find(check_key);
    if (image_iter != m_images.end())
        return image_iter->second;
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















