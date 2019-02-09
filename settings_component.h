//
//      Created by Stephens Nunnally on 12/13/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      DrComponent - Class to hold a component for all DrClass objects
//
//

#ifndef DRCOMPONENT_H
#define DRCOMPONENT_H

#include "enums.h"

class DrSettings;
class DrProperty;
typedef std::map<long, DrProperty*> PropertyMap;


//####################################################################################
//##    DrComponent
//##        Class to hold a component for all DrClass objects
//############################
class DrComponent
{
private:
    // Local variables
    PropertyMap m_properties;                                   // map of pointers to DrProperty classes

    DrSettings *m_parent_settings;

    QString     m_display_name = "Unknown Component";
    QString     m_description = "No description";
    QString     m_icon = "";
    QColor      m_color = QColor(Qt::gray);
    bool        m_turned_on = false;
    long        m_component_key;


public:
    // Constructor & destructor
    DrComponent(DrSettings *parent_settings, QString new_display_name, QString new_description, QColor new_color, long new_key, bool new_turned_on);
    ~DrComponent();


    // Getters and setters
    QString     getDisplayName() { return m_display_name; }
    QString     getDisplayNameQString() { return m_display_name; }
    QString     getDescription() { return m_description; }
    QString     getIcon() { return m_icon; }
    QColor      getColor() { return m_color; }
    bool        isTurnedOn() { return m_turned_on; }
    long        getComponentKey() { return m_component_key; }
    DrSettings* getParentSettings() { return m_parent_settings; }

    PropertyMap getPropertyList() { return m_properties; }
    DrProperty* getProperty(long setting) { return m_properties[setting]; }
    DrProperty* getProperty(World_Properties setting) { return m_properties[static_cast<int>(setting)]; }
    DrProperty* getProperty(Stage_Properties setting) { return m_properties[static_cast<int>(setting)]; }
    DrProperty* getProperty(Object_Properties setting) { return m_properties[static_cast<int>(setting)]; }
    DrProperty* getProperty(Asset_Properties setting) { return m_properties[static_cast<int>(setting)]; }

    void setProperty(long setting, QVariant value);
    void setProperty(World_Properties setting, QVariant value);
    void setProperty(Stage_Properties setting, QVariant value);
    void setProperty(Object_Properties setting, QVariant value);
    void setProperty(Asset_Properties setting, QVariant value);

    void setDisplayName(QString new_display_name) { m_display_name = new_display_name; }
    void setDescription(QString new_description) { m_description = new_description; }
    void setIcon(QString new_icon) { m_icon = new_icon; }
    void setColor(QColor new_color) { m_color = new_color; }
    void setOnOrOff(bool new_turned_on) { m_turned_on = new_turned_on; }
    void turnOn() { m_turned_on = true; }
    void turnOff() { m_turned_on = false; }



    // External calls
    void addProperty(long property_number, Property_Type new_type, QVariant new_value, QString new_display_name, QString new_description, bool is_hidden = false);
    void addProperty(World_Properties property_number, Property_Type new_type, QVariant new_value, QString new_display_name, QString new_description, bool is_hidden = false);
    void addProperty(Stage_Properties property_number, Property_Type new_type, QVariant new_value, QString new_display_name, QString new_description, bool is_hidden = false);
    void addProperty(Object_Properties property_number, Property_Type new_type, QVariant new_value, QString new_display_name, QString new_description, bool is_hidden = false);
    void addProperty(Asset_Properties property_number, Property_Type new_type, QVariant new_value, QString new_display_name, QString new_description, bool is_hidden = false);

};

#endif // DRCOMPONENT_H









