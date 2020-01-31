//
//      Created by Stephens Nunnally on 11/21/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "core/dr_debug.h"
#include "core/dr_string.h"
#include "project/constants_component_info.h"
#include "project/dr_project.h"
#include "project/entities/dr_animation.h"
#include "project/entities/dr_image.h"
#include "project/settings/settings.h"
#include "project/settings/settings_component.h"
#include "project/settings/settings_component_property.h"


//####################################################################################
//##    Constructor, Destructor
//####################################################################################
DrAnimation::DrAnimation(DrProject *parent_project, long new_animation_key, std::list<long> image_keys)
        : DrSettings(parent_project) {
    this->setKey(new_animation_key);

    std::string new_animation_name = "Empty";

    // If passed Image Keys, use name from first image, create DrFrames
    if (image_keys.size() > 0) {
        // Get first image from image keys so we can use it's name
        DrImage *first_image = parent_project->findImageFromKey(image_keys.front());
        if (first_image == nullptr) {
            Dr::PrintDebug("Eror in DrAnimation constructor! Could not load first image with key: " + std::to_string(image_keys.front()) );
        }

        // Remove trailing numbers and spaces from name
        new_animation_name = first_image->getSimplifiedName();
        bool is_number = Dr::IsInteger( Dr::Right(new_animation_name, 1) );
        while (new_animation_name.length() > 1 && is_number) {
            new_animation_name = Dr::Left(new_animation_name, (new_animation_name.length() - 1));
            is_number = Dr::IsInteger( Dr::Right(new_animation_name, 1) );
            if (Dr::Right(new_animation_name, 1) == std::string(" ")) is_number = true;
        }
    }

    // Initialize Settings
    initializeAnimationSettings(new_animation_name);

    // Add Frames
    for (auto key : image_keys) {
        m_frames.push_back(new DrFrame(parent_project, { key }));
    }
}

DrAnimation::~DrAnimation() {
    for (auto frame: m_frames) { delete frame; }
}

//####################################################################################
//##    Adds frame using given DrImage Key
//####################################################################################
long DrAnimation::addFrame(long image_key) {
    m_frames.push_back(new DrFrame(getParentProject(), { image_key }));
    return static_cast<long>(m_frames.size());
}

//####################################################################################
//##    Returns image based on first frame
//####################################################################################
int DrAnimation::getStartFrameNumber() {
    return getComponentPropertyValue(Comps::Animation_Settings, Props::Animation_Settings_Start_Frame).toInt();
}

DrImage* DrAnimation::getFirstFrameImage() {
    long first_frame = getComponentPropertyValue(Comps::Animation_Settings, Props::Animation_Settings_Start_Frame).toInt();
    long image_key =   getFrame(first_frame)->getKey();
    return getParentProject()->findImageFromKey(image_key);
}


//####################################################################################
//##    Property loading - initializeAnimationSettings
//####################################################################################
void DrAnimation::initializeAnimationSettings(std::string new_name) {
    DrProperty *property_name = getComponentProperty(Comps::Entity_Settings, Props::Entity_Name);
    property_name->setDisplayName("Animation Name");
    property_name->setDescription("Name of this Animation.");
    property_name->setValue(new_name);

    addComponent(Comps::Animation_Settings, "Animation Settings", "Settings for this Animation.", Component_Colors::White_Snow, true);
    getComponent(Comps::Animation_Settings)->setIcon(Component_Icons::Animation);

    addPropertyToComponent(Comps::Animation_Settings, Props::Animation_Settings_Speed, Property_Type::Double, 10.0,
                           "Play Speed", "Speed, in frames per second, that Animation will play.");
    addPropertyToComponent(Comps::Animation_Settings, Props::Animation_Settings_Loops, Property_Type::Int, -1,
                           "Number of Loops", "Number of times to play Animation. Enter a value of -1 for infinite looping.");
    addPropertyToComponent(Comps::Animation_Settings, Props::Animation_Settings_Start_Frame, Property_Type::Positive, 1,
                           "Start Frame", "Which frame to start playback of Animation. <b>NOTE:</b> If playing a specific number of loops, "
                                          "play will end on the LAST frame, not the frame before the start frame.");
}










