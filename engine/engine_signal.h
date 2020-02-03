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
    DrEngineSignal(std::string name, DrVariant value,
                   std::string thing_a_name = "", long thing_a_asset_key = c_no_key, long thing_a_engine_key = c_no_key,
                   std::string thing_b_name = "", long thing_b_asset_key = c_no_key, long thing_b_engine_key = c_no_key);
    ~DrEngineSignal();


private:
    std::string     m_name;                                 // Name of this signal, can be from project/constants_signals.h, or custom
    DrVariant       m_value;                                // Event data
    Signal_Life     m_life;                                 // Tracks age of signal

    std::string     m_thing_a_name        { "" };           // Name of Primary Thing that emiited signal
    long            m_thing_a_asset_key   { c_no_key };     // Project Asset ID Key of Primary Thing that emiited signal
    long            m_thing_a_engine_key  { c_no_key };     // Engine ID Key of Primary Thing that emiited signal

    std::string     m_thing_b_name        { "" };           // Name of Primary Thing that emiited signal
    long            m_thing_b_asset_key   { c_no_key };     // Project Asset ID Key of Primary Thing that emiited signal
    long            m_thing_b_engine_key  { c_no_key };     // Engine ID Key of Primary Thing that emiited signal


    // #################### FUNCTIONS TO BE EXPOSED TO API ####################
public:
    std::string     name()                          { return m_name; }
    DrVariant&      value()                         { return m_value; }

    std::string     thingNameA()                     { return m_thing_a_name; }
    long            thingAssetKeyA()                 { return m_thing_a_asset_key; }
    long            thingEngineKeyA()                { return m_thing_a_engine_key; }

    std::string     thingNameB()                     { return m_thing_b_name; }
    long            thingAssetKeyB()                 { return m_thing_b_asset_key; }
    long            thingEngineKeyB()                { return m_thing_b_engine_key; }


    // #################### INTERNAL FUNCTIONS ####################
public:
    // Getters / Setters
    Signal_Life     getLife()                       { return m_life; }
    void            setLife(Signal_Life life)       { m_life = life; }

};

#endif // ENGINE_SIGNAL_H

























