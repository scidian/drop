//
//      Created by Stephens Nunnally on 3/3/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QDockWidget>
#include <QKeyEvent>
#include <QScrollBar>
#include <QTimer>

#include "colors.h"
#include "debug.h"
#include "enums.h"

#include "editor_item.h"
#include "editor_scene.h"
#include "editor_view.h"
#include "editor_tree_advisor.h"
#include "editor_tree_assets.h"
#include "editor_tree_inspector.h"
#include "editor_tree_project.h"

#include "form_main.h"

#include "globals.h"
#include "library.h"

#include "project.h"
#include "project_asset.h"
#include "project_world.h"
#include "project_world_stage.h"
#include "project_world_stage_object.h"
#include "settings.h"
#include "settings_component.h"
#include "settings_component_property.h"


//####################################################################################
//##        Editor Interface Relay Handlers
//####################################################################################
//enum class Editor_Widgets {
//    Asset_Tree,
//    Object_Inspector,
//    Project_Tree,
//    Scene_View,
//  Toolbar?

void FormMain::buildAssetTree() {
    treeAsset->buildAssetTree();
}

// Sends new list to Object Inspector
void FormMain::buildObjectInspector(QList<long> key_list) {
    // If we're currently in the middle of selecting with rubber band box, don't update yet
    if (currentViewMode() == View_Mode::Selecting) return;

    // If key_list.count() = 0, then an empty list was passed in. This will clear the object inspector.
    // If we're doing anything at all in viewMain (i.e. View_Mode != None), let's wait to clear the inspector.
    if (currentViewMode() != View_Mode::None && key_list.count() == 0) return;

    treeInspector->buildInspectorFromKeys(key_list);
}

void FormMain::buildProjectTree() {
    treeProject->buildProjectTree();
}

// Fires an Undo stack command to change Stages within Scene
void FormMain::buildScene(long from_stage_key) {
    if (scene->scene_mutex.tryLock(10) == false) return;

    if (scene->getCurrentStageKeyShown() != from_stage_key) {
        emit newStageSelected(project, scene, scene->getCurrentStageKeyShown(), from_stage_key);
    }

    scene->scene_mutex.unlock();
}




void FormMain::updateEditorWidgetsAfterItemChange(Editor_Widgets changed_from, QList<DrSettings*> changed_items, QList<Properties> property_keys)
{
    QList<long> property_keys_as_long = Dr::ConvertPropertyListToLongs(property_keys);

    if (currentViewMode() == View_Mode::Translating) return;

    if (changed_from != Editor_Widgets::Object_Inspector)   treeInspector->updateInspectorPropertyBoxes(changed_items, property_keys_as_long);
    if (changed_from != Editor_Widgets::Scene_View)         scene->updateChangesInScene(changed_items, property_keys_as_long);
    if (changed_from != Editor_Widgets::Project_Tree)       treeProject->updateItemNames(changed_items, property_keys_as_long);
    if (changed_from != Editor_Widgets::Asset_Tree)         treeAsset->updateAssetList(changed_items, property_keys_as_long);

    // !!!!! TEMP: Testing to make sure not running non stop
    Dr::SetLabelText(Label_Names::Label_Bottom, "Update Editor Widgets: " + Dr::CurrentTimeAsString());
}

void FormMain::updateItemSelection(Editor_Widgets selected_from)
{
    // Selects items in scene based on new selection in tree view
    if (selected_from != Editor_Widgets::Scene_View)    scene->updateSelectionFromProjectTree( treeProject->selectedItems() );
    if (selected_from != Editor_Widgets::Project_Tree)  {
        if (viewMain->currentViewMode() != View_Mode::Selecting)
        treeProject->updateSelectionFromView( scene->getSelectionItems() );
    }

    // !!!!! TEMP: Testing to make sure not running non stop
    Dr::SetLabelText(Label_Names::Label_Bottom, "Update Selection: " + Dr::CurrentTimeAsString());
}



// Fires a single shot timer to update view coordinates after event calls are done,
// sometimes centerOn function doesnt work until after an update() has been processed in the event loop
void FormMain::centerViewOnPoint(QPointF center_point) {
    viewMain->centerOn(center_point);
    QTimer::singleShot(0, this, [this, center_point] { this->centerViewTimer(center_point); } );
}
void FormMain::centerViewTimer(QPointF center_point) { viewMain->centerOn(center_point); }
double FormMain::currentViewGridAngle()                     { return viewMain->currentGridAngle(); }
QPointF FormMain::currentViewGridScale()                    { return viewMain->currentGridScale(); }
View_Mode FormMain::currentViewMode()                       { return viewMain->currentViewMode(); }
QPointF FormMain::roundPointToGrid(QPointF point_in_scene)  { return viewMain->roundToGrid(point_in_scene); }



//s Call to change the Advisor
void FormMain::setAdvisorInfo(HeaderBodyList header_body_list) {
    setAdvisorInfo(header_body_list[0], header_body_list[1]);  }
void FormMain::setAdvisorInfo(QString header, QString body) {
    Form_Main_Mode mode = static_cast<Form_Main_Mode>(Dr::GetPreference(Preferences::Form_Main_Mode).toInt());
    if (mode != Form_Main_Mode::World_Editor) return;
    if (advisor == nullptr) return;
    if (advisor->isHidden()) return;                                        // If Advisor dock was closed, cancel
    if (treeAdvisor == nullptr) return;
    if (treeAdvisor->getAdvisorHeader() == header) return;                  // If Advisor header is already set to proper info, cancel
    treeAdvisor->changeAdvisor(header, body);
}












