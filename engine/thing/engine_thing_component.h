//
//      Created by Stephens Nunnally on 3/10/2020, (c) 2020 Scidian Studios, All Rights Reserved
//
//  File:
//
//
//
#ifndef ENGINE_THING_COMPONENT_H
#define ENGINE_THING_COMPONENT_H

#include "engine/globals_engine.h"


//  Future messages:
//      When "emit"ing messages, add to world message stack.
//      When building message attach calling component / entity as owner.
//      When all other objects have processed message, and we're back at emitting entity, delete message from stack.
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
    DrEngineWorld      *m_world         { nullptr };                        // Pointer to the parent DrEngineWorld
    DrEngineThing      *m_thing         { nullptr };                        // Pointer to the parent DrEngineThing

    // Component Usage Flags
    bool                m_call_draw     { true };                           // When classes are using base empty functions, this is false and functions are skipped
    bool                m_call_update   { true };                           // When classes are using base empty functions, this is false and functions are skipped

    // Local Variables
    std::string         m_name;                                             // Name of this component
    Attributes          m_attributes;                                       // Holds variables for this component by name (as std::string)


    // #################### FUNCTIONS TO BE EXPOSED TO API ####################
public:
    // Component Events
    virtual void        init();                                                                     // Called when component is first created
    virtual void        addToWorld();                                                               // Called when Thing is added to m_things DrEngineWorld vector
    virtual void        draw();                                                                     // Called when it is time to Render Thing
    virtual bool        update(double time_passed, double time_warp);                               // Called during DrEngineWorld->updateWorld() step, return true to remove m_thing
    virtual void        destroy();                                                                  // Called when component is destroyed

    // Messages
    virtual void        emitMessage(std::string name, DrVariant value, DrEngineThing *thing_b);     // Adds message to stack
    EngineMessages&     messageList(std::string name, long thing_key = c_no_key);                   // Returns list of messages with name, optional attached to thing

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
    bool                callDrawFunction()      { return m_call_draw; }
    bool                callUpdateFunction()    { return m_call_update; }


};

#endif // ENGINE_THING_COMPONENT_H


















