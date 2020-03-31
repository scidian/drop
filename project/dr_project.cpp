//
//      Created by Stephens Nunnally on 12/7/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      DrProject Class Definitions
//
//
#include "core/dr_debug.h"
#include "core/dr_containers.h"
#include "core/imaging/imaging.h"
#include "core/interface/dr_progress.h"
#include "project/constants_component_info.h"
#include "project/dr_project.h"
#include "project/entities/dr_animation.h"
#include "project/entities/dr_asset.h"
#include "project/entities/dr_device.h"
#include "project/entities/dr_effect.h"
#include "project/entities/dr_font.h"
#include "project/entities/dr_image.h"
#include "project/entities/dr_item.h"
#include "project/entities/dr_prefab.h"
#include "project/entities/dr_stage.h"
#include "project/entities/dr_thing.h"
#include "project/entities/dr_world.h"


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

    // Clear Data Maps
    for (auto it = m_options.begin();       it != m_options.end(); )    {   it = m_options.erase(it);   }

    for (auto it = m_animations.begin();    it != m_animations.end(); ) {   delete it->second; it = m_animations.erase(it); }
    for (auto it = m_assets.begin();        it != m_assets.end(); )     {   delete it->second; it = m_assets.erase(it);     }
    for (auto it = m_devices.begin();       it != m_devices.end(); )    {   delete it->second; it = m_devices.erase(it);    }
    for (auto it = m_effects.begin();       it != m_effects.end(); )    {   delete it->second; it = m_effects.erase(it);    }
    for (auto it = m_fonts.begin();         it != m_fonts.end(); )      {   delete it->second; it = m_fonts.erase(it);      }
    for (auto it = m_images.begin();        it != m_images.end(); )     {   delete it->second; it = m_images.erase(it);     }
    for (auto it = m_items.begin();         it != m_items.end(); )      {   delete it->second; it = m_items.erase(it);      }
    for (auto it = m_prefabs.begin();       it != m_prefabs.end(); )    {   delete it->second; it = m_prefabs.erase(it);    }
    for (auto it = m_worlds.begin();        it != m_worlds.end(); )     {   delete it->second; it = m_worlds.erase(it);     }

    // Add these Images to every project for use with New Assets
    if (add_built_in_items) {
        addDefaultAssets();
    }

    // !!!!! #NOTE: Don't allow key to start at less than 1, having an item with key 0 could conflict with nullptr results
    //              (starts at 1001)
    m_key_generator = c_key_starting_number;
}


//####################################################################################
//##    Adds Default Assets to Project
//####################################################################################
void DrProject::addDefaultAssets() {

    // ***** Add Effects
    if (findEffectFromType(DrEffectType::Light) == nullptr)     this->addEffect(DrEffectType::Light,        c_key_effect_light);
    if (findEffectFromType(DrEffectType::Water) == nullptr)     this->addEffect(DrEffectType::Water,        c_key_effect_water);
    if (findEffectFromType(DrEffectType::Fire) == nullptr)      this->addEffect(DrEffectType::Fire,         c_key_effect_fire);
    if (findEffectFromType(DrEffectType::Mirror) == nullptr)    this->addEffect(DrEffectType::Mirror,       c_key_effect_mirror);
    if (findEffectFromType(DrEffectType::Fisheye) == nullptr)   this->addEffect(DrEffectType::Fisheye,      c_key_effect_fisheye);
    if (findEffectFromType(DrEffectType::Swirl) == nullptr)     this->addEffect(DrEffectType::Swirl,        c_key_effect_swirl);

    // ***** Add Devices
    if (findDeviceFromType(DrDeviceType::Camera) == nullptr)    this->addDevice(DrDeviceType::Camera,       c_key_device_camera);

    // ***** Add Items
    if (findItemFromType(DrItemType::Tile) == nullptr)          this->addItem(DrItemType::Tile,             c_key_item_tile);

    // ***** Add Prefabs
    if (findPrefabFromType(DrPrefabType::Blob) == nullptr)      this->addPrefab(DrPrefabType::Blob,         c_key_prefab_blob);
    if (findPrefabFromType(DrPrefabType::Character) == nullptr) this->addPrefab(DrPrefabType::Character,    c_key_prefab_character);
    if (findPrefabFromType(DrPrefabType::Foliage) == nullptr)   this->addPrefab(DrPrefabType::Foliage,      c_key_prefab_foliage);
    if (findPrefabFromType(DrPrefabType::Ground) == nullptr)    this->addPrefab(DrPrefabType::Ground,       c_key_prefab_ground);
    if (findPrefabFromType(DrPrefabType::Ladder) == nullptr)    this->addPrefab(DrPrefabType::Ladder,       c_key_prefab_ladder);
    if (findPrefabFromType(DrPrefabType::Object) == nullptr)    this->addPrefab(DrPrefabType::Object,       c_key_prefab_object);
    if (findPrefabFromType(DrPrefabType::Spike) == nullptr)     this->addPrefab(DrPrefabType::Spike,        c_key_prefab_spike);
    if (findPrefabFromType(DrPrefabType::Spring) == nullptr)    this->addPrefab(DrPrefabType::Spring,       c_key_prefab_spring);
}


