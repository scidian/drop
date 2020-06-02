//
//      Created by Stephens Nunnally on 6/2/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef DRTRACK_H
#define DRTRACK_H

#include "3rd_party/soloud/soloud.h"
#include "3rd_party/soloud/soloud_audiosource.h"
#include "project/settings/settings.h"

// Forward Declarations
class DrMix;
class DrProject;
class DrSound;


//####################################################################################
//##    DrTrack
//##        Class to hold one DrTrack within a DrMix
//############################
class DrTrack : public DrSettings
{
private:
    // External Borrowed Pointers
    DrMix              *m_parent_mix;                       // Holds reference to parent DrMix class (composition that holds this DrTrack)
    DrSound            *m_sound_asset;                      // Holds reference to DrSound that this DrTrack is an instance of

    // Local Variables
    int                 m_track             { 0 };          // Track number this DrTrack is in within parent DrMix
    double              m_x_pos             { 0 };          // X Position of this track within parent DrMix


public:
    // Constructor / Destructor
    explicit DrTrack(DrProject *parent_project, DrMix *parent_mix, DrSound *from_sound, long new_track_key, int track, double x_position);
    virtual ~DrTrack() override;

    // DrSettings Base Class Overrides
    virtual DrType      getType() override          { return DrType::Track; }

    // Initializing
    void                initializeTrackSettings();

    // Getters / Setters
    DrMix*              getMix()                    { return m_parent_mix; }
    DrSound*            getSound()                  { return m_sound_asset; }



};


#endif // DRTRACK_H











