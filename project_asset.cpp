//
//      Created by Stephens Nunnally on 12/13/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      DrAsset Class Definitions
//
//

#include "project_asset.h"

#include "project.h"
#include "project_world.h"
#include "project_world_scene.h"
#include "project_world_scene_object.h"

#include "settings.h"
#include "settings_component.h"
#include "settings_component_property.h"


//####################################################################################
//##    Constructor, Destructor - addScene
//####################################################################################
DrAsset::DrAsset(DrProject *parent_project, long new_asset_key, QString new_asset_name, DrAsset_Types new_asset_type)
{
    m_parent_project = parent_project;
    setKey(new_asset_key);

    m_asset_type = new_asset_type;

    initializeAssetSettings(new_asset_name);

}

DrAsset::~DrAsset() {}



//####################################################################################
//##    Property loading - initializeAssetSettings
//####################################################################################

void DrAsset::initializeAssetSettings(QString new_name)
{
    addComponent(Object_Components::settings, "Settings", "Basic settings for current object.", Component_Colors::White_Snow, true);
    getComponent(Object_Components::settings)->setIcon(Component_Icons::Settings);

    addPropertyToComponent(Asset_Components::settings, Asset_Properties::name, Property_Type::String, new_name,
                           "Asset Name", "Name of the current asset.");
    addPropertyToComponent(Asset_Components::settings, Asset_Properties::collision_shape, Property_Type::Polygon, QPolygonF(),
                           "Collision Shape", "Shape of the object as it interacts with other objects in the world.");
    addPropertyToComponent(Asset_Components::settings, Asset_Properties::animation_default, Property_Type::Image, QPixmap(),
                           "Collision Shape", "Shape of the object as it interacts with other objects in the world.");

}




















