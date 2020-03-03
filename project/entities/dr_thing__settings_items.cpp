//
//      Created by Stephens Nunnally on 3/3/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "engine/constants_engine.h"
#include "engine/enums_engine.h"
#include "project/constants_component_info.h"
#include "project/dr_project.h"
#include "project/entities/dr_asset.h"
#include "project/entities/dr_world.h"
#include "project/entities/dr_stage.h"
#include "project/entities/dr_thing.h"
#include "project/entities/dr_item.h"
#include "project/settings/settings.h"
#include "project/settings/settings_component.h"
#include "project/settings/settings_component_property.h"


//####################################################################################
//##    Foliage Components
//####################################################################################
void DrThing::addComponentSettingsFoliage(std::string new_name) {
    DrProperty *property_name = getComponentProperty(Comps::Entity_Settings, Props::Entity_Name);
    property_name->setDisplayName("Item Name");
    property_name->setDescription("Name of this Item.");
    property_name->setValue(new_name);

    addComponent(Comps::Thing_Settings_Foliage, "Foliage Settings", "Settings for this Foliage.", Component_Colors::RGB_10_Green, true);
    getComponent(Comps::Thing_Settings_Foliage)->setIcon(Component_Icons::Foliage);

    addPropertyToComponent(Comps::Thing_Settings_Foliage, Props::Thing_Foliage_Springiness, Property_Type::Percent, 100.0,
                           "Springiness", "How springy this Foliage will be, from 0 (not springy) to 100 (most springy).");


}























