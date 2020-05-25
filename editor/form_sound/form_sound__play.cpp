//
//      Created by Stephens Nunnally on 5/25/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QDebug>

#include "3rd_party/soloud/soloud.h"
#include "3rd_party/soloud/soloud_audiosource.h"
#include "3rd_party/soloud/soloud_sfxr.h"
#include "3rd_party/soloud/soloud_speech.h"
#include "3rd_party/soloud/soloud_wav.h"
#include "core/dr_random.h"
#include "editor/event_filters/event_filters.h"
#include "editor/form_sound/form_sound.h"
#include "editor/forms/form_color_magnifier.h"
#include "editor/helper_library.h"
#include "editor/interface_editor_relay.h"
#include "editor/preferences.h"
#include "editor/style/style.h"
#include "project/dr_project.h"


//####################################################################################
//##    Speech
//####################################################################################
void FormSound::playSpeech(std::string speech_text) {
    m_speech.setText(speech_text.data());

    m_speech.setParams( m_speech_slider_freq->value(),
                        static_cast<float>(m_speech_slider_speed->value()) / 10.0,
                        static_cast<float>(m_speech_slider_decline->value()) / 100.0, m_speech_waveform );

    m_so_loud->play(m_speech);
}


//####################################################################################
//##    Sound Effect
//####################################################################################
void FormSound::playSfxr(SoLoud::Sfxr::SFXR_PRESETS preset, int seed) {

    ///m_effect.loadPreset(preset, seed);
    ///m_so_loud->play(m_effect);

    SoLoud::Sfxr *effect = new SoLoud::Sfxr();
                  effect->loadPreset(preset, seed);
    long effect_key = getNextKey();
    m_effects[effect_key] = effect;
    m_so_loud->play(*m_effects[effect_key]);

    QListWidgetItem *item = new QListWidgetItem(stringFromEffectType(preset) + QString::number(effect_key));
    item->setData(User_Roles::Key, QVariant::fromValue(effect_key));
    m_list->addItem(item);

    // Looking into possible wav form loading
    ///SoLoud::AudioSourceInstance *asi = m_effect.createInstance();
    ///asi->getAudio(asi->mResampleData[0]->mData, SAMPLE_GRANULARITY, SAMPLE_GRANULARITY);
}


//####################################################################################
//##    Wav File
//####################################################################################
void FormSound::playWav(std::string wav_file) {
    m_wave.load(wav_file.data());
    m_so_loud->play(m_wave);
}




//####################################################################################
//##    Play Sounds from Containers
//####################################################################################
void FormSound::playEffect(long effect_key) {
    auto it = m_effects.find(effect_key);
    if (it != m_effects.end()) {
        m_so_loud->play(*m_effects[effect_key]);
    }
}

void FormSound::playItem(QListWidgetItem *item) {
    long sound_key = item->data(User_Roles::Key).toInt();

    auto it = m_effects.find(sound_key);
    if (it != m_effects.end()) {
        m_so_loud->play(*m_effects[sound_key]);
    }
}


QString FormSound::stringFromEffectType(SoLoud::Sfxr::SFXR_PRESETS preset) {
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















