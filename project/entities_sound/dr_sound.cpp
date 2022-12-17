//
//      Created by Stephens Nunnally on 6/2/2020, (c) 2020 Scidian Studios, All Rights Reserved
//
//  File:
//
//
//
#include "3rd_party/soloud/soloud_sfxr.h"
#include "core/colors/colors.h"
#include "project/constants_component_info.h"
#include "project/entities_sound/dr_sound.h"
#include "project/settings/settings_component.h"
#include "project/settings/settings_component_property.h"


//####################################################################################
//##    Constructor
//####################################################################################
DrSound::DrSound(DrProject *parent_project, DrSoundType sound_type, SoLoud::AudioSource *audio_source, long key, std::string new_sound_name)
    : DrSettings(parent_project) {

    this->setKey(key);

    m_sound_type =      sound_type;
    m_audio_source =    audio_source;

    initializeSoundSettings(new_sound_name);

    if (m_sound_type == DrSoundType::Sound_Effect) initializeSoundEffectSettings();

}

DrSound::~DrSound() {
    delete m_audio_source;
    m_audio_source = nullptr;
}


//####################################################################################
//##    Settings Component
//####################################################################################
void DrSound::initializeSoundSettings(std::string new_name) {
    DrProperty *property_name = getComponentProperty(Comps::Entity_Settings, Props::Entity_Name);
    property_name->setDisplayName("Sound Name");
    property_name->setDescription("Name of this Sound Asset.");
    property_name->setValue(new_name);

    addComponent(Comps::Sound_Settings, "Sound Settings", "Settings for this Sound.", Component_Colors::RGB_18_Gray, false);
    getComponent(Comps::Sound_Settings)->setIcon(Component_Icons::Sound);

    DrColor sound_color;
    switch (m_sound_type) {
        case DrSoundType::Mix:              sound_color = Component_Colors::RGB_01_Red;         break;
        case DrSoundType::Audio_File:       sound_color = Component_Colors::RGB_04_Purple;      break;
        case DrSoundType::Instrument:       sound_color = Component_Colors::RGB_07_LightBlue;   break;
        case DrSoundType::Sound_Effect:     sound_color = Component_Colors::RGB_10_Green;       break;
        case DrSoundType::Speech:           sound_color = Component_Colors::RGB_13_Yellow;      break;
        case DrSoundType::White_Noise:      sound_color = Component_Colors::RGB_16_Orange;      break;
    }
    addPropertyToComponent(Comps::Sound_Settings, Props::Sound_Settings_Color, Property_Type::Color, sound_color.rgba(),
                           "Sound Color", "Color this Sound will appear in the Asset Tree.");

}


