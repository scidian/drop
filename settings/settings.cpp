//
//      Created by Stephens Nunnally on 12/13/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      DrSettings Class Definitions
//
//
#include "constants.h"
#include "enums.h"
#include "helper.h"
#include "helper_qt.h"
#include "settings.h"
#include "settings_component.h"
#include "settings_component_property.h"


//####################################################################################
//##    Constructor, Destructor
//####################################################################################
DrSettings::DrSettings() {
    addComponentHiddenSettings();
}

DrSettings::~DrSettings() {
    for (auto i: m_components) { delete i.second; }
}



//####################################################################################
//##    Component / Property fetching
//####################################################################################
void DrSettings::setComponentPropertyValue(long component, long property, QVariant value) { m_components[component]->getProperty(property)->setValue(value); }
void DrSettings::setComponentPropertyValue(Components component, Properties property, QVariant value) { m_components[static_cast<long>(component)]->getProperty(property)->setValue(value); }

QVariant DrSettings::getComponentPropertyValue(long component, long property) {
    return getComponentProperty(component, property)->getValue(); }
QVariant DrSettings::getComponentPropertyValue(Components component, Properties property) {
    return getComponentProperty(static_cast<long>(component), static_cast<long>(property))->getValue(); }
DrProperty* DrSettings::getComponentProperty(Components component, Properties property) {
    return getComponentProperty(static_cast<long>(component), static_cast<long>(property)); }

DrProperty* DrSettings::getComponentProperty(long component, long property) {
    auto it = m_components.find(component);
    if (it == m_components.end()) {
        Dr::ShowMessageBox("ERROR! CODE: " + Error_Code::NoComponent + "\n\n"
                           "Component not found in current object \n\n"
                           "Component ID: \t" + QString::number(component) + "\n"
                           "Object Name: \t" + this->getName() + ". \n"
                           "Object Type: \t" + Dr::StringFromType(this->getType()) + "\n");
    }
    return m_components[component]->getProperty(property);
}

DrProperty* DrSettings::findPropertyFromPropertyKey(long property_key_to_find) {
    for (auto component: m_components) {
        for (auto property: component.second->getPropertyList()) {
            if (property.second->getPropertyKey() == property_key_to_find) {
                return property.second;
            }
        }
    }
    return nullptr;
}

DrComponent* DrSettings::findComponentFromPropertyKey(long property_key_to_find) {
    return findPropertyFromPropertyKey(property_key_to_find)->getParentComponent();
}


//####################################################################################
//##    Returns Name / Sets Name from shared "Entity_Name" component
//####################################################################################
QString DrSettings::getName() {
    DrComponent *name_component;
    DrProperty  *name_property;
    switch (getType()) {
        case DrType::World:
        case DrType::Stage:
        case DrType::StartStage:
        case DrType::Thing:
        case DrType::Asset:
            name_component = getComponent(Components::Entity_Name);
            if (!name_component) return "No Name Component";
            name_property  = name_component->getProperty(Properties::Entity_Name);
            if (!name_property)  return "No Name Property";
            return name_property->getValue().toString();
        case DrType::Image:     return "DrImage - Unknown Name";
        case DrType::Font:      return "DrFont - Unknown Name";
        case DrType::NotFound:  return "Type NotFound";
        case DrType::BaseClass: return "Base DrSettings Class";
    }
    return "Unknown Type, Can't Find Name";
}

// Returns Name from shared "Entity_Name" component
void DrSettings::setName(QString new_name) {
    DrComponent *name_component;
    DrProperty  *name_property;
    name_component = getComponent(Components::Entity_Name);
    if (!name_component) return;
    name_property  = name_component->getProperty(Properties::Entity_Name);
    if (!name_property)  return;
    name_property->setValue(new_name);
}


//####################################################################################
//##    Component Loading - addComponent / addComponentProperty
//####################################################################################
void DrSettings::addComponent(Components component, QString display_name, QString description, QColor color, bool is_turned_on) {
    DrComponent *comp = new DrComponent(this, display_name, description, color, static_cast<long>(component), is_turned_on);
    m_components[static_cast<long>(component)] = comp;
}

void DrSettings::addPropertyToComponent(Components component, Properties property_number, Property_Type type,
                                        QVariant value, QString display_name, QString description, bool is_hidden, bool is_editable) {
    m_components[static_cast<long>(component)]->addProperty(property_number, type, value, display_name, description, is_hidden, is_editable);
}

void DrSettings::addComponentHiddenSettings() {
    addComponent(Components::Hidden_Settings, "Hidden Settings", "Custom hidden entity properties for Droplets.", Component_Colors::Purple_Royal, true);
    getComponent(Components::Hidden_Settings)->setIcon(Component_Icons::Hidden);
    addPropertyToComponent(Components::Hidden_Settings, Properties::Hidden_Advisor_Description, Property_Type::Textbox, "",
                           "Description", "Custom Advisor Description for this item.");
    addPropertyToComponent(Components::Hidden_Settings, Properties::Hidden_Item_Locked, Property_Type::Bool, false,
                           "Locked?", "Can this item be moved / changed by user?");
    addPropertyToComponent(Components::Hidden_Settings, Properties::Hidden_Hide_From_Trees, Property_Type::Bool, false,
                           "Hide?", "Should this item be hidden from editor listings (Asset Tree / Project Tree?)");


}









