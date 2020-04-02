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
//##    Tile Components
//####################################################################################
void DrThing::addComponentSettingsTile(std::string new_name) {
    DrProperty *property_name = getComponentProperty(Comps::Entity_Settings, Props::Entity_Name);
    property_name->setDisplayName("Item Name");
    property_name->setDescription("Name of this Item.");
    property_name->setValue(new_name);

    addComponent(Comps::Thing_Settings_Tile, "Tile Settings", "Settings for this Tile.", Component_Colors::RGB_10_Green, true);
    getComponent(Comps::Thing_Settings_Tile)->setIcon(Component_Icons::Tile);

    addPropertyToComponent(Comps::Thing_Settings_Tile, Props::Thing_Tile_Animation, Property_Type::Image, c_no_key,
                           "Tile Animation", "Animation for this Tile.");
}























