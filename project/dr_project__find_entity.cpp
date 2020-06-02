//
//      Created by Stephens Nunnally on 5/21/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "core/dr_containers.h"
#include "core/dr_debug.h"
#include "core/imaging/imaging.h"
#include "core/interface/dr_progress.h"
#include "project/constants_component_info.h"
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
#include "project/settings/settings.h"
#include "project/settings/settings_component.h"
#include "project/settings/settings_component_slot.h"


//####################################################################################
//##
//##    Key Finding - These functions search the current project for the specified Key and return the requested info
//##                  (key's are generated from Project key generator upon item initialization)
//##
//####################################################################################
// Searches all member variables / containers for the specified unique project key
DrType DrProject::findSettingsTypeFromKey(long check_key) {
    DrSettings *entity = findSettingsFromKey(check_key);
    return ((entity == nullptr) ? DrType::NotFound : entity->getType());
}

// Searches for DrSlot
DrSlot* DrProject::findSlotFromKeys(long entity_key, long component_key, long slot_key) {
    DrSettings *entity = findSettingsFromKey(entity_key);
    if (entity != nullptr) {
        DrComponent *component = entity->getComponent(component_key);
        if (component != nullptr) {
            for (auto &signal_pair : component->getSignalMap()) {
                if (signal_pair.second->getSlotKey() == slot_key) return signal_pair.second;
            }
            for (auto &output_pair : component->getOutputMap()) {
                if (output_pair.second->getSlotKey() == slot_key) return output_pair.second;
            }
        }
    }
    return nullptr;
}

// Returns a pointer to the Base DrSettings class of the item with the specified Key and Type
DrSettings* DrProject::findSettingsFromKeyOfType(long check_key, DrType type) {
    switch (type) {
        case DrType::Animation:     return findAnimationFromKey(check_key);
        case DrType::Asset:         return findAssetFromKey(check_key);
        case DrType::Block:         /* NEED IMPLEMENT */    break;
        case DrType::Device:        return findDeviceFromKey(check_key);
        case DrType::Effect:        return findEffectFromKey(check_key);
        case DrType::Font:          return findFontFromKey(check_key);
        case DrType::Frame:         /* NEED IMPLEMENT - !!!!! #TEMP DrFrames currently use DrImage key */    break;
        case DrType::Image:         return findImageFromKey(check_key);
        case DrType::Item:          return findItemFromKey(check_key);
        case DrType::Mix:           return findMixFromKey(check_key);
        case DrType::NotFound:      return findSettingsFromKey(check_key);
        case DrType::Prefab:        return findPrefabFromKey(check_key);
        case DrType::Sound:         return findSoundFromKey(check_key);
        case DrType::Stage:         return findStageFromKey(check_key);
        case DrType::Thing:         return findThingFromKey(check_key);
        case DrType::Track:         /* NEED IMPLEMENT */    break;
        case DrType::World:         return findWorldFromKey(check_key);
    }
    return nullptr;
}

// Returns a pointer to the Base DrSettings class of the item with the specified Key
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

    MixMap::iterator mix_iter = m_mixes.find(check_key);
    if (mix_iter != m_mixes.end())              return mix_iter->second;

    SoundMap::iterator sound_iter = m_sounds.find(check_key);
    if (sound_iter != m_sounds.end())          return sound_iter->second;

    // Search Worlds, Stages and Things
    WorldMap::iterator world_iter = m_worlds.find(check_key);
    if (world_iter != m_worlds.end())           return world_iter->second;

    for (auto &world_pair : m_worlds) {
        StageMap &stages = world_pair.second->getStageMap();
        StageMap::iterator stage_iter = stages.find(check_key);
        if (stage_iter != stages.end())         return stage_iter->second;

        for (auto &stage_pair : stages) {
            ThingMap &things = stage_pair.second->getThingMap();
            ThingMap::iterator thing_iter = things.find(check_key);
            if (thing_iter != things.end())     return thing_iter->second;
        }
    }

//    // Search UIs and Widgets
//    UIMap::iterator ui_iter = m_uis.find(check_key);
//    if (ui_iter != m_uis.end())                 return ui_iter->second;
//    for (auto &ui_pair : m_uis) {
//        WidgetMap &widgets = ui_pair.second->getWidgetMap();
//        WidgetMap::iterator widget_iter = widgets.find(check_key);
//        if (widget_iter != widgets.end())       return widget_iter->second;
//    }

    // Couldnt find Entity with key value "check_key"
    if (show_warning) {
        if (custom_error == "") custom_error = "No more info available...";
        Dr::PrintDebug("WARNING: Did not find key (" + std::to_string(check_key) + ") in project! \n"
                       "Last key used in project: " + std::to_string(m_key_generator - 1) + "!\n"
                       "This warning called from \"DrProject::findChildSettingsFromKey\"" + "\n" +
                       custom_error + " - End Error.....");
    }
    return nullptr;
}


DrAnimation* DrProject::findAnimationFromKey(long check_key) {
    AnimationMap::iterator animation_iter = m_animations.find(check_key);
    return ((animation_iter != m_animations.end()) ? animation_iter->second : nullptr);
}

DrAsset* DrProject::findAssetFromKey(long check_key) {
    AssetMap::iterator asset_iter = m_assets.find(check_key);
    return ((asset_iter != m_assets.end()) ? asset_iter->second : nullptr);
}

DrDevice* DrProject::findDeviceFromKey(long check_key) {
    DeviceMap::iterator device_iter = m_devices.find(check_key);
    return ((device_iter != m_devices.end()) ? device_iter->second : nullptr);
}

DrEffect* DrProject::findEffectFromKey(long check_key) {
    EffectMap::iterator effect_iter = m_effects.find(check_key);
    return ((effect_iter != m_effects.end()) ? effect_iter->second : nullptr);
}

