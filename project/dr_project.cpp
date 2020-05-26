//
//      Created by Stephens Nunnally on 12/7/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      DrProject Class Definitions
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
#include "project/entities/dr_sound.h"
#include "project/entities/dr_stage.h"
#include "project/entities/dr_thing.h"
#include "project/entities/dr_world.h"
#include "project/entities_physics_2d/dr_asset.h"
#include "project/entities_physics_2d/dr_device.h"
#include "project/entities_physics_2d/dr_effect.h"
#include "project/entities_physics_2d/dr_item.h"
#include "project/entities_physics_2d/dr_prefab.h"
#include "project/settings/settings_component.h"
#include "project/settings/settings_component_slot.h"


//####################################################################################
//##    Constructor / Destructor
//####################################################################################
DrProject::DrProject() {
    clearProject();
}

DrProject::~DrProject() {
    clearProject(false);
}

void DrProject::clearProject(bool add_built_in_items) {
    m_world_graph_shown = false;
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
    for (auto it = m_sounds.begin();        it != m_sounds.end(); )     {   delete it->second; it = m_sounds.erase(it);    }
    for (auto it = m_worlds.begin();        it != m_worlds.end(); )     {   delete it->second; it = m_worlds.erase(it);     }

    // Add these Images to every project for use with New Assets
    if (add_built_in_items) {
        addDefaultAssets();
    }

    // !!!!! #NOTE: Don't allow key to start at less than 1, having an item with key 0 (or less) could conflict with nullptr results
    //             (c_key_entity_starting_number currently starts at 1001)
    m_key_generator = c_starting_key_entity;
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

























