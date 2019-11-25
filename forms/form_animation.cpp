//
//      Created by Stephens Nunnally on 11/25/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "editor_view/editor_view.h"
#include "forms/form_animation.h"


//####################################################################################
//##    Constructor / Destructor for Animation Editor Window
//####################################################################################
FormAnimation::FormAnimation(DrProject *project, long animation_key, QWidget *parent) : QMainWindow(parent) {
    m_project = project;
    m_animation_key = animation_key;

}

FormAnimation::~FormAnimation() { }




void FormAnimation::buildAssetTree() { };
void FormAnimation::buildInspector(QList<long> key_list, bool rebuild_only) { };
void FormAnimation::buildProjectTree() { };
void FormAnimation::buildScene(long stage_key) { };

void FormAnimation::updateEditorWidgetsAfterItemChange(Editor_Widgets changed_from, QList<DrSettings*> changed_items,
                                                       QList<Properties> property_keys) { };
void FormAnimation::updateInspectorEnabledProperties() { };
void FormAnimation::updateItemSelection(Editor_Widgets selected_from, QList<long> optional_key_list) { };



DrProject*  FormAnimation::currentProject()                         { return m_project; }
double      FormAnimation::currentViewGridAngle()                   { return viewEditor->currentGridAngle(); }
QPointF     FormAnimation::currentViewGridScale()                   { return viewEditor->currentGridScale(); }
View_Mode   FormAnimation::currentViewMode()                        { return viewEditor->currentViewMode(); }
double      FormAnimation::currentViewZoom()                        { return viewEditor->currentZoomLevel(); }
QPointF     FormAnimation::roundPointToGrid(QPointF point_in_scene) { return viewEditor->roundToGrid(point_in_scene); }


void        FormAnimation::viewCenterOnPoint(QPointF center_point) { };
void        FormAnimation::viewZoomToScale(double zoom_scale) { };


void        FormAnimation::setAdvisorInfo(HeaderBodyList header_body_list) { };
void        FormAnimation::setAdvisorInfo(QString header, QString body) { };




