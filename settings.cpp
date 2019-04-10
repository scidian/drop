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

QString DrSettings::getName() {
    switch (getType()) {
    case DrType::World:  return getComponent(Components::World_Settings )->getProperty(Properties::World_Name )->getValue().toString();
    case DrType::Stage:  return getComponent(Components::Stage_Settings )->getProperty(Properties::Stage_Name )->getValue().toString();
    case DrType::Object: return getComponent(Components::Object_Settings)->getProperty(Properties::Object_Name)->getValue().toString();
    case DrType::Asset:  return getComponent(Components::Asset_Settings )->getProperty(Properties::Asset_Name )->getValue().toString();
    }
    return "Unknown Type, Can't Find Name";
}

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

void DrSettings::addComponent(Components component, QString display_name, QString description, QColor color, bool is_turned_on) {
    DrComponent *comp = new DrComponent(this, display_name, description, color, static_cast<long>(component), is_turned_on);
    m_components[static_cast<long>(component)] = comp;
}


//####################################################################################

void DrSettings::addPropertyToComponent(Components component, Properties property_number, Property_Type type,
                                        QVariant value, QString display_name, QString description, bool is_hidden, bool is_editable) {
    m_components[static_cast<long>(component)]->addProperty(property_number, type, value, display_name, description, is_hidden, is_editable);
}










