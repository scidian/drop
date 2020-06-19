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
#include "core/sound.h"
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
    // Check that m_speech has been initialized
    if (m_speech == nullptr) m_speech = new SoLoud::Speech();

    // Set text / parameters
    m_speech_text = speech_text;
    m_speech->setVolume(2.0);
    m_speech->setText(speech_text.data());
    m_speech->setParams(m_speech_slider_freq->value(),
                        static_cast<float>(m_speech_slider_speed->value()) / 10.0,
                        static_cast<float>(m_speech_slider_decline->value()) / 100.0, m_speech_waveform);

    // Draw wave form
    m_sound_wave->setSound(m_speech);
    m_sound_wave->repaint();

    // Play sound, start visualizer
    Dr::GetSoLoud()->play(*m_speech);
    m_visual_timer->start();
}


//####################################################################################
//##    Wav File
//####################################################################################
void FormSpeechSynthesis::playWav(std::string wav_file) {
    m_wave.load(wav_file.data());
    Dr::GetSoLoud()->play(m_wave);
    m_visual_timer->start();
}




















