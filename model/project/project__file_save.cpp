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
#include <QVariantMap>

#include "editor/globals_editor.h"
#include "editor/helper_library.h"
#include "model/properties/property_collision.h"
#include "model/project/project.h"
#include "model/project/project_animation.h"
#include "model/project/project_asset.h"
#include "model/project/project_effect.h"
#include "model/project/project_font.h"
#include "model/project/project_image.h"
#include "model/project/project_world.h"
#include "model/project/project_stage.h"
#include "model/project/project_thing.h"
#include "model/settings/settings.h"
#include "model/settings/settings_component.h"
#include "model/settings/settings_component_property.h"


// Local File Scope Functions
void    addSettingsToMap(DrSettings *entity, QVariantMap &map);


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
    ///qRegisterMetaTypeStreamOperators<DrPropertyCollision>("DrPropertyCollision");


    // ***** Open File for Writing
    QString   filename = QString::fromStdString(getOption(Project_Options::File_Name_Path).toString());
    QSettings settings(filename, QSettings::Format::IniFormat);
              ///if (settings.isWritable() == false) return;
              settings.clear();


    // ***** Write Options
    QVariantMap options;
    options["version_major"] =  QString::fromStdString( Dr::GetPreference(Preferences::Version_Major).toString());
    options["version_minor"] =  QString::fromStdString( Dr::GetPreference(Preferences::Version_Minor).toString());
    options["version_build"] =  QString::fromStdString( Dr::GetPreference(Preferences::Version_Build).toString());
    options["key_generator"] =  QVariant::fromValue(m_key_generator);
    options["name"] =           QString::fromStdString( getOption(Project_Options::Name).toString());
    options["file_path"] =      QString::fromStdString( getOption(Project_Options::File_Name_Path).toString());
    options["current_world"] =  QVariant::fromValue(getOption(Project_Options::Current_World).toInt());
    options["current_stage"] =  QVariant::fromValue(getOption(Project_Options::Current_Stage).toInt());
    options["orientation"] =    QVariant::fromValue(getOption(Project_Options::Orientation).toInt());
    options["width"] =          QVariant::fromValue(getOption(Project_Options::Width).toInt());
    options["height"] =         QVariant::fromValue(getOption(Project_Options::Height).toInt());
    settings.beginWriteArray("options");
    settings.setArrayIndex(0);
    settings.setValue("options", options);
    settings.endArray();


    // ***** Write Effects
    /**
    int effect_count = 0;
    for (auto effect_pair : m_effects) {
        if (effect_pair.first < c_key_starting_number) continue;                        // Don't save reserved items, keys / items handled by editor
        DrEffect *effect = effect_pair.second;
        QVariantMap effect_data;
        effect_data["key"] =        QVariant::fromValue(effect->getKey());
        effect_data["type"] =       QVariant(static_cast<int>(effect->getEffectType()));
        settings.beginWriteArray("effects");
        settings.setArrayIndex(effect_count++);
        settings.setValue("effect", effect_data);
        settings.endArray();
    }
    */


    // ***** Write Images
    int image_count = 0;
    for (auto image_pair : m_images) {
        if (image_pair.first < c_key_starting_number) continue;                        // Don't save reserved items, keys / items handled by editor
        DrImage *image = image_pair.second;
        QVariantMap image_data;
        image_data["key"] =         QVariant::fromValue(image->getKey());
        image_data["simple_name"] = QString::fromStdString(image->getSimplifiedName());
        image_data["image"] =       QVariant(QPixmap::fromImage(Dr::ToQImage(image->getBitmap())));
        settings.beginWriteArray("images");
        settings.setArrayIndex(image_count++);
        settings.setValue("image", image_data);
        settings.endArray();
    }

    // ***** Write Animations
    int animation_count = 0;
    for (auto animation_pair : m_animations) {
        if (animation_pair.first < c_key_starting_number) continue;                     // Don't save reserved items, keys / items handled by editor
        DrAnimation *animation = animation_pair.second;
        QVariantMap animation_data;
        animation_data["key"] =     QVariant::fromValue(animation->getKey());
        addSettingsToMap(animation, animation_data);
        settings.beginWriteArray("animations");
        settings.setArrayIndex(animation_count++);
        settings.setValue("animation", animation_data);
        settings.endArray();

        // Write Frames
        QString animation_array = "frames_in_animation:" + animation_data["key"].toString();
        int frame_count = 0;
        for (auto frame : animation->getFrames()) {
            QVariantMap frame_data;
            frame_data["key"] =     QVariant::fromValue(frame->getKey());
            addSettingsToMap(frame, frame_data);
            settings.beginWriteArray(animation_array);
            settings.setArrayIndex(frame_count++);
            settings.setValue("frame", frame_data);
            settings.endArray();
        }
    }

    // ***** Write Fonts
    int font_count = 0;
    for (auto font_pair : m_fonts) {
        if (font_pair.first < c_key_starting_number) continue;                          // Don't save reserved items, keys / items handled by editor
        DrFont *font = font_pair.second;
        QVariantMap font_data;
        font_data["key"] =          QVariant::fromValue(font->getKey());
        font_data["font_name"] =    QString::fromStdString(font->getName());
        font_data["font_family"] =  QString::fromStdString(font->getPropertyFontFamily());
        font_data["font_size"] =    QVariant::fromValue(font->getPropertyFontSize());
        font_data["image"] =        font->getPixmap();
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
        asset_data["source_key"] =  QVariant::fromValue(asset->getBaseKey());
        asset_data["type"] =        QVariant::fromValue(static_cast<int>(asset->getAssetType()));
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
        world_data["tree_expanded"] =   QVariant::fromValue(world->getExpanded());
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
            stage_data["tree_expanded"] =   QVariant::fromValue(stage->getExpanded());
            stage_data["is_start_stage"] =  QVariant::fromValue(stage->isStartStage());
            stage_data["center_point"] =    Dr::ToQPointF(stage->getViewCenterPoint());
            stage_data["zoom_scale"] =      QVariant::fromValue(stage->getViewZoomLevel());
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
                thing_data["type"] =        QVariant::fromValue(static_cast<int>(thing->getThingType()));
                addSettingsToMap(thing, thing_data);
                settings.beginWriteArray(stage_array);
                settings.setArrayIndex(thing_count++);
                settings.setValue("thing", thing_data);
                settings.endArray();
            } // end thing
        } // end stage
    } // end world

    // ***** Important! Let Drop know we don't need to save at this point, meaning its safe to close...
    //                  At least until some changes to the Project are made.
    setHasSaved(true);
}


