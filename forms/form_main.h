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

#include <QMainWindow>

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMouseEvent>
#include <QPushButton>
#include <QScrollArea>
#include <QSplitter>
#include <QToolBar>
#include <QToolButton>
#include <QWidgetAction>

#include "colors/colors.h"
#include "interface_editor_relay.h"

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
//##    FormMain
//##        Main editor window
//############################
class FormMain : public QMainWindow, public IEditorRelay
{
    Q_OBJECT

private:
    // ***** Local Variables
    DrProject              *m_project;                                              // BASE DEFINITION of DrProject!!!!! Holds current open game project
    DrFilterHoverHandler   *m_filter_hover;                                         // Pointer to an event filter hover handler

    Form_Main_Mode          m_current_mode = Form_Main_Mode::Program_Loading;       // Holds what Form_Main_Mode state formMain is in


    // ***** Menu Widgets
    QMenuBar       *menuBar;
    QAction        *actionUndo, *actionRedo;


    // ***** Toolbar Widgets
    QList<QWidget*>      toolbarWidgets { };
    QList<QLayoutItem*>  toolbarSpacers { };

    QToolBar    *toolbar;
    QWidget     *widgetToolbar;         QHBoxLayout     *widgetToolbarLayout;

    QWidget     *widgetGroupMode;       QButtonGroup    *buttonsGroupMode;
    QWidget     *widgetGroupAdd;        QButtonGroup    *buttonsGroupAdd;
    QWidget     *widgetGroupEdit;       QButtonGroup    *buttonsGroupEdit;
    QWidget     *widgetGroupLayering;   QButtonGroup    *buttonsGroupLayering;
    QWidget     *widgetGroupTransform;  QButtonGroup    *buttonsGroupTransform;
    QWidget     *widgetGroupGrid;       QButtonGroup    *buttonsGroupGrid;
    QWidget     *widgetGroupPlay;       QButtonGroup    *buttonsGroupPlay;
    QWidget     *widgetGroupSettings;


    // ***** Shared FormMain Widgets
    TreeAdvisor    *treeAdvisor;                            // Custom classes for Advisor Window
    TreeInspector  *treeInspector;                          // Custom classes for Inspector Window
    QDockWidget    *dockAdvisor, *dockInspector;
    QWidget        *widgetCentral, *widgetAdvisor, *widgetInspector;


    // ***** "Editor" (World Editor) Widget
    TreeAssets     *treeAssetEditor;                        // Custom classes for Asset Tree
    TreeProject    *treeProjectEditor;                      // Custom classes for Project Tree
    DrScene        *sceneEditor;                            // Behind the scene data model that holds the currently selected Stage
    DrView         *viewEditor;                             // Renders the scene, allows for interaction
    QDockWidget    *dockAssetsEditor;
    QWidget        *widgetCentralEditor, *widgetAssestEditor, *widgetStage, *widgetStageView;
    QScrollArea    *areaBottom;
    QFrame         *statusBar;

    //       "Editor" Status Bar Widgets
    QLabel         *labelSelected;


    // ***** Labels to display info
    QLabel         *label1,         *label2,        *label3,        *labelMouse1,   *labelMouse2;
    QLabel         *labelObject1,   *labelObject2,  *labelObject3,  *labelObject4,  *labelObject5;
    QLabel         *labelPosition,  *labelCenter,   *labelScale,    *labelRotate,   *labelZOrder,   *labelPosFlag;
    QLabel         *labelBottom;


public:
    // Constructor and Destructor
    explicit FormMain(QWidget *parent = nullptr);
    virtual ~FormMain() override;

    // Misc functions
    void                setLabelText(Label_Names label_name, QString new_text);

    // Event Handlers
    virtual void        closeEvent(QCloseEvent *event) override;                                            // Inherited from QWidget
    virtual bool        eventFilter(QObject *obj, QEvent *event) override;                                  // Inherited from QObject
    virtual void        resizeEvent(QResizeEvent *event) override;                                          // Inherited from QWidget


