//
//      Created by Stephens Nunnally on 4/22/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef THING_COMP_PHYSICS_H
#define THING_COMP_PHYSICS_H

#include "3rd_party/chipmunk/chipmunk.h"
#include "engine/globals_engine.h"
#include "engine/thing/engine_thing_component.h"


//####################################################################################
//##    ThingCompPhysics
//##        Built-In Component for DrEngineThing dealing with Physics
//############################
class ThingCompPhysics : public DrThingComponent
{
public:
    // Constructor / Destructor
    ThingCompPhysics(DrEngineWorld *engine_world, DrEngineThing *parent_thing);
    virtual ~ThingCompPhysics() override;


    // #################### VARIABLES ####################
public:
    // Physics Variables
    cpBody             *m_anchor_bottom         { nullptr };            // Holds anchor point for the pivot joint to the base
    cpBody             *m_anchor_middle         { nullptr };            // Holds anchor point for the spring joint to the middle

    double              m_springiness           { 0.0 };                // Springiness of foliage (0.0 == none, 1.0 == most)


    // #################### FUNCTIONS TO BE EXPOSED TO API ####################
public:
    // Basic Component Events
    virtual void        init() override;                                                // Called when component is first created
    virtual void        addToWorld() override;                                          // Called when Thing is added to m_things DrEngineWorld vector
    virtual void        draw() override;                                                // Called when it is time to Render Thing
    virtual bool        update(double time_passed, double time_warp) override;          // Called during DrEngineWorld->updateWorld() step
    virtual void        destroy() override;                                             // Called when component is destroyed


    // #################### INTERNAL FUNCTIONS ####################
public:




};

#endif // THING_COMP_PHYSICS_H
