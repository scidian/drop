//
//      Created by Stephens Nunnally on 5/15/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef ENGINE_MESSAGE_H
#define ENGINE_MESSAGE_H

#include "core/types/dr_variant.h"
#include "engine/constants_engine.h"
#include "project/enums_entity_types.h"

// Forward Declarations
class DrEngineThing;

// Local Enums
enum class Message_Life {
    Born                    = 0,                            // First pushed into messaging system
    Active                  = 1,                            // Ready to be processed by others
};


//####################################################################################
//##    DrEngineMessage
//##        Class that allows messages to be sent around Engine
//############################
class DrEngineMessage
{
public:
    // Constructor / Destructor
    DrEngineMessage(std::string name, DrVariant value, DrEngineThing *thing_a = nullptr, DrEngineThing *thing_b = nullptr);
    ~DrEngineMessage();


private:
    std::string         m_name;                                 // Name of this message, can be from project/constants_messages.h, or custom
    DrVariant           m_value;                                // Event data
    Message_Life        m_life;                                 // Tracks age of message

    DrEngineThing*      m_thing_a;                              // Primary Thing that emitted message (if there was one)
    DrEngineThing*      m_thing_b;                              // Secondary Thing that interated with Primary Thing (if there was one)


    // #################### FUNCTIONS TO BE EXPOSED TO API ####################
public:
    std::string         name()                          { return m_name; }
    DrVariant&          value()                         { return m_value; }

    DrEngineThing*      thingA()                        { return m_thing_a; }
    DrEngineThing*      thingB()                        { return m_thing_b; }

    std::string         thingNameA();
    std::string         thingNameB();
    long                thingAssetKeyA();
    long                thingAssetKeyB();


    // #################### INTERNAL FUNCTIONS ####################
public:
    // Getters / Setters
    Message_Life        getLife()                       { return m_life; }
    void                setLife(Message_Life life)      { m_life = life; }

};

#endif // ENGINE_MESSAGE_H












