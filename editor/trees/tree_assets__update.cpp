//
//      Created by Stephens Nunnally on 4/9/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QDebug>
#include <QScrollBar>

#include "editor/globals_editor.h"
#include "editor/helper_editor.h"
#include "editor/interface_editor_relay.h"
#include "editor/trees/tree_assets.h"
#include "library/dr_containers.h"
#include "model/project/project.h"
#include "model/project/project_asset.h"
#include "model/project/project_stage.h"
#include "model/project/project_thing.h"
#include "model/project/project_world.h"


//####################################################################################
//##    Sets Selected Key, causes selected item to highlight
//####################################################################################
void TreeAssets::setSelectedKey(long key) {
    m_selected_key = key;
    ensureSelectedKeyVisible();
}

// Scrolls to selected item if necessary
void TreeAssets::ensureSelectedKeyVisible() {
    for (auto frame : m_asset_frames) {
        if (frame->property(User_Property::Key).toLongLong() == m_selected_key) {
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
    this->update();
}


//####################################################################################
//##    Updates Asset List (like asset names) if items have been changed
//####################################################################################
void TreeAssets::updateAssetList(std::list<DrSettings*> changed_entities, std::list<long> property_keys) {
    if (changed_entities.empty()) return;
    if (property_keys.empty())    return;

    std::list<DrSettings*> newly_changed_items;
    std::list<Properties>  newly_changed_properties;

    QFrame  *text_holder;
    QLabel  *asset_name;
    QString  asset_text;

    for (auto entity : changed_entities) {
        long entity_key = entity->getKey();

        for (auto frame : m_asset_frames) {
            long label_key = frame->property(User_Property::Key).toLongLong();

            if (entity_key == label_key) {
                for (auto property : property_keys) {
                    Properties check_property = static_cast<Properties>(property);

                    switch (check_property) {
                        case Properties::Entity_Name:
                            asset_text = entity->getName();

                            // Update all Things in the project that use this asset name
                            for (auto world : getParentProject()->getWorldMap()) {
                                for (auto stage : world.second->getStageMap()) {
                                    for (auto thing : stage.second->getThingMap()) {
                                        if (thing.second->getAssetKey() == entity->getKey()) {
                                            thing.second->setComponentPropertyValue(Components::Entity_Settings, Properties::Entity_Name, asset_text);
                                            newly_changed_items.push_back(thing.second);
                                            if (Dr::ListContains(newly_changed_properties, Properties::Entity_Name) == false) {
                                                newly_changed_properties.push_back(Properties::Entity_Name);
                                            }
                                        }
                                    }
                                }
                            }

                            // Update asset name label
                            asset_name = frame->findChild<QLabel*>("assetName");
                            if (asset_name != nullptr) {
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

    // ***** Expand / collapse top level items based on last known setting
    expandCollapseComponents();

    // ***** Re-Sort Layouts
    for (auto layout : m_grid_layouts) {
        AssetFlowLayout *flow = layout.second;
        flow->sortItems();
        flow->update();
    }
    update();

    // ***** If some assets were changed, and items were updated, update those items in the other widgets
    if (newly_changed_items.empty() == false) {
        m_editor_relay->updateEditorWidgetsAfterItemChange(Editor_Widgets::Asset_Tree, newly_changed_items, newly_changed_properties);
    }
}


//####################################################################################
//##    SLOTS: Handles Expand / Collapse of QTreeWidgetItems
//####################################################################################
void TreeAssets::handleCollapsed(QTreeWidgetItem *item) {
    long type = item->data(0, User_Roles::Type).toLongLong();
    Asset_Category asset_type = static_cast<Asset_Category>(type);
    Dr::SetAssetExpanded(asset_type, false);
}

void TreeAssets::handleExpanded(QTreeWidgetItem *item) {
    long type = item->data(0, User_Roles::Type).toLongLong();
    Asset_Category asset_type = static_cast<Asset_Category>(type);
    Dr::SetAssetExpanded(asset_type, true);
}


//####################################################################################
//##    Expand / collapse top level items based on last known setting
//####################################################################################
void TreeAssets::expandCollapseComponents() {
    for (auto &item : getListOfTopLevelItems()) {
        long type = item->data(0, User_Roles::Type).toLongLong();
        Asset_Category asset_type = static_cast<Asset_Category>(type);

        if (Dr::GetAssetExpanded(asset_type)) {
            if (item->isExpanded() == false) {
                this->expandItem( item );
            }
        } else {
            if (item->isExpanded()) {
                this->collapseItem( item );
            }
        }
    }
}







