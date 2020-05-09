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
void DrNode::addInputSlot(std::string slot_name, long from_key) {
    for (auto &slot : m_input_slots) {
        if (slot.name == slot_name) {
            slot.key = from_key;
            return;
        }
    }
    DrSlot slot;
    slot.name = slot_name;
    slot.key =  from_key;
    m_input_slots.push_back(slot);
}

void DrNode::addOutputSlot(std::string slot_name, long to_key) {
    for (auto &slot : m_output_slots) {
        if (slot.name == slot_name) {
            slot.key = to_key;
            return;
        }
    }
    DrSlot slot;
    slot.name = slot_name;
    slot.key =  to_key;
    m_output_slots.push_back(slot);
}







