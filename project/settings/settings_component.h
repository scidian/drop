//
//      Created by Stephens Nunnally on 12/13/18, (c) 2019 Scidian Studios, All Rights Reserved
//
//  File:
//      DrComponent - Class to hold a component for all Project objects
//
//
#ifndef DRCOMPONENT_H
#define DRCOMPONENT_H

#include "core/types/dr_variant.h"
#include "project/constants_comps_and_props.h"
#include "project/constants_entity_keys.h"
#include "project/enums_entity_types.h"

// Forward Declarations
class DrProperty;
class DrSettings;
class DrSlot;

// Type Definitions
typedef std::map<std::string, DrProperty*>  PropertyMap;            // Properties are stored Attributes/Variables for this DrComponent
typedef std::map<long,        DrSlot*>      SignalMap;              // Signals are INPUTS that receive signals from DrOutput
typedef std::map<long,        DrSlot*>      OutputMap;              // Outputs are OUTPUTS that send signals to activate DrSignals


//####################################################################################
//##    DrComponent
//##        Class to hold a component for all Project objects
//##
//##    !!!!! #NOTE: Make sure all DrComponent variables are copied during DrSettings::copyEntitySettings()
//############################
class DrComponent
{
private:
    // External Borrowed Pointers
    DrSettings         *m_parent_settings;                                              // Points to the DrSettings entity that holds this component

    // Child Object Containers
    long                m_property_key_generator    { c_starting_key_property };        // Variable to hand out unique id key's to all child DrProperties
    long                m_slot_key_generator        { c_starting_key_slot };            // Variable to hand out unique id key's to all child DrSlots (Signals / Outputs)
    PropertyMap         m_properties;                                                   // Map of pointers to DrProperty classes    (Variables)
    SignalMap           m_signals;                                                      // Map of pointers to DrSignal classes      (Input Slots)
    OutputMap           m_outputs;                                                      // Map of pointers to DrOutput classes      (Output Slots)

    // The 9 Parts of Data for Every Component
    long                m_component_key             { c_no_key };                       // ID Key of Component, unique to and assigned from Parent DrSettings
    std::string         m_component_name            { "" };                             // Type name of Component
    std::string         m_display_name              { "Unknown Component" };            // Display Name of Component in Object Inspector and TreeAdvisor
    std::string         m_description               { "No description." };              // Description of Component in TreeAdvisor
    std::string         m_icon                      { "" };                             // Icon string
    DrColor             m_color                     { DrColor(128, 128, 128, 255) };    // Object Inspector / Node Box header color
    bool                m_is_hidden                 { false };                          // Should this Component be hidden from Object Insepctor?
    DrPointF            m_node_position             { 0, 0 };                           // Node Graph position of this Component
    DrPointF            m_node_size                 { 0, 0 };                           // Node Graph size of this Component


    // For now this is not saved, assigned during creation in DrStage::addComponent()
    // Used for sorting in Object Inspector
    int                 m_list_order                { 0 };


public:
    // Constructor / Destructor
    explicit DrComponent(DrSettings    *parent_settings,
                         long           component_key,
                         std::string    component_name,
                         std::string    display_name,
                         std::string    description,
                         DrColor        color,
                         bool           is_hidden);
    ~DrComponent();

    // Getters / Setters
    DrSettings*         getParentSettings() { return m_parent_settings; }

    PropertyMap&        getPropertyMap() { return m_properties; }
    DrProperty*         getProperty(std::string property_key, bool show_error = true);

    SignalMap&          getSignalMap() { return m_signals; }
    DrSlot*             getSignal(long signal_key);
    DrSlot*             getSignal(std::string signal_name);

    OutputMap&          getOutputMap() { return m_outputs; }
    DrSlot*             getOutput(long output_key);
    DrSlot*             getOutput(std::string output_name);

    std::list<DrSlot*>  getAllSlots();

    long                getComponentKey()   { return m_component_key; }
    std::string         getComponentName()  { return m_component_name; }
    std::string         getDisplayName()    { return m_display_name; }
    std::string         getDescription()    { return m_description; }
    std::string         getIcon()           { return m_icon; }
    DrColor             getColor()          { return m_color; }
    bool                isHidden()          { return m_is_hidden; }
    DrPointF            getNodePosition()   { return m_node_position; }
    DrPointF            getNodeSize()       { return m_node_size; }

    void                setComponentKey(long key)       { m_component_key = key; }
    void                setDisplayName(std::string new_display_name) { m_display_name = new_display_name; }
    void                setDescription(std::string new_description) { m_description = new_description; }
    void                setIcon(std::string new_icon)   { m_icon = new_icon; }
    void                setColor(DrColor new_color)     { m_color = new_color; }
    void                setHidden(bool hidden)          { m_is_hidden = hidden; }
    void                hide()                          { m_is_hidden = true; }
    void                show()                          { m_is_hidden = false; }
    void                setNodePosition(DrPointF pos)   { m_node_position = pos; }
    void                setNodeSize(DrPointF size)      { m_node_size = size; }

    // Component Key Generator
    long            checkCurrentPropertyGeneratorKey()      { return m_property_key_generator; }
    long            checkCurrentSlotGeneratorKey()          { return m_slot_key_generator; }
    long            getNextPropertyKey()                    { return m_property_key_generator++; }
    long            getNextSlotKey()                        { return m_slot_key_generator++; }
    void            setStartNumberPropertyGeneratorKey(long initial_key)    { m_property_key_generator = initial_key; }
    void            setStartNumberSlotGeneratorKey(long initial_key)        { m_slot_key_generator = initial_key; }

    // Inspector Sorting Variable
    int             getListOrder()          { return m_list_order; }
    void            setListOrder(int order) { m_list_order = order; }

    // Building Functions
    DrProperty*     addProperty(std::string     property_name,
                                Property_Type   type,
                                DrVariant       value,
                                std::string     display_name,
                                std::string     description,
                                bool            is_hidden = false,
                                bool            is_editable = true);
    DrSlot*         addSignal(std::string signal_name, bool is_deletable = false);
    DrSlot*         addOutput(std::string output_name, bool multiple_connections = false, bool is_deletable = false);
    bool            connectOutput(std::string output_name, DrSlot *signal);
    bool            connectOutput(long output_key, DrSlot *signal);
    void            setNodePositionFromOtherComponent(DrComponent *from_component, Direction direction);
    void            updateNodeSize();

};

#endif // DRCOMPONENT_H

















