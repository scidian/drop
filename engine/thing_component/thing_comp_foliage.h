//
//      Created by Stephens Nunnally on 2/28/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef THING_COMP_FOLIAGE_H
#define THING_COMP_FOLIAGE_H

#include "3rd_party/chipmunk/chipmunk.h"
#include "engine/engine_component.h"
#include "engine/globals_engine.h"



//####################################################################################
//##    ThingCompFoliage
//##        Built-In Component for DrEngineThing dealing with Foliage physics
//############################
class ThingCompFoliage : public DrEngineComponent
{
public:
    // Constructor / Destructor
    ThingCompFoliage(DrEngineWorld *engine_world, DrEngineObject *parent_object);
    virtual ~ThingCompFoliage();


    // #################### VARIABLES ####################
public:
    // Foliage Variables
    cpBody                 *m_anchor_bottom         { nullptr };        // Holds anchor point for the pivot joint to the base
    cpBody                 *m_anchor_middle         { nullptr };        // Holds anchor point for the spring joint to the middle


    // #################### FUNCTIONS TO BE EXPOSED TO API ####################
public:
    // Basic Component Events
    virtual void    init();                                                         // Called when component is first created
    virtual void    update(double time_passed, double time_warp);                   // Called during DrEngineWorld->updateWorld() step
    virtual void    destroy();


    // #################### INTERNAL FUNCTIONS ####################
public:
    // Mesh Functions



};


#endif // THING_COMP_FOLIAGE_H


















