//
//      Created by Stephens Nunnally on 1/3/2019, (c) 2019 Scidian Studios, All Rights Reserved
//
//  File:
//      Base class to receive signals from Editor objects and relay them to derived form
//
//
#ifndef I_EDITOR_RELAY_H
#define I_EDITOR_RELAY_H

#include <QDockWidget>
#include <QList>
#include <QPointF>
#include <QString>
#include <QWidget>

#include "core/types/dr_variant.h"
#include "editor/constants_advisor_info.h"
#include "editor/enums_editor.h"
#include "project/constants_comps_and_props.h"
#include "project/enums_entity_types.h"

// Forward Declarations
class DrProject;
class DrSettings;
class DrThing;

class TreeAdvisor;
class TreeAssets;
class TreeInspector;
class TreeProject;
class TreeWaveForm;

class EditorView;
class MixerView;
class NodeView;

// Type Definitions
typedef std::map<Editor_Widgets, QDockWidget*>  Editor_Docks;


//####################################################################################
//##    IEditorRelay
//##        An interface class used to communicate between Editor Mode widgets
//############################
class IEditorRelay
{
private:
    // Editor Widgets
    Editor_Widgets              m_active_widget         { Editor_Widgets::View };           // Tracks which Editor_Widget has focus
    Editor_Docks                m_dock_widigets         { };                                // Holds all Docks for editor


public:
    // Constructor / Destructor
    IEditorRelay() { }
    virtual ~IEditorRelay();


    //############################ Base Class Functions

    // Editor Widgets
    Editor_Widgets              getActiveWidget()   { return m_active_widget; }
    void                        setActiveWidget(Editor_Widgets widget);

    Editor_Docks&               dockWidgets()       { return m_dock_widigets; }
    QDockWidget*                getDock(Editor_Widgets editor_widget);
    void                        setDock(Editor_Widgets editor_widget, QDockWidget *dock_widget) { m_dock_widigets[editor_widget] = dock_widget; }



    //############################ Abstract Functions

    // Virtual Editor Widget Accessors
    virtual TreeAdvisor*        getAdvisor()        { return nullptr; }
    virtual TreeAssets*         getAssetTree()      { return nullptr; }
    virtual TreeInspector*      getInspector()      { return nullptr; }
    virtual TreeProject*        getProjectTree()    { return nullptr; }
    virtual TreeWaveForm*       getWaveForm()       { return nullptr; }

    virtual EditorView*         getViewEditor()     { return nullptr; }
    virtual MixerView*          getViewMixer()      { return nullptr; }
    virtual NodeView*           getViewNode()       { return nullptr; }

    // Abstract Getters / Setters
    virtual Editor_Mode         getEditorMode() = 0;
    virtual void                setEditorMode(Editor_Mode new_mode) = 0;

    // Abstract Editor Functions
    virtual void        buildAssetTree() = 0;
    virtual void        buildInspector(QList<long> entity_key_list, bool force_rebuild = false) = 0;
    virtual void        buildProjectTree(bool total_rebuild = false) = 0;
    virtual void        buildScene(long from_key) = 0;

    virtual void        updateEditorWidgetsAfterItemChange(Editor_Widgets changed_from, std::list<DrSettings*> changed_items, std::list<ComponentProperty> property_names) = 0;
    virtual void        updateInspectorEnabledProperties() = 0;
    virtual void        updateItemSelection(Editor_Widgets selected_from, QList<long> optional_key_list = QList<long>{ }) = 0;

    virtual DrProject*  currentProject() = 0;
    virtual double      currentViewGridAngle() = 0;
    virtual QPointF     currentViewGridScale() = 0;
    virtual View_Mode   currentViewMode() = 0;
    virtual double      currentViewZoom() = 0;
    virtual QPointF     roundPointToGrid(QPointF point_in_scene) = 0;
    virtual void        viewCenterOnPoint(QPointF center_point) = 0;
    virtual void        viewFitToContents() = 0;
    virtual void        viewUpdateToolbar(int button_id) = 0;
    virtual void        viewZoomToScale(double zoom_scale) = 0;

    virtual void        stopAllSound() = 0;

    virtual void        setAdvisorInfo(HeaderBodyList header_body_list) = 0;
    virtual void        setAdvisorInfo(QString header, QString body) = 0;
    virtual void        setMousePosition(std::string x, std::string y) = 0;
};


#endif // I_EDITOR_RELAY_H
























