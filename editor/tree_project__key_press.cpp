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
//##    Handles Key Event, forwards some keys to SceneView
//####################################################################################
void TreeProject::keyPressEvent(QKeyEvent *event) {

    // ***** Delete Key
    if (event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace) {
        if (selectedItems().count() > 0) {
            // Get type of selected items
            long        first_key = selectedItems().first()->data(0, User_Roles::Key).toLongLong();
            DrSettings *settings =  m_project->findSettingsFromKey( first_key );
            DrType      type =      settings->getType();

            // Delete selected Things
            if (type == DrType::Thing) {
                m_editor_relay->getSceneView()->keyPressEvent(event);

            // Delete selected Stages
            } else if (type == DrType::Stage) {
                DrStage *first_stage_selected = dynamic_cast<DrStage*>(settings);
                DrWorld *first_world_selected = first_stage_selected->getParentWorld();

                // If not trying to delete a StartStage, continue deletion
                if (first_stage_selected->isStartStage() == false) {

                    // Select stage before the selected stage
                    auto it = first_world_selected->getStageMap().find(first_key);
                    DrStage *new_selection = (--it)->second;
                    m_editor_relay->buildScene(new_selection->getKey());

                    // Delete selected Stages
                    for (auto item : selectedItems()) {
                        DrStage *stage = m_project->findStageFromKey( item->data(0, User_Roles::Key).toLongLong() );
                        DrWorld *world = stage->getParentWorld();
                        world->deleteStage(stage);
                    }

                    // Rebuild Project Tree, select new shown Stage
                    buildProjectTree();
                    m_editor_relay->buildInspector( { new_selection->getKey() } );
                    m_editor_relay->updateItemSelection(Editor_Widgets::Scene_View, { new_selection->getKey() } );
                }

            // Delete selected Worlds
            } else if (type == DrType::World) {
                if ((m_project->getWorldMap().size() > 1) && (static_cast<int>(m_project->getWorldMap().size()) > selectedItems().count())) {
                    // Build list of selected keys
                    QList<long> selected_world_keys;
                    for (auto item : selectedItems()) {
                        selected_world_keys.append( item->data(0, User_Roles::Key).toLongLong() );
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
                        DrWorld *world = m_project->findWorldFromKey( item->data(0, User_Roles::Key).toLongLong() );
                        m_project->deleteWorld(world);
                    }

                    // Rebuild Project Tree, select new shown Stage
                    buildProjectTree();
                    m_editor_relay->buildInspector( { new_selection->getKey() } );
                    m_editor_relay->updateItemSelection(Editor_Widgets::Scene_View, { new_selection->getKey() } );
                }
            }
            return;
        }
    }

    // ***** Forward event to base class
    QTreeWidget::keyPressEvent(event);
}























