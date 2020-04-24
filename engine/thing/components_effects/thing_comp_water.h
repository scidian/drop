//
//      Created by Stephens Nunnally on 3/10/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef THING_COMP_WATER_H
#define THING_COMP_WATER_H

#include "3rd_party/chipmunk/chipmunk.h"
#include "engine/globals_engine.h"
#include "engine/thing/engine_thing_component.h"


//####################################################################################
//##    ThingCompWater
//##        Built-In Component for DrEngineThing dealing with Water effect
//############################
class ThingCompWater : public DrThingComponent
{
public:
    // Constructor / Destructor
    ThingCompWater(DrEngineWorld *engine_world, DrEngineThing *parent_thing,
                   Water_Texture texture, DrColor start_color, DrColor end_color,
                   float    tint,           float reflection,
                   float    rip_length,     float rip_speed,        float rip_amplitude,    float rip_stretch,
                   float    w_length,       float w_speed,          float w_amplitude,
                   DrColor  foam_color,     float foam_tint,        float foam_height,      bool foam_flat,
                   float    r_reflection,   float r_underwater,     float r_texture,        float r_foam,
                   float    texture_movement);
    virtual ~ThingCompWater() override;


    // #################### VARIABLES ####################
public:
    // Water Settings
    Water_Texture       water_texture           { Water_Texture::Ripples };             // Texture to use
    DrColor             start_color             { DrColor(128, 128, 255) };             // Color of top of water
    DrColor             end_color               { DrColor(128, 128, 255) };             // Color of bottom of water
    float               water_tint              { 0.5f };                               // Color tint percentage     0 to 1
    float               reflection_opacity      { 0.5f };                               // Reflection opacity        0 to 1

    float               ripple_frequency        { 100.0 };                              // Ripple frequency         25 to 100 is pretty good
    float               ripple_speed            { 50.0 };                               // Ripple speed              0 to 100 is pretty good
    float               ripple_amplitude        { 25.0 };                               // Ripple amplitude          0 to 100 is pretty good
    float               ripple_stretch          { 25.0 };                               // Ripple stretch            0 to 100 is pretty good

    float               wave_frequency          { 20.0 };                               // Wave frequency
    float               wave_speed              { 20.0 };                               // Wave frequency
    float               wave_amplitude          { 20.0 };                               // Wave frequency

    DrColor             surface_color           { DrColor(255, 255, 255) };             // Surface color
    float               surface_tint            { 0.5f };                               // Surface tint percentage
    float               surface_height          { 5.0f };                               // Surface height
    bool                surface_keep_flat       { false };                              // Keep top of water flat?

    float               refract_reflection      { 1.0f };
    float               refract_underwater      { 1.0f };
    float               refract_texture         { 1.0f };
    float               refract_foam            { 1.0f };

    float               movement_speed          { 0.0f };                               // Moves texture left / right


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


};


#endif // THING_COMP_WATER_H
























