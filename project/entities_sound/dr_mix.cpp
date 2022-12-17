//
//      Created by Stephens Nunnally on 6/2/2020, (c) 2020 Scidian Studios, All Rights Reserved
//
//  File:
//
//
//
#include "project/constants_component_info.h"
#include "project/dr_project.h"
#include "project/entities_sound/dr_mix.h"
#include "project/entities_sound/dr_track.h"
#include "project/settings/settings_component.h"
#include "project/settings/settings_component_property.h"


//####################################################################################
//##    Constructor
//####################################################################################
DrMix::DrMix(DrProject *parent_project, long key, std::string new_mix_name) : DrSettings(parent_project) {
    this->setKey(key);

    // ***** Initialize Mix Settings
    initializeMixSettings(new_mix_name);

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



//####################################################################################
//##    Adding Tracks Components
//####################################################################################
DrTrack* DrMix::addTrack(DrSound *from_sound, long track_key, long track_number, double x_position) {
    long new_track_key = (track_key == c_no_key) ? getParentProject()->getNextKey() : track_key;
    m_tracks[new_track_key] = new DrTrack(getParentProject(), this, from_sound, new_track_key, track_number, x_position);
    return m_tracks[new_track_key];
}

// Adds a Track as a copy from another Track
DrTrack* DrMix::addTrackCopyFromTrack(DrTrack *from_track) {
    DrTrack *copy_track = addTrack(from_track->getSound());
             copy_track->copyEntitySettings(copy_track);
    return copy_track;
}















