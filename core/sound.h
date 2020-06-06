//
//      Created by Stephens Nunnally on 6/6/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef SOUND_H
#define SOUND_H

// Forward Declarations
namespace SoLoud { class Soloud; }


//####################################################################################
//##    Sound Functions
//############################
namespace Dr {

    // ***** SoLoud Functions
    void            InitializeSound();
    void            CleanUpSound();
    SoLoud::Soloud* GetSoLoud();



}   // namespace Dr


#endif // SOUND_H










