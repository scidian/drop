//
//      Created by Stephens Nunnally on 10/10/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QSettings>
#include <QStandardPaths>

#include "project/project.h"
#include "project/project_animation.h"
#include "project/project_asset.h"
#include "project/project_effect.h"
#include "project/project_font.h"
#include "project/project_image.h"
#include "project/project_world.h"
#include "project/project_stage.h"
#include "project/project_thing.h"
#include "properties/property_collision.h"
#include "settings/settings.h"
#include "settings/settings_component.h"
#include "settings/settings_component_property.h"

#include "helper.h"
#include "helper_qt.h"


//####################################################################################
//##    Checks if map has Key
//####################################################################################
bool checkMapHasKey(QVariantMap &map, QString key) {
    QVariantMap::iterator it = map.find(key);
    return (it != map.end());
}


//####################################################################################
//##    Opens a Project File, returns false if unsuccessful
//####################################################################################
bool DrProject::openProjectFromFile(QString open_file) {

    // !!!!! #IMPORTANT: Register custom QVariant Types
    qRegisterMetaTypeStreamOperators<DrPropertyCollision>("DrPropertyCollision");
    qRegisterMetaTypeStreamOperators<DrPropertyCollision>("DrShapeList");               // !!!!! OLD !!!!!


    // ***** Open File
    QSettings settings(open_file, QSettings::Format::IniFormat);


    // ***** Read Options
    settings.beginReadArray("options");
    settings.setArrayIndex(0);
    QVariantMap options = settings.value("options").value<QVariantMap>();
    QString version_major = options["version_major"].toString();
    QString version_minor = options["version_minor"].toString();
    QString version_build = options["version_build"].toString();
    long    key_generator = options["key_generator"].toLongLong();
          m_key_generator = key_generator;

    // ***** Test for a basic property, if doesnt have this probably not a valid save file, return false
    if (checkMapHasKey(options, "name") == false) return false;

    setOption(Project_Options::Name,            options["name"]);
    setOption(Project_Options::File_Name_Path,  QVariant(open_file)); ///options["file_path"]);
    setOption(Project_Options::Current_World,   options["current_world"]);
    setOption(Project_Options::Current_Stage,   options["current_stage"]);
    setOption(Project_Options::Orientation,     options["orientation"]);
    setOption(Project_Options::Width,           options["width"]);
    setOption(Project_Options::Height,          options["height"]);
    settings.endArray();


    // ***** Adds Default Assets
    addDefaultAssets();


    // ***** Read Effects
    /**
    int effect_count = settings.beginReadArray("effects");
    for (int i = 0; i < effect_count; ++i) {
        settings.setArrayIndex(i);
        // Load Effect
        QVariantMap  effect_data =  settings.value("effect").value<QVariantMap>();
        long         effect_key =   effect_data["key"].toLongLong();
        DrEffectType effect_type =  static_cast<DrEffectType>(effect_data["type"].toInt());
        // If key doesnt already exist, initialize Effect
        if (findSettingsFromKey(effect_key, false) != nullptr) continue;
        addEffect(effect_type, effect_key);
    }
    settings.endArray();
    */


    // ***** Read Images
    int image_count = settings.beginReadArray("images");
    for (int i = 0; i < image_count; ++i) {
        settings.setArrayIndex(i);
        // Load Image
        QVariantMap image_data =    settings.value("image").value<QVariantMap>();
        long        image_key =     image_data["key"].toLongLong();
        QString     full_path =     image_data["full_path"].toString();
        QString     filename =      image_data["filename"].toString();
        QString     simple_name =   image_data["simple_name"].toString();
        QImage      image =         image_data["image"].value<QPixmap>().toImage().convertToFormat(QImage::Format_ARGB32);

        // If key doesnt already exist, initialize Image
        if (findSettingsFromKey(image_key, false) != nullptr) continue;
        addImage(image_key, full_path, filename, simple_name, image);
    }
    settings.endArray();


    // ***** Read Animations
    int animation_count = settings.beginReadArray("animations");
                          settings.endArray();
    for (int i = 0; i < animation_count; ++i) {
        settings.beginReadArray("animations");
        settings.setArrayIndex(i);

        // Load Animation
        QVariantMap animation_data =    settings.value("animation").value<QVariantMap>();
        long        animation_key =     checkMapHasKey(animation_data, "key") ? animation_data["key"].toLongLong() : c_no_key;

        // If key doesnt already exist, initialize Animation
        if (findSettingsFromKey(animation_key, false) != nullptr) continue;
        addAnimation( { }, animation_key );

        // Load World Settings, Variables
        DrAnimation *animation = findAnimationFromKey(animation_key);
        loadSettingsFromMap(animation, animation_data);
        settings.endArray();


        // ***** Read Frames
        QString animation_array = "frames_in_animation:" + animation_data["key"].toString();
        int frame_count = settings.beginReadArray(animation_array);
                          settings.endArray();
        for (int f = 0; f < frame_count; ++f) {
            settings.beginReadArray(animation_array);
            settings.setArrayIndex(f);

            // Load Frame
            QVariantMap frame_data =    settings.value("frame").value<QVariantMap>();
            long frame_key =            checkMapHasKey(frame_data, "key") ? frame_data["key"].toLongLong() : c_no_key;
            long frame_number = animation->addFrame(frame_key);

            // Load Thing Settings, Variables
            DrFrame *frame = animation->getFrame(frame_number);
            loadSettingsFromMap(frame, frame_data);
            settings.endArray();
        }
    }


    // ***** Read Fonts
    int font_count = settings.beginReadArray("fonts");
    for (int i = 0; i < font_count; ++i) {
        settings.setArrayIndex(i);
        // Load Font
        QVariantMap font_data =     settings.value("font").value<QVariantMap>();
        long        font_key =      font_data["key"].toLongLong();
        QString     name =          font_data["font_name"].toString();
        QString     family =        font_data["font_family"].toString();
        int         font_size =     font_data["font_size"].toInt();
        QPixmap     pix =           font_data["image"].value<QPixmap>();

        // If key doesnt already exist, initialize Font
        if (findSettingsFromKey(font_key, false) != nullptr) continue;
        addFont(name, pix, family, font_size, true, font_key);
    }
    settings.endArray();


    // ***** Read Assets
    int asset_count = settings.beginReadArray("assets");
    for (int i = 0; i < asset_count; ++i) {
        settings.setArrayIndex(i);
        // Load Asset
        QVariantMap asset_data =        settings.value("asset").value<QVariantMap>();
        long        asset_key =         asset_data["key"].toLongLong();
        long        base_key =          asset_data["source_key"].toLongLong();

        int         asset_type_as_int = asset_data["type"].toInt();
        if (asset_type_as_int != Dr::EnumToInt(DrAssetType::Object) &&
            asset_type_as_int != Dr::EnumToInt(DrAssetType::Character)) continue;
        DrAssetType asset_type =    static_cast<DrAssetType>(asset_type_as_int);

        // If key doesnt already exist, initialize Asset
        if (findSettingsFromKey(asset_key, false) != nullptr) continue;
        DrAsset* asset = addAsset(asset_type, base_key, asset_key);

        // Load Asset Settings, Variables
        loadSettingsFromMap(asset,  asset_data);
    }
    settings.endArray();


    // ***** Read Worlds
    int world_count = settings.beginReadArray("worlds");
                      settings.endArray();
    for (int i = 0; i < world_count; ++i) {
        settings.beginReadArray("worlds");
        settings.setArrayIndex(i);

        // Load World
        QVariantMap world_data =        settings.value("world").value<QVariantMap>();
        long        world_key =         checkMapHasKey(world_data, "key")               ? world_data["key"].toLongLong()            : c_no_key;
        long        start_stage_key =   checkMapHasKey(world_data, "start_stage")       ? world_data["start_stage"].toLongLong()    : c_no_key;
        long        editor_stage_key =  checkMapHasKey(world_data, "editor_stage")      ? world_data["editor_stage"].toLongLong()   : c_no_key;
        bool        world_expanded =    checkMapHasKey(world_data, "tree_expanded")     ? world_data["tree_expanded"].toBool()      : true;

        // If key doesnt already exist, initialize World
        if (findSettingsFromKey(world_key, false) != nullptr) continue;
        addWorld(world_key, start_stage_key, editor_stage_key);

        // Load World Settings, Variables
        DrWorld *world = findWorldFromKey(world_key);
        loadSettingsFromMap(world, world_data);
        world->setExpanded(world_expanded);
        settings.endArray();


        // ***** Read Stages
        QString world_array = "stages_in_world:" + world_data["key"].toString();
        int stage_count = settings.beginReadArray(world_array);
                          settings.endArray();
        for (int j = 0; j < stage_count; ++j) {
            settings.beginReadArray(world_array);
            settings.setArrayIndex(j);

            // Load Stage
            QVariantMap stage_data =        settings.value("stage").value<QVariantMap>();
            long        stage_key =         checkMapHasKey(stage_data, "key")            ? stage_data["key"].toLongLong()        : c_no_key;
            bool        stage_expanded =    checkMapHasKey(stage_data, "tree_expanded")  ? stage_data["tree_expanded"].toBool()  : true;
            bool        start_stage =       checkMapHasKey(stage_data, "is_start_stage") ? stage_data["is_start_stage"].toBool() : false;
            QPointF     center_point =      checkMapHasKey(stage_data, "center_point")   ? stage_data["center_point"].toPointF() : QPointF(0, 0);
            double      zoom_scale =        checkMapHasKey(stage_data, "zoom_scale")     ? stage_data["zoom_scale"].toDouble()   : 0.5;

            // If key doesnt already exist, initialize Stage
            if (findSettingsFromKey(stage_key, false) != nullptr) continue;
            world->addStage(stage_key, start_stage, center_point, zoom_scale);

            // Load Stage Settings, Variables
            DrStage *stage = findStageFromKey(stage_key);
            loadSettingsFromMap(stage, stage_data);
            stage->setExpanded(stage_expanded);
            settings.endArray();


            // ***** Read Things
            QString stage_array = "things_in_stage:" + stage_data["key"].toString();
            int thing_count = settings.beginReadArray(stage_array);
                              settings.endArray();
            for (int t = 0; t < thing_count; ++t) {
                settings.beginReadArray(stage_array);
                settings.setArrayIndex(t);

                // Load Thing
                QVariantMap thing_data =    settings.value("thing").value<QVariantMap>();
                long thing_key =            checkMapHasKey(thing_data, "key")            ? thing_data["key"].toLongLong()        : c_no_key;
                long asset_key =            checkMapHasKey(thing_data, "asset_key")      ? thing_data["asset_key"].toLongLong()  : c_no_key;
                DrThingType thing_type =    checkMapHasKey(thing_data, "type")           ? static_cast<DrThingType>(thing_data["type"].toInt()) : DrThingType::Object;

                // If key already exists, move on
                if (findSettingsFromKey(thing_key, false) != nullptr) continue;

                // Make sure thing uses Built-In Device / Effect Asset Key
                switch (thing_type) {
                    case DrThingType::Camera:       asset_key = c_key_device_camera;    break;

                    case DrThingType::Light:        asset_key = c_key_effect_light;     break;
                    case DrThingType::Water:        asset_key = c_key_effect_water;     break;
                    case DrThingType::Fire:         asset_key = c_key_effect_fire;      break;
                    case DrThingType::Mirror:       asset_key = c_key_effect_mirror;    break;
                    case DrThingType::Fisheye:      asset_key = c_key_effect_fisheye;   break;
                    case DrThingType::Swirl:        asset_key = c_key_effect_swirl;     break;
                    default: ;
                }

                // If we can find associated Asset, initialize Thing
                if (findSettingsFromKey(asset_key) == nullptr) continue;
                stage->addThing(thing_type, asset_key, 0, 0, 0, true, thing_key);

                // Load Thing Settings, Variables
                DrThing *thing = findThingFromKey(thing_key);
                loadSettingsFromMap(thing, thing_data);
                settings.endArray();
            }
        }
    }


    // ***** Important! Signify we don't need to save at this point!
    setHasSaved(true);


    // ***** Set Key Generator from Save file
    if (m_key_generator != key_generator) {
        ///Dr::ShowMessageBox("Warning, key generator was changed during File Open! \n "
        ///                   "Before: " + QString::number(key_generator) + ", After: " + QString::number(m_key_generator));
        ///m_key_generator = key_generator;
    }

    return true;
}


