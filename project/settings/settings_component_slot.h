//
//      Created by Stephens Nunnally on 5/15/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//      DrSlots are Node Slots that can hold a Signal (Input) or Output Slot
//
//
#ifndef SETTINGS_COMPONENT_SLOT_H
#define SETTINGS_COMPONENT_SLOT_H

#include "core/types/dr_variant.h"
#include "project/constants_comps_and_props.h"
#include "project/enums_entity_types.h"

// Forward Declarations
class DrComponent;
class DrSettings;

// Local Struct
struct Output_Connection {
    long        connected_entity_key            { c_no_key };
    long        connected_component_key         { c_no_key };
    long        connected_signal_key            { c_no_key };
};

// Type Definitions
typedef std::list<Output_Connection> Output_Connections;


//####################################################################################
//##    DrSlot
//##        Class to hold an Output Slot for a DrComponent
//############################
class DrSlot
{
private:
    // External Borrowed Pointers
    DrSettings             *m_parent_settings;                                      // Points to the DrSettings entity that holds the component that holds this property
    DrComponent            *m_parent_component;                                     // Points to the DrComponent that holds this DrSlot

    // Slot Properties
    DrSlotType              m_slot_type             { DrSlotType::Signal };         // Slot type
    long                    m_slot_key              { c_no_key };                   // Unique ID key (unique to owner Component only), assigned by DrComponent
    std::string             m_slot_name             { "Untitled" };                 // Name of this Output Slot, displayed in Editor
    bool                    m_is_editable           { false };                      // Should this be editable / deletable in the Node Editor?

    // Output Slot Variables
    bool                    m_multiple_connections  { false };                      // Set to true to allow multiple connections in Node Editor
    Output_Connections      m_connections;                                          // Holds DrSlotType::Output connections to DrSignals


    // Internal Variables - UPDATED INTERNALLY BY EDITOR
    //                      DO NOT NEED TO BE SAVED
    std::set<DrSlot*>       m_recent_line_slots;                                    // Slots that were connected to this Slot last time WorldMapView::paintEvent() executed


public:
    // Constructor / Destructor
    DrSlot(DrComponent *parent_component, DrSlotType slot_type, long slot_key, std::string name, bool is_editable);

    // Getter / Setters
    DrSettings*         getParentSettings()         { return m_parent_settings; }
    DrComponent*        getParentComponent()        { return m_parent_component; }

    DrSlotType          getSlotType()               { return m_slot_type; }
    long                getSlotKey()                { return m_slot_key; }
    std::string         getSlotName()               { return m_slot_name; }
    bool                isEditable()                { return m_is_editable; }

    void                setSlotType(DrSlotType type)        { m_slot_type = type; }
    void                setSlotKey(long key)                { m_slot_key = key; }
    void                setSlotName(std::string new_name)   { m_slot_name = new_name; }


    // Connections - for DrSlotType::Output only
    bool                addConnection(DrSlot *signal);
    bool                addConnection(long entity_key, long component_key, long signal_key);
    void                clearConnections();
    void                removeConnection(DrSlot *signal);
    void                removeConnection(long entity_key, long component_key, long signal_key);

    Output_Connections& connections()                           { return m_connections; }

    bool                multipleConnections()                   { return m_multiple_connections; }
    void                setMultipleConnections(bool allowed)    { m_multiple_connections = allowed; }


    // Internal Functions
    void                addRecentLineSlot(DrSlot *slot)         { m_recent_line_slots.insert(slot); }
    void                clearRecentLineSlots()                  { m_recent_line_slots.clear(); }
    std::set<DrSlot*>&  recentLineSlots()                       { return m_recent_line_slots; }
    int                 recentLineSlotCount()                   { return static_cast<int>(m_recent_line_slots.size()); }


};


#endif // SETTINGS_COMPONENT_SLOT_H


















