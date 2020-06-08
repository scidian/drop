//
//      Created by Stephens Nunnally on 6/6/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "core/sound.h"

namespace Dr {


//####################################################################################
//##    Local Static Variables
//####################################################################################
static SoLoud::Soloud      *l_so_loud;                              // Soloud instance



void InitializeSound() {
    l_so_loud = new SoLoud::Soloud();
    l_so_loud->init();
    l_so_loud->setVisualizationEnable(true);
}

void CleanUpSound() {
    l_so_loud->deinit();
    delete l_so_loud;
}

SoLoud::Soloud* GetSoLoud() {
    return l_so_loud;
}



}   // namespace Dr












