//
//      Created by Stephens Nunnally on 5/27/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef FORM_ANIMATION_H
#define FORM_ANIMATION_H

#include <QHBoxLayout>
#include <QMainWindow>
#include <QSplitter>
#include <QScrollArea>

#include "core/colors/colors.h"
#include "editor/interface_editor_relay.h"

// Forward Declarations
class DrFilterHoverHandler;
class EditorScene;
class EditorView;
class FormPopup;
class ViewRubberBand;


//####################################################################################
//##    FormAnimation
//##        Animation Editor
//############################
class FormAnimation : public QMainWindow, public IEditorRelay
{
    Q_OBJECT

private:
    // External Borrowed Pointers
    DrProject      *m_project;                                          // Pointer to the open project

    // Local Variables
    DrFilterHoverHandler   *m_filter_hover = nullptr;                   // Pointer to an event filter hover handler
    long                    m_animation_key;                            // Project Key for Animation we wish to Edit


    // ***** Shared FormAnimation Widgets
    TreeAdvisor    *m_tree_advisor;                                     // Custom classes for Advisor Window
    TreeAssets     *m_tree_assets;                                      // Shows Project Assets
    TreeInspector  *m_tree_inspector;                                   // Custom classes for Inspector Window
    TreeProject    *m_tree_project;
    QDockWidget    *m_dock_advisor, *m_dock_assets, *m_dock_inspector;
    QWidget        *m_widget_central;

    // ***** "Editor" (Animation Editor) Widgets
    EditorScene    *m_scene_editor;                                     // Holds the currently selected Stage
    EditorView     *m_view_editor;                                      // Renders the EditorView
    QScrollArea    *m_area_bottom;


public:
    // Constructor / Destructor
    FormAnimation(DrProject *project, long animation_key, QWidget *parent = nullptr);
    virtual ~FormAnimation() override;

    // Property Getters / Setters
    long            getAnimationKey() { return m_animation_key; }
    void            setAnimationKey(long key) { m_animation_key = key; }


    //############################ START: Interface Relay Implementations
    virtual TreeAdvisor*        getAdvisor() override       { return m_tree_advisor; }
    virtual TreeAssets*         getAssetTree() override     { return m_tree_assets; }
    virtual TreeInspector*      getInspector() override     { return m_tree_inspector; }
    virtual TreeProject*        getProjectTree() override   { return m_tree_project; }

    virtual EditorView*         getViewEditor() override    { return m_view_editor; }
    virtual MixerView*          getViewMixer() override     { return nullptr; }
    virtual NodeView*           getViewNode() override      { return nullptr; }

    virtual Editor_Mode         getEditorMode() override;
    virtual void                setEditorMode(Editor_Mode new_mode) override;

    virtual void        buildAssetTree() override;
    virtual void        buildInspector(QList<long> entity_key_list, bool force_rebuild = false) override;
    virtual void        buildProjectTree(bool total_rebuild = false) override;
    virtual void        buildScene(long stage_key) override;

    virtual void        updateEditorWidgetsAfterItemChange(Editor_Widgets changed_from, std::list<DrSettings*> changed_items,
                                                           std::list<ComponentProperty> property_names) override;
    virtual void        updateInspectorEnabledProperties() override;
    virtual void        updateItemSelection(Editor_Widgets selected_from, QList<long> optional_key_list = QList<long>{ }) override;

    virtual DrProject*  currentProject() override;
    virtual double      currentViewGridAngle() override;
    virtual QPointF     currentViewGridScale() override;
    virtual View_Mode   currentViewMode() override;
    virtual double      currentViewZoom() override;
    virtual QPointF     roundPointToGrid(QPointF point_in_scene) override;
    virtual void        viewCenterOnPoint(QPointF center_point) override;
    virtual void        viewFitToContents() override;
    virtual void        viewUpdateToolbar(int button_id) override;
    virtual void        viewZoomToScale(double zoom_scale) override;

public slots:
    virtual void        setAdvisorInfo(HeaderBodyList header_body_list) override;
    virtual void        setAdvisorInfo(QString header, QString body) override;
    virtual void        setMousePosition(std::string x, std::string y) override;
    //############################ END: Interface Relay Implementations


private:
    // Form Building / Setup
    void        buildCentralWidget();
    void        initializeFormAnimation();


};

#endif // FORM_ANIMATION_H

















