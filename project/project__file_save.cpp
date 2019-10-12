//
//      Created by Stephens Nunnally on 10/10/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QDebug>
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
    // Done
    //      long        m_key_generator;                    // Variable to hand out unique id key's to all children items
    //      OptionMap   m_options;                          // Map holding DrProject Wide options
    //      EffectMap   m_effects;                          // Holds effects for the project
    //      ImageMap    m_images;                           // Holds images for the project
    //      FontMap     m_fonts;                            // Holds custom fonts for the project
    //      AssetMap    m_assets;                           // Holds assets for the project

    // Need
    //      WorldMap    m_worlds;                           // Holds worlds for the project


    // !!!!! #IMPORTANT: Register custom QVariant Types
    qRegisterMetaTypeStreamOperators<DrShapeList>("DrShapeList");


    // ***** Open File for Writing
    QString   filename = getOption(Project_Options::File_Name_Path).toString();
    QSettings settings(filename, QSettings::Format::IniFormat);
              settings.clear();


    // ***** Write Options
    QVariantMap options;
    options["key_generator"] =  QVariant::fromValue(m_key_generator);
    options["name"] =           getOption(Project_Options::Name);
    options["file_path"] =      getOption(Project_Options::File_Name_Path);
    options["current_world"] =  getOption(Project_Options::Current_World);
    options["current_stage"] =  getOption(Project_Options::Current_Stage);
    options["orientation"] =    getOption(Project_Options::Orientation);
    options["width"] =          getOption(Project_Options::Width);
    options["height"] =         getOption(Project_Options::Height);
    settings.beginWriteArray("options");
    settings.setArrayIndex(0);
    settings.setValue("options", options);
    settings.endArray();


    // ***** Write Effects
    int effect_count = 0;
    for (auto effect_pair : m_effects) {
        if (effect_pair.first < c_key_starting_number) continue;                        // Don't save reserved items, keys / items handled by editor
        DrEffect *effect = effect_pair.second;
        QVariantMap effect_data;
        effect_data["key"] =        QVariant::fromValue(effect->getKey());
        effect_data["type"] =       QVariant(Dr::EnumToInt(effect->getEffectType()));
        settings.beginWriteArray("effects");
        settings.setArrayIndex(effect_count++);
        settings.setValue("effect", effect_data);
        settings.endArray();
    }


    // ***** Write Images
    int image_count = 0;
    for (auto image_pair : m_images) {
        if (image_pair.first < c_key_starting_number) continue;                        // Don't save reserved items, keys / items handled by editor
        DrImage *image = image_pair.second;
        QVariantMap image_data;
        image_data["key"] =         QVariant::fromValue(image->getKey());
        image_data["full_path"] =   QVariant(image->getFullPath());
        image_data["filename"] =    QVariant(image->getFilename());
        image_data["simple_name"] = QVariant(image->getSimplifiedName());
        image_data["image"] =       QVariant(QPixmap::fromImage(image->getImage()));
        settings.beginWriteArray("images");
        settings.setArrayIndex(image_count++);
        settings.setValue("image", image_data);
        settings.endArray();
    }


    // ***** Write Fonts
    int font_count = 0;
    for (auto font_pair : m_fonts) {
        if (font_pair.first < c_key_starting_number) continue;                        // Don't save reserved items, keys / items handled by editor
        DrFont *font = font_pair.second;
        QVariantMap font_data;
        font_data["key"] =          QVariant::fromValue(font->getKey());
        font_data["font_name"] =    QVariant(font->getName());
        font_data["font_family"] =  QVariant(font->getPropertyFontFamily());
        font_data["font_size"] =    QVariant(font->getPropertyFontSize());
        font_data["image"] =        QVariant(font->getPixmap());
        settings.beginWriteArray("fonts");
        settings.setArrayIndex(font_count++);
        settings.setValue("font", font_data);
        settings.endArray();
    }


    // ***** Write Assets
    int asset_count = 0;
    for (auto asset_pair : m_assets) {
        if (asset_pair.first < c_key_starting_number) continue;                        // Don't save reserved items, keys / items handled by editor
        DrAsset *asset = asset_pair.second;
        QVariantMap asset_data;
        asset_data["key"] =         QVariant::fromValue(asset->getKey());
        asset_data["source_key"] =  QVariant::fromValue(asset->getSourceKey());
        asset_data["type"] =        QVariant(Dr::EnumToInt(asset->getAssetType()));
        asset_data["list_order"] =  QVariant::fromValue(asset->getListOrder());
        addSettingsToMap(asset, asset_data);
        settings.beginWriteArray("assets");
        settings.setArrayIndex(asset_count++);
        settings.setValue("asset", asset_data);
        settings.endArray();
    }


    // ***** Write Worlds
    int world_count = 0;
    for (auto world_pair : m_worlds) {
        if (world_pair.first < c_key_starting_number) continue;                        // Don't save reserved items, keys / items handled by editor
        DrWorld *world = world_pair.second;
        QVariantMap world_data;
        world_data["key"] =             QVariant::fromValue(world->getKey());
        world_data["start_stage"] =     QVariant::fromValue(world->getStartStageKey());
        world_data["editor_stage"] =    QVariant::fromValue(world->getLastStageShownKey());
        addSettingsToMap(world, world_data);
        settings.beginWriteArray("worlds");
        settings.setArrayIndex(world_count++);
        settings.setValue("world", world_data);
        settings.endArray();

        // Write Stages
        QString world_array = "stages_in_world:" + world_data["key"].toString();
        int stage_count = 0;
        for (auto stage_pair : world->getStageMap()) {
            if (stage_pair.first < c_key_starting_number) continue;                     // Don't save reserved items, keys / items handled by editor
            DrStage *stage = stage_pair.second;
            QVariantMap stage_data;
            stage_data["key"] =             QVariant::fromValue(stage->getKey());
            stage_data["is_start_stage"] =  QVariant(stage->isStartStage());
            stage_data["center_point"] =    QVariant(stage->getViewCenterPoint());
            addSettingsToMap(stage, stage_data);
            settings.beginWriteArray(world_array);
            settings.setArrayIndex(stage_count++);
            settings.setValue("stage", stage_data);
            settings.endArray();

            // Write Things
            QString stage_array = "things_in_stage:" + stage_data["key"].toString();
            int thing_count = 0;
            for (auto thing_pair : stage->getThingMap()) {
                if (thing_pair.first < c_key_starting_number) continue;                 // Don't save reserved items, keys / items handled by editor
                DrThing *thing = thing_pair.second;
                QVariantMap thing_data;
                thing_data["key"] =         QVariant::fromValue(thing->getKey());
                thing_data["asset_key"] =   QVariant::fromValue(thing->getAssetKey());
                thing_data["type"] =        QVariant(Dr::EnumToInt(thing->getThingType()));
                addSettingsToMap(thing, thing_data);
                settings.beginWriteArray(stage_array);
                settings.setArrayIndex(thing_count++);
                settings.setValue("thing", thing_data);
                settings.endArray();
            } // end thing
        } // end stage
    } // end world


}


//####################################################################################
//##    Copy all Components / Properties into QVariantMap for saving
//####################################################################################
void DrProject::addSettingsToMap(DrSettings *entity, QVariantMap &map) {
    map["locked"] =  QVariant::fromValue(entity->isLocked());
    map["visible"] = QVariant::fromValue(entity->isVisible());
    for (auto component_pair : entity->getComponentMap()) {
        DrComponent *component = component_pair.second;
        QString map_key = QString::number(component->getComponentKey()) + ":";
        map[map_key + "display_name"] = QVariant(component->getDisplayName());
        map[map_key + "description"] =  QVariant(component->getDescription());
        map[map_key + "icon"] =         QVariant(component->getIcon());
        map[map_key + "color"] =        QVariant(component->getColor().rgba());
        map[map_key + "turned_on"] =    QVariant(component->isTurnedOn());
        map[map_key + "comp_key"] =     QVariant::fromValue(component->getComponentKey());

        for (auto property_pair : component->getPropertyMap()) {
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




















