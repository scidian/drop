//
//      Created by Stephens Nunnally on 6/6/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef SOUND_H
#define SOUND_H

#include "3rd_party/soloud/soloud.h"

// Forward Declarations
namespace SoLoud { class Soloud; }


//####################################################################################
//##    Sound Functions
//############################
namespace Dr {

    // ***** SoLoud Functions
    void            InitializeSound();
    void            CleanUpSound();

    bool            IsSoundInitialized();

    SoLoud::Soloud* GetSoLoud();                        // Returns active SoLoud singleton
    SoLoud::Soloud* GetSound();                         // Returns active SoLoud singleton



}   // namespace Dr


#endif // SOUND_H










