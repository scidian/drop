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
#include "project/entities/dr_animation.h"
#include "project/entities/dr_image.h"
#include "project/entities/dr_thing.h"
#include "project/entities_physics_2d/dr_asset.h"
#include "project/properties/property_collision.h"
#include "project/settings/settings.h"
#include "project/settings/settings_component.h"
#include "project/settings/settings_component_property.h"
#include "project/settings/settings_component_slot.h"


//####################################################################################
//##    Constructor / Destructor
//####################################################################################
DrSettings::DrSettings(DrProject *parent_project) : m_parent_project(parent_project) {
    getParentProject()->setHasSaved(false);

    addComponentEntitySettings();
    addComponentHiddenSettings();
    addComponentSizeSettings();

    addComponentLocalVariables();
    addComponentUserVariables();
}

DrSettings::~DrSettings() {
    // Mark parent DrProject as changed so we know to alert user before closing unsaved work
    getParentProject()->setHasSaved(false);

    // Delete pointers from children containers
    clearComponents();
}

// Deletes all Components
void DrSettings::clearComponents() {
    for (auto it = m_components.begin(); it != m_components.end(); ) {
        delete it->second;
        it = m_components.erase(it);
    }
}


//####################################################################################
//##    Initial Properties of all DrSettings
//####################################################################################
void DrSettings::addComponentEntitySettings() {
    addComponent(Comps::Entity_Settings, "Name", "Name of selected item.", Component_Colors::RGB_20_Tan, false);
    getComponent(Comps::Entity_Settings)->setIcon(Component_Icons::Name);
    addPropertyToComponent(Comps::Entity_Settings, Props::Entity_Name, Property_Type::String, "",
                           "Name", "Name of the current item.");
    addPropertyToComponent(Comps::Entity_Settings, Props::Entity_Key, Property_Type::Int, -1,
                           "ID Key", "Unique Project Key for this item.", false, false);
}

