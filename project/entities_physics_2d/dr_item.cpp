//
//      Created by Stephens Nunnally on 3/3/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "project/constants_component_info.h"
#include "project/dr_project.h"
#include "project/entities_physics_2d/dr_item.h"
#include "project/settings/settings_component_property.h"


//####################################################################################
//##    Constructor / Destructor
//####################################################################################
DrItem::DrItem(DrProject *parent_project, long key, DrItemType item_type) : DrSettings(parent_project) {
    this->setKey(key);

    m_item_type = item_type;

    // ***** Initialize Item Settings
    DrProperty *property_name = getComponentProperty(Comps::Entity_Settings, Props::Entity_Name);
    property_name->setEditable(false);
    property_name->setDisplayName("Item Name");
    property_name->setDescription("Name of this Item Asset.");
    property_name->setValue( Dr::StringFromItemType(item_type) );
}








