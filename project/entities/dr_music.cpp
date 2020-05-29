//
//      Created by Stephens Nunnally on 5/29/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "project/constants_component_info.h"
#include "project/entities/dr_music.h"
#include "project/settings/settings_component.h"
#include "project/settings/settings_component_property.h"


//####################################################################################
//##    Constructor
//####################################################################################
DrMusic::DrMusic(DrProject *parent_project, long key) : DrSettings(parent_project) {
    this->setKey(key);

    // ***** Initialize Music Mix Settings
    DrProperty *property_name = getComponentProperty(Comps::Entity_Settings, Props::Entity_Name);
    property_name->setDisplayName("Music Name");
    property_name->setDescription("Name of this Music Composition.");
    property_name->setValue("New Mix");


}
