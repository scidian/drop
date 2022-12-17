//
//      Created by Stephens Nunnally on 5/25/2020, (c) 2020 Scidian Studios, All Rights Reserved
//
//  File:
//
//
//
#include <QAction>
#include <QDebug>
#include <QTimer>

#include "3rd_party/soloud/soloud.h"
#include "3rd_party/soloud/soloud_audiosource.h"
#include "3rd_party/soloud/soloud_sfxr.h"
#include "core/colors/colors.h"
#include "core/dr_random.h"
#include "core/dr_math.h"
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
//##    Effect Type Enum as a String
//####################################################################################
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

QString FormSoundEffect::stringFromWaveType(SoundEffectWaveType wave_type) {
    switch (wave_type) {
        case (SoundEffectWaveType::Square):     return "Square";
        case (SoundEffectWaveType::Sawtooth):   return "Sawtooth";
        case (SoundEffectWaveType::Sine):       return "Sine";
        case (SoundEffectWaveType::Noise):      return "Noise";
    }
}


//####################################################################################
//##    Create Sound Effect
//####################################################################################
void FormSoundEffect::generateSfxr(SoundEffectType preset, int seed) {
    // Generate Effect
    SoLoud::Sfxr *effect = new SoLoud::Sfxr();
                  effect->loadPreset(preset, seed);
    long effect_key = getNextKey();
    m_effects[effect_key] = effect;

    // Add List Widget Item
    long item_count = m_type_counts[preset]++;
    QString item_text = stringFromEffectType(preset) + QString::number(item_count);
    QListWidgetItem *item = new QListWidgetItem(item_text);
    item->setData(User_Roles::Key,  QVariant::fromValue(effect_key));           // Store effect key in widget user data
    m_list->addItem(item);

    // Setting as current fires list SIGNAL(currentItemChanged()) connected to this SLOT(playSelected())
    m_list->setCurrentItem(item);
}


//####################################################################################
//##    Play Sounds from Containers
//####################################################################################
SoLoud::Sfxr* FormSoundEffect::getEffect(long effect_key) {
    auto it = m_effects.find(effect_key);
    if (it != m_effects.end()) return it->second; else return nullptr;
}

void FormSoundEffect::playSelected() {
    // Get selected item underlying Sfxr AudioSource
    SoLoud::Sfxr* effect = getEffect(m_current_effect);

    // Play sound, update wave form
    if (effect != nullptr) {
        Dr::GetSoLoud()->play(*effect);
        updateSliders(effect);
        updateWaveType(static_cast<SoundEffectWaveType>(effect->mParams.wave_type));
        drawVisuals();
    }

    // Update Wave Form Graphic
    m_sound_wave->setSound(effect);
    m_sound_wave->repaint();
}

void FormSoundEffect::currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous) {
    if (current == nullptr) {
        m_current_effect = c_no_key;
        return;
    }

    if (current != previous) {
        long sound_key = current->data(User_Roles::Key).toInt();
        m_current_effect = sound_key;
        playSelected();
    }
}

// If item was clicked and it was already selected, play it again
void FormSoundEffect::itemClicked(QListWidgetItem *clicked) {
    if (clicked == nullptr) return;
    long was_selected = clicked->data(User_Roles::Bool).toBool();
    if (was_selected) playSelected();
    clicked->setData(User_Roles::Bool, true);
}
void FormSoundEffect::itemEntered(QListWidgetItem *entered) {
    if (entered == nullptr) return;
    entered->setData(User_Roles::Bool, entered->isSelected());
}


//####################################################################################
//##    Update Sliders / Selected Sopund Effect
//####################################################################################
void FormSoundEffect::updateSliders(SoLoud::Sfxr *effect) {
    m_slider_0->setValue(effect->mParams.p_base_freq        * 1000.0);      // Frequency
    m_slider_1->setValue(effect->mParams.p_freq_limit       * 1000.0);      // Frequency Cutoff
    m_slider_2->setValue(effect->mParams.p_freq_ramp        * 1000.0);      // Frequency Slide
    m_slider_3->setValue(effect->mParams.p_freq_dramp       * 1000.0);      // Delta Slide
    m_slider_4->setValue(effect->mParams.p_vib_strength     * 1000.0);      // Vibrato Strength
    m_slider_5->setValue(effect->mParams.p_vib_speed        * 1000.0);      // Vibrato Speed
    m_slider_6->setValue(effect->mParams.p_env_attack       * 1000.0);      // Attack Time
    m_slider_7->setValue(effect->mParams.p_env_sustain      * 1000.0);      // Sustain Time
    m_slider_8->setValue(effect->mParams.p_env_decay        * 1000.0);      // Decay Time
    m_slider_9->setValue(effect->mParams.p_env_punch        * 1000.0);      // Sustain Punch

    m_slider_10->setValue(effect->mParams.p_repeat_speed    * 1000.0);      // Repeat Speed
    m_slider_11->setValue(effect->mParams.p_pha_offset      * 1000.0);      // Phaser (Flanger) Offset
    m_slider_12->setValue(effect->mParams.p_pha_ramp        * 1000.0);      // Phaser (Flanger) Sweep
    m_slider_13->setValue(effect->mParams.p_arp_speed       * 1000.0);      // Pitch Jump (Change) Speed
    m_slider_14->setValue(effect->mParams.p_arp_mod         * 1000.0);      // Pitch Jump (Change) Amount
    m_slider_15->setValue(effect->mParams.p_lpf_resonance   * 1000.0);      // Low-pass Filter Resonance
    m_slider_16->setValue(effect->mParams.p_lpf_freq        * 1000.0);      // Low-pass Filter Cutoff
    m_slider_17->setValue(effect->mParams.p_lpf_ramp        * 1000.0);      // Low-pass Filter Sweep
    m_slider_18->setValue(effect->mParams.p_hpf_freq        * 1000.0);      // High-pass Filter Cutoff
    m_slider_19->setValue(effect->mParams.p_hpf_ramp        * 1000.0);      // High-pass Filter Sweep
}

