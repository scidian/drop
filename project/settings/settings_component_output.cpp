//
//      Created by Stephens Nunnally on 5/15/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "project/settings/settings_component.h"
#include "project/settings/settings_component_output.h"


//####################################################################################
//##    Constructor
//####################################################################################
DrOutput::DrOutput(DrComponent *parent_component, long output_key, std::string name, bool is_deletable) {
    m_parent_component = parent_component;
    m_parent_settings =  parent_component->getParentSettings();

    m_output_key =      output_key;
    m_output_name =     name;
    m_is_deletable =    is_deletable;
}


//####################################################################################
//##    Connection Functions
//####################################################################################
// Adds connection to output connection list
void DrOutput::addConnection(long entity_key, long component_key, long signal_key) {
    // Check if we already have this connection
    for (auto it = m_connections.begin(); it != m_connections.end(); it++) {
        if ((*it).connected_entity_key == entity_key && (*it).connected_component_key == component_key && (*it).connected_signal_key == signal_key) {
            return;
        }
    }
    // Otherwise add it
    Output_Connection connection;
    connection.connected_entity_key =       entity_key;
    connection.connected_component_key =    component_key;
    connection.connected_signal_key =       signal_key;
    m_connections.push_back(connection);
}

void DrOutput::clearConnections() {
    m_connections.clear();
}

// Removes one connection from output conneciton list
void DrOutput::removeConnection(long entity_key, long component_key, long signal_key) {
    for (auto it = m_connections.begin(); it != m_connections.end(); ) {
        if ((*it).connected_entity_key == entity_key && (*it).connected_component_key == component_key && (*it).connected_signal_key == signal_key) {
            it = m_connections.erase(it);
        } else {
            it++;
        }
    }
}






















