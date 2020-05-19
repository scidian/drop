//
//      Created by Stephens Nunnally on 12/13/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      DrComponent Class Definitions
//
//
#include "core/dr_debug.h"
#include "project/constants_component_info.h"
#include "project/constants_messages.h"
#include "project/settings/settings.h"
#include "project/settings/settings_component.h"
#include "project/settings/settings_component_slot.h"
#include "project/settings/settings_component_property.h"


//####################################################################################
//##    Constructor / Destructor
//####################################################################################
DrComponent::DrComponent(DrSettings    *parent_settings,
                         long           component_key,
                         std::string    component_name,
                         std::string    display_name,
                         std::string    description,
                         DrColor        color,
                         bool           is_hidden) {
    m_parent_settings = parent_settings;

    m_component_key =   component_key;
    m_component_name =  component_name;
    m_display_name =    display_name;
    m_description =     description;
    m_color =           color;
    m_is_hidden =       is_hidden;
}

DrComponent::~DrComponent() {
    for (auto &property_pair:   m_properties)   { delete property_pair.second; }
    for (auto &signal_pair:     m_signals)      { delete signal_pair.second; }
    for (auto &output_pair:     m_outputs)      { delete output_pair.second; }
}



//####################################################################################
//##    Get Children
//####################################################################################
DrProperty* DrComponent::getProperty(std::string property_name, bool show_error) {
    auto it = m_properties.find(property_name);
    if (it == m_properties.end()) {
        if (show_error) {
            Dr::PrintDebug("Error! Property not found by name in component! \n"
                           "Property Name: \t" + property_name + "\n"
                           "Component Key: \t" + std::to_string(this->getComponentKey()) + "\n"
                           "Component Name: \t" + this->getComponentName() + "\n"
                           "Component Display Name: \t" + this->getDisplayName() + "\n"
                           "Entity Name: \t" + this->m_parent_settings->getName() + "\n"
                           "Entity Type: \t" + Dr::StringFromType(this->m_parent_settings->getType()) + "\n"
                           "End Error...");
        }
        return nullptr;
    }
    return it->second;
}

DrSlot* DrComponent::getSignal(long signal_key) {
    auto it = m_signals.find(signal_key);
    if (it != m_signals.end()) return it->second; else return nullptr;
}
DrSlot* DrComponent::getSignal(std::string signal_name) {
    for (auto &signal_pair : getSignalMap()) {
        if (signal_pair.second->getSlotName() == signal_name) return signal_pair.second;
    }
    return nullptr;
}

DrSlot* DrComponent::getOutput(long output_key) {
    auto it = m_outputs.find(output_key);
    if (it != m_outputs.end()) return it->second; else return nullptr;
}
DrSlot* DrComponent::getOutput(std::string output_name) {
    for (auto &output_pair : getOutputMap()) {
        if (output_pair.second->getSlotName() == output_name) return output_pair.second;
    }
    return nullptr;
}


//####################################################################################
//##    Building Functions
//####################################################################################
DrProperty* DrComponent::addProperty(std::string    property_name,
                                     Property_Type  type,
                                     DrVariant      value,
                                     std::string    display_name,
                                     std::string    description,
                                     bool is_hidden,
                                     bool is_editable) {
    DrProperty *prop = new DrProperty(m_parent_settings, this, this->getNextPropertyKey(), property_name, display_name, description, type, value, is_hidden, is_editable);
    prop->setListOrder(static_cast<int>(m_properties.size()));
    m_properties.insert(std::make_pair(property_name, prop));
    return prop;
}

DrSlot* DrComponent::addSignal(std::string signal_name, bool is_deletable) {
    DrSlot *signal = new DrSlot(this, DrSlotType::Signal, this->getNextSlotKey(), signal_name, is_deletable);
    m_signals.insert(std::make_pair(signal->getSlotKey(), signal));
    return signal;
}

DrSlot* DrComponent::addOutput(std::string output_name, bool multiple_connections, bool is_deletable) {
    DrSlot *output = new DrSlot(this, DrSlotType::Output, this->getNextSlotKey(), output_name, is_deletable);
    output->setMultipleConnections(multiple_connections);
    m_outputs.insert(std::make_pair(output->getSlotKey(), output));
    return output;
}


//####################################################################################
//##    Connects Output Slot to Signal Slot, returns true if successful
//####################################################################################
bool DrComponent::connectOutput(std::string output_name, DrSlot *signal) {
    DrSlot *output = getOutput(output_name);
        if (output == nullptr) return false;
    return output->addConnection(signal);
}

bool DrComponent::connectOutput(long output_key, DrSlot *signal) {
    DrSlot *output = getOutput(output_key);
        if (output == nullptr) return false;
    return output->addConnection(signal);
}














