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
void DrProject::openProjectFromFile(QString open_file) {

    // !!!!! #IMPORTANT: Register custom QVariant Types
    qRegisterMetaTypeStreamOperators<DrShapeList>("DrShapeList");


    // ***** Open File
    QSettings settings(open_file, QSettings::Format::IniFormat);


    // ***** Read Options
    settings.beginReadArray("options");
    settings.setArrayIndex(0);
    QVariantMap options = settings.value("options").value<QVariantMap>();
    m_key_generator =                           options["key_generator"].toLongLong();
    setOption(Project_Options::Name,            options["name"]);
    setOption(Project_Options::File_Name_Path,  options["file_path"]);
    setOption(Project_Options::Current_World,   options["current_world"]);
    setOption(Project_Options::Orientation,     options["orientation"]);
    setOption(Project_Options::Width,           options["width"]);
    setOption(Project_Options::Height,          options["height"]);
    settings.endArray();


    // ***** Read Effects
    int effect_count = settings.beginReadArray("effects");
    for (int i = 0; i < effect_count; ++i) {
        settings.setArrayIndex(i);
        QVariantMap effect_data =   settings.value("effect").value<QVariantMap>();
        long    key =               effect_data["key"].toLongLong();
        QString name =              effect_data["name"].toString();
        DrEffectType type =         static_cast<DrEffectType>(effect_data["type"].toInt());
        addEffect(name, type, key);
    }
    settings.endArray();


    // ***** Read Images
    int image_count = settings.beginReadArray("images");
    for (int i = 0; i < image_count; ++i) {
        settings.setArrayIndex(i);
        QVariantMap image_data =    settings.value("image").value<QVariantMap>();
        long    key =               image_data["key"].toLongLong();
        QString full_path =         image_data["full_path"].toString();
        QString filename =          image_data["filename"].toString();
        QString simple_name =       image_data["simple_name"].toString();
        QImage  image =             image_data["image"].value<QPixmap>().toImage();
        addImage(key, full_path, filename, simple_name, image);
    }
    settings.endArray();


    // ***** Read Fonts
    int font_count = settings.beginReadArray("fonts");
    for (int i = 0; i < font_count; ++i) {
        settings.setArrayIndex(i);
        QVariantMap font_data =     settings.value("font").value<QVariantMap>();
        long    key =               font_data["key"].toLongLong();
        QString name =              font_data["font_name"].toString();
        QString family =            font_data["font_family"].toString();
        int font_size =             font_data["font_size"].toInt();
        QPixmap pix =               font_data["image"].value<QPixmap>();
        addFont(name, pix, family, font_size, true, key);
    }
    settings.endArray();


    // ***** Read Assets
    int asset_count = settings.beginReadArray("assets");
    for (int i = 0; i < asset_count; ++i) {
        settings.setArrayIndex(i);
        QVariantMap asset_data =    settings.value("asset").value<QVariantMap>();
        long key =                  asset_data["key"].toLongLong();
        long source_key =           asset_data["source_key"].toLongLong();
        DrAssetType type =          static_cast<DrAssetType>(asset_data["type"].toInt());
        addAsset(type, source_key, key);
    }
    settings.endArray();


    //addSettingsToMap(asset, asset_data);






    // ***** Add Initial World
    long world_1 = this->addWorld();
    DrWorld* current_world = this->findWorldFromKey(world_1);
    this->setOption(Project_Options::Current_World, QVariant::fromValue(world_1));
    this->setOption(Project_Options::Current_Stage, QVariant::fromValue(current_world->getFirstStageKey()) );



}




















