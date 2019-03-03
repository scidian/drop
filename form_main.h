//
//      Created by Stephens Nunnally on 12/7/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      FormMain - Class that holds our main form window
//
//      FormMain Modes:
//          World Editor
//          UI Editor
//          World Map: World / UI Layout
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
#include <QPushButton>
#include <QScrollArea>
#include <QSplitter>

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
class DrView;
class DrViewRubberBand;

typedef std::map<Options, QVariant> OptionMap;

//####################################################################################
//##    FormMain - Main editor window
//############################
class FormMain : public QMainWindow, public IEditorRelay
{
    Q_OBJECT

public:
    // Locals
    Form_Main_Mode  current_form_main_mode;                             // Holds current editing mode of FormMain

    // Locals that need to be SAVED / LOADED from each project
    DrProject      *project;                                            // Holds whatever the current open game project is
    OptionMap       options;                                            // Map holding project wide options (deinfed in IEditorRelay)


private:   
    TreeAdvisor    *treeAdvisor;           // Custom classes for Advisor Window
    TreeAssets     *treeAsset;             // Custom classes for Asset Tree
    TreeInspector  *treeInspector;         // Custom classes for Object Inspector
    TreeProject    *treeProject;           // Custom classes for Project Tree

    DrScene        *scene;                 // Behind the scene data model that holds the currently selected Stage
    DrView         *viewMain;              // Renders the scene, allows for interaction

    // Normal Qt Classes for simple objects
    QMenuBar       *menuBar;
    QAction        *actionUndo, *actionRedo;
    QWidget        *widgetAdvisor, *widgetAssests, *widgetCentral, *widgetStage, *widgetInspector, *widgetToolbar, *widgetStageView;
    QScrollArea    *areaBottom;
    QFrame         *statusBar;

    QHBoxLayout    *horizontalLayout;
    QVBoxLayout    *verticalLayout, *verticalLayoutObject, *verticalLayoutAdvisor, *verticalLayoutAsset, *verticalLayoutView;
    ColorSplitter  *splitterHorizontal, *splitterVertical;

    QDockWidget    *advisor, *assets, *inspector, *toolbar;
    QPushButton    *buttonAtlas, *buttonFonts, *buttonPlay, *buttonSettings, *buttonWorlds;

    // Labels to display info
    QLabel         *label_1,         *label_2,           *label_3,           *label_mouse_1,     *label_mouse_2;
    QLabel         *label_object_1,  *label_object_2,    *label_object_3,    *label_object_4,    *label_object_5;
    QLabel         *label_position,  *label_center,      *label_scale,       *label_rotate,       *label_z_order,    *label_pos_flag;
    QLabel         *label_bottom;

public:
    // Constructor and Destructor
    explicit FormMain(QWidget *parent = nullptr);
    virtual ~FormMain() override;

    // Event Handlers
    virtual bool        eventFilter(QObject *obj, QEvent *event) override;                                          // Inherited from QObject

    // Interface Relay Implementations
    virtual void        buildAssetTree() override;
    virtual void        buildObjectInspector(QList<long> key_list) override;
    virtual void        buildProjectTree() override;
    virtual void        buildScene(long from_stage_key) override;

    virtual void        updateEditorWidgetsAfterItemChange(Editor_Widgets changed_from, QList<DrSettings*> changed_items,
                                                           QList<Properties> property_keys) override;
    virtual void        updateItemSelection(Editor_Widgets selected_from) override;

    virtual QVariant    getOption(Options option_to_get) override;
    virtual void        setOption(Options option_to_set, QVariant new_value)override;

    virtual void        centerViewOnPoint(QPointF center_point) override;
    virtual View_Mode   currentViewMode() override;

    virtual void        setAdvisorInfo(HeaderBodyList header_body_list) override;
    virtual void        setAdvisorInfo(QString header, QString body) override;
    virtual void        setLabelText(Label_Names label_name, QString new_text) override;

private:
    // Form Building / Setup
    void        buildMenu();
    void        buildWindow(Form_Main_Mode new_layout);
    void        buildWindowModeEditStage();
    void        changePalette(Color_Scheme new_color_scheme);
    void        connectSignals();
    void        disconnectSignals();

    // Menu Bar Functions
    void        menuAbout();
    void        menuUndo();
    void        menuRedo();
    void        menuListChildren();

private slots:
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


#endif // MAINWINDOW_H















