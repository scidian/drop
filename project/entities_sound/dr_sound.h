//
//      Created by Stephens Nunnally on 6/2/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef DRSOUND_H
#define DRSOUND_H

#include "3rd_party/soloud/soloud.h"
#include "3rd_party/soloud/soloud_audiosource.h"
#include "core/types/dr_color.h"
#include "core/types/dr_point.h"
#include "core/types/dr_pointf.h"
#include "core/types/dr_rect.h"
#include "project/settings/settings.h"

// Forward Declarations
class DrProject;


//####################################################################################
//##    DrSound
//##        Class to hold a Sound for use in a Project
//############################
class DrSound : public DrSettings
{
private:
    // Local Variables
    DrSoundType             m_sound_type        { DrSoundType::Sound_Effect };      // Type of this sound
    SoLoud::AudioSource    *m_audio_source      { nullptr };                        // Audio Source
    DrBitmap                m_wave_form;                                            // Wave form image


public:
    // Constructor / Destructor
    DrSound(DrProject *parent_project, DrSoundType sound_type, SoLoud::AudioSource *audio_source,
            long key = c_no_key, std::string new_sound_name = "New Sound");
    virtual ~DrSound() override;

    // DrSettings Overrides
    virtual DrType          getType() override              { return DrType::Sound; }

    // Initializing
    void                    initializeSoundSettings(std::string new_name);
    void                    initializeSoundEffectSettings();

    // Updating
    void                    updateSoundEffectProperties();

    // Getters / Setters
    SoLoud::AudioSource*    getAudioSource()                { return m_audio_source; }
    DrSoundType             getSoundType()                  { return m_sound_type; }
    DrBitmap                getWaveForm()                   { return m_wave_form; }
    void                    setWaveForm(DrBitmap bitmap)    { m_wave_form = bitmap; }


};

#endif // DRSOUND_H





















