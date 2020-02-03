//
//      Created by Stephens Nunnally on 12/13/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      DrSettings Class Definitions
//
//
#include "core/dr_debug.h"
#include "project/constants_comps_and_props.h"
#include "project/constants_component_info.h"
#include "project/enums_entity_types.h"
#include "project/dr_project.h"
#include "project/entities/dr_asset.h"
#include "project/entities/dr_thing.h"
#include "project/settings/settings.h"
#include "project/settings/settings_component.h"
#include "project/settings/settings_component_property.h"


//####################################################################################
//##    Constructor, Destructor
//####################################################################################
DrSettings::DrSettings(DrProject *parent_project) : m_parent_project(parent_project) {
    getParentProject()->setHasSaved(false);

    addComponentEntitySettings();
    addComponentHiddenSettings();
    addComponentSizeSettings();
}

DrSettings::~DrSettings() {
    getParentProject()->setHasSaved(false);

    for (auto i: m_components) { delete i.second; }
}


//####################################################################################
//##    Initial Properties of all DrSettings
//####################################################################################
void DrSettings::addComponentEntitySettings() {
    addComponent(Comps::Entity_Settings, "Name", "Name of selected item.", Component_Colors::Red_Tuscan, true);
    getComponent(Comps::Entity_Settings)->setIcon(Component_Icons::Name);
    addPropertyToComponent(Comps::Entity_Settings, Props::Entity_Name, Property_Type::String, "",
                           "Name", "Name of the current item.");
    addPropertyToComponent(Comps::Entity_Settings, Props::Entity_Key, Property_Type::Int, -1,
                           "ID Key", "Unique Project Key for this item.", false, false);
}

void DrSettings::addComponentHiddenSettings() {
    addComponent(Comps::Hidden_Settings, "Hidden Settings", "Custom hidden entity properties for Droplets.", Component_Colors::Purple_Royal, true);
    getComponent(Comps::Hidden_Settings)->setIcon(Component_Icons::Hidden);
    addPropertyToComponent(Comps::Hidden_Settings, Props::Hidden_Advisor_Description, Property_Type::Textbox, "",
                           "Description", "Custom Advisor Description for this item.");
    addPropertyToComponent(Comps::Hidden_Settings, Props::Hidden_Item_Locked, Property_Type::Bool, false,
                           "Locked?", "Can this item be moved / changed by user?");
    addPropertyToComponent(Comps::Hidden_Settings, Props::Hidden_Hide_From_Trees, Property_Type::Bool, false,
                           "Hide?", "Should this item be hidden from editor listings (Asset Tree / Project Tree?)");
}

void DrSettings::addComponentSizeSettings() {
    addComponent(Comps::Size_Settings, "Size Settings", "Hidden size settings for object while it's in Editor.", Component_Colors::Blue_Drop_3, true);
    getComponent(Comps::Size_Settings)->setIcon(Component_Icons::Transform);
    addPropertyToComponent(Comps::Size_Settings, Props::Size_Keep_Square, Property_Type::Bool, false,
                           "Keep Square?", "Should this item be forced to stay square?");
    addPropertyToComponent(Comps::Size_Settings, Props::Size_Max_Size, Property_Type::PointF, DrPointF(0, 0),
                           "Max Size", "Maximum size of item while it's in the Editor. A value of zero signifies no maximum size.");
    addPropertyToComponent(Comps::Size_Settings, Props::Size_Min_Size, Property_Type::PointF, DrPointF(0, 0),
                           "Min Size", "Minimum size of item while it's in the Editor. A value of zero signifies  no minimum size.");
}


//####################################################################################
//##    Component / Property fetching
//####################################################################################
bool DrSettings::hasComponent(std::string component_key) {
    return (m_components.find(component_key) != m_components.end());
}

DrComponent* DrSettings::getComponent(std::string component_key) {
    auto it = m_components.find(component_key);
    if (it == m_components.end()) {
        Dr::PrintDebug("Error Code: " + Error_Code::NoComponent + "\n\n"
                       "Component not found in current object \n\n"
                       "Component ID: \t" + component_key + "\n"
                       "Object Name: \t" + this->getName() + ". \n"
                       "Object Type: \t" + Dr::StringFromType(this->getType()) + "\n");
        return nullptr;
    }
    return (*it).second;
}

void DrSettings::setComponentPropertyValue(ComponentProperty component_property_pair, DrVariant value) {
    setComponentPropertyValue(component_property_pair.first, component_property_pair.second, value);
}

void DrSettings::setComponentPropertyValue(std::string component_key, std::string property_key, DrVariant value) {
    DrComponent *component = getComponent(component_key);                       if (component == nullptr) return;
    DrProperty *property = component->getProperty(property_key);                if (property == nullptr)  return;
    property->setValue(value);
}

DrVariant DrSettings::getComponentPropertyValue(std::string component_key, std::string property_key) {
    DrProperty *property = getComponentProperty(component_key, property_key);
    return ((property == nullptr) ? DrVariant(static_cast<int>(0)) : property->getValue());
}

