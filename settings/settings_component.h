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

// Forward Declarations
class DrSettings;
class DrProperty;

// Type definitions
typedef std::map<long, DrProperty*> PropertyMap;

//####################################################################################
//##    DrComponent
//##        Class to hold a component for all DrClass objects
//############################
class DrComponent
{
private:
    // Local variables
    PropertyMap m_properties;                                       // Map of pointers to DrProperty classes

    DrSettings *m_parent_settings;                                  // Point to the DrSettings class that holds this component

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
    QString         getDisplayName() { return m_display_name; }
    QString         getDisplayNameQString() { return m_display_name; }
    QString         getDescription() { return m_description; }
    QString         getIcon() { return m_icon; }
    QColor          getColor() { return m_color; }
    bool            isTurnedOn() { return m_turned_on; }
    long            getComponentKey() { return m_component_key; }
    DrSettings*     getParentSettings() { return m_parent_settings; }

    PropertyMap&    getPropertyList() { return m_properties; }
    DrProperty*     getProperty(long setting);
    DrProperty*     getProperty(Properties setting);

    void    setProperty(long setting, QVariant value);
    void    setProperty(Properties setting, QVariant value);


    void    setDisplayName(QString new_display_name) { m_display_name = new_display_name; }
    void    setDescription(QString new_description) { m_description = new_description; }
    void    setIcon(QString new_icon) { m_icon = new_icon; }
    void    setColor(QColor new_color) { m_color = new_color; }
    void    setOnOrOff(bool new_turned_on) { m_turned_on = new_turned_on; }
    void    turnOn() { m_turned_on = true; }
    void    turnOff() { m_turned_on = false; }



    // External calls
    void addProperty(Properties     property_number,
                     Property_Type  type,
                     QVariant       value,
                     QString        display_name,
                     QString        description,
                     bool           is_hidden = false,
                     bool           is_editable = true);

};

#endif // DRCOMPONENT_H





