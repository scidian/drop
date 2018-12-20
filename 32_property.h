//
//
//      DrProperty - Class to hold one property of any DrClass object component
//
//

#ifndef DRPROPERTY_H
#define DRPROPERTY_H

#include "enums.h"


class DrSettings;
class DrComponent;

class DrProperty
{
private:
    // Local variables
    DrSettings   *m_parent_settings;
    DrComponent  *m_parent_component;

    std::string   m_display_name = "Unknown Property";
    std::string   m_description = "No description";
    Property_Type m_preferred_type = Property_Type::STRING;
    QVariant      m_value = QString::fromStdString("0");
    long          m_property_key;

public:
    // Constructor & destructor
    DrProperty(DrSettings *parent_settings, DrComponent *parent_component,
               std::string new_display_name, std::string new_description, Property_Type new_type, QVariant new_value, long new_key);


    // Getters and setters
    std::string   getDisplayName() { return m_display_name; }
    QString       getDisplayNameQString() { return QString::fromStdString(m_display_name); }
    std::string   getDescription() { return m_description; }
    Property_Type getPropertyType() { return m_preferred_type; }
    QVariant      getValue() { return m_value; }
    DrSettings*   getParentSettings() { return m_parent_settings; }
    DrComponent*  getParentComponent() { return m_parent_component; }

    void setDisplayName(std::string new_display_name) { m_display_name = new_display_name; }
    void setDescription(std::string new_description) { m_description = new_description; }
    void setPropertyType(Property_Type new_type) { m_preferred_type = new_type; }

    void setValue(int new_int) { m_value = new_int; }
    void setValue(float new_float) { m_value = new_float; }
    void setValue(double new_double) { m_value = new_double; }
    void setValue(std::string new_string) { m_value = QString::fromStdString(new_string); }
    void setValue(QVariant new_value) { m_value = new_value; }
    void setValue(QList<QVariant> new_value) { m_value = new_value; }

    long getPropertyKey() { return m_property_key; }


    // External calls



};


#endif // DRPROPERTY_H












