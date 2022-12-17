//
//      Created by Stephens Nunnally on 5/26/2020, (c) 2020 Scidian Studios, All Rights Reserved
//
//  File:
//
//
//
#include "editor/view_mixer/mixer_item.h"
#include "editor/view_mixer/mixer_scene.h"
#include "editor/view_mixer/mixer_view.h"
#include "project/entities_sound/dr_mix.h"
#include "project/entities_sound/dr_sound.h"
#include "project/entities_sound/dr_track.h"


//####################################################################################
//##    Constructor / Destructor
//####################################################################################
MixerScene::MixerScene(QWidget *parent, DrProject *project, IEditorRelay *editor_relay, long mixer_entity_key)
    : QGraphicsScene(parent), m_project(project), m_editor_relay(editor_relay), m_mixer_entity(mixer_entity_key) {

    connect(this, SIGNAL(selectionChanged()),       this, SLOT(selectionChanged()));

}

MixerScene::~MixerScene() { }

void MixerScene::clearSceneOverride() {
    this->clearSelection();
    this->clear();
}


//####################################################################################
//##    Return scene()->items() as list of MixerItem pointers
//####################################################################################
QList<MixerItem*> MixerScene::mixerItems() {
    QList<MixerItem*> mixer_items { };

    for (auto item : items()) {
        MixerItem *mixer_item = dynamic_cast<MixerItem*>(item);
        if (mixer_item != nullptr) mixer_items.push_back(mixer_item);
    }
    return mixer_items;
}

//####################################################################################
//##    Returns MixerItem from list with the designated key
//####################################################################################
MixerItem* MixerScene::mixerItemWithKey(QList<MixerItem*> &mixer_items, long entity_key) {
    if (entity_key == c_no_key) return nullptr;
    for (auto mixer_item : mixer_items) {
        if (mixer_item->getTrack()->getKey() == entity_key) return mixer_item;
    }
    return nullptr;
}

























