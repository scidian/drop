//
//
//      DrSettings class functions
//
//

#include "30_settings.h"
#include "31_component.h"
#include "32_property.h"

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
DrProperty* DrSettings::getComponentProperty(World_Components component, World_Properties property) { return m_components[static_cast<long>(component)]->getProperty(property); }
DrProperty* DrSettings::getComponentProperty(Scene_Components component, Scene_Properties property) { return m_components[static_cast<long>(component)]->getProperty(property); }
DrProperty* DrSettings::getComponentProperty(Object_Components component, Object_Properties property) { return m_components[static_cast<long>(component)]->getProperty(property); }

QVariant DrSettings::getComponentPropertyValue(long component, long property) { return m_components[component]->getProperty(property)->getValue(); }
QVariant DrSettings::getComponentPropertyValue(World_Components component, World_Properties property) { return m_components[static_cast<long>(component)]->getProperty(property)->getValue(); }
QVariant DrSettings::getComponentPropertyValue(Scene_Components component, Scene_Properties property) { return m_components[static_cast<long>(component)]->getProperty(property)->getValue(); }
QVariant DrSettings::getComponentPropertyValue(Object_Components component, Object_Properties property) { return m_components[static_cast<long>(component)]->getProperty(property)->getValue(); }

std::string DrSettings::getWorldName() { return m_components[static_cast<long>(World_Components::settings)]->getProperty(World_Properties::name)->getValue().toString().toStdString(); }
std::string DrSettings::getSceneName() { return m_components[static_cast<long>(Scene_Components::settings)]->getProperty(Scene_Properties::name)->getValue().toString().toStdString(); }

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

void DrSettings::addComponent(long component, std::string new_display_name, std::string new_description, QColor new_color, bool new_turned_on) {
    DrComponent *comp = new DrComponent(this, new_display_name, new_description, new_color, component, new_turned_on);
    m_components[component] = comp;
}

void DrSettings::addComponent(World_Components component, std::string new_display_name, std::string new_description, QColor new_color, bool new_turned_on) {
    DrComponent *comp = new DrComponent(this, new_display_name, new_description, new_color, static_cast<long>(component), new_turned_on);
    m_components[static_cast<long>(component)] = comp;
}

void DrSettings::addComponent(Scene_Components component, std::string new_display_name, std::string new_description, QColor new_color, bool new_turned_on) {
    DrComponent *comp = new DrComponent(this, new_display_name, new_description, new_color, static_cast<long>(component), new_turned_on);
    m_components[static_cast<long>(component)] = comp;
}

void DrSettings::addComponent(Object_Components component, std::string new_display_name, std::string new_description, QColor new_color, bool new_turned_on) {
    DrComponent *comp = new DrComponent(this, new_display_name, new_description, new_color, static_cast<long>(component), new_turned_on);
    m_components[static_cast<long>(component)] = comp;
}

//####################################################################################

void DrSettings::addPropertyToComponent(long component, long property_number, Property_Type new_type, QVariant new_value, std::string new_display_name, std::string new_description) {
    m_components[component]->addProperty(property_number, new_type, new_value, new_display_name, new_description);
}

void DrSettings::addPropertyToComponent(World_Components component, World_Properties property_number, Property_Type new_type,
                                        QVariant new_value, std::string new_display_name, std::string new_description) {
    m_components[static_cast<long>(component)]->addProperty(property_number, new_type, new_value, new_display_name, new_description);
}
void DrSettings::addPropertyToComponent(Scene_Components component, Scene_Properties property_number, Property_Type new_type,
                                        QVariant new_value, std::string new_display_name, std::string new_description) {
    m_components[static_cast<long>(component)]->addProperty(property_number, new_type, new_value, new_display_name, new_description);
}
void DrSettings::addPropertyToComponent(Object_Components component, Object_Properties property_number, Property_Type new_type,
                                        QVariant new_value, std::string new_display_name, std::string new_description) {
    m_components[static_cast<long>(component)]->addProperty(property_number, new_type, new_value, new_display_name, new_description);
}









