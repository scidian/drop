//
//      Created by Stephens Nunnally on 5/12/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef DRSLOT
#define DRSLOT

#include "core/types/dr_variant.h"
#include "project/constants_comps_and_props.h"
#include "project/enums_entity_types.h"

// Forward Declarations
class DrComponent;
class DrSettings;

//struct DrSlot {
//    DrSlotType      owner_slot_type         { DrSlotType::Input };
//    long            owner_key               { c_no_key };
//    std::string     owner_slot_name         { "" };
//    long            connected_key           { c_no_key };
//    std::string     connected_slot_name     { "" };
//    DrPointF        scene_position          { 0, 0 };
//};


// Local Struct
struct Slot_Connection {
    long            connected_entity_key        { c_no_key };
    std::string     connected_component;
    long            connected_slot_key          { c_no_key };
};

// Type Definitions
typedef std::list<Slot_Connection>  Connections;


class DrSlot
{
private:
    // External Borrowed Pointers
    DrSettings         *m_parent_settings;                                          // Points to the DrSettings entity that holds the component that holds this property
    DrComponent        *m_parent_component;                                         // Points to the DrComponent that holds this DrSlot

    // Slot Properties
    long                m_slot_key              { c_no_key };                       // Unique ID key (unique to owner component only), assigned by Dr Component
    DrSlotType          m_slot_type             { DrSlotType::Input };              // Input or Output??
    std::string         m_display_name          { "Untitled" };                     // Name of this slot, used to pass as Signal, also to be displayed in Editor
    Connections         m_connections;                                              // Holds connections if this is a DrSlotType::Output slot
    bool                m_is_deletable          { false };                          // Should this be deletable in the Node Editor


public:
    // Constructor / Destructor
    DrSlot(DrSettings *parent_settings, DrComponent *parent_component);

    // Getters / Setters
    DrSettings*         getParentSettings() { return m_parent_settings; }
    DrComponent*        getParentComponent() { return m_parent_component; }

};


#endif // DRSLOT























