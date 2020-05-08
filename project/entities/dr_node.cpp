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
void DrNode::addInputSlot(std::string signal_name, long from_key) {
    m_input_slots[signal_name] = from_key;
}

void DrNode::addOutputSlot(std::string signal_name, long to_key) {
    m_output_slots[signal_name] = to_key;
}