    // Interface Relay Implementations
    virtual TreeAssets*     getAssetTree() override     { return treeAssetEditor; }
    virtual TreeInspector*  getInspector() override     { return treeInspector; }
    virtual TreeProject*    getProjectTree() override   { return treeProjectEditor; }
    virtual DrView*         getSceneView() override     { return viewEditor; }

    virtual void        buildAssetTree() override;
    virtual void        buildInspector(QList<long> key_list) override;
    virtual void        buildProjectTree() override;
    virtual void        buildScene(long stage_key) override;

    virtual void        updateEditorWidgetsAfterItemChange(Editor_Widgets changed_from, QList<DrSettings*> changed_items,
                                                           QList<Properties> property_keys) override;
    virtual void        updateInspectorEnabledProperties() override;
    virtual void        updateItemSelection(Editor_Widgets selected_from, QList<long> optional_key_list = QList<long>{ }) override;

    virtual DrProject*  currentProject() override;
    virtual void        centerViewOnPoint(QPointF center_point) override;
    virtual double      currentViewGridAngle() override;
    virtual QPointF     currentViewGridScale() override;
    virtual View_Mode   currentViewMode() override;
    virtual double      currentViewZoom() override;
    virtual QPointF     roundPointToGrid(QPointF point_in_scene) override;

public slots:
    virtual void        setAdvisorInfo(HeaderBodyList header_body_list) override;
    virtual void        setAdvisorInfo(QString header, QString body) override;


private:
    // Form Building / Setup
    void        buildMenu();
    void        buildSceneAfterLoading(long stage_key);
    void        buildToolBar();
    void        buildWidgetsEditor();
    void        buildWidgetsShared();
    void        changePalette(Color_Scheme new_color_scheme);
    QLabel*     createLabel(QWidget *parent, QString object_name, QRect label_rect, QFont &label_font);
    void        initializeFormMainSettings();
    void        setFormMainMode(Form_Main_Mode new_mode);

    void        initializeDockWidgets();
    void        lockDockWidth(QDockWidget *dock, int width);
    void        unlockDockWidth(QDockWidget *dock);

    // File Menu Functions
    void        menuNew();
    void        menuOpen();
    void        menuSave(bool save_as = false);
    // Edit Menu Functions
    void        menuUndo();
    void        menuRedo();
    // Debug Menu Functions
    void        menuListChildren();
    // Help Menu Functions
    void        menuAbout();

    // Toolbar Functions
    void            addToolbarGroup(QWidget *group, bool add_spacer = true);
    void            buttonGroupModeSetChecked(int id);
    void            clearToolbar();
    QPushButton*    createPushButton(QString name = "toolbarButton", QString text = "Button");
    QToolButton*    createToolbarButton(const QString &style_sheet_name, HeaderBodyList advisor_text, int w, int h,
                                        bool checkable = false, bool enabled = true);
    QLabel*         createToolbarSpacer(int height = 24, int space_on_the_right = 1, bool visible = true);
    void            setToolbar(Form_Main_Mode new_mode);
    void            updateToolbar();

private slots:
    void            buttonGroupAddClicked(int id);
    void            buttonGroupEditClicked(int id);
    void            buttonGroupGridClicked(int id);
    void            buttonGroupLayeringClicked(int id);
    void            buttonGroupModeClicked(int id);
    void            buttonGroupPlayClicked(int id);
    void            buttonGroupTransformClicked(int id);

    void            centerViewTimer(QPointF center_point);
    void            editMenuAboutToShow();
    void            editMenuAboutToHide();

signals:
    // Undo Stack Signals
    void        newStageSelected(DrProject *project, DrScene *scene, long old_stage, long new_stage);

};



//####################################################################################
//##    DrQSplitterColor
//##        A demo custom class to show how style sheeting works on a specific class
//############################
class DrQSplitterColor : public QSplitter
{
    Q_OBJECT

public:
    DrQSplitterColor(QWidget *parent = nullptr) : QSplitter(parent) {}

};





#endif // MAINWINDOW_H















