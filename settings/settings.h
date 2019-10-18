//
//      Created by Stephens Nunnally on 12/13/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      DrSettings - Class to hold settings and a collection of components for all Project objects
//
//

#ifndef DRSETTINGS_H
#define DRSETTINGS_H

#include "enums.h"
#include "enums_dr_settings.h"

// Forward declarations
class DrProperty;
class DrComponent;

// Type definitions
typedef std::map<long, DrComponent*> ComponentMap;


//####################################################################################
//##    DrComponent
//##        Class to hold a component for all Project Entities
//############################
class DrSettings
{
private:
    // Local Variables
    ComponentMap m_components;                          // Map of pointers to DrComponent classes

    long         m_is_visible = true;                   // Should this be visible in editor?
    long         m_is_locked = false;                   // Should this Entity be locked from editing?


protected:       // so derived classes can access
    void         setKey (long new_key) { setComponentPropertyValue(Components::Entity_Settings, Properties::Entity_Key, QVariant::fromValue(new_key)); }

public:
    // Constructor and Destructor
    DrSettings();
    virtual ~DrSettings();


    // Getters and Setters
    long            getKey()                { return getComponentPropertyValue(Components::Entity_Settings, Properties::Entity_Key).toLongLong(); }
    virtual DrType  getType()               { return DrType::BaseClass; }       // Returns different types depending on reimplementation in child class

    DrSettings*     getSettings()           { return this; }
    virtual QString getName();                                                  // Returns Name from shared "Entity_Name" component
    virtual bool    setName(QString new_name);                                  // Returns true if successful

    bool            isLocked()                  { return (m_is_locked ||
                                                          getComponentPropertyValue(Components::Hidden_Settings, Properties::Hidden_Item_Locked).toBool()); }
    bool            isVisible()                 { return m_is_visible; }
    void            setLocked(bool locked)      { m_is_locked = locked; }
    void            setVisible(bool visible)    { m_is_visible = visible; }


    // Component Handling
    ComponentMap&   getComponentMap()                   { return m_components; }
    long            getComponentCount()                 { return static_cast<int>(m_components.size()); }

    DrComponent*    getComponent(long component)        { return m_components[component]; }
    DrComponent*    getComponent(Components component)  { return m_components[static_cast<long>(component)]; }
    DrProperty*     getComponentProperty(long component, long property);
    DrProperty*     getComponentProperty(Components component, Properties property);
    QVariant        getComponentPropertyValue(long component, long property);
    QVariant        getComponentPropertyValue(Components component, Properties property);

    void            setComponentPropertyValue(long component, long property, QVariant value);
    void            setComponentPropertyValue(Components component, Properties property, QVariant value);

    // Component / Property Searching
    DrComponent*    findComponentFromPropertyKey(long property_key_to_find);
    DrComponent*    findComponentFromPropertyKey(Components property_key_to_find);
    DrProperty*     findPropertyFromPropertyKey(long property_key_to_find);
    DrProperty*     findPropertyFromPropertyKey(Properties property_key_to_find);

    // Component / Property Building
    void            addComponent(Components component, QString display_name, QString description, QColor color, bool is_turned_on);
    void            addPropertyToComponent(Components component, Properties property_number, Property_Type type, QVariant value,
                                           QString display_name, QString description, bool is_hidden = false, bool is_editable = true);
    void            addComponentEntitySettings();
    void            addComponentHiddenSettings();

};


#endif // DRSETTINGS_H









