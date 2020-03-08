//
//      Created by Stephens Nunnally on 3/6/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QDebug>

#include "core/dr_debug.h"
#include "core/interface/dr_progress.h"
#include "editor/project/project.h"
#include "editor/helper_library.h"
#include "editor/interface_editor_relay.h"
#include "editor/preferences.h"
#include "editor/trees/tree_assets.h"
#include "engine/enums_engine.h"
#include "project/dr_project.h"
#include "project/entities/dr_asset.h"
#include "project/entities/dr_image.h"
#include "project/entities/dr_stage.h"
#include "project/entities/dr_thing.h"
#include "project/entities/dr_world.h"
#include "project/properties/property_collision.h"
#include "project/settings/settings_component_property.h"

namespace Dr {


//####################################################################################
//##    Checks that an Asset name is not already in the Project
//####################################################################################
QString FindEmptyAssetName(AssetMap &asset_map, DrAssetType asset_type, QString base_name) {
    QString new_name = "";
    bool    already_exists;
    int     count = 1;
    do {
        already_exists = false;
        new_name = base_name + " " + QString::number(count);

        // If name already exists we add to the count that will appear after the name
        for (auto asset_pair : asset_map) {
            if (asset_pair.second->getAssetType() == asset_type) {
                if (asset_pair.second->getName() == new_name.toStdString()) {
                    already_exists = true;
                }
            }
        }
        count++;
    } while (already_exists);
    return new_name;
}


//####################################################################################
//##    Adds Singular Asset to Project
//####################################################################################
DrAsset* AddAsset(DrProject *project, DrAssetType asset_type, long source_key, QString base_name) {
    DrAsset *asset = project->addAsset(asset_type, source_key);

    // If we didn't pass in a base name, derive from DrAssetType
    if (base_name == "") base_name = QString::fromStdString(Dr::StringFromAssetType(asset_type));

    // Find name with count for similar names
    QString new_name = FindEmptyAssetName(project->getAssetMap(), asset_type, base_name);
    asset->setName( new_name.toStdString() );
    return asset;
}


//####################################################################################
//##    Adds Singular Asset from Prefab Type to Project
//####################################################################################
DrAsset* AddPrefab(DrProject *project, DrPrefabType prefab_type) {
    DrAsset *asset = nullptr;
    switch (prefab_type) {
        case DrPrefabType::Blob:
            asset = Dr::AddAsset(project, DrAssetType::Object, c_key_image_blob, "Blob");
            asset->getComponentProperty(Comps::Asset_Physics, Props::Asset_Physics_Body_Style)->setValue(static_cast<int>(Body_Style::Circular_Blob));
            asset->getComponentProperty(Comps::Asset_Physics, Props::Asset_Physics_Body_Rigidness)->setValue(50.0);
            asset->getComponentProperty(Comps::Asset_Physics, Props::Asset_Physics_Body_Rigidness)->setEditable(true);
            break;
        case DrPrefabType::Foliage:
            asset = Dr::AddAsset(project, DrAssetType::Object, c_key_image_foliage, "Foliage");
            asset->getComponentProperty(Comps::Asset_Physics, Props::Asset_Physics_Body_Style)->setValue(static_cast<int>(Body_Style::Foliage));
            asset->getComponentProperty(Comps::Asset_Physics, Props::Asset_Physics_Body_Rigidness)->setValue(25.0);
            asset->getComponentProperty(Comps::Asset_Physics, Props::Asset_Physics_Body_Rigidness)->setEditable(true);
            break;
        case DrPrefabType::Spike:
            asset = Dr::AddAsset(project, DrAssetType::Object, c_key_image_spike, "Spike");
            asset->getComponentProperty(Comps::Asset_Collision, Props::Asset_Collision_Shape)->setValue(static_cast<int>(Collision_Shape::Triangle));
            asset->getComponentProperty(Comps::Asset_Health, Props::Asset_Health_Health)->setValue(100.0);
        break;
    }
    return asset;
}



}   // End namespace Dr















