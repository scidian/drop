//
//      Created by Stephens Nunnally on 6/2/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "project/constants_component_info.h"
#include "project/entities_sound/dr_mix.h"
#include "project/entities_sound/dr_track.h"
#include "project/settings/settings_component.h"
#include "project/settings/settings_component_property.h"


//####################################################################################
//##    Constructor
//####################################################################################
DrMix::DrMix(DrProject *parent_project, long key) : DrSettings(parent_project) {
    this->setKey(key);

    // ***** Initialize Mix Settings
    initializeMixSettings("Mix 1");

}

DrMix::~DrMix() {
    for (auto &track_pair: m_tracks) { delete track_pair.second; }
}


//####################################################################################
//##    Settings Components
//####################################################################################
void DrMix::initializeMixSettings(std::string new_name) {
    DrProperty *property_name = getComponentProperty(Comps::Entity_Settings, Props::Entity_Name);
    property_name->setDisplayName("Mix Name");
    property_name->setDescription("Name of this Mix Composition.");
    property_name->setValue(new_name);

    addComponent(Comps::Mix_Settings, "Mix Settings", "Settings for this Mix Composition.", Component_Colors::RGB_18_Gray, false);
    getComponent(Comps::Mix_Settings)->setIcon(Component_Icons::Sound);

    addPropertyToComponent(Comps::Mix_Settings, Props::Mix_Settings_Color, Property_Type::Color, DrColor(Component_Colors::RGB_01_Red).rgba(),
                           "Mix Color", "Color this Mix will appear in the Asset Tree.");

}














