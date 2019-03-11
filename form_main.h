//
//      Created by Stephens Nunnally on 12/7/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      FormMain - Class that holds our main form window
//
//      FormMain Modes:
//          World Map: World / UI Layout
//          World Editor
//          UI Editor
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
class ColorSplitter;
class TreeAssets;
class TreeAdvisor;
class TreeInspector;
class TreeProject;
class DrProject;
class DrScene;
class DrToolBar;
class DrView;
class DrViewRubberBand;


//####################################################################################
//##    FormMain - Main editor window
//############################
class FormMain : public QMainWindow, public IEditorRelay
{
    Q_OBJECT

public:
    DrProject      *project;                                            // Holds whatever the current open game project is


private:   
    // Menu Widgets
    QMenuBar       *menuBar;
    QAction        *actionUndo, *actionRedo;

    // World Editor Widgets
    TreeAdvisor    *treeAdvisor;           // Custom classes for Advisor Window
    TreeAssets     *treeAsset;             // Custom classes for Asset Tree
    TreeInspector  *treeInspector;         // Custom classes for Object Inspector
    TreeProject    *treeProject;           // Custom classes for Project Tree

    DrScene        *scene;                 // Behind the scene data model that holds the currently selected Stage
    DrView         *viewMain;              // Renders the scene, allows for interaction

    QWidget        *widgetAdvisor, *widgetAssests, *widgetCentral, *widgetStage, *widgetInspector, *widgetStageView;
    QScrollArea    *areaBottom;
    QFrame         *statusBar;

    QVBoxLayout    *verticalLayout, *verticalLayoutObject, *verticalLayoutAdvisor, *verticalLayoutAsset, *verticalLayoutView;
    ColorSplitter  *splitterHorizontal, *splitterVertical;

    QDockWidget    *advisor, *assets, *inspector;

    // Toolbar widgets
    QWidget        *widgetToolbar;
    QButtonGroup   *buttonGroupEditor;

    // Labels to display info
    QLabel         *label_1,         *label_2,           *label_3,           *label_mouse_1,     *label_mouse_2;
    QLabel         *label_object_1,  *label_object_2,    *label_object_3,    *label_object_4,    *label_object_5;
    QLabel         *label_position,  *label_center,      *label_scale,       *label_rotate,       *label_z_order,    *label_pos_flag;
    QLabel         *label_bottom;

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

    virtual void        centerViewOnPoint(QPointF center_point) override;
    virtual double      currentViewGridAngle() override;
    virtual QPointF     currentViewGridScale() override;
    virtual View_Mode   currentViewMode() override;
    virtual QPointF     roundPointToGrid(QPointF point_in_scene) override;

    virtual void        setAdvisorInfo(HeaderBodyList header_body_list) override;
    virtual void        setAdvisorInfo(QString header, QString body) override;

private:
    // Form Building / Setup
    void        buildMenu();
    void        buildSceneAfterLoading(long stage_key);
    void        buildWindow(Form_Main_Mode new_layout);
    void        buildWindowModeWorldEditor();
    DrToolBar*  buildWindowModeWorldEditorToolbar();
    void        changePalette(Color_Scheme new_color_scheme);
    void        connectSignalsEditor();
    void        disconnectSignalsEditor();

    QLabel*     createLabel(QWidget *parent, QString object_name, QRect label_rect, QFont &label_font);

    // Menu Bar Functions
    void        menuAbout();
    void        menuUndo();
    void        menuRedo();
    void        menuListChildren();

    // Toolbar Functions
    void            buttonGroupEditorSetChecked(int id);
    QToolButton*    createToolbarButtonCheckable(const QString &name);
    QLabel*         createToolbarSpacer();


private slots:
    void        buttonGroupEditorClicked(int id);

    void        centerViewTimer(QPointF center_point);
    void        editMenuAboutToShow();
    void        editMenuAboutToHide();

signals:
    void        sendAdvisorInfo(QString header, QString body);              // Forwards info to MainWindow::changeAdvisor

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
    QPoint  pressPos;
    bool    isMoving = false;       // Flag makes sure that the drag and drop event originated from within the titlebar

public:
    DrToolBar(QWidget *parent = nullptr) : QToolBar(parent) { }
    virtual ~DrToolBar() override;

protected:
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
};





#endif // MAINWINDOW_H















