//
//      Created by Stephens Nunnally on 6/28/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "model/constants_components.h"
#include "model/project/project.h"
#include "model/project/project_effect.h"
#include "model/settings/settings_component_property.h"


//####################################################################################
//##    Constructor, Destructor
//####################################################################################
DrEffect::DrEffect(DrProject *parent_project, long key, DrEffectType effect_type) : DrSettings(parent_project) {
    this->setKey(key);

    m_effect_type = effect_type;

    // ***** Initialize Effect Settings
    DrProperty *property_name = getComponentProperty(Components::Entity_Settings, Properties::Entity_Name);
    property_name->setEditable(false);
    property_name->setDisplayName("Effect Name");
    property_name->setDescription("Name of this Effect Asset.");
    property_name->setValue( Dr::StringFromEffectType(effect_type) );
}