//####################################################################################
//##    Copy all Components / Properties into QVariantMap for saving
//####################################################################################
void addSettingsToMap(DrSettings *entity, QVariantMap &map) {
    map["locked"] =  QVariant::fromValue(entity->isLocked());
    map["visible"] = QVariant::fromValue(entity->isVisible());
    for (auto component_pair : entity->getComponentMap()) {
        DrComponent *component = component_pair.second;
        QString map_key = QString::number(component->getComponentKey()) + ":";
        map[map_key + "display_name"] = QString::fromStdString(component->getDisplayName());
        map[map_key + "description"] =  QString::fromStdString(component->getDescription());
        map[map_key + "icon"] =         QString::fromStdString(component->getIcon());
        map[map_key + "color"] =        QVariant::fromValue(component->getColor().rgba());
        map[map_key + "turned_on"] =    QVariant::fromValue(component->isTurnedOn());
        map[map_key + "comp_key"] =     QVariant::fromValue(component->getComponentKey());

        for (auto property_pair : component->getPropertyMap()) {
            DrProperty *property = property_pair.second;
            QString map_key = QString::number(component->getComponentKey()) + ":" + QString::number(property->getPropertyKey()) + ":";
            map[map_key + "display_name"] = QString::fromStdString(property->getDisplayName());
            map[map_key + "description"] =  QString::fromStdString(property->getDescription());
            map[map_key + "data_type"] =    QVariant::fromValue(static_cast<int>(property->getPropertyType()));

// !!!!! #NEED_FIX_VARIANT_UPDATE
//            map[map_key + "value"] =      property->getValue();

            map[map_key + "prop_key"] =     QVariant::fromValue(property->getPropertyKey());
            map[map_key + "is_hidden"] =    QVariant::fromValue(property->isHidden());
            map[map_key + "is_editable"] =  QVariant::fromValue(property->isEditable());
        }
    }
}




















