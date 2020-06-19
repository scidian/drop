//
//      Created by Stephens Nunnally on 3/3/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QApplication>
#include <QDebug>
#include <QDockWidget>
#include <QKeyEvent>
#include <QTimer>

#include "core/colors/colors.h"
#include "core/sound.h"
#include "editor/form_main/form_main.h"
#include "editor/helper_library.h"
#include "editor/interface_editor_relay.h"
#include "editor/preferences.h"
#include "editor/trees/tree_advisor.h"
#include "editor/trees/tree_assets.h"
#include "editor/trees/tree_inspector.h"
#include "editor/trees/tree_project.h"
#include "editor/view_editor/editor_item.h"
#include "editor/view_editor/editor_scene.h"
#include "editor/view_editor/editor_view.h"
#include "editor/view_mixer/mixer_scene.h"
#include "editor/view_mixer/mixer_view.h"
#include "editor/view_node/node_scene.h"
#include "editor/view_node/node_view.h"
#include "editor/widgets/widgets_view.h"
#include "editor/widgets/widgets_view_toolbar.h"
#include "engine/debug_flags.h"
#include "project/dr_project.h"
#include "project/enums_entity_types.h"
#include "project/entities/dr_stage.h"
#include "project/entities/dr_thing.h"
#include "project/entities/dr_world.h"
#include "project/entities_physics_2d/dr_asset.h"
#include "project/settings/settings.h"
#include "project/settings/settings_component.h"
#include "project/settings/settings_component_property.h"


//####################################################################################
//##    Editor Interface Relay Handlers
//####################################################################################
//enum class Editor_Widgets {
// Global Widgets
//      ToolBar,
//      View,
// Docks
//      Advisor_Tree,
//      Asset_Tree,
//      Inspector_Tree,
//      Project_Tree,

Editor_Mode FormMain::getEditorMode() { return m_current_mode; }
void FormMain::setEditorMode(Editor_Mode new_mode) {
    switch (new_mode) {
        case Editor_Mode::World_Graph:          this->rebuildFormMain(Editor_Mode::World_Graph);        break;
        case Editor_Mode::World_Creator:        this->rebuildFormMain(Editor_Mode::World_Creator);      break;
        case Editor_Mode::UI_Creator:           this->rebuildFormMain(Editor_Mode::Clear);              break;
        case Editor_Mode::Sound_Creator:        this->rebuildFormMain(Editor_Mode::Sound_Creator);      break;
        case Editor_Mode::Clear:                this->rebuildFormMain(Editor_Mode::Clear);              break;

        case Editor_Mode::Animation_Creator:
        case Editor_Mode::Component_Graph:
            this->rebuildFormMain(Editor_Mode::Clear);
            break;
        case Editor_Mode::Program_Loading:
            break;
    }
}

void FormMain::buildAssetTree() {
    m_tree_assets->buildAssetTree();
}

// Sends new list to Inspector
void FormMain::buildInspector(QList<long> entity_key_list, bool force_rebuild) {
    // If passed the no key constant, remove it to make an empty list
    entity_key_list.removeOne(c_no_key);

    // If we're currently in the middle of selecting with rubber band box, don't update yet
    if (currentViewMode() == View_Mode::Selecting) return;

    // If key_list.count() == 0, then an empty list was passed in. This will clear the Inspector.
    // If we're doing anything at all in m_view_editor (i.e. View_Mode != None), let's wait to clear the Inspector.
    if (currentViewMode() != View_Mode::None && entity_key_list.count() == 0) return;

    m_tree_inspector->buildInspectorFromKeys(entity_key_list, force_rebuild);

    // !!!!! #TEMP: Testing to make sure not running non stop
    ///static long build_count = 0;
    ///Dr::SetLabelText(Label_Names::Label_Bottom, "Build Inspector: " + QString::number(build_count++));
}

void FormMain::buildProjectTree(bool total_rebuild) {
    m_tree_project->buildProjectTree(total_rebuild);
}

// Fires an Undo stack command to change Stages within Scene
void FormMain::buildScene(long from_key) {
    if (getEditorMode() == Editor_Mode::World_Graph) {
        m_scene_world_graph->buildScene();

    } else if (getEditorMode() == Editor_Mode::World_Creator) {
        // Rebuild existing Stage
        if (from_key == c_same_key) {
            emit newStageSelected(m_project, m_scene_editor, m_scene_editor->getCurrentStageKeyShown(), m_scene_editor->getCurrentStageKeyShown());
        // Select new stage
        } else if (m_scene_editor->getCurrentStageKeyShown() != from_key) {
            emit newStageSelected(m_project, m_scene_editor, m_scene_editor->getCurrentStageKeyShown(), from_key);
        }

    } else if (getEditorMode() == Editor_Mode::World_Graph) {
        m_scene_mixer->buildScene(from_key);
    }
}

