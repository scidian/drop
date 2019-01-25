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
//##    addProperty functions, need one for each different Properties enum type
//####################################################################################
void DrComponent::addProperty(long setting, Property_Type new_type, QVariant new_value, QString new_display_name, QString new_description)
{
    DrProperty *prop = new DrProperty(m_parent_settings, this, new_display_name, new_description, new_type, new_value, setting);
    m_properties[setting] = prop;
}

void DrComponent::addProperty(World_Properties setting, Property_Type new_type, QVariant new_value, QString new_display_name, QString new_description)
{
    DrProperty *prop = new DrProperty(m_parent_settings, this, new_display_name, new_description, new_type, new_value, static_cast<int>(setting));
    m_properties[static_cast<int>(setting)] = prop;
}

void DrComponent::addProperty(Scene_Properties setting, Property_Type new_type, QVariant new_value, QString new_display_name, QString new_description)
{
    DrProperty *prop = new DrProperty(m_parent_settings, this, new_display_name, new_description, new_type, new_value, static_cast<int>(setting));
    m_properties[static_cast<int>(setting)] = prop;
}

void DrComponent::addProperty(Object_Properties setting, Property_Type new_type, QVariant new_value, QString new_display_name, QString new_description)
{
    DrProperty *prop = new DrProperty(m_parent_settings, this, new_display_name, new_description, new_type, new_value, static_cast<int>(setting));
    m_properties[static_cast<int>(setting)] = prop;
}