//####################################################################################
//##    Delete
//##        Functions to Delete different item types into project
//####################################################################################
// Removes an Animation from the Project if not used by another Asset
void DrProject::deleteAnimation(long animation_key_to_delete, long ignore_asset_key) {
    if (animation_key_to_delete < c_key_starting_number) return;
    DrAnimation *animation = findAnimationFromKey(animation_key_to_delete);
    if (animation == nullptr) return;

    // See if Animations are used by any other Asset
    for (auto &asset_pair : getAssetMap()) {
        if (asset_pair.first == ignore_asset_key) continue;
        if (asset_pair.second == nullptr) continue;

        // If key is used by another Asset, don't delete
        for (auto &check_key : asset_pair.second->animationsUsedByAsset()) {
            if (check_key == animation_key_to_delete) return;
        }
    }

    // Delete all Images in Animation
    for (auto frame : animation->getFrames()) {
        long image_key = frame->getKey();
        if  (image_key < c_key_starting_number) continue;

        // See if Image is used by any other Animaiton
        bool another_is_using_image = false;
        for (auto &animation_pair : getAnimationMap()) {
            if (animation_pair.first != animation_key_to_delete) {
                for (auto check_frame : animation_pair.second->getFrames()) {
                    if (check_frame->getKey() == image_key) {
                        another_is_using_image = true;
                        break;
                    }
                }
            }
            if (another_is_using_image) break;
        }

        // If no other Animation is using Image, delete it
        if (another_is_using_image == false) {
            ///Dr::PrintDebug("Deleting image key: " + std::to_string(image_key));
            deleteImage( image_key );
        }
    }

    // Delete Animation
    m_animations.erase(animation_key_to_delete);
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
    if (image_key < c_key_starting_number) return;
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
DrAnimation* DrProject::addAnimation(std::list<long> source_image_keys, long key) {
    long new_animation_key = (key == c_no_key) ? getNextKey() : key;
    m_animations[new_animation_key] = new DrAnimation(this, new_animation_key, source_image_keys);
    return m_animations[new_animation_key];
}

DrAsset* DrProject::addAsset(DrAssetType new_asset_type, long source_image_key, long key) {
    long new_asset_key = (key == c_no_key) ? getNextKey() : key;
    m_assets[new_asset_key] = new DrAsset(this, new_asset_key, new_asset_type, source_image_key);
    if (new_asset_key <= 0) {
        Dr::PrintDebug("DrProject::addAsset() invalid key!!! Key: " + std::to_string(new_asset_key) + ", Type: " + Dr::StringFromAssetType(new_asset_type) );
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

long DrProject::addFont(std::string font_name, DrBitmap font_bitmap, std::string font_family, int font_size, bool use_test_rects, long key) {
    long new_font_key = (key == c_no_key) ? getNextKey() : key;
    m_fonts[new_font_key] = new DrFont(this, new_font_key, font_name, font_bitmap, font_family, font_size, use_test_rects);
    return new_font_key;
}

DrImage* DrProject::addImage(std::string image_name, DrBitmap &bitmap, Asset_Category category, long key, IProgressBar *progress) {
    // Check if copy of this Bitmap already exists in DrImage map, if so return that DrImage
    for (auto &image_pair : getImageMap()) {
        DrImage *dr_image = image_pair.second;
        if (dr_image->getSimplifiedName() == image_name && dr_image->getKey() >= c_key_starting_number) {
            if (Dr::CompareBitmaps(bitmap, dr_image->getBitmap())) {
                if (progress != nullptr) {
                    progress->unlockProgress();
                    progress->moveToNextItem(); progress->updateValue(100.0);
                    progress->moveToNextItem(); progress->updateValue(100.0);
                }
                return dr_image;
            }
        }
    }

    // Create new DrImage with Bitmap and add to Project
    long new_image_key = (key == c_no_key) ? getNextKey() : key;
    m_images[new_image_key] = new DrImage(this, new_image_key, image_name, bitmap, category, false, progress);
    return m_images[new_image_key];
}

long DrProject::addItem(DrItemType item_type, long key) {
    long new_item_key = (key == c_no_key) ? getNextKey() : key;
    m_items[new_item_key] = new DrItem(this, new_item_key, item_type);
    return new_item_key;
}

long DrProject::addPrefab(DrPrefabType prefab_type, long key) {
    long new_prefab_key = (key == c_no_key) ? getNextKey() : key;
    m_prefabs[new_prefab_key] = new DrPrefab(this, new_prefab_key, prefab_type);
    return new_prefab_key;
}

// Adds a World to the map container, finds next available "World xxx" name to assign to World
DrWorld* DrProject::addWorld() {
    int test_num = static_cast<int>(m_worlds.size());
    std::string new_name;
    do {
        ++test_num;
        new_name = "World " + std::to_string(test_num);
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
DrWorld* DrProject::addWorldCopyFromWorld(DrWorld* from_world, std::string new_name) {
    DrWorld *copy_world = addWorld();
    copy_world->copyEntitySettings(from_world);

    // Set new name of copy World
    copy_world->setName( new_name );

    // Copy all Stages from World
    int stage_count = 0;
    for (auto &stage_pair : from_world->getStageMap()) {
        DrStage *stage = stage_pair.second;
        DrStage *copy_stage = copy_world->addStageCopyFromStage(stage, stage->getName(), (stage_count == 0));
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
DrSettings* DrProject::findSettingsFromKey(long check_key, bool show_warning, std::string custom_error) {
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

    ItemMap::iterator item_iter = m_items.find(check_key);
    if (item_iter != m_items.end())             return item_iter->second;

    PrefabMap::iterator prefab_iter = m_prefabs.find(check_key);
    if (prefab_iter != m_prefabs.end())         return prefab_iter->second;

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
        Dr::PrintDebug("WARNING: Did not find key (" + std::to_string(check_key) + ") in project! \n"
                       "Last key used in project: " + std::to_string(m_key_generator - 1) + "!\n"
                       "This warning called from \"DrProject::findChildSettingsFromKey\"" + "\n" +
                       custom_error + " - End Error.....");
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

DrItem* DrProject::findItemFromKey(long check_key) {
    ItemMap::iterator item_iter = m_items.find(check_key);
    if (item_iter != m_items.end())
        return item_iter->second;
    else
        return nullptr;
}

DrItem* DrProject::findItemFromType(DrItemType type) {
    for (auto item_pair : m_items) {
        if (item_pair.second->getItemType() == type) {
            return item_pair.second;
        }
    }
    return nullptr;
}

DrPrefab* DrProject::findPrefabFromKey(long check_key) {
    PrefabMap::iterator prefab_iter = m_prefabs.find(check_key);
    if (prefab_iter != m_prefabs.end())
        return prefab_iter->second;
    else
        return nullptr;
}

DrPrefab* DrProject::findPrefabFromType(DrPrefabType type) {
    for (auto prefab_pair : m_prefabs) {
        if (prefab_pair.second->getPrefabType() == type) {
            return prefab_pair.second;
        }
    }
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
DrWorld* DrProject::findWorldWithName(std::string world_name) {
    std::string compare_name;
    for (auto world_pair: m_worlds) {
        compare_name = world_pair.second->getName();
        if (compare_name == world_name) { return world_pair.second; }
    }
    return nullptr;
}
















