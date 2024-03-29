//
//      Created by Stephens Nunnally on 12/13/18, (c) 2019 Scidian Studios, All Rights Reserved
//
//  File:
//      DrProperty - Class to hold one property of any Project object component
//
//
#ifndef DRPROPERTY_H
#define DRPROPERTY_H

#include "core/types/dr_variant.h"
#include "project/constants_comps_and_props.h"
#include "project/enums_entity_types.h"

// Forward Declarations
class DrComponent;
class DrSettings;


//####################################################################################
//##    DrProperty
//##        Class to hold one property of any Project object component
//##
//##    !!!!! #NOTE: Make sure all DrProperty variables are copied during DrSettings::copyEntitySettings()
//############################
class DrProperty
{
private:
    // External Borrowed Pointers
    DrSettings         *m_parent_settings;                                          // Points to the DrSettings entity that holds the component that holds this property
    DrComponent        *m_parent_component;                                         // Points to the DrComponent that holds this DrProperty

    // The 8 Parts of Data for Every Property
    long                m_property_key          { c_no_key };                       // The unique id of this property within the parent Component
    std::string         m_property_name         { "" };                             // The name of this property within the parent (common Properties are in "namespace Props")
    std::string         m_display_name          { "Unknown Property" };             // Name to be displayed in Editor
    std::string         m_description           { "No description." };              // Description to be displayed in Editor
    Property_Type       m_property_type         { Property_Type::String };          // Variable type of 'm_value'
    DrVariant           m_value                 { std::string("0") };               // The stored data this property represents
    bool                m_is_hidden             { false };                          // Should this appear in the Inspector
    bool                m_is_editable           { true };                           // Should this be editable in the Inspector

    // For now this is not saved, assigned during creation in DrComponent::addProperty()
    // Used for sorting in Object Inspector
    int                 m_list_order            { 0 };


public:
    // Constructor / Destructor
    DrProperty(DrSettings      *parent_settings,
               DrComponent     *parent_component,
               long             property_key,
               std::string      property_name,
               std::string      display_name,
               std::string      description,
               Property_Type    type,
               DrVariant        value,
               bool             is_hidden = false,
               bool             is_editable = true);

    // Getters / Setters
    DrSettings*         getParentSettings()     { return m_parent_settings; }
    DrComponent*        getParentComponent()    { return m_parent_component; }

    ComponentProperty   getCompPropPair();

    long                getPropertyKey()        { return m_property_key; }
    std::string         getPropertyName()       { return m_property_name; }
    std::string         getDisplayName()        { return m_display_name; }
    std::string         getDescription()        { return m_description; }
    Property_Type       getPropertyType()       { return m_property_type; }
    DrVariant           getValue()              { return m_value; }
    bool                isHidden()              { return m_is_hidden; }
    bool                isEditable()            { return m_is_editable; }

    void                setPropertyKey(long key)                        { m_property_key = key; }
    void                setDisplayName(std::string new_display_name)    { m_display_name = new_display_name; }
    void                setDescription(std::string new_description)     { m_description = new_description; }
    void                setPropertyType(Property_Type new_type)         { m_property_type = new_type; }
    void                setHidden(bool is_hidden)                       { m_is_hidden = is_hidden; }
    void                setEditable(bool is_editable)                   { m_is_editable = is_editable; }

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