//####################################################################################
//##    Sound Effect Settings Component
//####################################################################################
void DrSound::initializeSoundEffectSettings() {
    // Cast audio source into Sfxr
    SoLoud::Sfxr *sfxr = dynamic_cast<SoLoud::Sfxr*>(m_audio_source);
    if (m_audio_source == nullptr) return;

    // Frequency
    //  0   float p_base_freq;          //  0, 1    Frequency                       Base note of sound
    //  1   float p_freq_limit;         //  0, 1    Frequency Cutoff                If sliding, frequency will stop here to prevent low notes
    //  2   float p_freq_ramp;          // -1, 1    Frequency Slide                 Slides the frequency up or down
    //  3   float p_freq_dramp;         // -1, 1    Delta Slide                     Accelerates frequency slide, can be used to change slide directions
    // Vibrato
    //  4   float p_vib_strength;       //  0, 1    Vibrato Depth                   Strength of the vibrato effect
    //  5   float p_vib_speed;          //  0, 1    Vibrato Speed                   Speed of the bibrato effect (i.e. frequency)
    // Volume Envelope
    //  6   float p_env_attack;         //  0, 1    Attack Time                     Length of the volume envelope attack
    //  7   float p_env_sustain;        //  0, 1    Sustain Time                    Length of the volume envelope sustain
    //  8   float p_env_decay;          //  0, 1    Decay Time                      Length of the volume envelope decay (aka release)
    //  9   float p_env_punch;          //  0, 1    Sustain Punch                   Tilts the sustain envelope for more 'pop'

    // Repeat
    // 10   float p_repeat_speed;       //  0, 1    Repeat Speed                    Speed of the note repeating
    // Phaser
    // 11   float p_pha_offset;         // -1, 1    Phaser (Flanger) Offset         Offsets a second copy of the wave by a small phase, changing the timbre
    // 12   float p_pha_ramp;           // -1, 1    Phaser (Flanger) Sweep          Sweeps the phase up or down
    // Pitch Jump
    // 13   float p_arp_speed;          //  0, 1    Pitch Jump (Change) Speed       Larger values means more pitch jumps, useful for arpeggiation
    // 14   float p_arp_mod;            // -1, 1    Pitch Jump (Change) Amount      Jump in pitch, up or down
    // Low-pass / High-pass Filters
    // 15   float p_lpf_resonance;      //  0, 1    Low-pass Filter Resonance       Changes the attenuation rate for the low-pass filter, changing the timbre
    // 16   float p_lpf_freq;           //  0, 1    Low-pass Filter Cutoff          Frequency at which the low-pass filter starts attenuating higher frequencies
    // 17   float p_lpf_ramp;           // -1, 1    Low-pass Filter Sweep           Sweeps the low-pass cutoff up or down
    // 18   float p_hpf_freq;           //  0, 1    High-pass Filter Cutoff         Frequency at which the high-pass filter starts attenuating lower frequencies
    // 19   float p_hpf_ramp;           // -1, 1    High-pass Filter Sweep          Sweeps the high-pass filter up or down

    //    Slider          = 12,   // vector<variant>      Decimal >= min and <= max, with a slider
    //                            //                          6 values: double, double min value, double max value, double step size, string suffix ("%", etc), decimal precision

    // Initialize Sound Effect settings
    addComponent(Comps::Sound_Effect_Settings, "Sound Effect Settings", "Settings for this Sound Effect.", Component_Colors::RGB_10_Green, false);
    getComponent(Comps::Sound_Effect_Settings)->setIcon(Component_Icons::Sound_Effect);

    addPropertyToComponent(Comps::Sound_Effect_Settings, Props::Sound_Effect_Frequency,         Property_Type::Slider, std::vector<DrVariant>({sfxr->mParams.p_base_freq, 0.0, 1.0, 0.1, "", 2}),
                           "Frequency", "Base note of sound.");
    addPropertyToComponent(Comps::Sound_Effect_Settings, Props::Sound_Effect_Frequency_Cutoff,  Property_Type::Slider, std::vector<DrVariant>({sfxr->mParams.p_freq_limit, 0.0, 1.0, 0.1, "", 2}),
                           "Frequency Cutoff", "If sliding, frequency will stop here to prevent low notes.");
    addPropertyToComponent(Comps::Sound_Effect_Settings, Props::Sound_Effect_Frequency_Slide,   Property_Type::Slider, std::vector<DrVariant>({sfxr->mParams.p_freq_ramp, -1.0, 1.0, 0.1, "", 2}),
                           "Frequency Slide", "Slides the frequency up or down.");
    addPropertyToComponent(Comps::Sound_Effect_Settings, Props::Sound_Effect_Delta_Slide,       Property_Type::Slider, std::vector<DrVariant>({sfxr->mParams.p_freq_dramp, -1.0, 1.0, 0.1, "", 2}),
                           "Delta Slide", "Accelerates frequency slide, can be used to change slide directions.");
    addPropertyToComponent(Comps::Sound_Effect_Settings, Props::Sound_Effect_Vibrato_Strength,  Property_Type::Slider, std::vector<DrVariant>({sfxr->mParams.p_vib_strength, 0.0, 1.0, 0.1, "", 2}),
                           "Vibrato Depth", "Strength of the vibrato effect.");
    addPropertyToComponent(Comps::Sound_Effect_Settings, Props::Sound_Effect_Vibrato_Speed,     Property_Type::Slider, std::vector<DrVariant>({sfxr->mParams.p_vib_speed, 0.0, 1.0, 0.1, "", 2}),
                           "Vibrato Speed", "Speed of the vibrato effect (i.e. frequency).");
    addPropertyToComponent(Comps::Sound_Effect_Settings, Props::Sound_Effect_Attack_Time,       Property_Type::Slider, std::vector<DrVariant>({sfxr->mParams.p_env_attack, 0.0, 1.0, 0.1, "", 2}),
                           "Attack Time", "Length of the volume envelope attack.");
    addPropertyToComponent(Comps::Sound_Effect_Settings, Props::Sound_Effect_Sustain_Time,      Property_Type::Slider, std::vector<DrVariant>({sfxr->mParams.p_env_sustain, 0.0, 1.0, 0.1, "", 2}),
                           "Sustain Time", "Length of the volume envelope sustain.");
    addPropertyToComponent(Comps::Sound_Effect_Settings, Props::Sound_Effect_Decay_Time,        Property_Type::Slider, std::vector<DrVariant>({sfxr->mParams.p_env_decay, 0.0, 1.0, 0.1, "", 2}),
                           "Decay Time", "Length of the volume envelope decay.");
    addPropertyToComponent(Comps::Sound_Effect_Settings, Props::Sound_Effect_Sustain_Punch,     Property_Type::Slider, std::vector<DrVariant>({sfxr->mParams.p_env_punch, 0.0, 1.0, 0.1, "", 2}),
                           "Sustain Punch", "Tilts the sustain envelope for more 'pop'.");

    addPropertyToComponent(Comps::Sound_Effect_Settings, Props::Sound_Effect_Repeat_Speed,      Property_Type::Slider, std::vector<DrVariant>({sfxr->mParams.p_repeat_speed, 0.0, 1.0, 0.1, "", 2}),
                           "Repeat Speed", "Speed of the note repeating.");
    addPropertyToComponent(Comps::Sound_Effect_Settings, Props::Sound_Effect_Phaser_Offset,     Property_Type::Slider, std::vector<DrVariant>({sfxr->mParams.p_pha_offset, -1.0, 1.0, 0.1, "", 2}),
                           "Phaser Offset", "Offsets a second copy of the wave by a small phase, changing the timbre.");
    addPropertyToComponent(Comps::Sound_Effect_Settings, Props::Sound_Effect_Phaser_Sweep,      Property_Type::Slider, std::vector<DrVariant>({sfxr->mParams.p_pha_ramp, -1.0, 1.0, 0.1, "", 2}),
                           "Phaser Sweep", "Sweeps the phase up or down.");
    addPropertyToComponent(Comps::Sound_Effect_Settings, Props::Sound_Effect_Pitch_Jump_Speed,  Property_Type::Slider, std::vector<DrVariant>({sfxr->mParams.p_arp_speed, 0.0, 1.0, 0.1, "", 2}),
                           "Pitch Jump Speed", "Larger values means more pitch jumps, useful for arpeggiation.");
    addPropertyToComponent(Comps::Sound_Effect_Settings, Props::Sound_Effect_Pitch_Jump_Amount, Property_Type::Slider, std::vector<DrVariant>({sfxr->mParams.p_arp_mod, -1.0, 1.0, 0.1, "", 2}),
                           "Pitch Jump Amount", "Jump in pitch, up or down.");
    addPropertyToComponent(Comps::Sound_Effect_Settings, Props::Sound_Effect_Lpf_Resonance,     Property_Type::Slider, std::vector<DrVariant>({sfxr->mParams.p_lpf_resonance, 0.0, 1.0, 0.1, "", 2}),
                           "Low-pass Resonance", "Changes the attenuation rate for the low-pass filter, changing the timbre.");
    addPropertyToComponent(Comps::Sound_Effect_Settings, Props::Sound_Effect_Lpf_Cuttoff,       Property_Type::Slider, std::vector<DrVariant>({sfxr->mParams.p_lpf_freq, 0.0, 1.0, 0.1, "", 2}),
                           "Low-pass Cutoff", "Frequency at which the low-pass filter starts attenuating higher frequencies.");
    addPropertyToComponent(Comps::Sound_Effect_Settings, Props::Sound_Effect_Lpf_Sweep,         Property_Type::Slider, std::vector<DrVariant>({sfxr->mParams.p_lpf_ramp, -1.0, 1.0, 0.1, "", 2}),
                           "Low-pass Sweep", "Sweeps the low-pass filter cutoff up or down.");
    addPropertyToComponent(Comps::Sound_Effect_Settings, Props::Sound_Effect_Hpf_Cutoff,        Property_Type::Slider, std::vector<DrVariant>({sfxr->mParams.p_hpf_freq, 0.0, 1.0, 0.1, "", 2}),
                           "High-pass Cutoff", "Frequency at which the high-pass filter starts attenuating lower frequencies.");
    addPropertyToComponent(Comps::Sound_Effect_Settings, Props::Sound_Effect_Hpf_Sweep,         Property_Type::Slider, std::vector<DrVariant>({sfxr->mParams.p_hpf_ramp, -1.0, 1.0, 0.1, "", 2}),
                           "High-pass Sweep", "Sweeps the high-pass filter cutoff up or down.");

}

