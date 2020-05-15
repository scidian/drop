//
//      Created by Stephens Nunnally on 5/15/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//      Signals are INPUTS that receive signals from DrOutput
//
//
#ifndef DRSIGNAL_H
#define DRSIGNAL_H

#include "core/types/dr_variant.h"
#include "project/constants_comps_and_props.h"
#include "project/enums_entity_types.h"

// Forward Declarations
class DrComponent;
class DrSettings;


//####################################################################################
//##    DrSignal
//##        Class to hold an Input Slot for a DrComponent
//############################
class DrSignal
{
private:
    // External Borrowed Pointers
    DrSettings         *m_parent_settings;                                          // Points to the DrSettings entity that holds the component that holds this property
    DrComponent        *m_parent_component;                                         // Points to the DrComponent that holds this DrSlot

    // Slot Properties
    long                m_signal_key            { c_no_key };                       // Unique ID key (unique to owner Component only), assigned by DrComponent
    std::string         m_signal_name           { "Untitled" };                     // Name of this Input Signal Slot, displayed in Editor
    bool                m_is_deletable          { false };                          // Should this be deletable in the Node Editor?


public:
    // Constructor / Destructor
    DrSignal(DrComponent *parent_component, long signal_key, std::string name, bool is_deletable);

    // Getter / Setters
    DrSettings*         getParentSettings()         { return m_parent_settings; }
    DrComponent*        getParentComponent()        { return m_parent_component; }

    long                getSignalKey()              { return m_signal_key; }
    std::string         getSignalName()             { return m_signal_name; }
    bool                isDeletable()               { return m_is_deletable; }
    void                setInputName(std::string new_name) { m_signal_name = new_name; }

};



#endif // DRSIGNAL_H












