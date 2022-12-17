//
//      Created by Stephens Nunnally on 6/2/2020, (c) 2020 Scidian Studios, All Rights Reserved
//
//  File:
//
//
//
#include "core/colors/colors.h"
#include "project/constants_component_info.h"
#include "project/entities_sound/dr_sound.h"
#include "project/entities_sound/dr_track.h"
#include "project/settings/settings_component.h"
#include "project/settings/settings_component_property.h"


//####################################################################################
//##    Constructor
//####################################################################################
DrTrack::DrTrack(DrProject *parent_project, DrMix *parent_mix, DrSound *from_sound, long new_track_key, int track, double x_position)
    : DrSettings(parent_project) {

    this->setKey(new_track_key);

    m_parent_mix = parent_mix;
    m_sound_asset = from_sound;

    m_track = track;
    m_x_pos = x_position;

    DrProperty *property_name = getComponentProperty(Comps::Entity_Settings, Props::Entity_Name);
    property_name->setDisplayName("Track Name");
    property_name->setDescription("Name of this Track.");
    property_name->setValue(from_sound->getName());
    property_name->setEditable(false);
    getComponentProperty(Comps::Entity_Settings, Props::Entity_Key)->setHidden(true);
    addPropertyToComponent(Comps::Entity_Settings, Props::Entity_Asset_Key, Property_Type::Int, from_sound->getKey(),
                           "Sound ID Key", "ID Key of Sound this Track represents.", false, false);

    initializeTrackSettings();
}

DrTrack::~DrTrack() { }


//####################################################################################
//##    Settings Components
//####################################################################################
void DrTrack::initializeTrackSettings() {

    addComponent(Comps::Track_Settings, "Track Settings", "Settings for this Track.", Component_Colors::RGB_18_Gray, false);
    getComponent(Comps::Track_Settings)->setIcon(Component_Icons::Sound);

    addPropertyToComponent(Comps::Track_Settings, Props::Track_Settings_Color, Property_Type::Color, DrColor(Component_Colors::RGB_06_Blue).rgba(),
                           "Track Color", "Color this Track will appear in the Sound Creator.");

}
























