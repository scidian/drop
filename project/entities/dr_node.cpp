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
DrNode::DrNode(DrNodeType node_type) {
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
void DrNode::addInputSlot(std::string new_slot_name, long connected_slot_key, std::string connected_slot_name) {
    for (auto &slot : m_input_slots) {
        if (slot.slot_name == new_slot_name) {
            slot.connected_key =    connected_slot_key;
            slot.connected_name =   connected_slot_name;
            return;
        }
    }
    DrSlot slot;
    slot.slot_name =        new_slot_name;
    slot.connected_key =    connected_slot_key;
    slot.connected_name =   connected_slot_name;
    m_input_slots.push_back(slot);
}

void DrNode::addOutputSlot(std::string new_slot_name, long connected_slot_key, std::string connected_slot_name) {
    for (auto &slot : m_output_slots) {
        if (slot.slot_name == new_slot_name) {
            slot.connected_key =    connected_slot_key;
            slot.connected_name =   connected_slot_name;
            return;
        }
    }
    DrSlot slot;
    slot.slot_name =        new_slot_name;
    slot.connected_key =    connected_slot_key;
    slot.connected_name =   connected_slot_name;
    m_output_slots.push_back(slot);
}







