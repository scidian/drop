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

#include "editor/colors/colors.h"
#include "editor/debug.h"
#include "editor/forms/form_main.h"
#include "editor/globals_editor.h"
#include "editor/helper_library.h"
#include "editor/trees/tree_advisor.h"
#include "editor/trees/tree_assets.h"
#include "editor/trees/tree_inspector.h"
#include "editor/trees/tree_project.h"
#include "editor/view/editor_item.h"
#include "editor/view/editor_scene.h"
#include "editor/view/editor_view.h"
#include "model/enums_model_types.h"
#include "model/project/project.h"
#include "model/project/project_asset.h"
#include "model/project/project_world.h"
#include "model/project/project_stage.h"
#include "model/project/project_thing.h"
#include "model/settings/settings.h"
#include "model/settings/settings_component.h"
#include "model/settings/settings_component_property.h"


//####################################################################################
//##    Editor Interface Relay Handlers
//####################################################################################
//enum class Editor_Widgets {
//    Asset_Tree,
//    Inspector_Tree,
//    Project_Tree,
//    Stage_View,
//  Toolbar?

void FormMain::buildAssetTree() {
    treeAssetEditor->buildAssetTree();
}

// Sends new list to Inspector
void FormMain::buildInspector(QList<long> key_list, bool rebuild_only) {
    // If passed the no key constant, remove it to make an empty list
    key_list.removeOne(c_no_key);

    // If we're currently in the middle of selecting with rubber band box, don't update yet
    if (currentViewMode() == View_Mode::Selecting) return;

    // If key_list.count() == 0, then an empty list was passed in. This will clear the Inspector.
    // If we're doing anything at all in viewEditor (i.e. View_Mode != None), let's wait to clear the Inspector.
    if (currentViewMode() != View_Mode::None && key_list.count() == 0) return;

    treeInspector->buildInspectorFromKeys(key_list, rebuild_only);

    // !!!!! #TEMP: Testing to make sure not running non stop
    ///static long build_count = 0;
    ///Dr::SetLabelText(Label_Names::Label_Bottom, "Build Inspector: " + QString::number(build_count++));
}

void FormMain::buildProjectTree() {
    treeProjectEditor->buildProjectTree();
}

// Fires an Undo stack command to change Stages within Scene
void FormMain::buildScene(long stage_key) {
    if (sceneEditor->scene_mutex.tryLock(10) == false) return;

    // Rebuild existing Stage
    if (stage_key == c_same_key) {
        emit newStageSelected(m_project, sceneEditor, sceneEditor->getCurrentStageKeyShown(), sceneEditor->getCurrentStageKeyShown());

    // Select new stage
    } else if (sceneEditor->getCurrentStageKeyShown() != stage_key) {
        emit newStageSelected(m_project, sceneEditor, sceneEditor->getCurrentStageKeyShown(), stage_key);
    }

    sceneEditor->scene_mutex.unlock();
}

void FormMain::updateEditorWidgetsAfterItemChange(Editor_Widgets changed_from, std::list<DrSettings*> changed_items, std::list<Properties> property_keys) {
    // If theres notthing to update, go ahead and get out now
    if (changed_items.empty()) return;
    if (property_keys.empty()) return;

    // Don't update Inspector constantly while objects are being moved around, very slow
    if (currentViewMode() == View_Mode::Disable_Update) return;
    if (currentViewMode() == View_Mode::Translating)    return;
    if (currentViewMode() == View_Mode::Resizing)       return;
    if (currentViewMode() == View_Mode::Rotating)       return;

    // Convert list to longs
    std::list<long> property_keys_as_long = Dr::ConvertPropertyListToLongs(property_keys);

    // ***** This order is semi important, best not to try and change it
    if (changed_from != Editor_Widgets::Stage_View)         sceneEditor->updateChangesInScene(changed_items, property_keys_as_long);
    if (changed_from != Editor_Widgets::Inspector_Tree)     treeInspector->updateInspectorPropertyBoxes(changed_items, property_keys_as_long);
    if (changed_from != Editor_Widgets::Project_Tree)       treeProjectEditor->updateItems(changed_items, property_keys_as_long);
    if (changed_from != Editor_Widgets::Asset_Tree)         treeAssetEditor->updateAssetList(changed_items, property_keys_as_long);

    // !!!!! #TEMP: Testing to make sure not running non stop
    ///static long update_count = 0;
    ///Dr::SetLabelText(Label_Names::Label_Bottom, "Update Editor Widgets: " + QString::number(update_count++) +
    ///                                            ", Mode: " + viewEditor->currentViewModeAsString());
}

