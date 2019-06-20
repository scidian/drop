//
//      Created by Stephens Nunnally on 12/13/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      DrAsset Class Definitions
//
//
#include "helper.h"
#include "project.h"
#include "project_asset.h"
#include "project_font.h"
#include "project_image.h"
#include "project_world.h"
#include "project_world_stage.h"
#include "project_world_stage_thing.h"
#include "settings/settings.h"
#include "settings/settings_component.h"
#include "settings/settings_component_property.h"


//####################################################################################
//##    Constructor, Destructor
//####################################################################################
DrAsset::DrAsset(DrProject *parent_project, long new_asset_key, DrAssetType new_asset_type, long source_key) {
    m_parent_project = parent_project;
    setKey(new_asset_key);

    m_asset_type = new_asset_type;
    m_source_key = source_key;

    m_list_order = new_asset_key;
    m_group_number = 0;

    QPixmap my_starting_pixmap;
    switch (new_asset_type) {
    case DrAssetType::Object:
        my_starting_pixmap = m_parent_project->getDrImage(source_key)->getPixmapFromImage();
        initializeAssetSettingsObject(m_parent_project->getDrImage(source_key)->getSimplifiedName(), my_starting_pixmap );
        break;
    case DrAssetType::Text:
        my_starting_pixmap = m_parent_project->getDrFont(source_key)->getPixmap();
        initializeAssetSettingsFont(m_parent_project->getDrFont(source_key));
        break;
    case DrAssetType::Character:
        my_starting_pixmap = m_parent_project->getDrImage(source_key)->getPixmapFromImage();
        initializeAssetSettingsCharacter(m_parent_project->getDrImage(source_key)->getSimplifiedName(), my_starting_pixmap );
        break;
    }

    m_width =  my_starting_pixmap.width();
    m_height = my_starting_pixmap.height();
}

DrAsset::~DrAsset() { }



//####################################################################################
//##    Property loading - initializeAssetSettings
//####################################################################################

void DrAsset::initializeAssetSettingsObject(QString new_name, QPixmap pixmap) {
    addComponent(Components::Asset_Settings, "Settings", "Basic settings for current asset.", Component_Colors::White_Snow, true);
    getComponent(Components::Asset_Settings)->setIcon(Component_Icons::Settings);

    addPropertyToComponent(Components::Asset_Settings, Properties::Asset_Name, Property_Type::String, new_name,
                           "Asset Name", "Name of the current asset.");
    addPropertyToComponent(Components::Asset_Settings, Properties::Asset_Collision_Shape, Property_Type::Polygon, QPolygonF(),
                           "Collision Shape", "Shape of the Object as it interacts with other Objects in the world.");


    addComponent(Components::Asset_Animation, "Animation", "Images to show for this asset", Component_Colors::Green_SeaGrass, true);
    getComponent(Components::Asset_Animation)->setIcon(Component_Icons::Animation);

    addPropertyToComponent(Components::Asset_Animation, Properties::Asset_Animation_Default, Property_Type::Image, QVariant(pixmap),
                           "Default Animation", "Image shown for this Object.");

}


void DrAsset::initializeAssetSettingsFont(DrFont *font) {
    addComponent(Components::Asset_Settings, "Settings", "Basic settings for current asset.", Component_Colors::White_Snow, true);
    getComponent(Components::Asset_Settings)->setIcon(Component_Icons::Settings);

    addPropertyToComponent(Components::Asset_Settings, Properties::Asset_Name, Property_Type::String, font->getName(),
                           "Asset Name", "Name of the current asset.");

    addComponent(Components::Asset_Font, "Font", "Font properties for this text asset.", Component_Colors::Orange_Medium, true);
    getComponent(Components::Asset_Font)->setIcon(Component_Icons::Font);

    addPropertyToComponent(Components::Asset_Font, Properties::Asset_Font_Family, Property_Type::String, font->getPropertyFontFamily(),
                           "Font Family", "Font used for this text asset.", false, false);
    addPropertyToComponent(Components::Asset_Font, Properties::Asset_Font_Size, Property_Type::Int, font->getPropertyFontSize(),
                           "Font Size", "Font size of this text asset.", false, false);

}


void DrAsset::initializeAssetSettingsCharacter(QString new_name, QPixmap pixmap) {
    addComponent(Components::Asset_Settings, "Settings", "Basic settings for current asset.", Component_Colors::White_Snow, true);
    getComponent(Components::Asset_Settings)->setIcon(Component_Icons::Settings);

    addPropertyToComponent(Components::Asset_Settings, Properties::Asset_Name, Property_Type::String, new_name,
                           "Asset Name", "Name of the current asset.");
    addPropertyToComponent(Components::Asset_Settings, Properties::Asset_Collision_Shape, Property_Type::Polygon, QPolygonF(),
                           "Collision Shape", "Shape of the Object as it interacts with other Objects in the world.");


    addComponent(Components::Asset_Animation, "Animation", "Images to show for this asset", Component_Colors::Green_SeaGrass, true);
    getComponent(Components::Asset_Animation)->setIcon(Component_Icons::Animation);

    addPropertyToComponent(Components::Asset_Animation, Properties::Asset_Animation_Default, Property_Type::Image, QVariant(pixmap),
                           "Default Animation", "Image shown for this Object.");

}

















