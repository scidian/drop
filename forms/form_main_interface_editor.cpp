//
//      Created by Stephens Nunnally on 3/3/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QApplication>
#include <QDockWidget>
#include <QKeyEvent>
#include <QTimer>

#include "colors/colors.h"
#include "debug.h"
#include "editor/editor_tree_advisor.h"
#include "editor/editor_tree_assets.h"
#include "editor/editor_tree_inspector.h"
#include "editor/editor_tree_project.h"
#include "editor_view/editor_item.h"
#include "editor_view/editor_scene.h"
#include "editor_view/editor_view.h"
#include "enums.h"
#include "form_main.h"
#include "globals.h"
#include "helper.h"
#include "helper_qt.h"
#include "project/project.h"
#include "project/project_asset.h"
#include "project/project_world.h"
#include "project/project_world_stage.h"
#include "project/project_world_stage_thing.h"
#include "settings/settings.h"
#include "settings/settings_component.h"
#include "settings/settings_component_property.h"


//####################################################################################
//##    Editor Interface Relay Handlers
//####################################################################################
//enum class Editor_Widgets {
//    Asset_Tree,
//    Inspector_Tree,
//    Project_Tree,
//    Scene_View,
//  Toolbar?

void FormMain::buildAssetTree() {
    treeAssetEditor->buildAssetTree();
}

// Sends new list to Inspector
void FormMain::buildInspector(QList<long> key_list) {
    // If we're currently in the middle of selecting with rubber band box, don't update yet
    if (currentViewMode() == View_Mode::Selecting) return;

    // If key_list.count() == 0, then an empty list was passed in. This will clear the Inspector.
    // If we're doing anything at all in viewEditor (i.e. View_Mode != None), let's wait to clear the Inspector.
    if (currentViewMode() != View_Mode::None && key_list.count() == 0) return;

    treeInspector->buildInspectorFromKeys(key_list);
}

void FormMain::buildProjectTree() {
    treeProjectEditor->buildProjectTree();
}

// Fires an Undo stack command to change Stages within Scene
void FormMain::buildScene(long from_stage_key) {
    if (sceneEditor->scene_mutex.tryLock(10) == false) return;

    if (sceneEditor->getCurrentStageKeyShown() != from_stage_key) {
        emit newStageSelected(m_project, sceneEditor, sceneEditor->getCurrentStageKeyShown(), from_stage_key);
    }

    sceneEditor->scene_mutex.unlock();
}

void FormMain::updateEditorWidgetsAfterItemChange(Editor_Widgets changed_from, QList<DrSettings*> changed_items, QList<Properties> property_keys) {

    // If theres notthing to update, go ahead and get out now
    if (changed_items.isEmpty()) return;
    if (property_keys.isEmpty()) return;

    // Don't update Inspector constantly while objects are being moved around, very slow
    if (currentViewMode() == View_Mode::Disable_Update) return;
    if (currentViewMode() == View_Mode::Translating)    return;
    if (currentViewMode() == View_Mode::Resizing)       return;
    if (currentViewMode() == View_Mode::Rotating)       return;

    // Convert list to longs
    QList<long> property_keys_as_long = Dr::ConvertPropertyListToLongs(property_keys);

    // ***** This order is semi important, best not to try and change it
    if (changed_from != Editor_Widgets::Scene_View)         sceneEditor->updateChangesInScene(changed_items, property_keys_as_long);
    if (changed_from != Editor_Widgets::Inspector_Tree)     treeInspector->updateInspectorPropertyBoxes(changed_items, property_keys_as_long);
    if (changed_from != Editor_Widgets::Project_Tree)       treeProjectEditor->updateItemNames(changed_items, property_keys_as_long);
    if (changed_from != Editor_Widgets::Asset_Tree)         treeAssetEditor->updateAssetList(changed_items, property_keys_as_long);

    // !!!!! #TEMP: Testing to make sure not running non stop
    Dr::SetLabelText(Label_Names::Label_Bottom, "Update Editor Widgets: " + Dr::CurrentTimeAsString() + ", Mode: " + viewEditor->currentViewModeAsString());
}

void FormMain::updateItemSelection(Editor_Widgets selected_from) {
    // Selects items in scene based on new selection in tree view
    if (selected_from != Editor_Widgets::Scene_View)    sceneEditor->updateSelectionFromProjectTree( treeProjectEditor->selectedItems() );
    if (selected_from != Editor_Widgets::Project_Tree)  {
        // Don't update project tree while rubber band box is being moved around, very slow
        if (viewEditor->currentViewMode() != View_Mode::Selecting)
            treeProjectEditor->updateSelectionFromView( sceneEditor->getSelectionItems() );
    }

    // Updates status bar, enables / disables toolbar buttons
    this->updateToolbar();


    // !!!!! #TEMP: Testing to make sure not running non stop
    Dr::SetLabelText(Label_Names::Label_Bottom, "Update Selection: " + Dr::CurrentTimeAsString());
}

void FormMain::updateInspectorEnabledProperties() {
    treeInspector->updateLockedSettings();
};



DrProject* FormMain::currentProject() { return m_project; }
// Fires a single shot timer to update view coordinates after event calls are done,
// sometimes centerOn function doesnt work until after an update() has been processed in the event loop
void FormMain::centerViewOnPoint(QPointF center_point) {
    viewEditor->centerOn(center_point);
    QTimer::singleShot(0, this, [this, center_point] {
        this->centerViewTimer(center_point);
        viewEditor->setHasShownAScene(true);
    } );
}
void      FormMain::centerViewTimer(QPointF center_point) { viewEditor->centerOn(center_point); }
double    FormMain::currentViewGridAngle()                   { return viewEditor->currentGridAngle(); }
QPointF   FormMain::currentViewGridScale()                   { return viewEditor->currentGridScale(); }
View_Mode FormMain::currentViewMode()                        { return viewEditor->currentViewMode(); }
double    FormMain::currentViewZoom()                        { return viewEditor->currentZoomLevel(); }
QPointF   FormMain::roundPointToGrid(QPointF point_in_scene) { return viewEditor->roundToGrid(point_in_scene); }


// Call to change the Advisor
void FormMain::setAdvisorInfo(HeaderBodyList header_body_list) {
    setAdvisorInfo(header_body_list[0], header_body_list[1]);  }
void FormMain::setAdvisorInfo(QString header, QString body) {
    if (dockAdvisor == nullptr) return;
    if (treeAdvisor == nullptr) return;
    if (dockAdvisor->isHidden()) return;                                    // If Advisor dock was closed, cancel
    if (treeAdvisor->getAdvisorHeader() == header) return;                  // If Advisor header is already set to proper info, cancel
    treeAdvisor->changeAdvisor(header, body);
}












