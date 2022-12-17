//
//      Created by Stephens Nunnally on 12/13/18, (c) 2019 Scidian Studios, All Rights Reserved
//
//  File:
//      DrProperty Class Definitions
//
#include "project/dr_project.h"
#include "project/settings/settings.h"
#include "project/settings/settings_component.h"
#include "project/settings/settings_component_property.h"


//####################################################################################
//##    Constructor
//####################################################################################
DrProperty::DrProperty(DrSettings      *parent_settings,
                       DrComponent     *parent_component,
                       long             property_key,
                       std::string      property_name,
                       std::string      display_name,
                       std::string      description,
                       Property_Type    type,
                       DrVariant        value,
                       bool             is_hidden,
                       bool             is_editable) {
    m_parent_settings =     parent_settings;
    m_parent_component =    parent_component;

    m_property_key =    property_key;
    m_property_name =   property_name;
    m_display_name =    display_name;
    m_description =     description;
    m_property_type =   type;
    m_value =           value;

    m_is_hidden =       is_hidden;
    m_is_editable =     is_editable;
}


//####################################################################################
//##    Returns a std::pair of this properties parent component name, and this properties' name
//####################################################################################
ComponentProperty DrProperty::getCompPropPair() {
    if (m_parent_component != nullptr) {
        return std::make_pair(m_parent_component->getComponentName(), m_property_name);
    } else {
        return std::make_pair("", "");
    }
}


//####################################################################################
//##    Called when template function setValue() is called
//####################################################################################
void DrProperty::setValueInternal() {
    m_parent_settings->getParentProject()->setHasSaved(false);

}







