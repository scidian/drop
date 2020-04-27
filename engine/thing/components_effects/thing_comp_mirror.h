//
//      Created by Stephens Nunnally on 2/28/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef THING_COMP_MIRROR_H
#define THING_COMP_MIRROR_H

#include "3rd_party/chipmunk/chipmunk.h"
#include "engine/globals_engine.h"
#include "engine/thing/engine_thing_component.h"


//####################################################################################
//##    ThingCompMirror
//##        Built-In Component for DrEngineThing dealing with Mirror effect
//############################
class ThingCompMirror : public DrThingComponent
{
public:
    // Constructor / Destructor
    ThingCompMirror(DrEngineWorld *engine_world, DrEngineThing *parent_thing,
                    DrColor color_1, DrColor color_2, float tint_amount, float blur, float blur_stretch, float mirror_scale);
    virtual ~ThingCompMirror() override;


    // #################### VARIABLES ####################
public:
    // Mirror Variables
    DrColor             start_color     { DrColor(192, 192, 192) };                     // Color tint of top of mirror
    DrColor             end_color       { DrColor(128, 128, 255) };                     // Color tint of bottom of mirror
    float               tint_percent    { 0.5f };                                       // Color tint percentage     0 to  1
    float               blur            { 0.0f };                                       // Scatter amount
    float               blur_stretch    { 0.0f };                                       // Scatter stretch away from start
    float               scale           { 1.0f };                                       // Noise (scatter) scale


    // #################### FUNCTIONS TO BE EXPOSED TO API ####################
public:
    // Basic Component Events
    virtual void        draw() override;                                                // Called when it is time to Render Thing


    // #################### INTERNAL FUNCTIONS ####################
public:
    // Getters / Setters


};


#endif // THING_COMP_MIRROR_H



















