//
//      Created by Stephens Nunnally on 12/13/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      DrSettings Class Definitions
//
//

#include "settings.h"
#include "settings_component.h"
#include "settings_component_property.h"

//####################################################################################
//##    Constructor, Destructor
//####################################################################################
DrSettings::DrSettings()
{

}

DrSettings::~DrSettings()
{
    for (auto i: m_components) { delete i.second; }
}



//####################################################################################
//##    Component / Property fetching
//####################################################################################

DrProperty* DrSettings::getComponentProperty(long component, long property) { return m_components[component]->getProperty(property); }
DrProperty* DrSettings::getComponentProperty(Components component, Properties property) { return m_components[static_cast<long>(component)]->getProperty(property); }

QVariant DrSettings::getComponentPropertyValue(long component, long property) { return m_components[component]->getProperty(property)->getValue(); }
QVariant DrSettings::getComponentPropertyValue(Components component, Properties property) { return m_components[static_cast<long>(component)]->getProperty(property)->getValue(); }
void DrSettings::setComponentPropertyValue(long component, long property, QVariant value) { m_components[component]->getProperty(property)->setValue(value); }
void DrSettings::setComponentPropertyValue(Components component, Properties property, QVariant value) { m_components[static_cast<long>(component)]->getProperty(property)->setValue(value); }

QString DrSettings::getWorldName() { return m_components[static_cast<long>(Components::World_Settings)]->getProperty(Properties::World_Name)->getValue().toString(); }
QString DrSettings::getStageName() { return m_components[static_cast<long>(Components::Stage_Settings)]->getProperty(Properties::Stage_Name)->getValue().toString(); }
QString DrSettings::getObjectName() { return m_components[static_cast<long>(Components::Object_Settings)]->getProperty(Properties::Object_Name)->getValue().toString(); }
QString DrSettings::getAssetName() { return m_components[static_cast<long>(Components::Asset_Settings)]->getProperty(Properties::Asset_Name)->getValue().toString(); }

DrProperty* DrSettings::findPropertyFromPropertyKey(long property_key_to_find)
{
    for (auto component: m_components) {
        for (auto property: component.second->getPropertyList()) {
            if (property.second->getPropertyKey() == property_key_to_find) {
                return property.second;
            }
        }
    }
    return nullptr;
}

DrComponent* DrSettings::findComponentFromPropertyKey(long property_key_to_find)
{
    return findPropertyFromPropertyKey(property_key_to_find)->getParentComponent();
}


//####################################################################################
//##    Component Loading - addComponent / addComponentProperty
//####################################################################################

void DrSettings::addComponent(long component, QString new_display_name, QString new_description, QColor new_color, bool is_turned_on) {
    DrComponent *comp = new DrComponent(this, new_display_name, new_description, new_color, component, is_turned_on);
    m_components[component] = comp;
}

void DrSettings::addComponent(Components component, QString new_display_name, QString new_description, QColor new_color, bool is_turned_on) {
    DrComponent *comp = new DrComponent(this, new_display_name, new_description, new_color, static_cast<long>(component), is_turned_on);
    m_components[static_cast<long>(component)] = comp;
}


//####################################################################################

void DrSettings::addPropertyToComponent(long component, long property_number, Property_Type new_type,
                                        QVariant new_value, QString new_display_name, QString new_description, bool is_hidden) {
    m_components[component]->addProperty(property_number, new_type, new_value, new_display_name, new_description, is_hidden);
}

void DrSettings::addPropertyToComponent(Components component, Properties property_number, Property_Type new_type,
                                        QVariant new_value, QString new_display_name, QString new_description, bool is_hidden) {
    m_components[static_cast<long>(component)]->addProperty(property_number, new_type, new_value, new_display_name, new_description, is_hidden);
}










