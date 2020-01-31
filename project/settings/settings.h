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
#include "project/components_and_properties.h"
#include "project/enums_entity_types.h"

// Forward declarations
class DrProject;
class DrProperty;
class DrComponent;

// Type definitions
typedef std::map<std::string, DrComponent*> ComponentMap;


//####################################################################################
//##    DrComponent
//##        Class to hold a component for all Project Entities
//############################
class DrSettings
{
private:
    // External Borrowed Pointers
    DrProject   *m_parent_project;                      // Holds reference to parent Project

    // Local Variables
    ComponentMap m_components;                          // Map of pointers to DrComponent classes

    long         m_is_visible = true;                   // Should this be visible in editor?
    long         m_is_locked = false;                   // Should this Entity be locked from editing?


protected:       // so derived classes can access
    void         setKey (long new_key) { setComponentPropertyValue(Comps::Entity_Settings, Props::Entity_Key, DrVariant(new_key)); }

public:
    // Constructor / Destructor
    DrSettings(DrProject *parent_project);
    virtual ~DrSettings();

    // Abstract Functions
    virtual DrType  getType() = 0;

    // Getters / Setters
    DrProject*          getParentProject()      { return m_parent_project; }

    long                getKey()                { return getComponentPropertyValue(Comps::Entity_Settings, Props::Entity_Key).toLong(); }
    virtual std::string getName();                                                  // Returns Name from shared "Entity_Name" component
    virtual bool        setName(std::string new_name);                              // Returns true if successful

    bool                isLocked()                  { return (m_is_locked ||
                                                              getComponentPropertyValue(Comps::Hidden_Settings, Props::Hidden_Item_Locked).toBool()); }
    bool                isVisible()                 { return m_is_visible; }
    void                setLocked(bool locked)      { m_is_locked = locked; }
    void                setVisible(bool visible)    { m_is_visible = visible; }


    // Component Handling
    ComponentMap&   getComponentMap()                   { return m_components; }
    long            getComponentCount()                 { return static_cast<int>(m_components.size()); }

    DrComponent*    getComponent(std::string component_key)  { return m_components[component_key]; }
    DrProperty*     getComponentProperty(std::string component_key, std::string property);
    DrProperty*     getComponentProperty(ComponentProperty component_property_pair);
    DrVariant       getComponentPropertyValue(std::string component_key, std::string property_key);

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

    // Setttings Helpers
    void            copyEntitySettings(DrSettings *from_entity);
};


#endif // DRSETTINGS_H









