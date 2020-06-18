//
//      Created by Stephens Nunnally on 5/27/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QDockWidget>

#include "core/sound.h"
#include "editor/form_animation/form_animation.h"
#include "editor/helper_library.h"
#include "editor/trees/tree_advisor.h"
#include "editor/trees/tree_assets.h"
#include "editor/view_editor/editor_view.h"


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

Editor_Mode FormAnimation::getEditorMode() { return Editor_Mode::Animation_Creator; }
void FormAnimation::setEditorMode(Editor_Mode new_mode) { }

void FormAnimation::buildAssetTree() {
    m_tree_assets->buildAssetTree("");
}


void FormAnimation::buildInspector(QList<long> entity_key_list, bool force_rebuild) { };
void FormAnimation::buildProjectTree(bool total_rebuild) { };
void FormAnimation::buildScene(long from_key) { };

void FormAnimation::updateEditorWidgetsAfterItemChange(Editor_Widgets changed_from, std::list<DrSettings*> changed_items,
                                                       std::list<ComponentProperty> property_names) { };
void FormAnimation::updateInspectorEnabledProperties() { };
void FormAnimation::updateItemSelection(Editor_Widgets selected_from, QList<long> optional_key_list) { };



DrProject*  FormAnimation::currentProject()                         { return m_project; }
double      FormAnimation::currentViewGridAngle()                   { return m_view_editor->currentGridAngle(); }
QPointF     FormAnimation::currentViewGridScale()                   { return m_view_editor->currentGridScale(); }
View_Mode   FormAnimation::currentViewMode()                        { return m_view_editor->currentViewMode(); }
double      FormAnimation::currentViewZoom()                        { return m_view_editor->currentZoomLevel(); }

QPointF     FormAnimation::roundPointToGrid(QPointF point_in_scene) { return m_view_editor->roundToGrid(point_in_scene); }

void        FormAnimation::viewCenterOnPoint(QPointF center_point)  { m_view_editor->centerOn(center_point); }
void        FormAnimation::viewFitToContents()                      { m_view_editor->zoomToContents(); }
void        FormAnimation::viewZoomToScale(double zoom_scale)       { m_view_editor->zoomToScale(zoom_scale); }

void        FormAnimation::stopAllSound()                           { Dr::GetSoLoud()->stopAll(); }

// Call to change the Advisor
void FormAnimation::setAdvisorInfo(HeaderBodyList header_body_list) { setAdvisorInfo(header_body_list[0], header_body_list[1]);  }
void FormAnimation::setAdvisorInfo(QString header, QString body) {
    if (m_dock_advisor == nullptr) return;
    if (m_tree_advisor == nullptr) return;
    if (m_dock_advisor->isHidden()) return;                                    // If Advisor dock was closed, cancel
    if (m_tree_advisor->getAdvisorHeader() == header &&
        m_tree_advisor->getAdvisorBody() == body) return;                      // If Advisor header and body is already set to proper info, cancel
    m_tree_advisor->changeAdvisor(header, body);
}
void FormAnimation::setMousePosition(std::string x, std::string y) {

}
void FormAnimation::viewUpdateToolbar(int button_id) { }








