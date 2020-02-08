//
//      Created by Stephens Nunnally on 2/11/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "project/constants_entity_keys.h"
#include "project/entities/dr_image.h"
#include "project/settings/settings_component_property.h"


//####################################################################################
//##    Constructors
//####################################################################################
DrImage::DrImage(DrProject *parent_project, long key, std::string image_name, DrBitmap &bitmap,
                 Asset_Category category, bool force_outline) : DrSettings(parent_project) {
    this->setKey(key);
    m_category = category;

    m_simple_name = image_name;
    m_bitmap = bitmap;

    if (key > c_key_starting_number || force_outline) {
        autoOutlinePoints();
    } else {
        m_poly_list.push_back( bitmap.polygon().points() );
        m_hole_list.push_back( { } );
    }

    // Set name in DrSettings Entity properties
    getComponentProperty(Comps::Entity_Settings, Props::Entity_Name)->setValue(m_simple_name);
    getComponentProperty(Comps::Entity_Settings, Props::Entity_Name)->setEditable(false);
}