void FormMain::updateEditorWidgetsAfterItemChange(Editor_Widgets changed_from, std::list<DrSettings*> changed_items, std::list<ComponentProperty> property_names) {
    // If theres notthing to update, go ahead and get out now
    if (changed_items.empty())  return;
    if (property_names.empty()) return;

    // Don't update Inspector constantly while objects are being moved around, very slow
    if (currentViewMode() == View_Mode::Disable_Update) return;
    if (currentViewMode() == View_Mode::Translating)    return;
    if (currentViewMode() == View_Mode::Resizing)       return;
    if (currentViewMode() == View_Mode::Rotating)       return;

    // !!!!! #NOTE: This order is semi important, best NOT TO CHANGE IT !!!!!
    if (changed_from != Editor_Widgets::View) {
        if (getEditorMode() == Editor_Mode::World_Creator)  {   m_scene_editor->updateChangesInScene(changed_items, property_names); }
        if (getEditorMode() == Editor_Mode::World_Graph)    {   m_scene_world_graph->updateChangesInScene(changed_items, property_names); }
        if (getEditorMode() == Editor_Mode::Sound_Creator)  {   m_scene_mixer->updateChangesInScene(changed_items, property_names); }
    }
    if (changed_from != Editor_Widgets::Inspector_Tree)         m_tree_inspector->updateInspectorPropertyBoxes(changed_items, property_names);
    if (changed_from != Editor_Widgets::Project_Tree)           m_tree_project->updateItems(changed_items, property_names);
    if (changed_from != Editor_Widgets::Asset_Tree)             m_tree_assets->updateAssetList(changed_items, property_names);

    // !!!!! #TEMP: Testing to make sure not running non stop
    ///static long update_count = 0;
    ///Dr::SetLabelText(Label_Names::Label_Bottom, "Update Editor Widgets: " + QString::number(update_count++) +
    ///                                            ", Mode: " + Dr::StringFromViewMode(m_view_editor->currentViewMode()) );
}

void FormMain::updateItemSelection(Editor_Widgets selected_from, QList<long> optional_key_list) {

    if (selected_from == Editor_Widgets::View || selected_from == Editor_Widgets::Project_Tree) {
        // Selects items in Scene View based on new selection in Project Tree
        if (selected_from != Editor_Widgets::View) {
            if (getEditorMode() == Editor_Mode::World_Graph)    { m_scene_world_graph->updateSelectionFromProjectTree( m_tree_project->selectedItems() ); }
            if (getEditorMode() == Editor_Mode::World_Creator)  { m_scene_editor->updateSelectionFromProjectTree( m_tree_project->selectedItems() ); }
        }

        // Selects items in Project Tree based on new selection in Scene View
        if (selected_from != Editor_Widgets::Project_Tree)  {
            if (getEditorMode() == Editor_Mode::World_Creator) {
                // If we passed custom list
                if (optional_key_list.count() > 0) {
                    m_tree_project->updateSelectionFromKeyList( optional_key_list );
                // Otherwise, don't update project tree while rubber band box is being moved around, very slow
                } else if (m_view_editor->currentViewMode() != View_Mode::Selecting) {
                    m_tree_project->updateSelectionFromView( m_scene_editor->getSelectionItems() );
                }
            }
        }

    } else if (selected_from == Editor_Widgets::Asset_Tree) {
        // Code commented on purpose, Asset Tree gaining focus should not clear Project Tree / Editor View selection
        ///m_scene_editor->updateSelectionFromKeyList( { } );
        ///treeProjectEditor->updateSelectionFromKeyList( { } );
    }

    // Updates status bar, enables / disables toolbar buttons
    updateToolBar();

    // !!!!! #TEMP: Testing to make sure not running non stop
    ///static long update_count = 0;
    ///Dr::SetLabelText(Label_Names::Label_Bottom, "Update Selection: " + QString::number(update_count++));
}

void FormMain::updateInspectorEnabledProperties() {
    m_tree_inspector->updateLockedSettings();
};



