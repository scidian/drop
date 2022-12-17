//
//      Created by Stephens Nunnally on 6/28/2019, (c) 2019 Scidian Studios, All Rights Reserved
//
//  File:
//
//
//
#include "project/constants_component_info.h"
#include "project/dr_project.h"
#include "project/entities_physics_2d/dr_effect.h"
#include "project/settings/settings_component_property.h"


//####################################################################################
//##    Constructor / Destructor
//####################################################################################
DrEffect::DrEffect(DrProject *parent_project, long key, DrEffectType effect_type) : DrSettings(parent_project) {
    this->setKey(key);

    m_effect_type = effect_type;

    // ***** Initialize Effect Settings
    DrProperty *property_name = getComponentProperty(Comps::Entity_Settings, Props::Entity_Name);
    property_name->setEditable(false);
    property_name->setDisplayName("Effect Name");
    property_name->setDescription("Name of this Effect Asset.");
    property_name->setValue( Dr::StringFromEffectType(effect_type) );
}