void DrSettings::addComponentHiddenSettings() {
    addComponent(Comps::Hidden_Settings, "Hidden Settings", "Custom hidden entity properties for Droplets.", Component_Colors::RGB_05_Royal, true);
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

void DrSettings::addComponentLocalVariables() {
    addComponent(Comps::Local_Variables, "Local Variables", "Local Variables", Component_Colors::RGB_19_Silver, true);
    getComponent(Comps::Local_Variables)->setIcon(Component_Icons::Prefab);
}

void DrSettings::addComponentUserVariables() {
    addComponent(Comps::User_Variables, "User Variables", "User Variables", Component_Colors::RGB_19_Silver, true);
    getComponent(Comps::User_Variables)->setIcon(Component_Icons::Prefab);
}


//####################################################################################
//##    Component / Property fetching
//####################################################################################
bool DrSettings::hasComponent(std::string component_name) {
    return (m_components.find(component_name) != m_components.end());
}

DrComponent* DrSettings::getComponent(std::string component_name, bool show_error) {
    auto it = m_components.find(component_name);
    if (it == m_components.end()) {
        if (show_error) {
            Dr::PrintDebug("Error! Component not found by name in current entity! \n"
                           "Component Name: \t" + component_name + "\n"
                           "Entity Name: \t" + this->getName() + "\n"
                           "Entity Type: \t" + Dr::StringFromType(this->getType()) + "\n"
                           "End Error...");
        }
        return nullptr;
    }
    return (*it).second;
}

DrComponent* DrSettings::getComponent(long component_key) {
    for (auto &component_pair : m_components) {
        if (component_pair.second->getComponentKey() == component_key) return component_pair.second;
    }
    return nullptr;
}

void DrSettings::setComponentPropertyValue(ComponentProperty component_property_pair, DrVariant value, bool show_error) {
    setComponentPropertyValue(component_property_pair.first, component_property_pair.second, value, show_error);
}

void DrSettings::setComponentPropertyValue(std::string component_name, std::string property_name, DrVariant value, bool show_error) {
    DrComponent *component = getComponent(component_name, show_error);          if (component == nullptr) return;
    DrProperty  *property =  component->getProperty(property_name, show_error); if (property == nullptr)  return;
    property->setValue(value);
}

DrVariant DrSettings::getComponentPropertyValue(std::string component_name, std::string property_name) {
    DrProperty *property = getComponentProperty(component_name, property_name);
    return ((property == nullptr) ? DrVariant(static_cast<int>(0)) : property->getValue());
}

DrProperty* DrSettings::getComponentProperty(ComponentProperty component_property_pair) {
    return getComponentProperty(component_property_pair.first, component_property_pair.second);
}
DrProperty* DrSettings::getComponentProperty(std::string component_name, std::string property_name, bool show_error) {
    DrComponent *component = getComponent(component_name, show_error);
    if (component == nullptr) return nullptr;
    return component->getProperty(property_name, show_error);
}

DrProperty* DrSettings::findPropertyFromPropertyName(std::string property_name_to_find) {
    for (auto &component_pair: m_components) {
        for (auto &property_pair: component_pair.second->getPropertyMap()) {
            if (property_pair.second->getPropertyName() == property_name_to_find) {
                return property_pair.second;
            }
        }
    }
    return nullptr;
}

DrComponent* DrSettings::findComponentFromPropertyName(std::string property_name_to_find) {
    return findPropertyFromPropertyName(property_name_to_find)->getParentComponent();
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
        case DrType::Block:
        case DrType::Device:
        case DrType::Effect:
        case DrType::Font:
        case DrType::Image:
        case DrType::Item:
        case DrType::Prefab:
        case DrType::Stage:
        case DrType::Thing:
        case DrType::World:        
            name_component = getComponent(Comps::Entity_Settings);              if (name_component == nullptr) return "No Name Component";
            name_property  = name_component->getProperty(Props::Entity_Name);   if (name_property ==  nullptr) return "No Name Property";
            return name_property->getValue().toString();
        case DrType::Frame:
            return "DrFrame - Child of Animation: " + dynamic_cast<DrFrame*>(this)->getParentAnimation()->getName();
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
DrComponent* DrSettings::addComponent(std::string component_name, std::string display_name, std::string description, DrColor color, bool is_hidden) {
    DrComponent *comp = new DrComponent(this, this->getNextKey(), component_name, display_name, description, color, is_hidden);
    comp->setListOrder( static_cast<int>(m_components.size()) );
    m_components.insert(std::make_pair(component_name, comp));
    return comp;
}

DrProperty* DrSettings::addPropertyToComponent(std::string component_name, std::string property_name, Property_Type type,
                                               DrVariant value, std::string display_name, std::string description, bool is_hidden, bool is_editable) {
    DrComponent *component = getComponent(component_name);                      if (component == nullptr) return nullptr;
    DrProperty *prop = component->addProperty(property_name, type, value, display_name, description, is_hidden, is_editable);
    return prop;
}




//####################################################################################
//##    List of Animations used by Entity
//####################################################################################
std::list<long> DrSettings::animationsUsedByEntity() {
    std::list<long> animation_keys_used { };
    for (auto &component_pair : getComponentMap()) {
        for (auto &property_pair : component_pair.second->getPropertyMap()) {
            DrProperty *property = property_pair.second;
            if (property->getPropertyType() == Property_Type::Image) {
                if (property->getValue().toLong() >= c_starting_key_entity)
                    animation_keys_used.push_back(property->getValue().toLong());
            }
        }
    }
    return animation_keys_used;
}

//####################################################################################
//##    Deletes Underlying Animations, called when DrAsset is deleted
//##        Verify first if Animations are used by other DrAssets or if Animation is built in
//####################################################################################
void DrSettings::deleteAnimations() {
    for (auto &animation_key : animationsUsedByEntity()) {
        getParentProject()->deleteAnimation(animation_key, this->getKey());
    }
}

//####################################################################################
//##    Updates Default Animation Images / Collsion Shape
//####################################################################################
void DrSettings::updateAnimationProperty(std::list<long> image_keys, ComponentProperty animation_component_property_name) {
    ///if (m_asset_type != DrAssetType::Object && m_asset_type != DrAssetType::Character) return;

    // ***** Get existing DrProperty to Replace
    DrProperty *property = getComponentProperty(animation_component_property_name);
    if (property == nullptr) return;
    long old_animation = property->getValue().toLong();

    // ***** Create new animation from new image keys
    DrAnimation *animation = getParentProject()->addAnimation(image_keys);
    property->setValue( animation->getKey() );
    DrImage *new_image = animation->getFirstFrameImage();

    if (property->getPropertyName() == Props::Asset_Animation_Idle) {
        DrAsset *asset = dynamic_cast<DrAsset*>(this);
        asset->setWidth( new_image->getBitmap().width );
        asset->setHeight( new_image->getBitmap().height );
        asset->setBaseKey( animation->getKey() );

        // Calculate new image collision shape
        DrPropertyCollision shape = DrAsset::autoCollisionShape(new_image);
        setComponentPropertyValue(Comps::Asset_Collision, Props::Asset_Collision_Image_Shape, shape);
    }

    // ***** Delete Old Animation
    getParentProject()->deleteAnimation(old_animation, this->getKey());
}


//####################################################################################
//##    CopyEntity - Copies all component / property settings from one Entity
//##                 to another Entity of the same type
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

    // Keep track of newly assigned Project key
    long new_key = this->getKey();

    // Clear all Components and rebuild from original copy source
    this->clearComponents();

    // ********** Go through and add all Components and copy other Variables
    for (auto &component_pair : from_entity->getComponentMap()) {
        DrComponent *from_component =   component_pair.second;
        DrComponent *to_component =     this->addComponent(from_component->getComponentName(), from_component->getDisplayName(), from_component->getDescription(),
                                                           from_component->getColor(), from_component->isHidden());
        to_component->setComponentKey(      from_component->getComponentKey());
        to_component->setIcon(              from_component->getIcon());
        to_component->setNodePosition(      from_component->getNodePosition());
        to_component->setStartNumberPropertyGeneratorKey(   from_component->checkCurrentPropertyGeneratorKey());
        to_component->setStartNumberSlotGeneratorKey(       from_component->checkCurrentSlotGeneratorKey());
        to_component->setListOrder(         from_component->getListOrder());

        // Go through and add all Properties and copy other Variables
        for (auto &property_pair : component_pair.second->getPropertyMap()) {
            DrProperty *from_property = property_pair.second;
            DrProperty *to_property =   to_component->addProperty(from_property->getPropertyName(), from_property->getPropertyType(), from_property->getValue(),
                                                                  from_property->getDisplayName(), from_property->getDescription(),
                                                                  from_property->isHidden(), from_property->isEditable());
            to_property->setPropertyKey(        from_property->getPropertyKey());
            to_property->setListOrder(          from_property->getListOrder());
        }

        // Go through and add all Signals and copy other Variables
        for (auto &signal_pair : component_pair.second->getSignalMap()) {
            DrSlot *from_signal =   signal_pair.second;
            DrSlot *to_signal =     to_component->addSignal(from_signal->getSlotName(), from_signal->isEditable());
            to_signal->setSlotKey(              from_signal->getSlotKey());
            to_signal->setSlotType(             from_signal->getSlotType());
        }

        // Go through and add all Outputs and copy other Variables
        for (auto &output_pair : component_pair.second->getOutputMap()) {
            DrSlot *from_output =   output_pair.second;
            DrSlot *to_output =     to_component->addOutput(from_output->getSlotName(), from_output->multipleConnections(), from_output->isEditable());
            to_output->setSlotKey(              from_output->getSlotKey());
            to_output->setSlotType(             from_output->getSlotType());

            // Add Output Connections
            for (auto &connection : from_output->connections()) {
                to_output->addConnection(connection.connected_entity_key, connection.connected_component_key, connection.connected_signal_key);
            }
        }
    }

    // ***** Set Entity Variables
    this->setKey(new_key);
    m_component_key_generator = from_entity->checkCurrentGeneratorKey();
    m_is_visible =              from_entity->isVisible();
    m_is_locked =               from_entity->isLocked();
}



//####################################################################################
//##    Variables
//####################################################################################
void DrSettings::addWorldVariables() {
    this->setVariable(Variables::Deaths,    0);
    this->setVariable(Variables::Coins,     0);
    this->setVariable(Variables::Gems,      0);
    this->setVariable(Variables::Stars,     0);

    this->setVariable(Variables::Distance,  0.0);
    this->setVariable(Variables::Time,      0.0);

    this->setVariable(Variables::PointsA,   0.0);
    this->setVariable(Variables::PointsB,   0.0);
    this->setVariable(Variables::PointsC,   0.0);
}

// Convert DrVariant::Variant_Type to Property_Type
Property_Type DrSettings::propertyTypeFromVariantType(Variant_Type type) {
    Property_Type property_type;
    switch (type) {
        case Variant_Type::Int:
        case Variant_Type::Long:
        case Variant_Type::Unsigned_Int:    property_type = Property_Type::Int;         break;
        case Variant_Type::Float:
        case Variant_Type::Double:          property_type = Property_Type::Double;      break;
        case Variant_Type::String:          property_type = Property_Type::String;      break;
        case Variant_Type::Bool:            property_type = Property_Type::Bool;        break;
        default:                            property_type = Property_Type::Double;      break;
    }
    return property_type;
}

// Sets User Variable in Project
void DrSettings::setVariable(std::string variable_name, DrVariant value, Variable_Info info) {
    DrProperty *property = getComponentProperty(Comps::User_Variables, variable_name, false);
    if (property == nullptr) {
        // ***** Stored as vector of 3 values in order:     "Current", "Best", "Total"
        std::vector<DrVariant> variable { value, value, value };
        ///Property_Type prop_type = DrSettings::propertyTypeFromVariantType(value.getType());
        ///addPropertyToComponent(Comps::User_Variables, variable_name, prop_type, variable, variable_name, "User Variable: " + variable_name);
        addPropertyToComponent(Comps::User_Variables, variable_name, Property_Type::Point3D, variable, variable_name, "User Variable: " + variable_name);
    } else {
        std::vector<DrVariant> variable = getComponentPropertyValue(Comps::User_Variables, variable_name).toVector();
        switch (info) {
            case Variable_Info::Current:    variable[0] = value;                    break;
            case Variable_Info::Best:       variable[1] = value;                    break;
            case Variable_Info::Total:      variable[2] = value;                    break;
            case Variable_Info::All:        variable = { value, value, value };     break;
        }
        setComponentPropertyValue(Comps::User_Variables, variable_name, variable);
    }
}

// Gets a User Variable
DrVariant DrSettings::variable(std::string variable_name, Variable_Info info) {
    DrProperty *property = getComponentProperty(Comps::User_Variables, variable_name);
    if (property == nullptr) return 0;
    switch (info) {
        case Variable_Info::Current:    return property->getValue().toVector()[0];
        case Variable_Info::Best:       return property->getValue().toVector()[1];
        case Variable_Info::Total:      return property->getValue().toVector()[2];
        case Variable_Info::All:        return property->getValue().toVector();
    }
}



