void DrSound::updateSoundEffectProperties() {
    // Check type, grab underlying Sfxr AudioSource and DrComponent with settings to update from
    if (getSoundType() != DrSoundType::Sound_Effect) return;
    SoLoud::Sfxr *effect = dynamic_cast<SoLoud::Sfxr*>(m_audio_source);
    if (effect == nullptr) return;
    DrComponent *settings_comp = getComponent(Comps::Sound_Effect_Settings);
    if (settings_comp == nullptr) return;

    // Update Settings
    effect->mParams.p_base_freq =       settings_comp->getProperty(Props::Sound_Effect_Frequency        )->getValue().toVector().at(0).toDouble();
    effect->mParams.p_freq_limit =      settings_comp->getProperty(Props::Sound_Effect_Frequency_Cutoff )->getValue().toVector().at(0).toDouble();
    effect->mParams.p_freq_ramp =       settings_comp->getProperty(Props::Sound_Effect_Frequency_Slide  )->getValue().toVector().at(0).toDouble();
    effect->mParams.p_freq_dramp =      settings_comp->getProperty(Props::Sound_Effect_Delta_Slide      )->getValue().toVector().at(0).toDouble();
    effect->mParams.p_vib_strength =    settings_comp->getProperty(Props::Sound_Effect_Vibrato_Strength )->getValue().toVector().at(0).toDouble();
    effect->mParams.p_vib_speed =       settings_comp->getProperty(Props::Sound_Effect_Vibrato_Speed    )->getValue().toVector().at(0).toDouble();
    effect->mParams.p_env_attack =      settings_comp->getProperty(Props::Sound_Effect_Attack_Time      )->getValue().toVector().at(0).toDouble();
    effect->mParams.p_env_sustain =     settings_comp->getProperty(Props::Sound_Effect_Sustain_Time     )->getValue().toVector().at(0).toDouble();
    effect->mParams.p_env_decay =       settings_comp->getProperty(Props::Sound_Effect_Decay_Time       )->getValue().toVector().at(0).toDouble();
    effect->mParams.p_env_punch =       settings_comp->getProperty(Props::Sound_Effect_Sustain_Punch    )->getValue().toVector().at(0).toDouble();

    effect->mParams.p_repeat_speed =    settings_comp->getProperty(Props::Sound_Effect_Repeat_Speed     )->getValue().toVector().at(0).toDouble();
    effect->mParams.p_pha_offset =      settings_comp->getProperty(Props::Sound_Effect_Phaser_Offset    )->getValue().toVector().at(0).toDouble();
    effect->mParams.p_pha_ramp =        settings_comp->getProperty(Props::Sound_Effect_Phaser_Sweep     )->getValue().toVector().at(0).toDouble();
    effect->mParams.p_arp_speed =       settings_comp->getProperty(Props::Sound_Effect_Pitch_Jump_Speed )->getValue().toVector().at(0).toDouble();
    effect->mParams.p_arp_mod =         settings_comp->getProperty(Props::Sound_Effect_Pitch_Jump_Amount)->getValue().toVector().at(0).toDouble();
    effect->mParams.p_lpf_resonance =   settings_comp->getProperty(Props::Sound_Effect_Lpf_Resonance    )->getValue().toVector().at(0).toDouble();
    effect->mParams.p_lpf_freq =        settings_comp->getProperty(Props::Sound_Effect_Lpf_Cuttoff      )->getValue().toVector().at(0).toDouble();
    effect->mParams.p_lpf_ramp =        settings_comp->getProperty(Props::Sound_Effect_Lpf_Sweep        )->getValue().toVector().at(0).toDouble();
    effect->mParams.p_hpf_freq =        settings_comp->getProperty(Props::Sound_Effect_Hpf_Cutoff       )->getValue().toVector().at(0).toDouble();
    effect->mParams.p_hpf_ramp =        settings_comp->getProperty(Props::Sound_Effect_Hpf_Sweep        )->getValue().toVector().at(0).toDouble();
}
















