//
//      Created by Stephens Nunnally on 12/13/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      DrSettings - Class to hold settings and a collection of components for all DrClass objects
//
//

#ifndef DRSETTINGS_H
#define DRSETTINGS_H

#include "constants.h"
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
    ComponentMap m_components;                      // map of pointers to DrComponent classes
    long         m_key;                             // holds unique key for each object with a base class DrSettings

protected:       // so derived classes can access
    void         setKey (long new_key) { m_key = new_key; }

public:
    // Constructor and Destructor
    DrSettings();
    virtual ~DrSettings();


    // Getters and Setters
    long            getKey()            { return m_key; }
    virtual DrType  getType()           { return DrType::BaseClass; }       // Returns different types depending on reimplementation in child class

    DrSettings*  getSettings()          { return this; }

    ComponentMap getComponentList()     { return m_components; }
    long         getComponentCount()    { return static_cast<int>(m_components.size()); }

    DrComponent* getComponent(long component)               { return m_components[component]; }
    DrComponent* getComponent(Components component)         { return m_components[static_cast<long>(component)]; }
    DrProperty*  getComponentProperty(long component, long property);
    DrProperty*  getComponentProperty(Components component, Properties property);
    QVariant     getComponentPropertyValue(long component, long property);
    QVariant     getComponentPropertyValue(Components component, Properties property);

    void         setComponentPropertyValue(long component, long property, QVariant value);
    void         setComponentPropertyValue(Components component, Properties property, QVariant value);

    // External Calls
    DrComponent* findComponentFromPropertyKey(long property_key_to_find);
    DrProperty*  findPropertyFromPropertyKey(long property_key_to_find);

    void         addComponent(Components component, QString display_name, QString description, QColor color, bool is_turned_on);
    void         addPropertyToComponent(Components component, Properties property_number, Property_Type type, QVariant value,
                                QString display_name, QString description, bool is_hidden = false, bool is_editable = true);

    QString      getWorldName();
    QString      getStageName();
    QString      getObjectName();
    QString      getAssetName();

};

#endif // DRSETTINGS_H




