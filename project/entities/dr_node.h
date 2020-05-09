//
//      Created by Stephens Nunnally on 5/5/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef DRNODE_H
#define DRNODE_H

#include "core/dr_containers.h"
#include "core/dr_string.h"
#include "core/types/dr_pointf.h"
#include "project/constants_signals.h"
#include "project/enums_entity_types.h"

// Local Struct
struct DrSlot {
    std::string     name        { "" };
    long            key         { c_no_key };
};

// Type Definitions
typedef std::list<DrSlot>       SlotMap;


//####################################################################################
//##    DrNode
//##        Class to hold Node info for World, UI, etc
//############################
class DrNode
{
private:
    // Slot Variables
    std::list<DrSlot>           m_input_slots;                              // List of input slots
    std::list<DrSlot>           m_output_slots;                             // List of output slots

    // Node Variables
    DrNodeType                  m_node_type;                                // Type of this DrNode
    DrPointF                    m_node_position     { 0, 0 };               // Position of this DrNode

public:
    // Constructor / Destructor
    DrNode(DrNodeType node_type);
    virtual ~DrNode();

    // Node Info
    DrNodeType          getDrNodeType()                 { return m_node_type; }
    DrPointF            getNodePosition()               { return m_node_position; }
    void                setNodePosition(DrPointF pos)   { m_node_position = pos; }

    // Slots
    SlotMap&            getInputSlots()                 { return m_input_slots; }
    SlotMap&            getOutputSlots()                { return m_output_slots; }
    void                addInputSlot( std::string slot_name, long from_key = c_no_key);
    void                addOutputSlot(std::string slot_name, long to_key = c_no_key);

};


#endif // DRNODE_H