DrFont* DrProject::findFontFromKey(long check_key) {
    FontMap::iterator font_iter = m_fonts.find(check_key);
    return ((font_iter != m_fonts.end()) ? font_iter->second : nullptr);
}

DrImage* DrProject::findImageFromKey(long check_key) {
    ImageMap::iterator image_iter = m_images.find(check_key);
    return ((image_iter != m_images.end()) ? image_iter->second : nullptr);
}

DrItem* DrProject::findItemFromKey(long check_key) {
    ItemMap::iterator item_iter = m_items.find(check_key);
    return ((item_iter != m_items.end()) ? item_iter->second : nullptr);
}

DrPrefab* DrProject::findPrefabFromKey(long check_key) {
    PrefabMap::iterator prefab_iter = m_prefabs.find(check_key);
    return ((prefab_iter != m_prefabs.end()) ? prefab_iter->second : nullptr);
}

DrMix* DrProject::findMixFromKey(long check_key) {
    MixMap::iterator mix_iter = m_mixes.find(check_key);
    return ((mix_iter != m_mixes.end()) ? mix_iter->second : nullptr);
}

DrSound* DrProject::findSoundFromKey(long check_key) {
    SoundMap::iterator sound_iter = m_sounds.find(check_key);
    return ((sound_iter != m_sounds.end()) ? sound_iter->second : nullptr);
}

DrStage* DrProject::findStageFromKey(long check_key, long world_key) {
    // We know world key
    if (world_key != c_no_key) {
        DrWorld *search_world = findWorldFromKey(world_key);                if (search_world == nullptr) return nullptr;
        return search_world->getStageFromKey(check_key);
    // Don't know world key
    } else {
        for (auto &world_pair : m_worlds) {
            StageMap &stages = world_pair.second->getStageMap();
            StageMap::iterator stage_iter = stages.find(check_key);
            if (stage_iter != stages.end()) return stage_iter->second;
        }
    }
    return nullptr;
}

DrThing* DrProject::findThingFromKey(long check_key, long stage_key, long world_key) {
    // We know world and stage keys
    if (world_key != c_no_key && stage_key != c_no_key) {
        DrWorld *search_world = findWorldFromKey(world_key);                if (search_world == nullptr) return nullptr;
        DrStage *search_stage = search_world->getStageFromKey(stage_key);   if (search_stage == nullptr) return nullptr;
        return   search_stage->findThing(check_key);
    // We know world key
    } else if (world_key != c_no_key) {
        DrWorld *search_world = findWorldFromKey(world_key);                if (search_world == nullptr) return nullptr;
        for (auto &stage_pair : search_world->getStageMap()) {
            DrThing *thing = stage_pair.second->findThing(check_key);
            if (thing != nullptr) return thing;
        }
    // We know stage key
    } else if (stage_key != c_no_key) {
        for (auto &world_pair : m_worlds) {
            DrStage *search_stage = world_pair.second->getStageFromKey(stage_key);
            if (search_stage != nullptr) {
                DrThing *thing = search_stage->findThing(check_key);
                if (thing != nullptr) return thing;
            }
        }
    // Don't know world or stage key
    } else {
        for (auto &world_pair : m_worlds) {
            for (auto &stage_pair : world_pair.second->getStageMap()) {
                DrThing *thing = stage_pair.second->findThing(check_key);
                if (thing != nullptr) return thing;
            }
        }
    }
    return nullptr;
}

DrWorld* DrProject::findWorldFromKey(long check_key) {
    WorldMap::iterator world_iter = m_worlds.find(check_key);
    return ((world_iter != m_worlds.end()) ? world_iter->second : nullptr);
}

// Returns a pointer to the World with the mathcing name
DrWorld* DrProject::findWorldWithName(std::string world_name) {
    std::string compare_name;
    for (auto &world_pair: m_worlds) {
        compare_name = world_pair.second->getName();
        if (compare_name == world_name) { return world_pair.second; }
    }
    return nullptr;
}


//####################################################################################
//##    Finding by Type
//##        Specific Entity by Type for those entities that are singletons
//####################################################################################
DrDevice* DrProject::findDeviceFromType(DrDeviceType type) {
    for (auto &device_pair : m_devices) {
        if (device_pair.second->getDeviceType() == type) {
            return device_pair.second;
        }
    }
    return nullptr;
}

DrEffect* DrProject::findEffectFromType(DrEffectType type) {
    for (auto &effect_pair : m_effects) {
        if (effect_pair.second->getEffectType() == type) {
            return effect_pair.second;
        }
    }
    return nullptr;
}

DrItem* DrProject::findItemFromType(DrItemType type) {
    for (auto &item_pair : m_items) {
        if (item_pair.second->getItemType() == type) {
            return item_pair.second;
        }
    }
    return nullptr;
}

DrPrefab* DrProject::findPrefabFromType(DrPrefabType type) {
    for (auto &prefab_pair : m_prefabs) {
        if (prefab_pair.second->getPrefabType() == type) {
            return prefab_pair.second;
        }
    }
    return nullptr;
}


//####################################################################################
//##    Image Functions
//####################################################################################
// Build list of DrImage keys used by Project, using std::set to ensure no duplicates
std::set<long> DrProject::getImageKeysUsedByProject() {
    std::set<long> image_keys_used;
    image_keys_used.insert(c_key_image_empty);
    for (auto &animation_pair : getAnimationMap()) {
        for (auto &frame : animation_pair.second->getFrames()) {
            long image_key = frame->getKey();
            if (image_keys_used.find(image_key) == image_keys_used.end()) {
                image_keys_used.insert(image_key);
            }
        }
    }
    return image_keys_used;
}



















