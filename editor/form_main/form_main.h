//
//      Created by Stephens Nunnally on 12/7/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      FormMain - Class that holds our main form window
//
//      FormMain Modes:                         Handle / Nickname
//          World Map: World / UI Layout            Map
//          World Editor                            Editor
//          UI Editor                               UI
//          Stage Map: Stage Layout
//
//      Main Components of FormMain while in normal "World Editor" mode:
//          Tool Bar
//          Advisor (Dock)
//          Inspector (Dock)
//
//          Components That Have Items that Can Appear in Inspector:
//              Asset List
//              Scene View
//                  Status Bar
//              Project Tree
//              Variable List
//              Bottom Area (Labels, Stages?)
//
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QMouseEvent>
#include <QPushButton>
#include <QScrollArea>
#include <QSplitter>
#include <QToolBar>
#include <QToolButton>
#include <QWidgetAction>

#include "core/colors/colors.h"
#include "editor/interface_editor_relay.h"
#include "project/constants_comps_and_props.h"

// Necessary forward declarations
class DrFilterHoverHandler;
class DrProject;
class EditorScene;
class EditorView;
class EditorViewRubberBand;
class FormPopup;
class NodeMapScene;
class NodeMapView;
class TreeAssets;
class TreeAdvisor;
class TreeInspector;
class TreeProject;


//####################################################################################
//##    FormMain
//##        Main editor window
//############################
class FormMain : public QMainWindow, public IEditorRelay
{
    Q_OBJECT

public:
    // Constructor / Destructor
    explicit FormMain(QWidget *parent = nullptr, std::string file_to_open = "");
    virtual ~FormMain() override;


private:
    // ***** Local Variables
    DrProject              *m_project           { nullptr };                        // BASE DEFINITION of DrProject!!!!! Holds current open game project
    DrProject              *m_external_images   { nullptr };                        // Project that loads included images
    DrFilterHoverHandler   *m_filter_hover      { nullptr };                        // Pointer to an event filter hover handler

    Editor_Mode             m_current_mode = Editor_Mode::Program_Loading;          // Holds what state FormMain is in


    // ***** Menu Widgets
    QMenuBar       *menuBar;
    QAction        *actionUndo, *actionRedo;


    // ***** Toolbar Widgets
    QList<QWidget*>      toolbarWidgets { };
    QList<QLayoutItem*>  toolbarSpacers { };

    QToolBar       *toolbar;
    QWidget        *widgetToolbar;              QHBoxLayout     *widgetToolbarLayout;

    QWidget        *widgetGroupMode;            QButtonGroup    *buttonsGroupMode;
    QWidget        *widgetGroupEdit;            QButtonGroup    *buttonsGroupEdit;              QToolButton *buttonAdd;
    QWidget        *widgetGroupLayering;        QButtonGroup    *buttonsGroupLayering;
    QWidget        *widgetGroupTransform;       QButtonGroup    *buttonsGroupTransform;
    QWidget        *widgetGroupGridFull;        QButtonGroup    *buttonsGroupGridFull;
    QWidget        *widgetGroupGridSimple;      QButtonGroup    *buttonsGroupGridSimple;
    QWidget        *widgetGroupPlay;            QButtonGroup    *buttonsGroupPlay;
    QWidget        *widgetGroupSettings;


    // ***** Shared FormMain Widgets
    QDockWidget    *dockAdvisor;            TreeAdvisor    *treeAdvisor;
    QDockWidget    *dockAssetsEditor;       TreeAssets     *treeAssetEditor;
    QDockWidget    *dockInspector;          TreeInspector  *treeInspector;


    // ***** "Empty" Main Widget
    QWidget        *widgetCentralClear;


    // ***** "Editor" (World Editor) Widgets
    TreeProject    *treeProjectEditor;                      // Shows Project Entities
    EditorScene    *sceneEditor;                            // Holds the currently selected Stage
    EditorView     *viewEditor;                             // Renders the StageView
    QWidget        *widgetCentralEditor;
    QScrollArea    *areaBottom;
    QFrame         *statusBar,      *viewToolBar;

    //       "Editor" Tool Bar Widgets
    QWidget        *widgetGroupMouse;       QButtonGroup   *buttonsGroupMouse;
    QWidget        *widgetGroupToggle;      QButtonGroup   *buttonsGroupToggle;
    QWidget        *widgetGroupHandTool;
    QWidget        *widgetGroupZoomTool;

    //       "Editor" Status Bar Widgets
    QLabel         *labelSelected,  *labelInfo,     *labelMousePosition;

