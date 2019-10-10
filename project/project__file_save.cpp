//
//      Created by Stephens Nunnally on 10/10/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QFile>
#include <QSettings>

#include "helper.h"
#include "project.h"
#include "project_asset.h"
#include "project_effect.h"
#include "project_font.h"
#include "project_image.h"
#include "project_world.h"
#include "project_world_stage.h"
#include "project_world_stage_thing.h"
#include "project/thing_shape_list.h"
#include "settings/settings.h"
#include "settings/settings_component.h"
#include "settings/settings_component_property.h"


//####################################################################################
//##    Save all Settings Component Properties to a File
//####################################################################################
void DrProject::saveProjectToFile() {

    // ***** Project Data to be Saved:
    //long        m_key_generator;                        // Variable to hand out unique id key's to all children items
    //OptionMap   m_options;                              // Map holding DrProject Wide options
    //WorldMap    m_worlds;                               // Holds worlds for the project
    //AssetMap    m_assets;                               // Holds assets for the project
    //EffectMap   m_effects;                              // Holds effects for the project
    //FontMap     m_fonts;                                // Holds custom fonts for the project
    //ImageMap    m_images;                               // Holds images for the project


    qRegisterMetaTypeStreamOperators<DrShapeList>("DrShapeList");



    // ***** Open File
    QString   filename = getOption(Project_Options::File_Name_Path).toString();
    QSettings settings(filename, QSettings::Format::IniFormat);
              settings.clear();

    // ***** Write Options
    settings.beginWriteArray("options");
    QVariantMap options;
    options["name"] =           getOption(Project_Options::Name);
    options["file_path"] =      getOption(Project_Options::Name);
    options["current_world"] =  getOption(Project_Options::Name);
    options["current_stage"] =  getOption(Project_Options::Name);
    options["orientation"] =    getOption(Project_Options::Name);
    options["width"] =          getOption(Project_Options::Name);
    options["height"] =         getOption(Project_Options::Name);
    settings.setArrayIndex(0);
    settings.setValue("options", options);

    // ***** Write Images
    settings.beginWriteArray("images");
    int image_count = 0;
    for (auto image_pair : m_images) {
        DrImage *image = image_pair.second;
        QVariantMap image_data;
        image_data["full_path"] =   QVariant(image->getFullPath());
        image_data["filename"] =    QVariant(image->getFilename());
        image_data["simple_name"] = QVariant(image->getSimplifiedName());
        image_data["data"] =        QVariant(image->getImage());
        settings.setArrayIndex(image_count++);
        settings.setValue("image", image_data);
    }

    // ***** Write Assets
    settings.beginWriteArray("assets");
    int asset_count = 0;
    for (auto asset_pair : m_assets) {
        DrAsset *asset = asset_pair.second;
        QVariantMap asset_data;
        asset_data["type"] = QVariant(Dr::EnumToInt(asset->getAssetType()) );
        asset_data["source_key"] =  QVariant::fromValue(asset->getKey());
        addSettingsToMap(asset, asset_data);
        settings.setArrayIndex(asset_count++);
        settings.setValue("asset", asset_data);
    }

}


//####################################################################################
//##    Copy all Components / Properties into QVariantMap for saving
//####################################################################################
void DrProject::addSettingsToMap(DrSettings *entity, QVariantMap &map) {
    map["locked"] =  QVariant::fromValue(entity->isLocked());
    map["visible"] = QVariant::fromValue(entity->isVisible());
    for (auto component_pair : entity->getComponentList()) {
        DrComponent *component = component_pair.second;
        QString map_key = QString::number(component->getComponentKey()) + ":";
        map[map_key + "display_name"] = QVariant(component->getDisplayName());
        map[map_key + "description"] =  QVariant(component->getDescription());
        map[map_key + "icon"] =         QVariant(component->getIcon());
        map[map_key + "color"] =        QVariant(component->getColor());
        map[map_key + "turned_on"] =    QVariant(component->isTurnedOn());
        map[map_key + "comp_key"] =     QVariant::fromValue(component->getComponentKey());

        for (auto property_pair : component->getPropertyList()) {
            DrProperty *property = property_pair.second;
            QString map_key = QString::number(component->getComponentKey()) + ":" + QString::number(property->getPropertyKey()) + ":";
            map[map_key + "display_name"] = QVariant(property->getDisplayName());
            map[map_key + "description"] =  QVariant(property->getDescription());
            map[map_key + "data_type"] =    QVariant(Dr::EnumToInt(property->getPropertyType()));
            map[map_key + "value"] =        property->getValue();
            map[map_key + "prop_key"] =     QVariant::fromValue(property->getPropertyKey());
            map[map_key + "is_hidden"] =    QVariant(property->isHidden());
            map[map_key + "is_editable"] =  QVariant(property->isEditable());
        }
    }
}




















