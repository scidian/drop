//
//      Created by Stephens Nunnally on 10/13/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QApplication>
#include <QCheckBox>
#include <QDebug>
#include <QKeyEvent>
#include <QPainter>

#include "core/colors/colors.h"
#include "editor/helper_library.h"
#include "editor/interface_editor_relay.h"
#include "editor/preferences.h"
#include "editor/trees/tree_project.h"
#include "editor/view/editor_scene.h"
#include "editor/view/editor_view.h"
#include "engine/debug_flags.h"
#include "project/dr_project.h"
#include "project/entities/dr_world.h"
#include "project/entities/dr_stage.h"
#include "project/entities/dr_thing.h"
#include "project/settings/settings.h"
#include "project/settings/settings_component.h"
#include "project/settings/settings_component_property.h"


//####################################################################################
//##    Handles Key Event, forwards some keys to StageView
//####################################################################################
void TreeProject::keyPressEvent(QKeyEvent *event) {

    // ***** If we dont have selected items exit, otherwise get type of selected items
    if (selectedItems().count() < 1) { QTreeWidget::keyPressEvent(event); return; }
    long        first_key = selectedItems().first()->data(COLUMN_TITLE, User_Roles::Key).toLongLong();
    DrSettings *settings =  getParentProject()->findSettingsFromKey( first_key, false );
    if (settings == nullptr)         { QTreeWidget::keyPressEvent(event); return; }
    DrType      type =      settings->getType();


    // ********** Duplication Keys
    if (event->key() == Qt::Key_W || event->key() == Qt::Key_A || event->key() == Qt::Key_S || event->key() == Qt::Key_D) {
        // ***** Duplicate Thing
        if (type == DrType::Thing) {
            m_editor_relay->getStageView()->keyPressEvent(event);
            return;
        }
    }

    if (event->key() == Qt::Key_D) {
        // ***** Duplicate Stage
        if (type == DrType::Stage) {
            DrStage *new_selected_stage = nullptr;
            int stage_count = 0;
            for (auto item : selectedItems()) {
                DrStage *stage = getParentProject()->findStageFromKey( item->data(COLUMN_TITLE, User_Roles::Key).toLongLong() );
                if (stage == nullptr) continue;

                // Find a new name for Stage
                std::vector<QString> stage_names;
                for (auto &stage_pair : stage->getParentWorld()->getStageMap()) {
                    stage_names.push_back( QString::fromStdString(stage_pair.second->getName()) );
                }
                QString new_name = Dr::FindCopyName(QString::fromStdString(stage->getName()), stage_names);

                DrStage *copy_stage = stage->getParentWorld()->addStageCopyFromStage(stage, new_name.toStdString());
                if (stage_count == 0) new_selected_stage = copy_stage;
                stage_count++;
            }
            if (new_selected_stage == nullptr) return;

            // Update Editor Widgets
            m_editor_relay->buildProjectTree();
            m_editor_relay->buildInspector( { new_selected_stage->getKey() } );
            m_editor_relay->updateItemSelection(Editor_Widgets::Stage_View, { new_selected_stage->getKey() } );
            m_editor_relay->buildScene( new_selected_stage->getKey() );
            return;

        // ***** Duplicate World
        } else if (type == DrType::World) {
            DrWorld *new_selected_world = nullptr;
            int world_count = 0;
            for (auto item : selectedItems()) {
                DrWorld *world = getParentProject()->findWorldFromKey( item->data(COLUMN_TITLE, User_Roles::Key).toLongLong() );
                if (world == nullptr) continue;

                // Find a new name for World
                std::vector<QString> world_names;
                for (auto &world_pair : getParentProject()->getWorldMap()) {
                    world_names.push_back( QString::fromStdString(world_pair.second->getName()) );
                }
                QString new_name = Dr::FindCopyName(QString::fromStdString(world->getName()), world_names);

                DrWorld *copy_world = getParentProject()->addWorldCopyFromWorld(world, new_name.toStdString());
                if (world_count == 0) new_selected_world = copy_world;
                world_count++;
            }
            if (new_selected_world == nullptr) return;

            // Update Editor Widgets
            m_editor_relay->buildProjectTree();
            m_editor_relay->buildInspector( { new_selected_world->getKey() } );
            m_editor_relay->updateItemSelection(Editor_Widgets::Stage_View, { new_selected_world->getKey() } );
            m_editor_relay->buildScene( new_selected_world->getLastStageShownKey() );
            return;
        }
    }


    // ********** Layering Keys
    if (event->key() == Qt::Key_Comma || event->key() == Qt::Key_Period || event->key() == Qt::Key_Less || event->key() == Qt::Key_Greater) {
        if (type == DrType::Thing) {
            m_editor_relay->getStageView()->keyPressEvent(event);
            return;
        }
    }


    // ********** Delete Keys
    if (event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace) {
        if (selectedItems().count() > 0) {

            // ***** Delete selected Things
            if (type == DrType::Thing) {
                m_editor_relay->getStageView()->keyPressEvent(event);

            // ***** Delete selected Stages
            } else if (type == DrType::Stage) {
                DrStage *first_stage_selected = dynamic_cast<DrStage*>(settings);
                DrWorld *first_world_selected = first_stage_selected->getParentWorld();

                // Check if any Stages trying to be deleted are StartStages
                bool are_start_stages = false;
                for (auto item : selectedItems()) {
                    DrStage *stage = getParentProject()->findStageFromKey( item->data(0, User_Roles::Key).toLongLong() );
                    if (stage->isStartStage()) are_start_stages = true;
                }

                // If not trying to delete a StartStage, continue deletion
                if (!are_start_stages) {
                    QMessageBox::StandardButton proceed;
                    proceed = Dr::ShowMessageBox("Are you sure you wish to delete the selected Stage(s)?", QMessageBox::Question,
                                                 "Delete Stage(s)?", this, QMessageBox::Yes | QMessageBox::No);
                    if (proceed == QMessageBox::StandardButton::No) return;

                    // Select Stage from Tree above the first selected Stage
                    DrStage *new_selection = nullptr;
                    QTreeWidgetItem *world_item = selectedItems().first()->parent();
                    int new_index = world_item->indexOfChild(selectedItems().first()) - 1;
                    if (new_index >= 0) {
                        QTreeWidgetItem *stage_item = world_item->child(new_index);
                        if (stage_item != nullptr) {
                            if (selectedItems().contains(stage_item) == false) {
                                long try_stage = stage_item->data(COLUMN_TITLE, User_Roles::Key).toLongLong();
                                new_selection = getParentProject()->findStageFromKey(try_stage);
                            }
                        }
                    }
                    // Fall back to trying for next highest Stage in StageMap
                    if (new_selection == nullptr) {
                        auto it = first_world_selected->getStageMap().find(first_key);
                        new_selection = (--it)->second;
                    }

                    // Build the new Stage
                    m_editor_relay->buildScene(new_selection->getKey());

                    // Delete selected Stages
                    for (auto item : selectedItems()) {
                        DrStage *stage = getParentProject()->findStageFromKey( item->data(COLUMN_TITLE, User_Roles::Key).toLongLong() );
                        DrWorld *world = stage->getParentWorld();
                        world->deleteStage(stage);
                    }

                    // Rebuild Project Tree, select new shown Stage
                    buildProjectTree(true);
                    m_editor_relay->buildInspector( { new_selection->getKey() } );
                    m_editor_relay->updateItemSelection(Editor_Widgets::Stage_View, { new_selection->getKey() } );

                } else {
                    Dr::ShowMessageBox("Start Stages cannot be deleted.", QMessageBox::Icon::Information, "Cannot Delete", this);
                }

            // ***** Delete selected Worlds
            } else if (type == DrType::World) {
                if ((getParentProject()->getWorldMap().size() > 1) && (static_cast<int>(getParentProject()->getWorldMap().size()) > selectedItems().count())) {
                    QMessageBox::StandardButton proceed;
                    proceed = Dr::ShowMessageBox("Are you sure you wish to delete the selected World(s)?", QMessageBox::Question,
                                                 "Delete World(s)?", this, QMessageBox::Yes | QMessageBox::No);
                    if (proceed == QMessageBox::StandardButton::No) return;

                    // Build list of selected keys
                    QList<long> selected_world_keys;
                    for (auto item : selectedItems()) {
                        selected_world_keys.append( item->data(COLUMN_TITLE, User_Roles::Key).toLongLong() );
                    }

                    // Find a new world to select, trying for QTreeWidgetItem above first selected
                    DrWorld *new_selection = nullptr;
                    int new_index = indexOfTopLevelItem(selectedItems().first()) - 1;
                    if (new_index >= 0) {
                        QTreeWidgetItem *world_item = topLevelItem(new_index);
                        if (world_item != nullptr) {
                            if (selectedItems().contains(world_item) == false) {
                                long try_world = world_item->data(COLUMN_TITLE, User_Roles::Key).toLongLong();
                                new_selection = getParentProject()->findWorldFromKey(try_world);
                            }
                        }
                    }
                    // Fall back to trying next highest World in WorldMap
                    if (new_selection == nullptr) {
                        WorldMap::reverse_iterator it;
                        for (it = getParentProject()->getWorldMap().rbegin(); it != getParentProject()->getWorldMap().rend(); it++) {
                            auto world_pair = it;
                            if (selected_world_keys.contains(world_pair->first) == false) {
                                new_selection = world_pair->second;
                                break;
                            }
                        }
                        if (new_selection == nullptr) return;
                    }

                    // Build the new Stage
                    m_editor_relay->buildScene(new_selection->getStartStageKey());

                    // Delete selected Worlds
                    for (auto item : selectedItems()) {
                        DrWorld *world = getParentProject()->findWorldFromKey( item->data(COLUMN_TITLE, User_Roles::Key).toLongLong() );
                        if (world != nullptr) getParentProject()->deleteWorld(world->getKey());
                    }

                    // Rebuild Project Tree, select new shown Stage
                    buildProjectTree(true);
                    m_editor_relay->buildInspector( { new_selection->getKey() } );
                    m_editor_relay->updateItemSelection(Editor_Widgets::Stage_View, { new_selection->getKey() } );
                } else {
                    Dr::ShowMessageBox("Cannot delete World(s)! <br><br> Project must have at least one World.",
                                       QMessageBox::Icon::Information, "Cannot Delete", this);
                }
            }
            return;
        }
    }

    // ***** Forward event to base class
    QTreeWidget::keyPressEvent(event);
}