DrProject*  FormMain::currentProject()                          { return m_project; }
double      FormMain::currentViewGridAngle() {
    if      (getEditorMode() == Editor_Mode::World_Graph)       { return m_view_world_graph->currentGridAngle(); }
    else if (getEditorMode() == Editor_Mode::World_Creator)     { return m_view_editor->currentGridAngle(); }
    return 0;
}
QPointF     FormMain::currentViewGridScale() {
    if      (getEditorMode() == Editor_Mode::World_Graph)       { return m_view_world_graph->currentGridScale(); }
    else if (getEditorMode() == Editor_Mode::World_Creator)     { return m_view_editor->currentGridScale(); }
    return QPointF(1.0, 1.0);
}
View_Mode   FormMain::currentViewMode() {
    if      (getEditorMode() == Editor_Mode::World_Graph)       { return m_view_world_graph->currentViewMode(); }
    else if (getEditorMode() == Editor_Mode::World_Creator)     { return m_view_editor->currentViewMode(); }
    return View_Mode::None;
}
double      FormMain::currentViewZoom() {
    if      (getEditorMode() == Editor_Mode::World_Graph)       { return m_view_world_graph->currentZoomLevel(); }
    else if (getEditorMode() == Editor_Mode::World_Creator)     { return m_view_editor->currentZoomLevel(); }
    return 1.0;
}
QPointF     FormMain::roundPointToGrid(QPointF point_in_scene) {
    if      (getEditorMode() == Editor_Mode::World_Graph)       { return m_view_world_graph->roundToGrid(point_in_scene); }
    else if (getEditorMode() == Editor_Mode::World_Creator)     { return m_view_editor->roundToGrid(point_in_scene); }
    return point_in_scene;
}

// Fires a single shot timer to update view coordinates after event calls are done,
// sometimes centerOn function doesnt work until after an update() has been processed in the event loop
void FormMain::centerViewTimer(QPointF center_point) {
    if (getEditorMode() == Editor_Mode::World_Graph) {
        m_view_world_graph->centerOn(center_point);
    } else if (getEditorMode() == Editor_Mode::World_Creator) {
        m_view_editor->centerOn(center_point);
        m_view_editor->setHasShownAScene(true);
    }
}
void FormMain::viewCenterOnPoint(QPointF center_point) {
    if      (getEditorMode() == Editor_Mode::World_Graph)       { m_view_world_graph->centerOn(center_point); }
    else if (getEditorMode() == Editor_Mode::World_Creator)     { m_view_editor->centerOn(center_point); }
    QTimer::singleShot(0, this, [this, center_point] { this->centerViewTimer(center_point); } );
}
void FormMain::viewFitToContents() {
    if      (getEditorMode() == Editor_Mode::World_Graph)       { m_view_world_graph->zoomToContents(); }
    else if (getEditorMode() == Editor_Mode::World_Creator)     { m_view_editor->zoomToContents(); }
}
void FormMain::viewUpdateToolbar(int button_id) {
    Mouse_Mode clicked = static_cast<Mouse_Mode>(button_id);
    if (m_view_world_graph != nullptr) {
        m_view_world_graph->setMouseMode(clicked);
        m_view_world_graph->spaceBarUp();
        if (m_toolbar_world_graph != nullptr) m_toolbar_world_graph->updateButtons(button_id);
    }
    if (m_view_editor != nullptr) {
        m_view_editor->setMouseMode(clicked);
        m_view_editor->spaceBarUp();
        if (m_toolbar_editor != nullptr) m_toolbar_editor->updateButtons(button_id);
    }
    updateToolBar();
}
void FormMain::viewZoomToScale(double zoom_scale) {
    if      (getEditorMode() == Editor_Mode::World_Graph)       { m_view_world_graph->zoomToScale(zoom_scale); }
    else if (getEditorMode() == Editor_Mode::World_Creator)     { m_view_editor->zoomToScale(zoom_scale); }
}

// Sound
void FormMain::stopAllSound() {
    Dr::GetSoLoud()->stopAll();
}

// SLOTS: Call to change the Advisor
void FormMain::setAdvisorInfo(HeaderBodyList header_body_list) { setAdvisorInfo(header_body_list[0], header_body_list[1]);  }
void FormMain::setAdvisorInfo(QString header, QString body) {
    // If Advisor not found, or Advisor dock was closed, cancel
    if (m_dock_advisor == nullptr) return;
    if (m_tree_advisor == nullptr) return;
    if (m_dock_advisor->isHidden()) return;

    // If Advisor header and body is already set to proper info, cancel
    if (m_tree_advisor->getAdvisorHeader() == header && m_tree_advisor->getAdvisorBody() == body) return;

    // Change Advisor text
    m_tree_advisor->changeAdvisor(header, body);
}
void FormMain::setMousePosition(std::string x, std::string y) {
    if (x == "" || y == "")
        m_label_mouse_position->setText(" ");
    else
        m_label_mouse_position->setText(" X: " + QString::fromStdString(x) + "  Y: " + QString::fromStdString(y) + " ");
}























