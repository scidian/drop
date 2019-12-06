//
//      Created by Stephens Nunnally on 12/13/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      DrComponent Class Definitions
//
//
#include "editor/constants_editor.h"
#include "editor/helper_editor.h"
#include "model/settings/settings.h"
#include "model/settings/settings_component.h"
#include "model/settings/settings_component_property.h"


//####################################################################################
//##    Constructor, Destructor
//####################################################################################
DrComponent::DrComponent(DrSettings *parent_settings,
                         QString new_display_name,
                         QString new_description,
                         QColor new_color,
                         long new_key,
                         bool new_turned_on) {
    m_parent_settings = parent_settings;

    m_display_name = new_display_name;
    m_description = new_description;
    m_color = new_color;
    m_component_key = new_key;
    m_turned_on = new_turned_on;
}

DrComponent::~DrComponent() {
    for (auto i: m_properties) { delete i.second; }
}



//####################################################################################
//##    Get Property
//####################################################################################
DrProperty* DrComponent::getProperty(Properties setting) { return getProperty(static_cast<long>(setting)); }
DrProperty* DrComponent::getProperty(long setting) {
    auto it = m_properties.find(setting);
    if (it == m_properties.end()) {
        Dr::ShowMessageBox("ERROR! CODE: " + Error_Code::NoProperty + "\n\n"
                           "Property not found in object / component \n\n"
                           "Property ID: \t" +    QString::number(setting) + "\n"
                           "Component Name: \t" + this->getDisplayName() + "\n"
                           "Component ID: \t" +   QString::number(this->getComponentKey()) + "\n"
                           "Object Name: \t" + this->m_parent_settings->getName() + "\n"
                           "Object Type: \t" + Dr::StringFromType(this->m_parent_settings->getType()) + "\n");
    }
    return m_properties[setting];
}




//####################################################################################
//##    addProperty functions
//####################################################################################
DrProperty* DrComponent::addProperty(Properties setting,
                                     Property_Type type,
                                     QVariant value,
                                     QString display_name,
                                     QString description,
                                     bool is_hidden,
                                     bool is_editable) {
    DrProperty *prop = new DrProperty(m_parent_settings, this, display_name, description, type, value, static_cast<int>(setting), is_hidden, is_editable);
    prop->setListOrder( static_cast<int>(m_properties.size()) );
    m_properties[static_cast<int>(setting)] = prop;
    return prop;
}















