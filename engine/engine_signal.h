//
//      Created by Stephens Nunnally on 2/3/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef ENGINE_SIGNAL_H
#define ENGINE_SIGNAL_H

#include "core/types/dr_variant.h"
#include "engine/constants_engine.h"
#include "project/enums_entity_types.h"

// Forward Declarations
class DrEngineThing;

// Local Enums
enum class Signal_Life {
    Born                    = 0,                            // First pushed into messaging system
    Active                  = 1,                            // Ready to be processed by others
};


//####################################################################################
//##    DrEngineSignal
//##        Class that allows messages to be sent around Engine
//############################
class DrEngineSignal
{
public:
    // Constructor / Destructor
    DrEngineSignal(std::string name, DrVariant value, DrEngineThing *thing_a = nullptr, DrEngineThing *thing_b = nullptr);
    ~DrEngineSignal();


private:
    std::string         m_name;                                 // Name of this signal, can be from project/constants_signals.h, or custom
    DrVariant           m_value;                                // Event data
    Signal_Life         m_life;                                 // Tracks age of signal

    DrEngineThing*      m_thing_a;                              // Primary Thing that emiited signal (if there was one)
    DrEngineThing*      m_thing_b;                              // Primary Thing that emiited signal (if there was one)


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
    Signal_Life         getLife()                       { return m_life; }
    void                setLife(Signal_Life life)       { m_life = life; }

};

#endif // ENGINE_SIGNAL_H

























