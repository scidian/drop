//
//      Created by Stephens Nunnally on 12/13/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      DrProperty - Class to hold one property of any DrClass object component
//
//

#ifndef DRPROPERTY_H
#define DRPROPERTY_H

#include "enums.h"


class DrSettings;
class DrComponent;


//####################################################################################
//##    DrProperty
//##        Class to hold one property of any DrClass object component
//############################
class DrProperty
{
private:
    // Local variables
    DrSettings   *m_parent_settings;
    DrComponent  *m_parent_component;

    // The 4 Parts pf Data for Every Property
    QString       m_display_name = "Unknown Property";
    QString       m_description = "No description";
    Property_Type m_preferred_type = Property_Type::String;
    QVariant      m_value = QString::fromStdString("0");

    // The unique id of this property within the parent object
    long          m_property_key;

public:
    // Constructor & destructor
    DrProperty(DrSettings *parent_settings, DrComponent *parent_component,
               QString new_display_name, QString new_description, Property_Type new_type, QVariant new_value, long new_key);


    // Getters and setters
    /**
    * @brief getDisplayName
    * @return
    * @author Stephens
    * @todo Fix me
    */
    DrSettings*   getParentSettings() { return m_parent_settings; }
    DrComponent*  getParentComponent() { return m_parent_component; }

    QString       getDisplayName() { return m_display_name; }
    QString       getDescription() { return m_description; }
    Property_Type getPropertyType() { return m_preferred_type; }
    QVariant      getValue() { return m_value; }

    long          getPropertyKey() { return m_property_key; }


    void setDisplayName(QString new_display_name) { m_display_name = new_display_name; }
    void setDescription(QString new_description) { m_description = new_description; }
    void setPropertyType(Property_Type new_type) { m_preferred_type = new_type; }

    // Function template allows any type to be passed in and assigned to QVariant
    template <class T>
    void setValue(T new_value) { m_value = new_value; }


};


#endif // DRPROPERTY_H












