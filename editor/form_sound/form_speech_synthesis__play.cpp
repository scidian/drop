//
//      Created by Stephens Nunnally on 6/4/2020, (c) 2020 Scidian Software, All Rights Reserved
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
#include "3rd_party/soloud/soloud_speech.h"
#include "3rd_party/soloud/soloud_wav.h"
#include "core/colors/colors.h"
#include "core/dr_random.h"
#include "editor/event_filters/event_filters.h"
#include "editor/form_sound/form_speech_synthesis.h"
#include "editor/form_sound/visualizer.h"
#include "editor/form_sound/wave_form.h"
#include "editor/helper_library.h"
#include "editor/interface_editor_relay.h"
#include "editor/preferences.h"
#include "editor/style/style.h"
#include "project/dr_project.h"


//####################################################################################
//##    Speech
//####################################################################################
void FormSpeechSynthesis::playSpeech(std::string speech_text) {
    m_speech.setText(speech_text.data());

    m_speech.setParams( m_speech_slider_freq->value(),
                        static_cast<float>(m_speech_slider_speed->value()) / 10.0,
                        static_cast<float>(m_speech_slider_decline->value()) / 100.0, m_speech_waveform );

    m_so_loud->play(m_speech);
    m_visual_timer->start();
}


//####################################################################################
//##    Sound Effect
//####################################################################################
void FormSpeechSynthesis::playSfxr(SoLoud::Sfxr::SFXR_PRESETS preset, int seed) {

    ///m_effect.loadPreset(preset, seed);
    ///m_so_loud->play(m_effect);

    SoLoud::Sfxr *effect = new SoLoud::Sfxr();
                  effect->loadPreset(preset, seed);
    long effect_key = getNextKey();
    m_effects[effect_key] = effect;
    m_so_loud->play(*m_effects[effect_key]);
    m_visual_timer->start();

    QString item_text = stringFromEffectType(preset) + QString::number(effect_key);
    QListWidgetItem *item = new QListWidgetItem(item_text);
    item->setData(User_Roles::Key, QVariant::fromValue(effect_key));
    m_list->addItem(item);
    item->setSelected(true);
}


//####################################################################################
//##    Wav File
//####################################################################################
void FormSpeechSynthesis::playWav(std::string wav_file) {
    m_wave.load(wav_file.data());
    m_so_loud->play(m_wave);
    m_visual_timer->start();
}




//####################################################################################
//##    Play Sounds from Containers
//####################################################################################
SoLoud::Sfxr* FormSpeechSynthesis::getEffect(long effect_key) {
    auto it = m_effects.find(effect_key);
    if (it != m_effects.end()) return it->second; else return nullptr;
}


void FormSpeechSynthesis::drawItem() {
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

void FormSpeechSynthesis::playItem(QListWidgetItem *item) {
    long sound_key = item->data(User_Roles::Key).toInt();
    SoLoud::Sfxr* sound_effect = getEffect(sound_key);
    if (sound_effect != nullptr) {
        m_so_loud->play(*sound_effect);
        m_visual_timer->start();
    }
}


QString FormSpeechSynthesis::stringFromEffectType(SoLoud::Sfxr::SFXR_PRESETS preset) {
    switch (preset) {
        case (SoLoud::Sfxr::SFXR_PRESETS::BLIP):        return "Blip";
        case (SoLoud::Sfxr::SFXR_PRESETS::COIN):        return "Coin";
        case (SoLoud::Sfxr::SFXR_PRESETS::HURT):        return "Hurt";
        case (SoLoud::Sfxr::SFXR_PRESETS::JUMP):        return "Jump";
        case (SoLoud::Sfxr::SFXR_PRESETS::LASER):       return "Laser";
        case (SoLoud::Sfxr::SFXR_PRESETS::POWERUP):     return "PowerUp";
        case (SoLoud::Sfxr::SFXR_PRESETS::EXPLOSION):   return "Explosion";
    }
}



















