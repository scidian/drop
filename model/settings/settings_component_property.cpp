//
//      Created by Stephens Nunnally on 12/13/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      DrProperty Class Definitions
//
#include "model/project/project.h"
#include "model/settings/settings.h"
#include "model/settings/settings_component.h"
#include "model/settings/settings_component_property.h"


//####################################################################################
//##    Constructor
//####################################################################################
DrProperty::DrProperty(DrSettings      *parent_settings,
                       DrComponent     *parent_component,
                       QString          display_name,
                       QString          description,
                       Property_Type    type,
                       DrVariant        value,
                       long             new_key,
                       bool             is_hidden,
                       bool             is_editable) {
    m_parent_settings =     parent_settings;
    m_parent_component =    parent_component;

    m_display_name =    display_name;
    m_description =     description;
    m_property_type =   type;
    m_value =           value;

    m_property_key =    new_key;

    m_is_hidden =       is_hidden;
    m_is_editable =     is_editable;
}


//####################################################################################
//##    Called when template function setValue() is called
//####################################################################################
void DrProperty::setValueInternal() {
    m_parent_settings->getParentProject()->setHasSaved(false);

}







