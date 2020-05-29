//
//      Created by Stephens Nunnally on 5/29/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef DRMUSIC_H
#define DRMUSIC_H

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
//##    DrMusic
//##        Class to hold a composition of DrSounds mixed together in Editor_Mode::Sound_Creator
//############################
class DrMusic : public DrSettings
{
private:
    // Local Variables


public:
    // Constructor / Destructor
    DrMusic(DrProject *parent_project, long key = c_no_key);

    // DrSettings Overrides
    virtual DrType      getType() override              { return DrType::Music; }


};

#endif // DRMUSIC_H
