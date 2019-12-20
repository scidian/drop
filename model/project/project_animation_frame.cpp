//
//      Created by Stephens Nunnally on 11/21/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "library/dr_debug.h"
#include "library/types/dr_pointf.h"
#include "model/constants_components.h"
#include "model/project/project.h"
#include "model/project/project_animation.h"
#include "model/project/project_image.h"
#include "model/settings/settings.h"
#include "model/settings/settings_component.h"
#include "model/settings/settings_component_property.h"


//####################################################################################
//##    Constructor, Destructor
//####################################################################################
DrFrame::DrFrame(DrProject *parent_project, std::list<long> image_keys) : DrSettings(parent_project) {
    this->setKey(image_keys.front());
    double width = 1, height = 1;

    DrImage *image = parent_project->findImageFromKey(image_keys.front());
    if (image == nullptr) {
        Dr::PrintDebug("Warning!!! DrFrame() Constructor - Could not load DrImage with key: " + std::to_string(image_keys.front()));

    } else {
        width =  image->getBitmap().width;
        height = image->getBitmap().height;
    }

    addComponentTransform(width, height, 0, 0);
}

DrFrame::~DrFrame() { }


//####################################################################################
//##    Transform Components
//####################################################################################
void DrFrame::addComponentTransform(double width, double height, double x, double y) {
    addComponent(Components::Thing_Transform, "Transform", "Sets the size and angle of the Image in the current Frame.", Component_Colors::Green_SeaGrass, true);
    getComponent(Components::Thing_Transform)->setIcon(Component_Icons::Transform);

    std::string position_text = "Location of Image within the current Frame.";
    addPropertyToComponent(Components::Thing_Transform, Properties::Thing_Position, Property_Type::PositionF, DrPointF(x, y), "Position", position_text);

    std::string transform_text = "Rotation, in degrees, of Image on the Z Axis.";
    addPropertyToComponent(Components::Thing_Transform, Properties::Thing_Rotation, Property_Type::Angle, 0, "Rotation", transform_text);

    std::string size_text = "Width and Height of Image in pixels, affected by Scale property.";
    addPropertyToComponent(Components::Thing_Transform, Properties::Thing_Size, Property_Type::SizeF, DrPointF(width, height), "Size", size_text);

    std::string scale_text = "X and Y scale of Image within the current Frame.";
    addPropertyToComponent(Components::Thing_Transform, Properties::Thing_Scale, Property_Type::ScaleF, DrPointF(1, 1), "Scale", scale_text);
}








