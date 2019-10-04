//
//      Created by Stephens Nunnally on 1/3/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Base class to receive signals from Editor objects and relay them to derived form
//
//
#ifndef I_EDITOR_RELAY_H
#define I_EDITOR_RELAY_H

#include "constants.h"
#include "enums.h"
#include "enums_form_main.h"

// Forward declarations
class DrThing;
class DrProject;
class DrSettings;

//####################################################################################
//##    IEditorRelay
//##        An interface class used to communicate between Editor Mode classes
//############################
class IEditorRelay
{
public:
    IEditorRelay() { }
    virtual ~IEditorRelay();

    virtual void        buildAssetTree() = 0;
    virtual void        buildInspector(QList<long> key_list) = 0;
    virtual void        buildProjectTree() = 0;
    virtual void        buildScene(long from_stage_key) = 0;

    virtual void        updateEditorWidgetsAfterItemChange(Editor_Widgets changed_from, QList<DrSettings*> changed_items, QList<Properties> property_keys) = 0;
    virtual void        updateItemSelection(Editor_Widgets selected_from) = 0;
    virtual void        updateInspectorEnabledProperties() = 0;

    virtual DrProject*  currentProject() = 0;
    virtual void        centerViewOnPoint(QPointF center_point) = 0;
    virtual double      currentViewGridAngle() = 0;
    virtual QPointF     currentViewGridScale() = 0;
    virtual View_Mode   currentViewMode() = 0;
    virtual double      currentViewZoom() = 0;
    virtual QPointF     roundPointToGrid(QPointF point_in_scene) = 0;

    virtual void        setAdvisorInfo(HeaderBodyList header_body_list) = 0;
    virtual void        setAdvisorInfo(QString header, QString body) = 0;
};





#endif // I_EDITOR_RELAY_H





