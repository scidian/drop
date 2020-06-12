//
//      Created by Stephens Nunnally on 5/25/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QDebug>
#include <QTimer>

#include "3rd_party/soloud/soloud.h"
#include "3rd_party/soloud/soloud_audiosource.h"
#include "3rd_party/soloud/soloud_sfxr.h"
#include "core/colors/colors.h"
#include "core/dr_random.h"
#include "core/sound.h"
#include "editor/event_filters/event_filters.h"
#include "editor/form_sound/form_sound_effect.h"
#include "editor/form_sound/visualizer.h"
#include "editor/form_sound/wave_form.h"
#include "editor/helper_library.h"
#include "editor/interface_editor_relay.h"
#include "editor/preferences.h"
#include "editor/style/style.h"
#include "project/dr_project.h"


//####################################################################################
//##    Sound Effect
//####################################################################################
void FormSoundEffect::playSfxr(SoundEffectType preset, int seed) {
    SoLoud::Sfxr *effect = new SoLoud::Sfxr();
                  effect->loadPreset(preset, seed);
    long effect_key = getNextKey();
    m_effects[effect_key] = effect;
    Dr::GetSoLoud()->play(*m_effects[effect_key]);
    drawVisuals();

    long item_count = m_type_counts[preset]++;
    QString item_text = stringFromEffectType(preset) + QString::number(item_count);
    QListWidgetItem *item = new QListWidgetItem(item_text);
    item->setData(User_Roles::Key,  QVariant::fromValue(effect_key));
    m_list->addItem(item);
    item->setSelected(true);

    m_selected_effect = effect_key;
}


//####################################################################################
//##    Play Sounds from Containers
//####################################################################################
SoLoud::Sfxr* FormSoundEffect::getEffect(long effect_key) {
    auto it = m_effects.find(effect_key);
    if (it != m_effects.end()) return it->second; else return nullptr;
}


void FormSoundEffect::drawItem() {
    if (m_list->selectedItems().size() > 0) {
        QListWidgetItem *item = m_list->selectedItems().first();
        long sound_key = item->data(User_Roles::Key).toInt();
        SoLoud::Sfxr* sound_effect = getEffect(sound_key);
        m_sound_wave->setSound(sound_effect);
    } else {
        m_sound_wave->setSound(nullptr);
    }
    m_sound_wave->repaint();
}

void FormSoundEffect::playItem(QListWidgetItem *item) {
    long sound_key = item->data(User_Roles::Key).toInt();
    m_selected_effect = sound_key;
    SoLoud::Sfxr* sound_effect = getEffect(sound_key);
    if (sound_effect != nullptr) {
        Dr::GetSoLoud()->play(*sound_effect);
        drawVisuals();
    }
}


QString FormSoundEffect::stringFromEffectType(SoundEffectType preset) {
    switch (preset) {
        case (SoundEffectType::BLIP):           return "Blip";
        case (SoundEffectType::COIN):           return "Coin";
        case (SoundEffectType::HURT):           return "Hurt";
        case (SoundEffectType::JUMP):           return "Jump";
        case (SoundEffectType::LASER):          return "Laser";
        case (SoundEffectType::POWERUP):        return "PowerUp";
        case (SoundEffectType::EXPLOSION):      return "Explosion";
    }
}
























