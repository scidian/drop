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
//      Main Components of FormMain while in normal "Edit Stage" mode:
//          Top Area (Toolbar)
//          Advisor (Dock)
//          Object Inspector (Dock)
//
//          Components That Have Items that Can Appear in Object Inspector:
//              Asset List
//              Scene View
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

#include "colors.h"
#include "interface_editor_relay.h"

// Necessary forward declarations
class TreeAssets;
class TreeAdvisor;
class TreeInspector;
class TreeProject;
class DrProject;
class DrScene;
class DrToolBar;
class DrView;
class DrViewRubberBand;
class WidgetHoverHandler;


//####################################################################################
//##    FormMain - Main editor window
//############################
class FormMain : public QMainWindow, public IEditorRelay
{
    Q_OBJECT

public:
    DrProject      *project;                                            // Holds whatever the current open game project is

    Form_Main_Mode  current_mode = Form_Main_Mode::Program_Loading;     // Holds what Form_Main_Mode the program is in


private:   
    WidgetHoverHandler   *m_widget_hover;                               // Pointer to a widget hover handler

    // ***** Menu Widgets
    QMenuBar       *menuBar;
    QAction        *actionUndo, *actionRedo;

    // ***** Toolbar Widgets
    QList<QWidget*>      toolbarWidgets { };
    QList<QLayoutItem*>  toolbarSpacers { };

    DrToolBar      *toolbar;
    QWidget        *widgetToolbar;          QHBoxLayout    *widgetToolbarLayout;

    QWidget        *widgetGroupMode;        QButtonGroup   *buttonsGroupMode;
    QWidget        *widgetGroupLayering;    QButtonGroup   *buttonsGroupLayering;
    QWidget        *widgetGroupReset;       QButtonGroup   *buttonsGroupReset;
    QWidget        *widgetGroupSettings;


    // ***** Shared FormMain Widgets
    TreeAdvisor    *treeAdvisor;                            // Custom classes for Advisor Window
    TreeInspector  *treeInspector;                          // Custom classes for Object Inspector
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

    // "Editor" Status Bar Widgets
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
    virtual bool        eventFilter(QObject *obj, QEvent *event) override;                                  // Inherited from QObject
    virtual void        resizeEvent(QResizeEvent *event) override;                                          // Inherited from QWidget


    // Interface Relay Implementations
    virtual void        buildAssetTree() override;
    virtual void        buildObjectInspector(QList<long> key_list) override;
    virtual void        buildProjectTree() override;
    virtual void        buildScene(long from_stage_key) override;

    virtual void        updateEditorWidgetsAfterItemChange(Editor_Widgets changed_from, QList<DrSettings*> changed_items,
                                                           QList<Properties> property_keys) override;
    virtual void        updateItemSelection(Editor_Widgets selected_from) override;

    virtual DrProject*  currentProject() override;
    virtual void        centerViewOnPoint(QPointF center_point) override;
    virtual double      currentViewGridAngle() override;
    virtual QPointF     currentViewGridScale() override;
    virtual View_Mode   currentViewMode() override;
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

    void        lockDockWidth(QDockWidget *dock, int width);
    void        unlockDockWidth(QDockWidget *dock);

    // Menu Bar Functions
    void        menuAbout();
    void        menuUndo();
    void        menuRedo();
    void        menuListChildren();

    // Toolbar Functions
    void            addToolbarGroup(QWidget *group, bool add_spacer = true);
    void            buttonGroupModeSetChecked(int id);
    void            clearToolbar();
    QPushButton*    createPushButton(QString name = "toolbarButton", QString text = "Button");
    QToolButton*    createToolbarButton(const QString &style_sheet_name, int w, int h, bool checkable = false);
    QLabel*         createToolbarSpacer(int height = 24, int space_on_the_right = 1);
    void            setToolbar(Form_Main_Mode new_mode);

private slots:
    void            buttonGroupModeClicked(int id);
    void            buttonGroupLayeringClicked(int id);
    void            buttonGroupResetClicked(int id);

    void            centerViewTimer(QPointF center_point);
    void            editMenuAboutToShow();
    void            editMenuAboutToHide();

signals:
    // Undo Stack Signals
    void        newStageSelected(DrProject *project, DrScene *scene, long old_stage, long new_stage);

};



//####################################################################################
//##    ColorSplitter - Custom class allows for specific class specific style sheeting
//############################
class ColorSplitter : public QSplitter
{
    Q_OBJECT

public:
    ColorSplitter(QWidget *parent = nullptr) : QSplitter(parent) {}

};


//####################################################################################
//##    DrToolBar - Custom class allows for movement by mouseDown
//############################
class DrToolBar : public QToolBar
{
private:
    QPoint  press_pos;
    bool    is_moving = false;                       // Flag makes sure that the drag and drop event originated from within the titlebar

public:
    DrToolBar(QWidget *parent);
    virtual ~DrToolBar() override;

protected:
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;

};





#endif // MAINWINDOW_H















