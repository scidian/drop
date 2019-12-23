//
//      Created by Stephens Nunnally on 2/11/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "project/constants_components.h"
#include "project/entities/dr_image.h"
#include "project/settings/settings_component.h"
#include "project/settings/settings_component_property.h"


//####################################################################################
//##    Constructors
//####################################################################################
DrImage::DrImage(DrProject *parent_project, long key, std::string image_name, DrBitmap &bitmap, Asset_Category category) : DrSettings(parent_project) {
    this->setKey(key);
    m_category = category;

    m_simple_name = image_name;
    m_bitmap = bitmap;

    // Set name in DrSettings Entity properties
    getComponentProperty(Components::Entity_Settings, Properties::Entity_Name)->setValue(m_simple_name);
    getComponentProperty(Components::Entity_Settings, Properties::Entity_Name)->setEditable(false);
}










