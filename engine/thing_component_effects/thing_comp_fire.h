//
//      Created by Stephens Nunnally on 3/10/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef THING_COMP_FIRE_H
#define THING_COMP_FIRE_H

#include "3rd_party/chipmunk/chipmunk.h"
#include "engine/globals_engine.h"
#include "engine/thing/engine_thing_component.h"


//####################################################################################
//##    ThingCompFire
//##        Built-In Component for DrEngineThing dealing with Fire effect
//############################
class ThingCompFire : public DrThingComponent
{
public:
    // Constructor / Destructor
    ThingCompFire(DrEngineWorld *engine_world, DrEngineThing *parent_thing,
                  Fire_Mask mask, DrColor color_1, DrColor color_2, DrColor smoke, float color_intensity, float smooth, float wave, float speed);
    virtual ~ThingCompFire() override;


    // #################### VARIABLES ####################
public:
    // Fire Variables
    Fire_Mask           fire_mask       { Fire_Mask::Torch };                           // Shape mask to use for this fire
    DrColor             start_color     { DrColor(0, 0, 0) };                           // Color 1 of fire
    DrColor             end_color       { DrColor(0, 0, 0) };                           // Color 2 of fire
    DrColor             smoke_color     { DrColor(0, 0, 0) };                           // Background Color of fire
    float               intensity       { 0.5f };                                       // How much percent wise to add in alpha noise
    float               smoothness      { 0.2f };                                       // How smooth to make the flames
    float               wavy            { 0.5f };                                       // How wavyness to make the flames
    float               flame_speed     { 7.0f };                                       // Flame speed


    // #################### FUNCTIONS TO BE EXPOSED TO API ####################
public:
    // Basic Component Events
    virtual void        init() override;                                                // Called when component is first created
    virtual void        addToWorld() override;                                          // Called when Thing is added to m_things DrEngineWorld vector
    virtual void        draw() override;                                                // Called when it is time to Render Thing
    virtual void        update(double time_passed, double time_warp) override;          // Called during DrEngineWorld->updateWorld() step
    virtual void        destroy() override;                                             // Called when component is destroyed


    // #################### INTERNAL FUNCTIONS ####################
public:
    // Getters / Setters


};

#endif // THING_COMP_FIRE_H



















