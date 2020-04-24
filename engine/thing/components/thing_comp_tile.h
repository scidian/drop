//
//      Created by Stephens Nunnally on 4/2/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef THING_COMP_TILE_H
#define THING_COMP_TILE_H

#include "engine/globals_engine.h"
#include "engine/thing/engine_thing_component.h"


//####################################################################################
//##    ThingCompTile
//##        Built-In Component for DrEngineThing dealing with Tile item
//############################
class ThingCompTile : public DrThingComponent
{
public:
    // Constructor / Destructor
    ThingCompTile(DrEngineWorld *engine_world, DrEngineThing *parent_thing, long animation_key);
    virtual ~ThingCompTile() override;


    // #################### VARIABLES ####################
public:
    // Tile Variables
    long        m_animation_key         { c_no_key };                       // DrAnimation Key for Tile animation


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
    long        getAnimationKey()           { return m_animation_key; }
    void        setAnimationKey(long key)   { m_animation_key = key; }


};


#endif // THING_COMP_TILE_H












