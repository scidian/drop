//
//      Created by Stephens Nunnally on 12/13/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      DrProperty - Class to hold one property of any Project object component
//
//
#ifndef DRPROPERTY_H
#define DRPROPERTY_H

#include "core/types/dr_variant.h"
#include "project/enums_entity_types.h"
#include "project/enums_properties.h"

// Forward declarations
class DrSettings;
class DrComponent;


//####################################################################################
//##    DrProperty
//##        Class to hold one property of any Project object component
//############################
class DrProperty
{
private:
    // External Borrowed Pointers
    DrSettings     *m_parent_settings;
    DrComponent    *m_parent_component;

    // The 7 Parts of Data for Every Property
    std::string     m_display_name =      "Unknown Property";                   // Name to be displayed in Editor
    std::string     m_description =       "No description";                     // Description to be displayed in Editor
    Property_Type   m_property_type =     Property_Type::String;                // Variable type of 'm_value'
    DrVariant       m_value =             std::string("0");                     // The stored data this property represents
    long            m_property_key =      0;                                    // The unique id of this property within the parent object
    bool            m_is_hidden =         false;                                // Should this appear in the Inspector
    bool            m_is_editable =       true;                                 // Should this be editable in the Inspector

    // For now this is not stored, assignined during creation in void DrComponent::addProperty()
    // Used for sorting in Object Inspector
    int             m_list_order = 0;

public:
    // Constructor / Destructor
    DrProperty(DrSettings      *parent_settings,
               DrComponent     *parent_component,
               std::string      display_name,
               std::string      description,
               Property_Type    type,
               DrVariant        value,
               long             new_key,
               bool             is_hidden = false,
               bool             is_editable = true);

    // Getters / setters
    DrSettings*     getParentSettings() { return m_parent_settings; }
    DrComponent*    getParentComponent() { return m_parent_component; }

    std::string     getDisplayName() { return m_display_name; }
    std::string     getDescription() { return m_description; }
    Property_Type   getPropertyType() { return m_property_type; }
    DrVariant       getValue() { return m_value; }
    long            getPropertyKey() { return m_property_key; }
    bool            isHidden() { return m_is_hidden; }
    bool            isEditable() { return m_is_editable; }

    void            setDisplayName(std::string new_display_name) { m_display_name = new_display_name; }
    void            setDescription(std::string new_description) { m_description = new_description; }
    void            setPropertyType(Property_Type new_type) { m_property_type = new_type; }
    void            setHidden(bool is_hidden) { m_is_hidden = is_hidden; }
    void            setEditable(bool is_editable) { m_is_editable = is_editable; }

    // Inspector Sorting Variable
    int             getListOrder() { return m_list_order; }
    void            setListOrder(int order) { m_list_order = order; }


    //####################################################################################
    //##    Function template allows any type to be passed in and assigned to DrVariant
    //##
    //##    !!!!! #NOTE: Setting a value here means the data model has changed !!!!!
    //##                 This implies we need to save if closing Drop, or store change in Undo / Redo stack !!!!!
    //####################################################################################
    template <class T>
    void setValue(T new_value) {
        m_value = new_value;
        setValueInternal();
    }
    void setValueInternal();

};


#endif // DRPROPERTY_H