DrProperty* DrSettings::getComponentProperty(ComponentProperty component_property_pair) {
    return getComponentProperty(component_property_pair.first, component_property_pair.second);
}
DrProperty* DrSettings::getComponentProperty(std::string component_key, std::string property_key) {
    DrComponent *component = getComponent(component_key);
    if (component == nullptr) return nullptr;
    return component->getProperty(property_key);
}

DrProperty* DrSettings::findPropertyFromPropertyKey(std::string property_key_to_find) {
    for (auto component: m_components) {
        for (auto property: component.second->getPropertyMap()) {
            if (property.second->getPropertyKey() == property_key_to_find) {
                return property.second;
            }
        }
    }
    return nullptr;
}

DrComponent* DrSettings::findComponentFromPropertyKey(std::string property_key_to_find) {
    return findPropertyFromPropertyKey(property_key_to_find)->getParentComponent();
}


//####################################################################################
//##    Returns Name / Sets Name from shared "Entity_Name" component
//####################################################################################
std::string DrSettings::getName() {
    DrComponent *name_component;
    DrProperty  *name_property;
    switch (getType()) {
        case DrType::Animation:
        case DrType::Asset:
        case DrType::Device:
        case DrType::Effect:
        case DrType::Font:
        case DrType::Image:
        case DrType::Stage:
        case DrType::Thing:
        case DrType::World:
            name_component = getComponent(Comps::Entity_Settings);              if (name_component == nullptr) return "No Name Component";
            name_property  = name_component->getProperty(Props::Entity_Name);   if (name_property ==  nullptr) return "No Name Property";
            return name_property->getValue().toString();
        case DrType::Frame:     return "DrFrame - Unknown Name";
        case DrType::NotFound:  return "Type \"DrType::NotFound\"";
    }
    return "Unknown Type, Can't Find Name";
}

// Sets Name from shared "Entity_Name" component, returns true if successful
bool DrSettings::setName(std::string new_name) {
    DrComponent *name_component;
    DrProperty  *name_property;
    name_component = getComponent(Comps::Entity_Settings);                      if (name_component == nullptr) return false;
    name_property  = name_component->getProperty(Props::Entity_Name);           if (name_property == nullptr)  return false;
    name_property->setValue(new_name);
    return true;
}


//####################################################################################
//##    Component Loading - addComponent / addComponentProperty
//####################################################################################
DrComponent* DrSettings::addComponent(std::string component_key, std::string display_name, std::string description, DrColor color, bool is_turned_on) {
    DrComponent *comp = new DrComponent(this, display_name, description, color, component_key, is_turned_on);
    comp->setListOrder( static_cast<int>(m_components.size()) );
    m_components.insert(std::make_pair(component_key, comp));
    return comp;
}

DrProperty* DrSettings::addPropertyToComponent(std::string component_key, std::string property_key, Property_Type type,
                                               DrVariant value, std::string display_name, std::string description, bool is_hidden, bool is_editable) {
    DrComponent *component = getComponent(component_key);                       if (component == nullptr) return nullptr;
    DrProperty *prop = component->addProperty(property_key, type, value, display_name, description, is_hidden, is_editable);
    return prop;
}



//####################################################################################
//##    Copies all component / property settings from one Entity to another Entity of the same type
//####################################################################################
void DrSettings::copyEntitySettings(DrSettings *from_entity) {
    // Check same Type
    if (from_entity->getType() != getType()) return;

    // If Asset, check same Asset Type
    if (from_entity->getType() == DrType::Asset) {
        DrAsset *from_asset = dynamic_cast<DrAsset*>(from_entity);
        DrAsset *to_asset   = dynamic_cast<DrAsset*>(this);
        if (from_asset->getAssetType() != to_asset->getAssetType()) return;
    }

    // If Thing, check same Thing Type
    if (from_entity->getType() == DrType::Thing) {
        DrThing *from_thing = dynamic_cast<DrThing*>(from_entity);
        DrThing *to_thing   = dynamic_cast<DrThing*>(this);
        if (from_thing->getThingType() != to_thing->getThingType()) return;
    }

    for (auto &component_pair : from_entity->getComponentMap()) {
        for (auto &property_pair : component_pair.second->getPropertyMap()) {
            DrProperty *from_property = property_pair.second;
            DrProperty *to_property =   getComponentProperty(component_pair.first, property_pair.first);

            // !!! DO NOT COPY UNIQUE ID KEY !!!
            if (to_property->getPropertyKey() == Props::Entity_Key) continue;

            // Copy all other properties
            to_property->setValue(      from_property->getValue());
            to_property->setEditable(   from_property->isEditable());
            to_property->setHidden(     from_property->isHidden());
            to_property->setDisplayName(from_property->getDisplayName());
            to_property->setDescription(from_property->getDescription());
        }
    }

    m_is_visible = from_entity->isVisible();
    m_is_locked =  from_entity->isLocked();
}








