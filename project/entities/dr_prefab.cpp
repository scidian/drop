//
//      Created by Stephens Nunnally on 3/4/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "project/constants_component_info.h"
#include "project/dr_project.h"
#include "project/entities/dr_prefab.h"
#include "project/settings/settings_component_property.h"


//####################################################################################
//##    Constructor, Destructor
//####################################################################################
DrPrefab::DrPrefab(DrProject *parent_project, long key, DrPrefabType prefab_type) : DrSettings(parent_project) {
    this->setKey(key);

    m_prefab_type = prefab_type;

    // ***** Initialize Effect Settings
    DrProperty *property_name = getComponentProperty(Comps::Entity_Settings, Props::Entity_Name);
    property_name->setEditable(false);
    property_name->setDisplayName("Prefab Name");
    property_name->setDescription("Name of this Prefab Asset.");
    property_name->setValue( Dr::StringFromPrefabType(prefab_type) );
}








