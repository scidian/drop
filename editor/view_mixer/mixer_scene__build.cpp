//
//      Created by Stephens Nunnally on 6/2/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "editor/view_mixer/mixer_item.h"
#include "editor/view_mixer/mixer_scene.h"
#include "editor/view_mixer/mixer_view.h"
#include "project/dr_project.h"
#include "project/entities_sound/dr_mix.h"
#include "project/entities_sound/dr_sound.h"
#include "project/entities_sound/dr_track.h"


//####################################################################################
//##    Builds Scene from DrMix
//####################################################################################
void MixerScene::buildScene(long mix_key) {

    // Clear scene, set scene rect
    this->clearSceneOverride();
    this->setSceneRect(-2500, -2500, 5000, 5000);

    DrMix *mix = m_project->findMixFromKey(mix_key);

    // Add Tracks from DrMix if DrMix is valid
    if (mix != nullptr) {
        m_mixer_entity = mix->getKey();

        for (auto &track_pair : mix->getTrackMap()) {
            this->addItemToSceneFromTrack(track_pair.second);
        }

        m_project->setOption(Project_Options::Current_Mix, mix->getKey());
    }
}


//####################################################################################
//##    Builds Scene from DrProject Worlds
//####################################################################################
MixerItem* MixerScene::addItemToSceneFromTrack(DrTrack *track) {
    // Create new item representing this Thing
    MixerItem *graphics_item = new MixerItem(m_project, this->getEditorRelay(), track);

    // Temporarily disable geometry signal itemChange updates
    bool flags_enabled_before = graphics_item->itemChangeFlagsEnabled();
    graphics_item->disableItemChangeFlags();

    // Add item to scene, set starting position
    this->addItem(graphics_item);
    graphics_item->setPos(graphics_item->startPosition());

    // Create a temporary group and destroy it, this causes some unknown but important changes to the items
    // sceneTransform that we really seem to need before we try to move a scaled item
    QGraphicsItemGroup *group = this->createItemGroup({ graphics_item });
    this->destroyItemGroup(group);

    // Re enable geometry signal itemChange() updates
    if (flags_enabled_before) graphics_item->enableItemChangeFlags();

    return graphics_item;
}























