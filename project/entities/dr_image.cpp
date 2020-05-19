//
//      Created by Stephens Nunnally on 2/11/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "core/imaging/imaging.h"
#include "project/constants_entity_keys.h"
#include "project/entities/dr_image.h"
#include "project/dr_project.h"
#include "project/settings/settings_component_property.h"


//####################################################################################
//##    Constructors
//####################################################################################
DrImage::DrImage(DrProject *parent_project, long key, std::string image_name, DrBitmap &bitmap, bool outline, IProgressBar *progress)
    : DrSettings(parent_project) {

    this->setKey(key);
    this->m_simple_name = image_name;
    this->m_bitmap = bitmap;

    if (outline) {
        autoOutlinePoints(progress);
    } else {
        m_poly_list.push_back( bitmap.polygon().points() );
        m_hole_list.push_back( { } );
    }

    // Set name in DrSettings Entity properties
    getComponentProperty(Comps::Entity_Settings, Props::Entity_Name)->setValue(m_simple_name);
    getComponentProperty(Comps::Entity_Settings, Props::Entity_Name)->setEditable(false);
}









