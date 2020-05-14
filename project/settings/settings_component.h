//
//      Created by Stephens Nunnally on 12/13/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      DrComponent - Class to hold a component for all Project objects
//
//
#ifndef DRCOMPONENT_H
#define DRCOMPONENT_H

#include "core/types/dr_variant.h"
#include "project/constants_comps_and_props.h"
#include "project/enums_entity_types.h"

// Forward Declarations
class DrSettings;
class DrProperty;

// Type Definitions
typedef std::map<std::string, DrProperty*> PropertyMap;

// Local Constants
const   long    c_property_starting_number =    1;


//####################################################################################
//##    DrComponent
//##        Class to hold a component for all Project objects
//############################
class DrComponent
{
private:
    // External Borrowed Pointers
    DrSettings         *m_parent_settings;                                          // Points to the DrSettings entity that holds this component

    // Local Variables
    long                m_key_generator     { c_property_starting_number };         // Variable to hand out unique id key's to all child objects (in this case DrProperties)
    PropertyMap         m_properties;                                               // Map of pointers to DrProperty classes

    // The 7 Parts of Data for Every Component
    long                m_component_kee     { c_no_key };
    std::string         m_component_name    { "" };
    std::string         m_display_name      { "Unknown Component" };
    std::string         m_description       { "No description." };
    std::string         m_icon              { "" };
    DrColor             m_color             { DrColor(128, 128, 128, 255) };
    bool                m_turned_on         { false };


    // For now this is not saved, assigned during creation in DrStage::addComponent()
    // Used for sorting in Object Inspector
    int                 m_list_order        { 0 };


public:
    // Constructor / Destructor
    explicit DrComponent(DrSettings    *parent_settings,
                         long           component_key,
                         std::string    component_name,
                         std::string    display_name,
                         std::string    description,
                         DrColor        color,
                         bool           is_turned_on);
    ~DrComponent();

    // Getters / Setters
    DrSettings*         getParentSettings() { return m_parent_settings; }

    PropertyMap&        getPropertyMap() { return m_properties; }
    DrProperty*         getProperty(std::string property_key, bool show_error = true);

    long                getComponentKey()   { return m_component_kee; }
    std::string         getComponentName()  { return m_component_name; }
    std::string         getDisplayName()    { return m_display_name; }
    std::string         getDescription()    { return m_description; }
    std::string         getIcon()           { return m_icon; }
    DrColor             getColor()          { return m_color; }
    bool                isTurnedOn()        { return m_turned_on; }

    void                setDisplayName(std::string new_display_name) { m_display_name = new_display_name; }
    void                setDescription(std::string new_description) { m_description = new_description; }
    void                setIcon(std::string new_icon) { m_icon = new_icon; }
    void                setColor(DrColor new_color) { m_color = new_color; }
    void                setOnOrOff(bool new_turned_on) { m_turned_on = new_turned_on; }
    void                turnOn() { m_turned_on = true; }
    void                turnOff() { m_turned_on = false; }

    // Component Key Generator
    long            checkCurrentGeneratorKey()                      { return m_key_generator; }
    long            getNextKey()                                    { return m_key_generator++; }
    void            setGeneratorKeyStartNumber(long initial_key)    { m_key_generator = initial_key; }

    // Inspector Sorting Variable
    int             getListOrder() { return m_list_order; }
    void            setListOrder(int order) { m_list_order = order; }

    // Building Calls
    DrProperty*     addProperty(std::string     property_name,
                                Property_Type   type,
                                DrVariant       value,
                                std::string     display_name,
                                std::string     description,
                                bool            is_hidden = false,
                                bool            is_editable = true);

};

#endif // DRCOMPONENT_H






