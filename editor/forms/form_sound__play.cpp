//
//      Created by Stephens Nunnally on 5/24/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "3rd_party/soloud/soloud.h"
#include "3rd_party/soloud/soloud_sfxr.h"
#include "3rd_party/soloud/soloud_speech.h"
#include "3rd_party/soloud/soloud_wav.h"
#include "core/dr_random.h"
#include "editor/event_filters.h"
#include "editor/forms/form_color_magnifier.h"
#include "editor/forms/form_sound.h"
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

    m_so_loud.play(m_speech);
}


//####################################################################################
//##    Sound Effect
//####################################################################################
void FormSound::playSfxr(SoLoud::Sfxr::SFXR_PRESETS preset, int seed) {
    ///m_effect.setVolume(100);
    m_effect.loadPreset(preset, seed);

    m_so_loud.play(m_effect);
}


//####################################################################################
//##    Wav File
//####################################################################################
void FormSound::playWav(std::string wav_file) {
    m_wave.load(wav_file.data());
    m_so_loud.play(m_wave);
}














