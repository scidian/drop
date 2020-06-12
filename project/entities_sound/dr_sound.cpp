//
//      Created by Stephens Nunnally on 6/2/2020, (c) 2020 Scidian Software, All Rights Reserved
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
}

DrSound::~DrSound() {
    delete m_audio_source;
}


//####################################################################################
//##    Settings Components
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

