// Update Parameters of selected Effect
void FormSoundEffect::updateSelectedEffect(QSlider *from_slider) {
    SoLoud::Sfxr *effect = getEffect(m_current_effect);
    if (effect != nullptr) {
        if (from_slider == m_slider_0)  effect->mParams.p_base_freq =       static_cast<double>(m_slider_0->value()) / 1000.0;
        if (from_slider == m_slider_1)  effect->mParams.p_freq_limit =      static_cast<double>(m_slider_1->value()) / 1000.0;
        if (from_slider == m_slider_2)  effect->mParams.p_freq_ramp =       static_cast<double>(m_slider_2->value()) / 1000.0;
        if (from_slider == m_slider_3)  effect->mParams.p_freq_dramp =      static_cast<double>(m_slider_3->value()) / 1000.0;
        if (from_slider == m_slider_4)  effect->mParams.p_vib_strength =    static_cast<double>(m_slider_4->value()) / 1000.0;
        if (from_slider == m_slider_5)  effect->mParams.p_vib_speed =       static_cast<double>(m_slider_5->value()) / 1000.0;
        if (from_slider == m_slider_6)  effect->mParams.p_env_attack =      static_cast<double>(m_slider_6->value()) / 1000.0;
        if (from_slider == m_slider_7)  effect->mParams.p_env_sustain =     static_cast<double>(m_slider_7->value()) / 1000.0;
        if (from_slider == m_slider_8)  effect->mParams.p_env_decay =       static_cast<double>(m_slider_8->value()) / 1000.0;
        if (from_slider == m_slider_9)  effect->mParams.p_env_punch =       static_cast<double>(m_slider_9->value()) / 1000.0;

        if (from_slider == m_slider_10) effect->mParams.p_repeat_speed =    static_cast<double>(m_slider_10->value()) / 1000.0;
        if (from_slider == m_slider_11) effect->mParams.p_pha_offset =      static_cast<double>(m_slider_11->value()) / 1000.0;
        if (from_slider == m_slider_12) effect->mParams.p_pha_ramp =        static_cast<double>(m_slider_12->value()) / 1000.0;
        if (from_slider == m_slider_13) effect->mParams.p_arp_speed =       static_cast<double>(m_slider_13->value()) / 1000.0;
        if (from_slider == m_slider_14) effect->mParams.p_arp_mod =         static_cast<double>(m_slider_14->value()) / 1000.0;
        if (from_slider == m_slider_15) effect->mParams.p_lpf_resonance =   static_cast<double>(m_slider_15->value()) / 1000.0;
        if (from_slider == m_slider_16) effect->mParams.p_lpf_freq =        static_cast<double>(m_slider_16->value()) / 1000.0;
        if (from_slider == m_slider_17) effect->mParams.p_lpf_ramp =        static_cast<double>(m_slider_17->value()) / 1000.0;
        if (from_slider == m_slider_18) effect->mParams.p_hpf_freq =        static_cast<double>(m_slider_18->value()) / 1000.0;
        if (from_slider == m_slider_19) effect->mParams.p_hpf_ramp =        static_cast<double>(m_slider_19->value()) / 1000.0;
    }
}

void FormSoundEffect::updateWaveType(SoundEffectWaveType wave_type) {
    SoLoud::Sfxr* effect = getEffect(m_current_effect);
    if (effect != nullptr) {
        effect->mParams.wave_type = static_cast<int>(wave_type);
    }

    switch (wave_type) {
        case Square:    m_action_square->setChecked(true);  break;
        case Sawtooth:  m_action_saw->setChecked(true);     break;
        case Sine:      m_action_sine->setChecked(true);    break;
        case Noise:     m_action_noise->setChecked(true);   break;
    }

    QString selected_wave_type = stringFromWaveType(wave_type);
    m_wave_button->setText(selected_wave_type);
}



















