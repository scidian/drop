//
//      Created by Stephens Nunnally on 3/10/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef THING_COMP_SWIRL_H
#define THING_COMP_SWIRL_H

#include "3rd_party/chipmunk/chipmunk.h"
#include "engine/globals_engine.h"
#include "engine/thing/engine_thing_component.h"


//####################################################################################
//##    ThingCompSwirl
//##        Built-In Component for DrEngineThing dealing with Swirl effect
//############################
class ThingCompSwirl : public DrThingComponent
{
public:
    // Constructor / Destructor
    ThingCompSwirl(DrEngineWorld *engine_world, DrEngineThing *parent_thing,
                   DrColor color, float tint, float rotation_amount);
    virtual ~ThingCompSwirl() override;


    // #################### VARIABLES ####################
public:
    // Swirl Settings
    DrColor             start_color     { DrColor(128, 128, 255) };                     // Color tint of lens
    float               color_tint      { 0.5f };                                       // Color tint percentage     0 to  1
    float               rotation        { 0.0f };                                       // Amount of rotation for Swirl

    // #################### FUNCTIONS TO BE EXPOSED TO API ####################
public:
    // Basic Component Events
    virtual void        draw() override;                                                // Called when it is time to Render Thing


    // #################### INTERNAL FUNCTIONS ####################
public:
    // Getters / Setters


};

#endif // THING_COMP_SWIRL_H












