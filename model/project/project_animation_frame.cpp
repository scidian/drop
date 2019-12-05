//
//      Created by Stephens Nunnally on 11/21/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "editor/constants_editor.h"
#include "editor/helper_editor.h"
#include "model/project/project.h"
#include "model/project/project_animation.h"
#include "model/project/project_image.h"
#include "model/settings/settings.h"
#include "model/settings/settings_component.h"
#include "model/settings/settings_component_property.h"


//####################################################################################
//##    Constructor, Destructor
//####################################################################################
DrFrame::DrFrame(DrProject *parent_project, long image_key) : DrSettings(parent_project) {
    this->setKey(image_key);
    double width = 1, height = 1;

    DrImage *image = parent_project->findImageFromKey(image_key);
    if (image == nullptr) {
        Dr::ShowErrorMessage("DrFrame::DrFrame", "Could not load DrImage with key: " + QString::number(image_key));

    } else {
        width =  image->getImage().width();
        height = image->getImage().height();
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

    QString position_text = "Location of Image within the current Frame.";
    addPropertyToComponent(Components::Thing_Transform, Properties::Thing_Position, Property_Type::PositionF, QPointF(x, y), "Position", position_text);

    QString transform_text = "Rotation, in degrees, of Image on the Z Axis.";
    addPropertyToComponent(Components::Thing_Transform, Properties::Thing_Rotation, Property_Type::Angle, 0, "Rotation", transform_text);

    QString size_text = "Width and Height of Image in pixels, affected by Scale property.";
    addPropertyToComponent(Components::Thing_Transform, Properties::Thing_Size, Property_Type::SizeF, QPointF(width, height), "Size", size_text);

    QString scale_text = "X and Y scale of Image within the current Frame.";
    addPropertyToComponent(Components::Thing_Transform, Properties::Thing_Scale, Property_Type::ScaleF, QPointF(1, 1), "Scale", scale_text);
}








