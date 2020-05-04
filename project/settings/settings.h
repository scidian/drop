//
//      Created by Stephens Nunnally on 12/13/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      DrSettings - Base class to hold Settings and a collection of Components for all Project entities
//
//
#ifndef DRSETTINGS_H
#define DRSETTINGS_H

#include "core/types/dr_variant.h"
#include "project/constants_comps_and_props.h"
#include "project/constants_variables.h"
#include "project/enums_entity_types.h"

// Forward Declarations
class DrProject;
class DrProperty;
class DrComponent;
class DrVariable;

// Type Definitions
typedef std::map<std::string, DrComponent*>     ComponentMap;       // Map of pointers to DrComponent classes, built in keys in file components_and_properties.h
typedef std::map<std::string, DrVariable*>      VariableMap;        // Map of pointers to DrVariables for this Entity


//####################################################################################
//##    DrComponent
//##        Class to hold a component for all Project Entities
//############################
class DrSettings
{
public:
    // Constructor / Destructor
    DrSettings(DrProject *parent_project);
    virtual ~DrSettings();


    // #################### VARIABLES ####################
private:
    // External Borrowed Pointers
    DrProject      *m_parent_project;                               // Holds reference to parent Project

    // Local Variables
    ComponentMap    m_components;                                   // Map of pointers to DrComponent classes
    VariableMap     m_variables;                                    // Holds variables for this Entity

    long            m_is_visible = true;                            // Should this be visible in editor?
    long            m_is_locked = false;                            // Should this Entity be locked from editing?


    // #################### FUNCTIONS TO BE EXPOSED TO API ####################
public:

    // Variables
    DrVariable*     variable(std::string variable_name);                            // Returns variable by name
    void            setVariable(std::string variable_name, DrVariant value);        // Sets variable by name


    // #################### INTERNAL FUNCTIONS ####################
public:
    // Abstract Functions
    virtual DrType      getType() = 0;

    // Getters / Setters
    DrProject*          project()               { return m_parent_project; }
    DrProject*          getParentProject()      { return m_parent_project; }
    long                getKey()                { return getComponentPropertyValue(Comps::Entity_Settings, Props::Entity_Key).toLong(); }
    void                setKey (long new_key)   { setComponentPropertyValue(Comps::Entity_Settings, Props::Entity_Key, DrVariant(new_key)); }

    virtual std::string getName();                              // Returns Name from shared "Entity_Name" component
    virtual bool        setName(std::string new_name);          // Returns true if successful

    bool            isLocked()                  { return (m_is_locked || getComponentPropertyValue(Comps::Hidden_Settings, Props::Hidden_Item_Locked).toBool()); }
    bool            isVisible()                 { return m_is_visible; }
    void            setLocked(bool locked)      { m_is_locked = locked; }
    void            setVisible(bool visible)    { m_is_visible = visible; }


    // Component Handling
    ComponentMap&   getComponentMap()           { return m_components; }
    long            getComponentCount()         { return static_cast<int>(m_components.size()); }

    bool            hasComponent(std::string component_key);

    DrComponent*    getComponent(std::string component_key, bool show_error = true);
    DrProperty*     getComponentProperty(std::string component_key, std::string property);
    DrProperty*     getComponentProperty(ComponentProperty component_property_pair);
    DrVariant       getComponentPropertyValue(std::string component_key, std::string property_key);

    void            setComponentPropertyValue(ComponentProperty component_property_pair, DrVariant value);
    void            setComponentPropertyValue(std::string component_key, std::string property_key, DrVariant value);

    // Component / Property Searching
    DrComponent*    findComponentFromPropertyKey(std::string property_key_to_find);
    DrProperty*     findPropertyFromPropertyKey(std::string property_key_to_find);

    // Component / Property Building
    DrComponent*    addComponent(std::string component_key, std::string display_name, std::string description, DrColor color, bool is_turned_on);
    DrProperty*     addPropertyToComponent(std::string component_key, std::string property_key, Property_Type type, DrVariant value,
                                           std::string display_name, std::string description, bool is_hidden = false, bool is_editable = true);
    void            addComponentEntitySettings();
    void            addComponentHiddenSettings();
    void            addComponentSizeSettings();


    // Animation Functions
    std::list<long>         animationsUsedByEntity();
    void                    deleteAnimations();
    void                    updateAnimationProperty(std::list<long> image_keys, ComponentProperty animation_component_property_key);

    // Setttings Helpers
    void                    copyEntitySettings(DrSettings *from_entity);

    // Variable Functions
    void                    addWorldVariables();
    static Property_Type    propertyTypeFromVariantType(Variant_Type type);
};


#endif // DRSETTINGS_H









