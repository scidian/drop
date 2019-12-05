//
//      Created by Stephens Nunnally on 1/3/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Base class to receive signals from Editor objects and relay them to derived form
//
//
#ifndef I_EDITOR_RELAY_H
#define I_EDITOR_RELAY_H

#include "editor/constants_editor.h"
#include "editor/enums_editor.h"
#include "model/enums_types.h"

// Forward declarations
class DrThing;
class DrProject;
class DrSettings;
class DrView;
class TreeAssets;
class TreeInspector;
class TreeProject;

//####################################################################################
//##    IEditorRelay
//##        An interface class used to communicate between Editor Mode widgets
//############################
class IEditorRelay
{
private:
    Editor_Widgets      m_active_widget = Editor_Widgets::Stage_View;

public:
    // Constructor / Destructor
    IEditorRelay() { }
    virtual ~IEditorRelay();

    // Editor Widget Accessors
    virtual TreeAssets*     getAssetTree() = 0;
    virtual TreeInspector*  getInspector() = 0;
    virtual TreeProject*    getProjectTree() = 0;
    virtual DrView*         getStageView() = 0;

    // Editor Functions
    virtual void        buildAssetTree() = 0;
    virtual void        buildInspector(QList<long> key_list, bool rebuild_only = false) = 0;
    virtual void        buildProjectTree() = 0;
    virtual void        buildScene(long stage_key) = 0;

    virtual void        updateEditorWidgetsAfterItemChange(Editor_Widgets changed_from, QList<DrSettings*> changed_items, QList<Properties> property_keys) = 0;
    virtual void        updateInspectorEnabledProperties() = 0;
    virtual void        updateItemSelection(Editor_Widgets selected_from, QList<long> optional_key_list = QList<long>{ }) = 0;

    virtual DrProject*  currentProject() = 0;
    virtual double      currentViewGridAngle() = 0;
    virtual QPointF     currentViewGridScale() = 0;
    virtual View_Mode   currentViewMode() = 0;
    virtual double      currentViewZoom() = 0;
    virtual QPointF     roundPointToGrid(QPointF point_in_scene) = 0;
    virtual void        viewCenterOnPoint(QPointF center_point) = 0;
    virtual void        viewZoomToScale(double zoom_scale) = 0;

    virtual void        setAdvisorInfo(HeaderBodyList header_body_list) = 0;
    virtual void        setAdvisorInfo(QString header, QString body) = 0;

    // Getter / Setters
    Editor_Widgets      getActiveWidget() { return m_active_widget; }
    void                setActiveWidget(Editor_Widgets widget);
};


#endif // I_EDITOR_RELAY_H








