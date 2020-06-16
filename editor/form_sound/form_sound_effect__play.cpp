//
//      Created by Stephens Nunnally on 5/25/2020, (c) 2020 Scidian Software, All Rights Reserved
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

    // Setting as selected fires list SIGNAL(itemSelectionChanged()) connected to this SLOT(playItem())
    item->setSelected(true);
}


//####################################################################################
//##    Play Sounds from Containers
//####################################################################################
SoLoud::Sfxr* FormSoundEffect::getEffect(long effect_key) {
    auto it = m_effects.find(effect_key);
    if (it != m_effects.end()) return it->second; else return nullptr;
}

void FormSoundEffect::playSelected() {
    qDebug() << "Play Item - " << Dr::CurrentTimeAsString();

    // Get selected item underlying Sfxr AudioSource
    SoLoud::Sfxr* effect = getEffect(m_selected_effect);

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

// Get selected item, mark as selected
void FormSoundEffect::selectionChanged() {
    if (m_list->selectedItems().count() > 0) {
        QListWidgetItem *item = m_list->selectedItems().first();
        long sound_key = item->data(User_Roles::Key).toInt();
        m_selected_effect = sound_key;
        playSelected();
    } else {
        m_selected_effect = c_no_key;
    }
}

// If item was selected when first clicked, play it again now that it has been clicked
void FormSoundEffect::itemClicked(QListWidgetItem *clicked_item) {
    if (clicked_item == nullptr) return;
    playSelected();
}


//####################################################################################
//##    Update Sliders / Selected Sopund Effect
//####################################################################################
void FormSoundEffect::updateSliders(SoLoud::Sfxr *effect) {
    m_slider_0->setValue(effect->mParams.p_base_freq    * 1000.0);      // Frequency
    m_slider_1->setValue(effect->mParams.p_freq_limit   * 1000.0);      // Frequency Cutoff
    m_slider_2->setValue(effect->mParams.p_freq_ramp    * 1000.0);      // Frequency Slide
    m_slider_3->setValue(effect->mParams.p_freq_dramp   * 1000.0);      // Delta Slide
    m_slider_3->setValue(effect->mParams.p_vib_strength * 1000.0);      // c
    m_slider_3->setValue(effect->mParams.p_vib_speed    * 1000.0);      // Vibrato Speed
}

// Update Parameters of selected Effect
void FormSoundEffect::updateSelectedEffect(QSlider *from_slider) {
    SoLoud::Sfxr *effect = getEffect(m_selected_effect);
    if (effect != nullptr) {
        if (from_slider == m_slider_0) effect->mParams.p_base_freq =    static_cast<double>(m_slider_0->value()) / 1000.0;
        if (from_slider == m_slider_1) effect->mParams.p_freq_limit =   static_cast<double>(m_slider_1->value()) / 1000.0;
        if (from_slider == m_slider_2) effect->mParams.p_freq_ramp =    static_cast<double>(m_slider_2->value()) / 1000.0;
        if (from_slider == m_slider_3) effect->mParams.p_freq_dramp =   static_cast<double>(m_slider_3->value()) / 1000.0;
        if (from_slider == m_slider_4) effect->mParams.p_vib_strength = static_cast<double>(m_slider_4->value()) / 1000.0;
        if (from_slider == m_slider_5) effect->mParams.p_vib_speed =    static_cast<double>(m_slider_5->value()) / 1000.0;
    }
}

void FormSoundEffect::updateWaveType(SoundEffectWaveType wave_type) {
    SoLoud::Sfxr* effect = getEffect(m_selected_effect);
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



















