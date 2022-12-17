//
//      Created by Stephens Nunnally on 4/9/2019, (c) 2019 Scidian Studios, All Rights Reserved
//
//  File:
//
//
//
#include <QDebug>
#include <QScrollBar>

#include "core/dr_containers.h"
#include "core/sound.h"
#include "editor/form_main/form_main.h"
#include "editor/helper_library.h"
#include "editor/interface_editor_relay.h"
#include "editor/preferences.h"
#include "editor/project/project.h"
#include "editor/trees/tree_assets.h"
#include "project/dr_project.h"
#include "project/entities/dr_stage.h"
#include "project/entities/dr_thing.h"
#include "project/entities/dr_world.h"
#include "project/entities_physics_2d/dr_asset.h"
#include "project/entities_sound/dr_mix.h"
#include "project/entities_sound/dr_sound.h"


//####################################################################################
//##    Sets Selected Key, causes selected item to highlight
//####################################################################################
void TreeAssets::setSelectedKey(long key, bool respond_to_selection) {
    m_selected_key = key;
    ensureSelectedKeyVisible();

    // Do something with new selection (play sound, etc) if respond_to_selection is true
    if (respond_to_selection) {
        DrSettings *entity = m_editor_relay->currentProject()->findSettingsFromKey(key);

        // If selected Asset is Sound, and auto play is enabled, play Sound
        if (entity->getType() == DrType::Sound) {
            if (Dr::GetPreference(Preferences::Mixer_Auto_Play_Asset_Sounds).toBool()) {
                DrSound *sound = static_cast<DrSound*>(entity);
                if (sound->getSoundType() == DrSoundType::Audio_File) {
                    m_editor_relay->stopAllSound();
                }
                Dr::GetSoLoud()->play(*sound->getAudioSource());
                if (Dr::GetActiveFormMain()) Dr::GetActiveFormMain()->drawVisuals();
            }
        }
    }

    m_editor_relay->updateItemSelection(Editor_Widgets::Asset_Tree, { key });
}

// Scrolls to selected item if necessary
void TreeAssets::ensureSelectedKeyVisible() {
    for (auto frame : m_asset_frames) {
        if (frame == nullptr) continue;

        if (frame->property(User_Property::Key).toLongLong() == m_selected_key) {
            QFrame *parent_frame = dynamic_cast<QFrame*>(frame->parentWidget());                // Parent is "Characters", "Objects", etc category frame
            if (parent_frame == nullptr) continue;

            int widget_top = frame->geometry().y() + parent_frame->geometry().y();
            int widget_bot = widget_top + frame->geometry().height();

            if (widget_bot > this->geometry().height()) {
                int new_position = verticalScrollBar()->value() + (widget_bot - this->geometry().height() + 10);
                if (new_position > this->verticalScrollBar()->maximum()) new_position = this->verticalScrollBar()->maximum();
                verticalScrollBar()->setValue( new_position );
            } else if (widget_top < 0) {
                int new_position = verticalScrollBar()->value() + (widget_top - 10);
                if (new_position > this->verticalScrollBar()->maximum()) new_position = this->verticalScrollBar()->maximum();
                verticalScrollBar()->setValue( new_position );
            }
            break;
        }
    }
    this->update();
}


//####################################################################################
//##    Updates Asset List (like asset names) if items have been changed
//####################################################################################
void TreeAssets::updateAssetList(std::list<DrSettings*> changed_entities, std::list<ComponentProperty> component_property_pairs) {
    if (changed_entities.empty()) return;
    if (component_property_pairs.empty()) return;

    std::list<DrSettings*> newly_changed_items;
    std::list<ComponentProperty> newly_changed_properties;

    QFrame  *text_holder;
    QLabel  *asset_name, *asset_pix;
    QString  asset_text;

    for (auto entity : changed_entities) {
        long entity_key = entity->getKey();

        for (auto frame : m_asset_frames) {
            long label_key = frame->property(User_Property::Key).toLongLong();

            if (entity_key == label_key) {
                for (auto component_property_pair : component_property_pairs) {
                    std::string comp = component_property_pair.first;
                    std::string prop = component_property_pair.second;

                    // ***** Update Asset Name
                    if (comp == Comps::Entity_Settings && prop == Props::Entity_Name) {
                        asset_text = QString::fromStdString(entity->getName());

                        // Update all Things in the project that use this asset name
                        for (auto &world_pair : getParentProject()->getWorldMap()) {
                            for (auto &stage_pair : world_pair.second->getStageMap()) {
                                for (auto &thing_pair : stage_pair.second->getThingMap()) {
                                    if (thing_pair.second->getAssetKey() == entity->getKey()) {
                                        thing_pair.second->setComponentPropertyValue(Comps::Entity_Settings, Props::Entity_Name, asset_text.toStdString());
                                        newly_changed_items.push_back(thing_pair.second);
                                        if (Dr::ListContains(newly_changed_properties, component_property_pair) == false) {
                                            newly_changed_properties.push_back(component_property_pair);
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
                    }

                    // ***** Update Colorized Pixmap of DrMix, DrSound
                    if (comp == Comps::Mix_Settings && prop == Props::Mix_Settings_Color) {
                        DrMix *mix = m_project->findMixFromKey(label_key);
                        asset_pix = frame->findChild<QLabel*>("assetPixmap");
                        if (asset_pix != nullptr && mix != nullptr) {
                            QSize pix_size = asset_pix->property(User_Property::Size).toSize();
                            asset_pix->setPixmap(Dr::GetAssetPixmapSound(DrSoundType::Mix, mix).scaled(pix_size, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                        }
                    }
                    if (comp == Comps::Sound_Settings && prop == Props::Sound_Settings_Color) {
                        DrSound *sound = m_project->findSoundFromKey(label_key);
                        asset_pix = frame->findChild<QLabel*>("assetPixmap");
                        if (asset_pix != nullptr && sound != nullptr) {
                            QSize pix_size = asset_pix->property(User_Property::Size).toSize();
                            asset_pix->setPixmap(Dr::GetAssetPixmapSound(sound->getSoundType(), sound).scaled(pix_size, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                        }
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
    QString type = item->data(0, User_Roles::Category_Name).toString();
    Dr::SetAssetExpanded(type.toStdString(), false);
}

void TreeAssets::handleExpanded(QTreeWidgetItem *item) {
    QString type = item->data(0, User_Roles::Category_Name).toString();
    Dr::SetAssetExpanded(type.toStdString(), true);
}


//####################################################################################
//##    Expand / collapse top level items based on last known setting
//####################################################################################
void TreeAssets::expandCollapseComponents() {
    for (auto &item : getListOfTopLevelItems()) {
        QString type = item->data(0, User_Roles::Category_Name).toString();

        if (Dr::GetAssetExpanded(type.toStdString())) {
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







