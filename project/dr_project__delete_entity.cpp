//
//      Created by Stephens Nunnally on 5/21/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//      Functions to Delete different type of Entities from DrProject
//
//
#include "project/dr_project.h"
#include "project/entities/dr_animation.h"
#include "project/entities/dr_font.h"
#include "project/entities/dr_image.h"
#include "project/entities/dr_music.h"
#include "project/entities/dr_sound.h"
#include "project/entities/dr_stage.h"
#include "project/entities/dr_thing.h"
#include "project/entities/dr_world.h"
#include "project/entities_physics_2d/dr_asset.h"


//####################################################################################
//##    Delete Entity
//####################################################################################
void DrProject::deleteEntity(long entity_key) {
    DrSettings *entity = findSettingsFromKey(entity_key);
    if (entity == nullptr) return;
    switch (entity->getType()) {
        case DrType::Asset:     deleteAsset(entity_key);    break;
        case DrType::Font:      deleteFont(entity_key);     break;
        case DrType::Image:     deleteImage(entity_key);    break;
        case DrType::Music:     deleteMusic(entity_key);    break;
        case DrType::Sound:     deleteSound(entity_key);    break;
        case DrType::World:     deleteWorld(entity_key);    break;
    }
}


//####################################################################################
//##    Delete Animation
//##        Removes an Animation from the Project if not used by another Asset
//####################################################################################
void DrProject::deleteAnimation(long animation_key_to_delete, long ignore_asset_key) {
    if (animation_key_to_delete < c_starting_key_entity) return;
    DrAnimation *animation = findAnimationFromKey(animation_key_to_delete);
    if (animation == nullptr) return;

    // See if Animations are used by any other Asset
    for (auto &asset_pair : getAssetMap()) {
        if (asset_pair.first == ignore_asset_key) continue;
        if (asset_pair.second == nullptr) continue;

        // If key is used by another Asset, don't delete
        for (auto &check_key : asset_pair.second->animationsUsedByEntity()) {
            if (check_key == animation_key_to_delete) return;
        }
    }

    // See if Animations are used by any other Thing
    for (auto &world_pair : getWorldMap()) {
        for (auto &stage_pair : world_pair.second->getStageMap()) {
            for (auto &thing_pair : stage_pair.second->getThingMap()) {
                if (thing_pair.first == ignore_asset_key) continue;
                if (thing_pair.second == nullptr) continue;

                // If key is used by another Thing, don't delete
                for (auto &check_key : thing_pair.second->animationsUsedByEntity()) {
                    if (check_key == animation_key_to_delete) return;
                }
            }

        }
    }

    // Delete all Images in Animation
    for (auto frame : animation->getFrames()) {
        long image_key = frame->getKey();
        if  (image_key < c_starting_key_entity) continue;

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


//####################################################################################
//##    Delete Asset
//####################################################################################
void DrProject::deleteAsset(long asset_key) {
    DrAsset *asset = findAssetFromKey(asset_key);
    if (asset == nullptr) return;
    asset->deleteAnimations();
    m_assets.erase(asset_key);
    delete asset;
}


//####################################################################################
//##    Delete Font
//####################################################################################
void DrProject::deleteFont(long font_key) {
    DrFont *font = findFontFromKey(font_key);
    if (font == nullptr) return;
    m_fonts.erase(font_key);
    delete font;
}

//####################################################################################
//##    Delete Image
//####################################################################################
void DrProject::deleteImage(long image_key) {
    if (image_key < c_starting_key_entity) return;
    DrImage *image = findImageFromKey(image_key);
    if (image == nullptr) return;
    m_images.erase(image_key);
    delete image;
}

//####################################################################################
//##    Delete Music
//####################################################################################
void DrProject::deleteMusic(long music_key) {
    DrMusic *music = findMusicFromKey(music_key);
    if (music == nullptr) return;
    m_music.erase(music->getKey());
    delete music;
}

//####################################################################################
//##    Delete Sound
//####################################################################################
void DrProject::deleteSound(long sound_key) {
    DrSound *sound = findSoundFromKey(sound_key);
    if (sound == nullptr) return;
    m_sounds.erase(sound->getKey());
    delete sound;
}

//####################################################################################
//##    Delete World
//####################################################################################
void DrProject::deleteWorld(long world_key) {
    DrWorld *world = findWorldFromKey(world_key);
    if (world == nullptr) return;
    m_worlds.erase(world->getKey());
    delete world;
}






















