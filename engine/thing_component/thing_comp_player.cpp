//
//      Created by Stephens Nunnally on 2/2/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "core/dr_random.h"
#include "engine/engine_signal.h"
#include "engine/thing_component/thing_comp_player.h"
#include "engine/thing/engine_thing.h"
#include "engine/world/engine_world.h"


//####################################################################################
//##    Constructor / Destructor
//####################################################################################
ThingCompPlayer::ThingCompPlayer(DrEngineWorld *engine_world, DrEngineThing *parent_thing)
    : DrEngineComponent(engine_world, parent_thing, Comps::Thing_Player) {

}

ThingCompPlayer::~ThingCompPlayer() {

}


//####################################################################################
//##    Basic Virtual Component Functions
//####################################################################################
// Called when component is first created
void ThingCompPlayer::init() {

}

// Called during DrEngineWorld->updateWorld() step
void ThingCompPlayer::update(double time_passed, double time_warp) {
    (void) time_passed;
    (void) time_warp;

//    if (signalList(Signals::MouseLeftDown).size() > 0) {
//        thing()->hue = static_cast<float>(Dr::RandomDouble(0, 360));
//    }

//    if (signalList(Signals::MouseScroll).size() > 0) {
//        g_info = "Mouse Scroll: " + std::to_string( signalList(Signals::MouseScroll).front()->value().toInt() );
//    }

//    if (signalList(Signals::ThingDied).size() > 0) {
//        g_info = "Thing Died: " + signalList(Signals::ThingDied).front()->thingNameA() +
//               ", Engine Key: " + std::to_string(signalList(Signals::ThingDied).front()->thingEngineKeyA()) +
//                ", Asset Key: " + std::to_string(signalList(Signals::ThingDied).front()->thingAssetKeyA()) +
//                ", Killed by: " + signalList(Signals::ThingDied).front()->thingNameB();
//    }

//    for (auto &signal : signalList(Signals::ThingDied)) {
//        if (signal->thingEngineKeyA() == thing()->getKey()) {
//            world()->setBackgroundColor( DrColor(Dr::RandomInt(0, 255), Dr::RandomInt(0, 255), Dr::RandomInt(0, 255)) );
//        }
//    }

}

// Called when component is destroyed
void ThingCompPlayer::destroy() {

}
























