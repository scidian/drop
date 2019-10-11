//
//      Created by Stephens Nunnally on 4/9/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QScrollBar>

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
//##    Sets Selected Key, causes selected item to highlight
//####################################################################################
void TreeAssets::setSelectedKey(long key) {
    m_selected_key = key;

    // Scrolls to selected item if necessary
    for (auto frame : m_asset_frames) {
        if (frame->property(User_Property::Key).toLongLong() == key) {
            QFrame *parent_frame = dynamic_cast<QFrame*>(frame->parentWidget());
            int widget_top = frame->geometry().y() + parent_frame->geometry().y();
            int widget_bot = widget_top + frame->geometry().height();

            if (widget_bot > this->geometry().height()) {
                verticalScrollBar()->setValue( verticalScrollBar()->value() + (widget_bot - this->geometry().height() + 10));
            } else if (widget_top < 0) {
                verticalScrollBar()->setValue( verticalScrollBar()->value() + (widget_top - 10));
            }
            break;
        }
    }

    this->repaint();
}


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
    if (newly_changed_items.isEmpty() == false) {
        m_editor_relay->updateEditorWidgetsAfterItemChange(Editor_Widgets::Asset_Tree, newly_changed_items, newly_changed_properties);
    }
}










