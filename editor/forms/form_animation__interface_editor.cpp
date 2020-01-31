//
//      Created by Stephens Nunnally on 11/25/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QDockWidget>

#include "editor/forms/form_animation.h"
#include "editor/helper_library.h"
#include "editor/trees/tree_advisor.h"
#include "editor/trees/tree_assets.h"
#include "editor/view/editor_view.h"


//####################################################################################
//##    Editor Interface Relay Handlers
//####################################################################################
//enum class Editor_Widgets {
//    Asset_Tree,
//    Inspector_Tree,
//    Project_Tree,
//    Stage_View,
//  Toolbar?

void FormAnimation::buildAssetTree() {
    treeAssetEditor->buildAssetTree("");
}




void FormAnimation::buildInspector(QList<long> entity_key_list, bool rebuild_only) { };
void FormAnimation::buildProjectTree() { };
void FormAnimation::buildScene(long stage_key) { };

void FormAnimation::updateEditorWidgetsAfterItemChange(Editor_Widgets changed_from, std::list<DrSettings*> changed_items,
                                                       std::list<ComponentProperty> property_keys) { };
void FormAnimation::updateInspectorEnabledProperties() { };
void FormAnimation::updateItemSelection(Editor_Widgets selected_from, QList<long> optional_key_list) { };



DrProject*  FormAnimation::currentProject()                         { return m_project; }
double      FormAnimation::currentViewGridAngle()                   { return viewEditor->currentGridAngle(); }
QPointF     FormAnimation::currentViewGridScale()                   { return viewEditor->currentGridScale(); }
View_Mode   FormAnimation::currentViewMode()                        { return viewEditor->currentViewMode(); }
double      FormAnimation::currentViewZoom()                        { return viewEditor->currentZoomLevel(); }

QPointF     FormAnimation::roundPointToGrid(QPointF point_in_scene) { return viewEditor->roundToGrid(point_in_scene); }

void        FormAnimation::viewCenterOnPoint(QPointF center_point)  { viewEditor->centerOn(center_point); }
void        FormAnimation::viewZoomToScale(double zoom_scale)       { viewEditor->zoomToScale(zoom_scale); }

// Call to change the Advisor
void FormAnimation::setAdvisorInfo(HeaderBodyList header_body_list) { setAdvisorInfo(header_body_list[0], header_body_list[1]);  }
void FormAnimation::setAdvisorInfo(QString header, QString body) {
    if (dockAdvisor == nullptr) return;
    if (treeAdvisor == nullptr) return;
    if (dockAdvisor->isHidden()) return;                                    // If Advisor dock was closed, cancel
    if (treeAdvisor->getAdvisorHeader() == header &&
        treeAdvisor->getAdvisorBody() == body) return;                      // If Advisor header and body is already set to proper info, cancel
    treeAdvisor->changeAdvisor(header, body);
}
void FormAnimation::setMousePosition(std::string x, std::string y) {

}









