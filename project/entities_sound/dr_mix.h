//
//      Created by Stephens Nunnally on 6/2/2020, (c) 2020 Scidian Studios, All Rights Reserved
//
//  File:
//
//
//
#ifndef DRMIX_H
#define DRMIX_H

#include "3rd_party/soloud/soloud.h"
#include "3rd_party/soloud/soloud_audiosource.h"
#include "core/dr_containers.h"
#include "core/types/dr_color.h"
#include "core/types/dr_point.h"
#include "core/types/dr_pointf.h"
#include "core/types/dr_rect.h"
#include "project/settings/settings.h"

// Forward Declarations
class DrProject;
class DrSound;
class DrTrack;

// Type Definitions
typedef std::map<long, DrTrack*> TrackMap;


//####################################################################################
//##    DrMix
//##        Class to hold a composition of DrTracks (instances of DrSounds) mixed together in Editor_Mode::Sound_Creator
//############################
class DrMix : public DrSettings
{
private:
    // Local Variables
    TrackMap            m_tracks;                                   // List of DrTracks within this DrMix


public:
    // Constructor / Destructor
    DrMix(DrProject *parent_project, long key, std::string new_mix_name);
    virtual ~DrMix() override;

    // DrSettings Overrides
    virtual DrType          getType() override              { return DrType::Mix; }

    // Initializing
    void                    initializeMixSettings(std::string new_name);

    // Getters / Setters
    TrackMap&               getTrackMap()                   { return m_tracks; }

    // Mix Construction
    DrTrack* addTrack(DrSound *from_sound, long track_key = c_no_key, long track_number = 0, double x_position = 0);
    DrTrack* addTrackCopyFromTrack(DrTrack *from_track);



};

#endif // DRMIX_H