//####################################################################################
//##    Load all Components / Properties Settings
//####################################################################################
void DrProject::loadSettingsFromMap(DrSettings *entity, QVariantMap &map) {
    // ***** Load class variables
    entity->setLocked(  map["locked"].toBool() );
    entity->setVisible( map["visible"].toBool() );

    // ***** Go through and load Components
    for (auto component_pair : entity->getComponentMap()) {
        DrComponent *component = component_pair.second;
        QString map_key = QString::number(component->getComponentKey()) + ":";
        QString k;
        ///k = map_key + "display_name";       if (checkMapHasKey(map, k)) component->setDisplayName(  map[k].toString()   );
        k = map_key + "description";        if (checkMapHasKey(map, k)) component->setDescription(  map[k].toString()   );
        ///k = map_key + "icon";               if (checkMapHasKey(map, k)) component->setIcon(         map[k].toString()   );
        ///k = map_key + "color";              if (checkMapHasKey(map, k)) component->setColor(        QColor::fromRgba(map[k].toUInt())   );
        k = map_key + "turned_on";          if (checkMapHasKey(map, k)) component->setOnOrOff(      map[k].toBool()     );
        ///k = map_key + "comp_key";           if (checkMapHasKey(map, k)) component->setComponentKey( map[k].toLongLong() );

        // ***** Go through and load Properties of each Component
        for (auto property_pair : component->getPropertyMap()) {
            DrProperty *property = property_pair.second;


            // !!!!! #TEMP: Don't try to load collision shape for now, need to implement QDataStream overloads for DrPropertyCollision class
            if (property->getPropertyKey() == static_cast<int>(Properties::Asset_Collision_Image_Shape)) continue;

            // !!!!! #NOTE: Base Key / Idle Animation is set upon Asset Creation
            if (property->getPropertyKey() == static_cast<int>(Properties::Asset_Animation_Idle)) continue;


            // ***** Check data type is the same as we were expecting
            QString map_key = QString::number(component->getComponentKey()) + ":" + QString::number(property->getPropertyKey()) + ":";
            Property_Type check_property_type;
            k = map_key + "data_type";
            if (checkMapHasKey(map, k)) {
                check_property_type = static_cast<Property_Type>(map[k].toInt());
                // Already assigned by Drop
                ///property->setPropertyType(  static_cast<Property_Type>(map[k].toInt())  );
            } else {
                continue;
            }

            // ***** Copy Saved Property Values
            //          Property is fine if is same type, or if it was Positive and now is Double
            if ( (check_property_type == property->getPropertyType() ||
                 (check_property_type == Property_Type::Positive && property->getPropertyType() == Property_Type::Double)) ) {
                // Already assigned by Drop
                ///k = map_key + "display_name";   if (checkMapHasKey(map, k)) property->setDisplayName(   map[k].toString() );
                ///k = map_key + "description";    if (checkMapHasKey(map, k)) property->setDescription(   map[k].toString() );
                k = map_key + "value";          if (checkMapHasKey(map, k)) property->setValue(         map[k] );

                if (property->getPropertyKey() != static_cast<long>(Properties::Entity_Key)) {
                    k = map_key + "is_hidden";      if (checkMapHasKey(map, k)) property->setHidden(        map[k].toBool() );
                    k = map_key + "is_editable";    if (checkMapHasKey(map, k)) property->setEditable(      map[k].toBool() );
                }

                // Check that name is disabled for Things
                if (entity->getType() == DrType::Thing && property->getPropertyKey() == static_cast<long>(Properties::Entity_Name)) {
                    property->setEditable( false );
                // Keep Sub Order hidden
                } else if (property->getPropertyKey() == static_cast<long>(Properties::Thing_Sub_Z_Order)) {
                    property->setEditable( false );
                    property->setHidden( true );
                }

                // Already assigned by Drop
                ///k = map_key + "prop_key";       if (checkMapHasKey(map, k)) property->setPropertyKey(   map[k].toLongLong() );
            }
        }
    }

}



















