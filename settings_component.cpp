//
//      Created by Stephens Nunnally on 12/13/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      DrComponent Class Definitions
//
//
#include "settings.h"
#include "settings_component.h"
#include "settings_component_property.h"

//####################################################################################
//##    Constructor, Destructor
//####################################################################################
DrComponent::DrComponent(DrSettings *parent_settings,
                         QString new_display_name,
                         QString new_description,
                         QColor new_color,
                         long new_key,
                         bool new_turned_on)
{
    m_parent_settings = parent_settings;

    m_display_name = new_display_name;
    m_description = new_description;
    m_color = new_color;
    m_component_key = new_key;
    m_turned_on = new_turned_on;
}

DrComponent::~DrComponent()
{
    for (auto i: m_properties) { delete i.second; }
}



//####################################################################################
//##    setProperty functions
//####################################################################################
void DrComponent::setProperty(long setting, QVariant value) { m_properties[setting]->setValue(value); }
void DrComponent::setProperty(Properties setting, QVariant value) { m_properties[static_cast<int>(setting)]->setValue(value); }



//####################################################################################
//##    addProperty functions
//####################################################################################
void DrComponent::addProperty(long setting, Property_Type new_type, QVariant new_value, QString new_display_name, QString new_description, bool is_hidden)
{
    DrProperty *prop = new DrProperty(m_parent_settings, this, new_display_name, new_description, new_type, new_value, setting, is_hidden);
    m_properties[setting] = prop;
}

void DrComponent::addProperty(Properties setting, Property_Type new_type, QVariant new_value, QString new_display_name, QString new_description, bool is_hidden)
{
    DrProperty *prop = new DrProperty(m_parent_settings, this, new_display_name, new_description, new_type, new_value, static_cast<int>(setting), is_hidden);
    m_properties[static_cast<int>(setting)] = prop;
}