    //       "Editor" Labels to display info
    QLabel         *label1,         *label2,        *label3,        *labelMouse1,   *labelMouse2;
    QLabel         *labelObject1,   *labelObject2,  *labelObject3,  *labelObject4,  *labelObject5;
    QLabel         *labelPosition,  *labelCenter,   *labelScale,    *labelRotate,   *labelZOrder,   *labelPosFlag;
    QLabel         *labelBottom;


    // ***** "WorldMap" Widgets
    QWidget        *widgetCentralWorldMap;
    NodeMapScene   *sceneWorldMap;
    NodeMapView    *viewWorldMap;



public:
    // Getters
    DrProject*          getProject()                { return m_project; }
    DrProject*          getExternalImageProject()   { return m_external_images; }

    // Misc functions
    void                setLabelText(Label_Names label_name, QString new_text);

    // Event Handlers
    virtual void        closeEvent(QCloseEvent *event) override;                                            // Inherited from QWidget
    virtual bool        eventFilter(QObject *obj, QEvent *event) override;                                  // Inherited from QObject
    virtual void        resizeEvent(QResizeEvent *event) override;                                          // Inherited from QWidget


    //############################ START: Interface Relay Implementations    
    virtual TreeAssets*         getAssetTree() override     { return treeAssetEditor; }
    virtual TreeInspector*      getInspector() override     { return treeInspector; }
    virtual TreeProject*        getProjectTree() override   { return treeProjectEditor; }
    virtual EditorView*         getStageView() override     { return viewEditor; }
    virtual NodeMapView*        getWorldMapView() override  { return viewWorldMap; }

    virtual Editor_Mode         getEditorMode() override;
    virtual void                setEditorMode(Editor_Mode new_mode) override;

    virtual void        buildAssetTree() override;
    virtual void        buildInspector(QList<long> entity_key_list, bool force_rebuild = false) override;
    virtual void        buildProjectTree() override;
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
    virtual void        viewZoomToScale(double zoom_scale) override;
    //############################ END: Interface Relay Implementations


public slots:
    virtual void        setAdvisorInfo(HeaderBodyList header_body_list) override;
    virtual void        setAdvisorInfo(QString header, QString body) override;
    virtual void        setMousePosition(std::string x, std::string y) override;


private:
    // Form Building / Setup
    void            buildMenu();
    void            buildSceneAfterLoading(long stage_key);
    void            buildToolBar();
    void            buildViewToolBar(QWidget *parent);
    void            buildCentralWidgetClear();
    void            buildCentralWidgetEditor();
    void            buildCentralWidgetWorldMap();
    void            changePalette(Color_Scheme new_color_scheme);
    QLabel*         createLabel(QWidget *parent, QString object_name, QRect label_rect, QFont &label_font);
    void            initializeFormMain();
    void            rebuildFormMain(Editor_Mode new_mode);

    // File Menu Functions
    bool            askShouldSave(QString before_what);
    void            menuNew(bool test_project = false);
    void            menuOpen();
    void            menuSave(bool save_as = false);
    // Edit Menu Functions
    void            menuUndo();
    void            menuRedo();
    // Debug Menu Functions
    void            menuListChildren();
    // Help Menu Functions
    void            menuAbout();

    // Toolbar Functions
    void            addToolbarGroup(QWidget *group, bool add_spacer = true);
    void            buttonGroupModeSetChecked(int id);
    void            clearToolbar();
    QPushButton*    createPushButton(QString name = "toolbarButton", QString text = "Button");
    QToolButton*    createToolbarButton(const QString &style_sheet_name, HeaderBodyList advisor_text, int w, int h,
                                        bool checkable = false, bool enabled = true);
    QLabel*         createToolbarSpacer(int height = 24, int space_on_the_right = 1, bool visible = true);
    void            setToolbar(Editor_Mode new_mode);
    void            updateToolbar();

private slots:
    // Main Toolbar
    void            buttonGroupEditClicked(int id);
    void            buttonGroupGridFullClicked(int id);
    void            buttonGroupGridSimpleClicked(int id);
    void            buttonGroupLayeringClicked(int id);
    void            buttonGroupModeClicked(int id);
    void            buttonGroupPlayClicked(int id);
    void            buttonGroupTransformClicked(int id);

    // View Toolbar
    void            buttonGroupMouseClicked(int id);
    void            buttonGroupToggleClicked(int id);

    void            buildSceneAfterWaiting(long stage_key);

    void            centerViewTimer(QPointF center_point);
    void            editMenuAboutToShow();
    void            editMenuAboutToHide();

signals:
    // Undo Stack Signals
    void            newStageSelected(DrProject *project, EditorScene *scene, long old_stage, long new_stage);

};



#endif // MAINWINDOW_H















