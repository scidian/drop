//
//      Created by Stephens Nunnally on 5/5/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "project/entities/dr_node.h"
#include "project/entities/dr_world.h"


//####################################################################################
//##    Constructor / Destructor
//####################################################################################
DrNode::DrNode(DrNodeType node_type, long parent_key) {
    m_entity_key = parent_key;
    m_node_type = node_type;

    switch (node_type) {
        case DrNodeType::Random_Out:
            break;
        case DrNodeType::Timed_Pause:
            break;
        case DrNodeType::World:
            break;
    }
}

DrNode::~DrNode() { }



//####################################################################################
//##    Signals
//####################################################################################
void DrNode::addInputSlot(long owner_key, std::string parent_slot_name, long connected_slot_key, std::string connected_slot_name) {
    for (auto &slot : m_input_slots) {
        if (slot.owner_slot_name == parent_slot_name) {
            slot.owner_slot_type =      DrSlotType::Input;
            slot.connected_key =        connected_slot_key;
            slot.connected_slot_name =  connected_slot_name;
            return;
        }
    }
    OldSlot slot;
    slot.owner_slot_type =      DrSlotType::Input;
    slot.owner_key =            owner_key;
    slot.owner_slot_name =      parent_slot_name;
    slot.connected_key =        connected_slot_key;
    slot.connected_slot_name =  connected_slot_name;

    m_input_slots.push_back(slot);
}

void DrNode::addOutputSlot(long owner_key, std::string parent_slot_name, long connected_slot_key, std::string connected_slot_name) {
    for (auto &slot : m_output_slots) {
        if (slot.owner_slot_name == parent_slot_name) {
            slot.owner_slot_type =      DrSlotType::Output;
            slot.connected_key =        connected_slot_key;
            slot.connected_slot_name =  connected_slot_name;
            return;
        }
    }
    OldSlot slot;
    slot.owner_slot_type =      DrSlotType::Output;
    slot.owner_key =            owner_key;
    slot.owner_slot_name =      parent_slot_name;
    slot.connected_key =        connected_slot_key;
    slot.connected_slot_name =  connected_slot_name;

    m_output_slots.push_back(slot);
}







