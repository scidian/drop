//
//      Created by Stephens Nunnally on 11/25/2019, (c) 2019 Scidian Software, All Rights Reserved
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

#include "editor/colors/colors.h"
#include "editor/interface_editor_relay.h"


// Necessary forward declarations
class FormPopup;
class TreeAssets;
class TreeAdvisor;
class TreeInspector;
class TreeProject;
class DrProject;
class DrScene;
class DrView;
class DrViewRubberBand;
class DrFilterHoverHandler;


//####################################################################################
//##    FormAnimation
//##        Animation Editor
//############################
class FormAnimation : public QMainWindow, public IEditorRelay
{
    Q_OBJECT

private:
    // External Borrowed Pointers
    DrProject   *m_project;                                             // Pointer to the open project

    // Local Variables
    DrFilterHoverHandler   *m_filter_hover = nullptr;                   // Pointer to an event filter hover handler
    long                    m_animation_key;                            // Project Key for Animation we wish to Edit


    // ***** Shared FormAnimation Widgets
    TreeAdvisor    *treeAdvisor;                            // Custom classes for Advisor Window
    TreeAssets     *treeAssetEditor;                        // Shows Project Assets
    TreeInspector  *treeInspector;                          // Custom classes for Inspector Window
    TreeProject    *treeProjectEditor;
    QDockWidget    *dockAdvisor, *dockAssetsEditor, *dockInspector;
    QWidget        *widgetCentral;

    // ***** "Editor" (Animation Editor) Widgets
    DrScene        *sceneEditor;                            // Holds the currently selected Stage
    DrView         *viewEditor;                             // Renders the StageView
    QScrollArea    *areaBottom;


public:
    // Constructor / Destructor
    FormAnimation(DrProject *project, long animation_key, QWidget *parent = nullptr);
    virtual ~FormAnimation() override;

    // Property Getters / Setters
    long            getAnimationKey() { return m_animation_key; }
    void            setAnimationKey(long key) { m_animation_key = key; }

    // Interface Relay Implementations
    virtual TreeAssets*         getAssetTree() override     { return treeAssetEditor; }
    virtual TreeInspector*      getInspector() override     { return treeInspector; }
    virtual TreeProject*        getProjectTree() override   { return treeProjectEditor; }
    virtual DrView*             getStageView() override     { return viewEditor; }

    virtual void        buildAssetTree() override;
    virtual void        buildInspector(QList<long> key_list, bool rebuild_only = false) override;
    virtual void        buildProjectTree() override;
    virtual void        buildScene(long stage_key) override;

    virtual void        updateEditorWidgetsAfterItemChange(Editor_Widgets changed_from, QList<DrSettings*> changed_items,
                                                           QList<Properties> property_keys) override;
    virtual void        updateInspectorEnabledProperties() override;
    virtual void        updateItemSelection(Editor_Widgets selected_from, QList<long> optional_key_list = QList<long>{ }) override;

    virtual DrProject*  currentProject() override;
    virtual double      currentViewGridAngle() override;
    virtual QPointF     currentViewGridScale() override;
    virtual View_Mode   currentViewMode() override;
    virtual double      currentViewZoom() override;
    virtual QPointF     roundPointToGrid(QPointF point_in_scene) override;
    virtual void        viewCenterOnPoint(QPointF center_point) override;
    virtual void        viewZoomToScale(double zoom_scale) override;

public slots:
    virtual void        setAdvisorInfo(HeaderBodyList header_body_list) override;
    virtual void        setAdvisorInfo(QString header, QString body) override;


private:
    // Form Building / Setup
    void        buildCentralWidget();
    void        initializeFormAnimation();


};

#endif // FORM_ANIMATION_H





















