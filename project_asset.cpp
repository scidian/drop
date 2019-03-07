//
//      Created by Stephens Nunnally on 12/13/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      DrAsset Class Definitions
//
//

#include <QBuffer>

#include "library.h"

#include "project.h"
#include "project_asset.h"
#include "project_image.h"
#include "project_world.h"
#include "project_world_stage.h"
#include "project_world_stage_object.h"

#include "settings.h"
#include "settings_component.h"
#include "settings_component_property.h"


//####################################################################################
//##    Constructor, Destructor
//####################################################################################
DrAsset::DrAsset(DrProject *parent_project, long new_asset_key, DrAssetType new_asset_type, long image_key)
{
    m_parent_project = parent_project;
    setKey(new_asset_key);

    m_asset_type = new_asset_type;

    m_list_order = new_asset_key;
    m_group_number = 0;

    QPixmap my_starting_pixmap = m_parent_project->getImage(image_key)->getPixmap();

    initializeAssetSettings(m_parent_project->getImage(image_key)->getSimplifiedName(), my_starting_pixmap );

    m_width =  my_starting_pixmap.width();
    m_height = my_starting_pixmap.height();
}

DrAsset::~DrAsset() {}



//####################################################################################
//##    Property loading - initializeAssetSettings
//####################################################################################

void DrAsset::initializeAssetSettings(QString new_name, QPixmap pixmap)
{
    addComponent(Components::Asset_Settings, "Settings", "Basic settings for current asset.", Component_Colors::White_Snow, true);
    getComponent(Components::Asset_Settings)->setIcon(Component_Icons::Settings);

    addPropertyToComponent(Components::Asset_Settings, Properties::Asset_Name, Property_Type::String, new_name,
                           "Asset Name", "Name of the current asset.");
    addPropertyToComponent(Components::Asset_Settings, Properties::Asset_Collision_Shape, Property_Type::Polygon, QPolygonF(),
                           "Collision Shape", "Shape of the object as it interacts with other objects in the world.");


    addComponent(Components::Asset_Animation, "Animation", "Images to show for this asset", Component_Colors::Green_SeaGrass, true);
    getComponent(Components::Asset_Animation)->setIcon( nullptr ); // !!!!! TODO

    addPropertyToComponent(Components::Asset_Animation, Properties::Asset_Animation_Default, Property_Type::Image, QVariant(pixmap),
                           "Default Animation", "Image shown for this object.");

}




















