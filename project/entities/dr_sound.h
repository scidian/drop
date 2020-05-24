//
//      Created by Stephens Nunnally on 5/24/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef DRSOUND_H
#define DRSOUND_H

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
    DrSoundType     m_sound_type        { DrSoundType::Sound_Effect };                 // Type of this sound


public:
    // Constructor / Destructor
    DrSound(DrProject *parent_project, DrSoundType sound_type, long key = c_no_key);

    // DrSettings Overrides
    virtual DrType      getType() override              { return DrType::Sound; }

    // Getters / Setters
    DrSoundType         getSoundType()                  { return m_sound_type; }

};

#endif // DRSOUND_H














