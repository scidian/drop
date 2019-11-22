//
//      Created by Stephens Nunnally on 11/21/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "colors/colors.h"
#include "project/project.h"
#include "project/project_animation.h"
#include "project/project_image.h"
#include "settings/settings.h"
#include "settings/settings_component.h"
#include "settings/settings_component_property.h"

#include "constants.h"
#include "helper.h"
#include "helper_qt.h"

//####################################################################################
//##    Constructor, Destructor
//####################################################################################
DrAnimation::DrAnimation(DrProject *parent_project, long new_animation_key, QList<long> image_keys)
        : DrSettings(parent_project) {
    this->setKey(new_animation_key);

    // Get first image from image keys so we can use it's name
    DrImage *first_image = parent_project->findImageFromKey(image_keys.first());
    if (first_image == nullptr) {
        Dr::ShowErrorMessage("DrAnimation::DrAnimation", "Could not load first image with key: " + QString::number(image_keys.first()) );
    }

    // Remove trailing numbers and spaces from name
    QString new_animation_name = first_image->getSimplifiedName();
    bool is_number;
    new_animation_name.right(1).toInt(&is_number);
    while (new_animation_name.length() > 1 && is_number) {
        new_animation_name = new_animation_name.left(new_animation_name.length() - 1);
        new_animation_name.right(1).toInt(&is_number);
        if (new_animation_name.right(1) == QString(" ")) is_number = true;
    }

    // Initialize Settings
    initializeAnimationSettings(new_animation_name);

    // Add Frames
    for (auto key : image_keys) {
        m_frames.push_back(new DrFrame(parent_project, key));
    }
}

DrAnimation::~DrAnimation() {
    for (auto frame: m_frames) { delete frame; }
}


//####################################################################################
//##    Returns image based on first frame
//####################################################################################
int DrAnimation::getStartFrameNumber() {
    return getComponentPropertyValue(Components::Animation_Settings, Properties::Animation_Settings_Start_Frame).toInt();
}

QPixmap DrAnimation::getPixmapFromFirstFrame() {
    long first_frame = getComponentPropertyValue(Components::Animation_Settings, Properties::Animation_Settings_Start_Frame).toInt();
    long image_key =   getFrame(first_frame)->getKey();

    DrImage *frame_image = getParentProject()->findImageFromKey(image_key);
    return   frame_image->getPixmapFromImage();
}


//####################################################################################
//##    Property loading - initializeAnimationSettings
//####################################################################################
void DrAnimation::initializeAnimationSettings(QString new_name) {
    DrProperty *property_name = getComponentProperty(Components::Entity_Settings, Properties::Entity_Name);
    property_name->setDisplayName("Animation Name");
    property_name->setDescription("Name of this Animation.");
    property_name->setValue(new_name);

    addComponent(Components::Animation_Settings, "Animation Settings", "Settings for this Animation.", Component_Colors::White_Snow, true);
    getComponent(Components::Animation_Settings)->setIcon(Component_Icons::Animation);

    addPropertyToComponent(Components::Animation_Settings, Properties::Animation_Settings_Speed, Property_Type::Double, 10.0,
                           "Play Speed", "Speed, in frames per second, that Animation will play.");
    addPropertyToComponent(Components::Animation_Settings, Properties::Animation_Settings_Loops, Property_Type::Int, -1,
                           "Number of Loops", "Number of times to play Animation. Enter a value of -1 for infinite looping.");
    addPropertyToComponent(Components::Animation_Settings, Properties::Animation_Settings_Start_Frame, Property_Type::Positive, 1,
                           "Start Frame", "Which frame to start playback of Animation. <b>NOTE:</b> If playing a specific number of loops, "
                                          "play will end on the LAST frame, not the frame before the start frame.");
}