void FormMain::updateItemSelection(Editor_Widgets selected_from, QList<long> optional_key_list) {

    if (selected_from == Editor_Widgets::Stage_View || selected_from == Editor_Widgets::Project_Tree) {
        // Selects items in Scene View based on new selection in Project Tree
        if (selected_from != Editor_Widgets::Stage_View) {
            sceneEditor->updateSelectionFromProjectTree( treeProjectEditor->selectedItems() );
        }

        // Selects items in Project Tree based on new selection in Scene View
        if (selected_from != Editor_Widgets::Project_Tree)  {
            // If we passed custom list
            if (optional_key_list.count() > 0) {
                treeProjectEditor->updateSelectionFromKeyList( optional_key_list );

            // Otherwise, don't update project tree while rubber band box is being moved around, very slow
            } else if (viewEditor->currentViewMode() != View_Mode::Selecting) {
                treeProjectEditor->updateSelectionFromView( sceneEditor->getSelectionItems() );
            }
        }

    } else if (selected_from == Editor_Widgets::Asset_Tree) {
        // Code commented on purpose, Asset Tree gaining focus should not clear Project Tree / Stage View selection
        ///sceneEditor->updateSelectionFromKeyList( { } );
        ///treeProjectEditor->updateSelectionFromKeyList( { } );
    }

    // Updates status bar, enables / disables toolbar buttons
    this->updateToolbar();

    // !!!!! #TEMP: Testing to make sure not running non stop
    ///static long update_count = 0;
    ///Dr::SetLabelText(Label_Names::Label_Bottom, "Update Selection: " + QString::number(update_count++));
}

void FormMain::updateInspectorEnabledProperties() {
    treeInspector->updateLockedSettings();
};



DrProject*  FormMain::currentProject()                         { return m_project; }
double      FormMain::currentViewGridAngle()                   { return viewEditor->currentGridAngle(); }
QPointF     FormMain::currentViewGridScale()                   { return viewEditor->currentGridScale(); }
View_Mode   FormMain::currentViewMode()                        { return viewEditor->currentViewMode(); }
double      FormMain::currentViewZoom()                        { return viewEditor->currentZoomLevel(); }
QPointF     FormMain::roundPointToGrid(QPointF point_in_scene) { return viewEditor->roundToGrid(point_in_scene); }

// Fires a single shot timer to update view coordinates after event calls are done,
// sometimes centerOn function doesnt work until after an update() has been processed in the event loop
void FormMain::centerViewTimer(QPointF center_point) { viewEditor->centerOn(center_point); }
void FormMain::viewCenterOnPoint(QPointF center_point) {
    viewEditor->centerOn(center_point);
    QTimer::singleShot(0, this, [this, center_point] {
        this->centerViewTimer(center_point);
        viewEditor->setHasShownAScene(true);
    } );
}
void FormMain::viewZoomToScale(double zoom_scale) { viewEditor->zoomToScale(zoom_scale); }


// Call to change the Advisor
void FormMain::setAdvisorInfo(HeaderBodyList header_body_list) {
    setAdvisorInfo(header_body_list[0], header_body_list[1]);  }
void FormMain::setAdvisorInfo(QString header, QString body) {
    if (dockAdvisor == nullptr) return;
    if (treeAdvisor == nullptr) return;
    if (dockAdvisor->isHidden()) return;                                    // If Advisor dock was closed, cancel
    if (treeAdvisor->getAdvisorHeader() == header &&
        treeAdvisor->getAdvisorBody() == body) return;                      // If Advisor header and body is already set to proper info, cancel
    treeAdvisor->changeAdvisor(header, body);
}












