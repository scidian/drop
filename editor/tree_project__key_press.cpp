//
//      Created by Stephens Nunnally on 10/13/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QCheckBox>
#include <QDebug>
#include <QKeyEvent>
#include <QPainter>

#include "colors/colors.h"
#include "debug.h"
#include "editor/tree_project.h"
#include "editor_view/editor_view.h"
#include "globals.h"
#include "helper.h"
#include "helper_qt.h"
#include "imaging/imaging.h"
#include "interface_editor_relay.h"
#include "project/project.h"
#include "project/project_world.h"
#include "project/project_world_stage.h"
#include "project/project_world_stage_thing.h"
#include "settings/settings.h"
#include "settings/settings_component.h"
#include "settings/settings_component_property.h"


//####################################################################################
//##    Handles Key Event, forwards some keys to StageView
//####################################################################################
void TreeProject::keyPressEvent(QKeyEvent *event) {

    // ***** If we dont have selected items exit, otherwise get type of selected items
    if (selectedItems().count() < 1) { QTreeWidget::keyPressEvent(event); return; }
    long        first_key = selectedItems().first()->data(COLUMN_TITLE, User_Roles::Key).toLongLong();
    DrSettings *settings =  m_project->findSettingsFromKey( first_key, false );
    if (settings == nullptr)         { QTreeWidget::keyPressEvent(event); return; }
    DrType      type =      settings->getType();

    // ***** Duplication Keys
    if (event->key() == Qt::Key_W || event->key() == Qt::Key_A || event->key() == Qt::Key_S || event->key() == Qt::Key_D) {
        if (type == DrType::Thing) {
            m_editor_relay->getStageView()->keyPressEvent(event);
            return;
        }
    }

    // ***** Delete Key
    if (event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace) {
        if (selectedItems().count() > 0) {

            // Delete selected Things
            if (type == DrType::Thing) {
                m_editor_relay->getStageView()->keyPressEvent(event);

            // Delete selected Stages
            } else if (type == DrType::Stage) {
                DrStage *first_stage_selected = dynamic_cast<DrStage*>(settings);
                DrWorld *first_world_selected = first_stage_selected->getParentWorld();

                // Check if any Stages trying to be deleted are StartStages
                bool are_start_stages = false;
                for (auto item : selectedItems()) {
                    DrStage *stage = m_project->findStageFromKey( item->data(0, User_Roles::Key).toLongLong() );
                    if (stage->isStartStage()) are_start_stages = true;
                }

                // If not trying to delete a StartStage, continue deletion
                if (!are_start_stages) {
                    QMessageBox::StandardButton proceed;
                    proceed = Dr::ShowMessageBox("Are you sure you wish to delete the selected Stage(s)?", QMessageBox::Question,
                                                 "Delete Stage(s)?", this, QMessageBox::Yes | QMessageBox::No);
                    if (proceed == QMessageBox::StandardButton::No) return;

                    // Select stage before the selected stage
                    auto it = first_world_selected->getStageMap().find(first_key);
                    DrStage *new_selection = (--it)->second;
                    m_editor_relay->buildScene(new_selection->getKey());

                    // Delete selected Stages
                    for (auto item : selectedItems()) {
                        DrStage *stage = m_project->findStageFromKey( item->data(COLUMN_TITLE, User_Roles::Key).toLongLong() );
                        DrWorld *world = stage->getParentWorld();
                        world->deleteStage(stage);
                    }

                    // Rebuild Project Tree, select new shown Stage
                    buildProjectTree();
                    m_editor_relay->buildInspector( { new_selection->getKey() } );
                    m_editor_relay->updateItemSelection(Editor_Widgets::Stage_View, { new_selection->getKey() } );

                } else {
                    Dr::ShowMessageBox("Cannot delete Start Stages!", QMessageBox::Icon::Information, "Cannot Delete", this);
                }

            // Delete selected Worlds
            } else if (type == DrType::World) {
                if ((m_project->getWorldMap().size() > 1) && (static_cast<int>(m_project->getWorldMap().size()) > selectedItems().count())) {
                    QMessageBox::StandardButton proceed;
                    proceed = Dr::ShowMessageBox("Are you sure you wish to delete the selected World(s)?", QMessageBox::Question,
                                                 "Delete World(s)?", this, QMessageBox::Yes | QMessageBox::No);
                    if (proceed == QMessageBox::StandardButton::No) return;

                    // Build list of selected keys
                    QList<long> selected_world_keys;
                    for (auto item : selectedItems()) {
                        selected_world_keys.append( item->data(COLUMN_TITLE, User_Roles::Key).toLongLong() );
                    }

                    // Find a new world to select, going from back to front
                    DrWorld *new_selection = nullptr;
                    WorldMap::reverse_iterator it;
                    for (it = m_project->getWorldMap().rbegin(); it != m_project->getWorldMap().rend(); it++) {
                        auto world_pair = it;
                        if (selected_world_keys.contains(world_pair->first) == false) {
                            new_selection = world_pair->second;
                            break;
                        }
                    }
                    if (new_selection == nullptr) return;
                    m_editor_relay->buildScene(new_selection->getStartStageKey());

                    // Delete selected Worlds
                    for (auto item : selectedItems()) {
                        DrWorld *world = m_project->findWorldFromKey( item->data(COLUMN_TITLE, User_Roles::Key).toLongLong() );
                        m_project->deleteWorld(world);
                    }

                    // Rebuild Project Tree, select new shown Stage
                    buildProjectTree();
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























