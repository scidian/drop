//
//      Created by Stephens Nunnally on 3/31/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "project/constants_component_info.h"
#include "project/entities/dr_variable.h"
#include "project/dr_project.h"
#include "project/settings/settings_component.h"
#include "project/settings/settings_component_property.h"


//####################################################################################
//##    Constructor / Destructor
//####################################################################################
DrVariable::DrVariable(DrProject *parent_project, std::string variable_name, Variant_Type variable_type, DrVariant initial_value)
    : DrSettings(parent_project) {
    m_variable_type = variable_type;

    // Set name in DrSettings Entity properties
    getComponentProperty(Comps::Entity_Settings, Props::Entity_Name)->setValue(variable_name);
    getComponentProperty(Comps::Entity_Settings, Props::Entity_Name)->setEditable(false);

    // Initialize Variable
    Property_Type prop_type = DrSettings::propertyTypeFromVariantType(variable_type);
    addComponent(Comps::Variable_Data, "Variables", "Variable Data", Component_Colors::RGB_19_Silver, true);
    addPropertyToComponent(Comps::Variable_Data, Props::Variable_Data_Current,  prop_type, initial_value, "Current",  "Current value of variable.");
    addPropertyToComponent(Comps::Variable_Data, Props::Variable_Data_Best,     prop_type, initial_value, "Best",     "Best result ever in game.");
    addPropertyToComponent(Comps::Variable_Data, Props::Variable_Data_Total,    prop_type, initial_value, "Total",    "Total value of variable since start.");
}

DrVariable::~DrVariable() { }


//####################################################################################
//##    Variable Values
//####################################################################################
DrVariant DrVariable::getCurrent()  { return getComponentPropertyValue(Comps::Variable_Data, Props::Variable_Data_Current); }
DrVariant DrVariable::getBest()     { return getComponentPropertyValue(Comps::Variable_Data, Props::Variable_Data_Best); }
DrVariant DrVariable::getTotal()    { return getComponentPropertyValue(Comps::Variable_Data, Props::Variable_Data_Total); }

void DrVariable::setCurrent(DrVariant value)    { setComponentPropertyValue(Comps::Variable_Data, Props::Variable_Data_Current, value); }
void DrVariable::setBest(DrVariant value)       { setComponentPropertyValue(Comps::Variable_Data, Props::Variable_Data_Best, value); }
void DrVariable::setTotal(DrVariant value)      { setComponentPropertyValue(Comps::Variable_Data, Props::Variable_Data_Total, value); }

















