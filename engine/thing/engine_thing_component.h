//
//      Created by Stephens Nunnally on 3/10/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef ENGINE_THING_COMPONENT_H
#define ENGINE_THING_COMPONENT_H

#include "engine/globals_engine.h"


//  Future signals:
//      When "emit"ing signal, add to world signal stack.
//      When building signal attach calling component / entity as owner.
//      When all other objects have processed signal, and we're back at emitting entity, delete signal from stack.
//

// Type Definitions
typedef std::map<std::string, DrVariant>    Attributes;


//####################################################################################
//##    DrThingComponent
//##        Base class for a component for a DrEngineThing
//############################
class DrThingComponent
{
public:
    // Constructor
    DrThingComponent(DrEngineWorld *engine_world, DrEngineThing *parent_thing, std::string component_name);
    virtual ~DrThingComponent();


    // #################### VARIABLES ####################
private:
    // External Borrowed Pointers
    DrEngineWorld      *m_world     { nullptr };                    // Pointer to the parent DrEngineWorld
    DrEngineThing      *m_thing     { nullptr };                    // Pointer to the parent DrEngineThing

    // Local Variables
    std::string         m_name;                                     // Name of this component
    Attributes          m_attributes;                               // Holds variables for this component by name (as std::string)


    // #################### FUNCTIONS TO BE EXPOSED TO API ####################
public:
    // Basic Component Events
//    virtual void      collide(cpArbiter *arb);
    virtual void        init();                                                                     // Called when component is first created
    virtual void        addToWorld();                                                               // Called when Thing is added to m_things DrEngineWorld vector
    virtual void        draw();                                                                     // Called when it is time to Render Thing
    virtual bool        update(double time_passed, double time_warp);                               // Called during DrEngineWorld->updateWorld() step, return true to remove m_thing
    virtual void        destroy();                                                                  // Called when component is destroyed

    // Component Functions
    virtual void        emitSignal(std::string name, DrVariant value, long thing_b = c_no_key);     // Adds signal to stack
    EngineSignals       signalList(std::string name);                                               // Returns list of signals with name

    // Getters
    std::string         name()      { return m_name; }                                              // Returns name of this Component
    DrEngineThing*      thing()     { return m_thing; }                                             // Returns parent DrEngineWorld
    DrEngineWorld*      world()     { return m_world; }                                             // Returns parent entity (Thing)

    // Attributes
    DrVariant           attribute(std::string attribute_name);                                      // Returns attribute by name
    void                setAttribute(std::string attribute_name, DrVariant value);                  // Sets attribute by name


    // #################### INTERNAL FUNCTIONS ####################
public:
    // Getters / Setters



};

#endif // ENGINE_THING_COMPONENT_H


















