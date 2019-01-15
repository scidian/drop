//
//      Created by Stephens Nunnally on 12/13/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      DrProperty Class Definitions
//
//

#include "settings.h"
#include "settings_component.h"
#include "settings_component_property.h"

//####################################################################################
//##    Constructor
//####################################################################################
DrProperty::DrProperty(DrSettings *parent_settings, DrComponent *parent_component,
                       std::string new_display_name,
                       std::string new_description,
                       Property_Type new_type,
                       QVariant new_value,
                       long new_key)
{
    m_parent_settings = parent_settings;
    m_parent_component = parent_component;

    m_display_name = new_display_name;
    m_description = new_description;
    m_preferred_type = new_type;
    m_value = new_value;
    m_property_key = new_key;
}




