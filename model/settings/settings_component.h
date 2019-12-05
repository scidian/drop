//
//      Created by Stephens Nunnally on 12/13/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      DrComponent - Class to hold a component for all Project objects
//
//
#ifndef DRCOMPONENT_H
#define DRCOMPONENT_H

#include <QColor>

#include <map>

#include "model/enums_model_types.h"
#include "model/enums_model_properties.h"

// Forward Declarations
class DrSettings;
class DrProperty;

// Type definitions
typedef std::map<long, DrProperty*> PropertyMap;

//####################################################################################
//##    DrComponent
//##        Class to hold a component for all Project objects
//############################
class DrComponent
{
private:
    // External Borrowed Pointers
    DrSettings      *m_parent_settings;                                 // Point to the DrSettings class that holds this component

    // Local Variables
    PropertyMap     m_properties;                                       // Map of pointers to DrProperty classes

    // The 6 Parts of Data for Every Component
    QString         m_display_name = "Unknown Component";
    QString         m_description = "No description";
    QString         m_icon = "";
    QColor          m_color = QColor(Qt::gray);
    bool            m_turned_on = false;
    long            m_component_key;

    // For now this is not stored, assignined during creation in void DrStage::addComponent()
    // Used for sorting in Object Inspector
    int             m_list_order = 0;

public:
    // Constructor / Destructor
    DrComponent(DrSettings *parent_settings, QString new_display_name, QString new_description, QColor new_color, long new_key, bool new_turned_on);
    ~DrComponent();

    // Getters / Setters
    DrSettings*     getParentSettings() { return m_parent_settings; }

    PropertyMap&    getPropertyMap() { return m_properties; }
    DrProperty*     getProperty(long setting);
    DrProperty*     getProperty(Properties setting);

    QString     getDisplayName() { return m_display_name; }
    QString     getDisplayNameQString() { return m_display_name; }
    QString     getDescription() { return m_description; }
    QString     getIcon() { return m_icon; }
    QColor      getColor() { return m_color; }
    bool        isTurnedOn() { return m_turned_on; }
    long        getComponentKey() { return m_component_key; }

    void        setDisplayName(QString new_display_name) { m_display_name = new_display_name; }
    void        setDescription(QString new_description) { m_description = new_description; }
    void        setIcon(QString new_icon) { m_icon = new_icon; }
    void        setColor(QColor new_color) { m_color = new_color; }
    void        setOnOrOff(bool new_turned_on) { m_turned_on = new_turned_on; }
    void        turnOn() { m_turned_on = true; }
    void        turnOff() { m_turned_on = false; }

    // Inspector Sorting Variable
    int         getListOrder() { return m_list_order; }
    void        setListOrder(int order) { m_list_order = order; }

    // Building Calls
    DrProperty* addProperty(Properties     property_number,
                            Property_Type  type,
                            QVariant       value,
                            QString        display_name,
                            QString        description,
                            bool           is_hidden = false,
                            bool           is_editable = true);

};

#endif // DRCOMPONENT_H





