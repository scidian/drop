//
//      Created by Stephens Nunnally on 5/21/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//      Functions to Add different item types into DrProject
//
//
#include "core/dr_containers.h"
#include "core/dr_debug.h"
#include "core/imaging/imaging.h"
#include "core/interface/dr_progress.h"
#include "project/dr_project.h"
#include "project/entities/dr_animation.h"
#include "project/entities/dr_font.h"
#include "project/entities/dr_image.h"
#include "project/entities/dr_stage.h"
#include "project/entities/dr_thing.h"
#include "project/entities/dr_world.h"
#include "project/entities_physics_2d/dr_asset.h"
#include "project/entities_physics_2d/dr_device.h"
#include "project/entities_physics_2d/dr_effect.h"
#include "project/entities_physics_2d/dr_item.h"
#include "project/entities_physics_2d/dr_prefab.h"
#include "project/entities_sound/dr_mix.h"
#include "project/entities_sound/dr_sound.h"
#include "project/entities_sound/dr_track.h"
#include "project/settings/settings_component.h"


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

DrImage* DrProject::addImage(std::string image_name, DrBitmap &bitmap, long key, bool outline, IProgressBar *progress) {
    // Check if copy of this Bitmap already exists in DrImage map, if so return that DrImage
    for (auto &image_pair : getImageMap()) {
        DrImage *dr_image = image_pair.second;
        if (dr_image->getSimplifiedName() == image_name && dr_image->getKey() >= c_starting_key_entity) {
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
    m_images[new_image_key] = new DrImage(this, new_image_key, image_name, bitmap, outline, progress);
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

DrMix* DrProject::addMix(long key) {
    int number_of_mixes = static_cast<int>(m_mixes.size());
    std::string new_name;
    do {
        ++number_of_mixes;
        new_name = "Mix " + std::to_string(number_of_mixes);
    } while (findMixWithName(new_name) != nullptr);
    long new_mix_key = (key == c_no_key) ? getNextKey() : key;
    m_mixes[new_mix_key] = new DrMix(this, new_mix_key, new_name);
    return m_mixes[new_mix_key];
}

// Adds a new Mix, copied from another Mix
DrMix* DrProject::addMixCopyFromMix(DrMix* from_mix, std::string new_name) {
    // Make Copy
    DrMix *copy_mix = addMix(c_no_key);
           copy_mix->copyEntitySettings(from_mix);
           copy_mix->setName( new_name );

    // Copy all Tracks from Mix
    int track_count = 0;
    for (auto &track_pair : from_mix->getTrackMap()) {
        // Make Copy
        DrTrack *track = track_pair.second;
        copy_mix->addTrackCopyFromTrack(track);

        track_count++;
    }
    return copy_mix;
}

DrSound* DrProject::addSound(DrSoundType sound_type, SoLoud::AudioSource *audio_source, long key, std::string new_name) {
    long new_sound_key = (key == c_no_key) ? getNextKey() : key;
    m_sounds[new_sound_key] = new DrSound(this, sound_type, audio_source, new_sound_key, new_name);
    return m_sounds[new_sound_key];
}

// Adds a World to the map container, finds next available "World xxx" name to assign to World
DrWorld* DrProject::addWorld(DrWorldType world_type) {
    int number_of_worlds = static_cast<int>(m_worlds.size());
    std::string new_name;
    do {
        ++number_of_worlds;
        new_name = "World " + std::to_string(number_of_worlds);
    } while (findWorldWithName(new_name) != nullptr);

    long new_world_key = getNextKey();
    bool add_start_stage = true;
    m_worlds[new_world_key] = new DrWorld(this, new_world_key, world_type, new_name, add_start_stage);
    return m_worlds[new_world_key];
}

// Adds a World to the map container, !!!!! DOES NOT CREATE Start Stage
DrWorld* DrProject::addWorld(DrWorldType world_type, long key, long start_stage_key, long last_stage_in_editor_key) {
    bool add_start_stage = false;
    m_worlds[key] = new DrWorld(this, key, world_type, "TEMP", add_start_stage);
    m_worlds[key]->setStartStageKey(start_stage_key);
    m_worlds[key]->setLastStageShownKey(last_stage_in_editor_key);
    return m_worlds[key];
}

// Adds a new World, copied from another World
DrWorld* DrProject::addWorldCopyFromWorld(DrWorld* from_world, std::string new_name) {
    // Make Copy
    DrWorld *copy_world = addWorld(from_world->getWorldType());
             copy_world->copyEntitySettings(from_world);
             copy_world->setName( new_name );

    // Copy non component-property variables of DrWorld class
    copy_world->setExpanded(from_world->getExpanded());

    // Copy all Stages from World
    int stage_count = 0;
    for (auto &stage_pair : from_world->getStageMap()) {
        // Make Copy
        DrStage *stage = stage_pair.second;
        DrStage *copy_stage = copy_world->addStageCopyFromStage(stage, stage->getName(), (stage_count == 0));

        // Copy non component-property variables of DrStage class
        copy_stage->setExpanded(stage->getExpanded());

        stage_count++;
    }
    return copy_world;
}




















