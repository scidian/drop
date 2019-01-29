//
//      Created by Stephens Nunnally on 12/13/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      DrSettings - Class to hold settings and a collection of components for all DrClass objects
//
//

#ifndef DRSETTINGS_H
#define DRSETTINGS_H

#include "enums.h"


class DrProperty;
class DrComponent;
typedef std::map<long, DrComponent*> ComponentMap;


//####################################################################################
//##    DrComponent
//##        Class to hold a component for all DrClass objects
//############################
class DrSettings
{
private:
    ComponentMap m_components;                      // map of pointers to DrComponent classes   (holds components for current scene)
    long         m_key;                             // holds unique key for each object with a base class DrSettings

protected:       // so derived classes can access
    void         setKey (long new_key) { m_key = new_key; }

public:
    // Constructor and Destructor
    DrSettings();
    virtual ~DrSettings();


    // Getters and Setters
    long            getKey()    { return m_key; }
    virtual DrTypes getType()   { return DrTypes::BaseClass; }

    DrSettings*  getSettings()  { return this; }

    ComponentMap getComponentList()     { return m_components; }
    long         getComponentCount()    { return static_cast<int>(m_components.size()); }

    DrComponent* getComponent(long component)               { return m_components[component]; }
    DrComponent* getComponent(World_Components component)   { return m_components[static_cast<long>(component)]; }
    DrComponent* getComponent(Scene_Components component)   { return m_components[static_cast<long>(component)]; }
    DrComponent* getComponent(Object_Components component)  { return m_components[static_cast<long>(component)]; }
    DrComponent* getComponent(Asset_Components component)   { return m_components[static_cast<long>(component)]; }

    DrProperty*  getComponentProperty(long component, long property);
    DrProperty*  getComponentProperty(World_Components component, World_Properties property);
    DrProperty*  getComponentProperty(Scene_Components component, Scene_Properties property);
    DrProperty*  getComponentProperty(Object_Components component, Object_Properties property);
    DrProperty*  getComponentProperty(Asset_Components component, Asset_Properties property);

    QVariant     getComponentPropertyValue(long component, long property);
    QVariant     getComponentPropertyValue(World_Components component, World_Properties property);
    QVariant     getComponentPropertyValue(Scene_Components component, Scene_Properties property);
    QVariant     getComponentPropertyValue(Object_Components component, Object_Properties property);
    QVariant     getComponentPropertyValue(Asset_Components component, Asset_Properties property);


    // External Calls
    DrComponent* findComponentFromPropertyKey(long property_key_to_find);
    DrProperty*  findPropertyFromPropertyKey(long property_key_to_find);

    void         addComponent(long component, QString new_display_name, QString new_description, QColor new_color, bool new_turned_on);
    void         addComponent(World_Components component, QString new_display_name, QString new_description, QColor new_color, bool new_turned_on);
    void         addComponent(Scene_Components component, QString new_display_name, QString new_description, QColor new_color, bool new_turned_on);
    void         addComponent(Object_Components component, QString new_display_name, QString new_description, QColor new_color, bool new_turned_on);
    void         addComponent(Asset_Components component, QString new_display_name, QString new_description, QColor new_color, bool new_turned_on);

    void addPropertyToComponent(long component, long property_number, Property_Type new_type, QVariant new_value, QString new_display_name, QString new_description);
    void addPropertyToComponent(World_Components component, World_Properties property_number, Property_Type new_type, QVariant new_value, QString new_display_name, QString new_description);
    void addPropertyToComponent(Scene_Components component, Scene_Properties property_number, Property_Type new_type, QVariant new_value, QString new_display_name, QString new_description);
    void addPropertyToComponent(Object_Components component, Object_Properties property_number, Property_Type new_type, QVariant new_value, QString new_display_name, QString new_description);
    void addPropertyToComponent(Asset_Components component, Asset_Properties property_number, Property_Type new_type, QVariant new_value, QString new_display_name, QString new_description);

    QString      getWorldName();
    QString      getSceneName();


};

#endif // DRSETTINGS_H




