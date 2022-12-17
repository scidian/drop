//
//      Created by Stephens Nunnally on 6/6/2020, (c) 2020 Scidian Studios, All Rights Reserved
//
//  File:
//
//
//
#include "core/dr_debug.h"
#include "core/dr_time.h"
#include "core/sound.h"

namespace Dr {


//####################################################################################
//##    Local Static Variables
//####################################################################################
static SoLoud::Soloud      *l_so_loud;                                      // Soloud instance
static bool                 l_sound_initialized         { false };          // Keeps track of if SoLoud::init has been called



void InitializeSound() {
    if (l_sound_initialized == false) {
        l_so_loud = new SoLoud::Soloud();
        l_so_loud->init();
        l_so_loud->setVisualizationEnable(true);
        l_sound_initialized = true;
    }
}

void CleanUpSound() {
    if (l_sound_initialized == true) {
        l_so_loud->deinit();
        delete l_so_loud;
        l_sound_initialized = false;
    }
}

bool            IsSoundInitialized()    { return l_sound_initialized; }
SoLoud::Soloud* GetSoLoud()             { return l_so_loud; }
SoLoud::Soloud* GetSound()              { return l_so_loud; }



}   // namespace Dr












