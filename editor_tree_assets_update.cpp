//
//      Created by Stephens Nunnally on 4/9/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "editor_tree_assets.h"
#include "interface_editor_relay.h"
#include "library.h"
#include "project.h"
#include "project_asset.h"
#include "project_world.h"
#include "project_world_stage.h"
#include "project_world_stage_object.h"


//####################################################################################
//##        Updates Asset List (like asset names) if items have been changed
//####################################################################################
void TreeAssets::updateAssetList(QList<DrSettings*> changed_items, QList<long> property_keys)
{
    if (changed_items.isEmpty()) return;
    if (property_keys.isEmpty()) return;

    QList<DrSettings*> newly_changed_items;
    QList<Properties>  newly_changed_properties;

    DrAsset *asset;
    QFrame  *text_holder;
    QLabel  *asset_name;
    QString  asset_text;

    for (auto item : changed_items) {
        long item_key = item->getKey();

        for (auto frame : m_asset_frames) {
            long label_key = frame->property(User_Property::Key).toLongLong();

            if (item_key == label_key) {
                for (auto property : property_keys) {
                    Properties check_property = static_cast<Properties>(property);

                    switch (check_property) {
                    case Properties::Asset_Name:
                        asset = m_project->getAsset(item_key);
                        asset_text = item->getComponentPropertyValue(Components::Asset_Settings, Properties::Asset_Name).toString();

                        // Update all objects in the project that use this asset name
                        for (auto world : m_project->getWorldMap()) {
                            for (auto stage : world.second->getStageMap()) {
                                for (auto object : stage.second->getObjectMap()) {
                                    if (object.second->getAssetKey() == asset->getKey()) {
                                        object.second->setComponentPropertyValue(Components::Object_Settings, Properties::Object_Name, asset_text);
                                        newly_changed_items.append(object.second);
                                        if (!newly_changed_properties.contains(Properties::Object_Name)) {
                                            newly_changed_properties.append(Properties::Object_Name);
                                        }
                                    }
                                }
                            }
                        }

                        // Update asset name label
                        asset_name = frame->findChild<QLabel*>("assetName");
                        if (asset_name) {
                            text_holder = frame->findChild<QFrame*>("textHolder");
                            asset_text = Dr::FitStringToWidth( asset_name->font(), asset_text, text_holder->width() );
                            asset_name->setText( asset_text );
                        }

                        break;
                    default: ;
                    }
                }
            }
        }
    }
    update();

    // If some assets were changed, and objects were updated, so update those objects in the other widgets
    if (newly_changed_items.isEmpty() == false)
        m_editor_relay->updateEditorWidgetsAfterItemChange(Editor_Widgets::Asset_Tree, newly_changed_items, newly_changed_properties);
}










