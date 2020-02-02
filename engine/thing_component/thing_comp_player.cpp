//
//      Created by Stephens Nunnally on 2/2/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "thing_comp_player.h"


//####################################################################################
//##    Constructor / Destructor
//####################################################################################
ThingCompPlayer::ThingCompPlayer(DrEngineWorld *engine_world, DrEngineThing *parent_thing)
    : DrEngineComponent(engine_world, parent_thing, Comps::Thing_Player) {

}

ThingCompPlayer::~ThingCompPlayer() {

}
