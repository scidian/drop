//
//      Created by Stephens Nunnally on 5/15/2020, (c) 2020 Scidian Studios, All Rights Reserved
//
//  File:
//
//
//
#include "project/dr_project.h"
#include "project/settings/settings.h"
#include "project/settings/settings_component.h"
#include "project/settings/settings_component_slot.h"


//####################################################################################
//##    Constructor
//####################################################################################
DrSlot::DrSlot(DrComponent *parent_component, DrSlotType slot_type, long slot_key, std::string name, bool is_editable) {
    m_parent_component = parent_component;
    m_parent_settings =  parent_component->getParentSettings();

    m_slot_type =       slot_type;
    m_slot_key =        slot_key;
    m_slot_name =       name;
    m_is_editable =     is_editable;
}


//####################################################################################
//##    Connection Functions
//####################################################################################
// Adds connection to output connection list, returns true if successful
bool DrSlot::addConnection(DrSlot *signal) {
    if (this->getSlotType() ==   DrSlotType::Signal) return false;
    if (signal->getSlotType() == DrSlotType::Output) return false;

    // If trying to connect to same DrComponent, fail
    long entity_key =       signal->getParentSettings()->getKey();
    long component_key =    signal->getParentComponent()->getComponentKey();
    long signal_key =       signal->getSlotKey();
    if (this->getParentSettings()->getKey() == entity_key && this->getParentComponent()->getComponentKey() == component_key) return false;

    // Check if we already have this connection
    for (auto it = m_connections.begin(); it != m_connections.end(); it++) {
        if (it->connected_entity_key == entity_key && it->connected_component_key == component_key && it->connected_signal_key == signal_key) {
            return true;
        }
    }

    // Otherwise add it... If multiple connecitons not allowed, clear existing connections
    if (this->multipleConnections() == false) this->clearConnections();
    Output_Connection connection;
        connection.connected_entity_key =       entity_key;
        connection.connected_component_key =    component_key;
        connection.connected_signal_key =       signal_key;
    m_connections.push_back(connection);
    return true;
}

bool DrSlot::addConnection(long entity_key, long component_key, long signal_key) {
    DrSlot *signal = getParentSettings()->getParentProject()->findSlotFromKeys(entity_key, component_key, signal_key);
    if (signal == nullptr) return false;
    return addConnection(signal);
}

void DrSlot::clearConnections() {
    m_connections.clear();
}

// Removes one connection from output conneciton list
void DrSlot::removeConnection(DrSlot *signal) {
    if (this->getSlotType() == DrSlotType::Signal) return;
    return removeConnection(signal->getParentSettings()->getKey(), signal->getParentComponent()->getComponentKey(), signal->getSlotKey());
}

void DrSlot::removeConnection(long entity_key, long component_key, long signal_key) {
    if (this->getSlotType() == DrSlotType::Signal) return;
    for (auto it = m_connections.begin(); it != m_connections.end(); ) {
        if ((*it).connected_entity_key == entity_key && (*it).connected_component_key == component_key && (*it).connected_signal_key == signal_key) {
            it = m_connections.erase(it);
        } else {
            it++;
        }
    }
}






















