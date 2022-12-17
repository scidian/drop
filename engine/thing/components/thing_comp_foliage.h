//
//      Created by Stephens Nunnally on 2/28/2020, (c) 2020 Scidian Studios, All Rights Reserved
//
//  File:
//
//
//
#ifndef THING_COMP_FOLIAGE_H
#define THING_COMP_FOLIAGE_H

#include "3rd_party/chipmunk/chipmunk.h"
#include "engine/globals_engine.h"
#include "engine/thing/engine_thing_component.h"


//####################################################################################
//##    ThingCompFoliage
//##        Built-In Component for DrEngineThing dealing with Foliage physics
//############################
class ThingCompFoliage : public DrThingComponent
{
public:
    // Constructor / Destructor
    ThingCompFoliage(DrEngineWorld *engine_world, DrEngineThing *parent_thing, double springiness);
    virtual ~ThingCompFoliage() override;


    // #################### VARIABLES ####################
public:
    // Foliage Variables
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
    // Getters / Setters
    double          getSpringiness()                    { return m_springiness; }
    void            setSpringiness(double spring)       { m_springiness = spring; }



};


#endif // THING_COMP_FOLIAGE_H



















