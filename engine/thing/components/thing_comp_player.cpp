//
//      Created by Stephens Nunnally on 2/2/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "core/dr_random.h"
#include "engine/engine_signal.h"
#include "engine/thing/components/thing_comp_player.h"
#include "engine/thing/engine_thing.h"
#include "engine/world/engine_world.h"


//####################################################################################
//##    Constructor / Destructor
//####################################################################################
ThingCompPlayer::ThingCompPlayer(DrEngineWorld *engine_world, DrEngineThing *parent_thing)
    : DrThingComponent(engine_world, parent_thing, Comps::Thing_Player) {


    //###########################################
    //##
    //## Ledge Grabber Test
    //##
    //###########################################
    /**
    DrEngineObject *object = dynamic_cast<DrEngineObject*>(parent_thing);
    if (object != nullptr) {
        // Create Ledge-Grab shape, add to World
        double width =  object->getSize().x;
        double height = object->getSize().y;
        double box_size = 10.0;
        cpBB box_right = cpBBNew(  (width/2.0) - box_size, (height/2.0) - box_size,  (width/2.0) + box_size, (height/2.0) + box_size);
        cpBB box_left =  cpBBNew( -(width/2.0) - box_size, (height/2.0) - box_size, -(width/2.0) + box_size, (height/2.0) + box_size);
        cpShape* ledge_grab_right = object->addShapeBox(box_right);
        cpShape* ledge_grab_left =  object->addShapeBox(box_left);
        cpShapeSetFriction(   ledge_grab_right, 5.0 );  cpShapeSetElasticity( ledge_grab_right, 0.0 );
        cpShapeSetFriction(   ledge_grab_left, 5.0 );   cpShapeSetElasticity( ledge_grab_left, 0.0 );
        cpSpaceAddShape( world()->getSpace(), ledge_grab_right );
        cpSpaceAddShape( world()->getSpace(), ledge_grab_left );

        // If we don't want the body to rotate, overwrite the precalculated moment of inertia with infinity
        if (object->canRotate() == false) {
            cpBodySetMoment( object->body, static_cast<double>(INFINITY) );
        }

        // Add shapes to list of Players' ledge grabber shapes
        m_ledge_grabbers.push_back(ledge_grab_right);
        m_ledge_grabbers.push_back(ledge_grab_left);
    }
    */



}

ThingCompPlayer::~ThingCompPlayer() {

}


//####################################################################################
//##    Basic Virtual Component Functions
//####################################################################################
// Called when component is first created
void ThingCompPlayer::init() {

}

// Called when Thing is added to m_things DrEngineWorld vector
void ThingCompPlayer::addToWorld() {

}

// Called when it is time to Render Thing
void ThingCompPlayer::draw() {

}

// Called during DrEngineWorld->updateWorld() step, returns true if parent DrEngineThing should be removed
bool ThingCompPlayer::update(double time_passed, double time_warp) {
    (void) time_passed;
    (void) time_warp;


    //###########################################
    //##
    //##    Process Messages
    //##
    //###########################################
    // If signal exists with this Foliage that is started colliding with something, apply collision force
    for (auto &signal : signalList(Signals::ThingCollide, thing()->getKey())) {
        DrEngineThing *thing_a = thing();
        DrEngineThing *thing_b = signal->thingB();
        ThingCompPhysics *physics_a = thing_a->physics();
        ThingCompPhysics *physics_b = thing_b->physics();
        Collision_Info info = boost::any_cast<Collision_Info>(signal->value().value());
    }


    //###########################################
    //##
    //##    Message System Testing
    //##
    //###########################################
    /**
    if (signalList(Signals::MouseLeftDown).size() > 0) {
        thing()->hue = static_cast<float>(Dr::RandomDouble(0, 360));
    }

    if (signalList(Signals::MouseScroll).size() > 0) {
        g_info = "Mouse Scroll: " + std::to_string( signalList(Signals::MouseScroll).front()->value().toInt() );
    }

    if (signalList(Signals::ThingDied).size() > 0) {
        g_info = "Thing Died: " + signalList(Signals::ThingDied).front()->thingNameA() +
               ", Engine Key: " + std::to_string(signalList(Signals::ThingDied).front()->thingEngineKeyA()) +
                ", Asset Key: " + std::to_string(signalList(Signals::ThingDied).front()->thingAssetKeyA()) +
                ", Killed by: " + signalList(Signals::ThingDied).front()->thingNameB();
    }

    for (auto &signal : signalList(Signals::ThingDied)) {
        if (signal->thingEngineKeyA() == thing()->getKey()) {
            world()->setBackgroundColor( DrColor(Dr::RandomInt(0, 255), Dr::RandomInt(0, 255), Dr::RandomInt(0, 255)) );
        }
    }
    */


    return false;
}

// Called when component is destroyed
void ThingCompPlayer::destroy() {

}























