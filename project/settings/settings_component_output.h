//
//      Created by Stephens Nunnally on 5/15/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//      Outputs are OUTPUTS that send signals to activate DrSignals
//
//
#ifndef SETTINGS_COMPONENT_OUTPUT_H
#define SETTINGS_COMPONENT_OUTPUT_H

#include "core/types/dr_variant.h"
#include "project/constants_comps_and_props.h"
#include "project/enums_entity_types.h"

// Forward Declarations
class DrComponent;
class DrSettings;

// Local Struct
struct Output_Connection {
    long        connected_entity_key        { c_no_key };
    long        connected_component_key     { c_no_key };
    long        connected_signal_key        { c_no_key };
};

// Type Definitions
typedef std::list<Output_Connection>  Output_Connections;


//####################################################################################
//##    DrOutput
//##        Class to hold an Output Slot for a DrComponent
//############################
class DrOutput
{
private:
    // External Borrowed Pointers
    DrSettings         *m_parent_settings;                                          // Points to the DrSettings entity that holds the component that holds this property
    DrComponent        *m_parent_component;                                         // Points to the DrComponent that holds this DrSlot

    // Slot Properties
    long                m_output_key            { c_no_key };                       // Unique ID key (unique to owner Component only), assigned by DrComponent
    std::string         m_output_name           { "Untitled" };                     // Name of this Output Slot, displayed in Editor
    bool                m_is_deletable          { false };                          // Should this be deletable in the Node Editor?
    Output_Connections  m_connections;                                              // Holds DrSignal connections


public:
    // Constructor / Destructor
    DrOutput(DrComponent *parent_component, long output_key, std::string name, bool is_deletable);

    // Getter / Setters
    DrSettings*         getParentSettings()         { return m_parent_settings; }
    DrComponent*        getParentComponent()        { return m_parent_component; }

    long                getOutputKey()              { return m_output_key; }
    std::string         getOutputName()             { return m_output_name; }
    bool                isDeletable()               { return m_is_deletable; }
    void                setOutputName(std::string new_name) { m_output_name = new_name; }

    // Connections
    void                addConnection(long entity_key, long component_key, long signal_key);
    void                clearConnections();
    void                removeConnection(long entity_key, long component_key, long signal_key);

};


#endif // SETTINGS_COMPONENT_OUTPUT_H


















