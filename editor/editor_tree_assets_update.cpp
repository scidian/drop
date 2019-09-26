//
//      Created by Stephens Nunnally on 4/9/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "editor_tree_assets.h"
#include "interface_editor_relay.h"
#include "helper.h"
#include "helper_qt.h"
#include "project/project.h"
#include "project/project_asset.h"
#include "project/project_world.h"
#include "project/project_world_stage.h"
#include "project/project_world_stage_thing.h"


//####################################################################################
//##    Updates Asset List (like asset names) if items have been changed
//####################################################################################
void TreeAssets::updateAssetList(QList<DrSettings*> changed_items, QList<long> property_keys) {
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
                        case Properties::Entity_Name:
                            asset = m_project->getAsset(item_key);
                            asset_text = item->getName();

                            // Update all Things in the project that use this asset name
                            for (auto world : m_project->getWorldMap()) {
                                for (auto stage : world.second->getStageMap()) {
                                    for (auto thing : stage.second->getThingMap()) {
                                        if (thing.second->getAssetKey() == asset->getKey()) {
                                            thing.second->setComponentPropertyValue(Components::Entity_Name, Properties::Entity_Name, asset_text);
                                            newly_changed_items.append(thing.second);
                                            if (!newly_changed_properties.contains(Properties::Entity_Name)) {
                                                newly_changed_properties.append(Properties::Entity_Name);
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

    // If some assets were changed, and items were updated, update those items in the other widgets
    if (newly_changed_items.isEmpty() == false)
        m_editor_relay->updateEditorWidgetsAfterItemChange(Editor_Widgets::Asset_Tree, newly_changed_items, newly_changed_properties);
}










