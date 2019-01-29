//
//      Created by Stephens Nunnally on 12/7/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      FormMain - Class that holds our main form window
//
//      FormMain Modes:
//          Edit Scene
//          Edit UI
//          Node Map: World / UI Layout
//          Scene Map: Scene Layout
//
//      Main Components of FormMain while in normal "Edit Scene" mode:
//          Top Area (Toolbar)
//          Advisor (Dock)
//          Object Inspector (Dock)
//
//          Components That Can Appear in Object Inspector:
//              Asset List (Dock)
//              Scene View
//              Scene List
//              Variable List
//              Bottom Area (Labels, Scenes?)
//

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>

#include "colors.h"
#include "debug.h"
#include "project.h"
#include "interface_relay.h"

// Necessary forward declarations
class ColorSplitter;
class InterfaceRelay;
class TreeAssetList;
class TreeAdvisor;
class TreeInspector;
class TreeScene;
class SceneGraphicsScene;
class SceneGraphicsView;
class SceneViewRubberBand;



//####################################################################################
//##    FormMain - Main editor window
//############################
class FormMain : public QMainWindow, public InterfaceRelay
{
    Q_OBJECT

public:
    // Locals
    Form_Main_Mode  current_mode;                                       // Holds current editing mode of FormMain
    Form_Main_Focus current_focus;                                      // Holds Widget that currently has focus
    bool            done_loading = false;                               // True after initial startup of FormMain,
                                                                        // makes sure done loading before any calls to SetLabelText

    // Locals that need to be SAVED / LOADED from each project
    DrProject      *project;                                            // Holds whatever the current open game project is
    long            current_world;                                      // Tracks which world to show in the scene viewer

private:
    SceneGraphicsScene  *scene;                 // Holds the currently selected scene, ready for rendering in SceneGraphicsView
    SceneGraphicsView   *viewMain;              // Renders scene for the viewer

    TreeScene           *treeScene;             // Custom classes for Scene List
    TreeInspector       *treeInspector;         // Custom classes for Object Inspector
    TreeAssetList       *treeAsset;             // Custom classes for Asset List
    TreeAdvisor         *treeAdvisor;           // Custom classes for Advisor List

    // Normal Qt Classes for simple objects
    QMenuBar      *menuBar;
    QAction       *actionUndo, *actionRedo;
    QWidget       *widgetAdvisor, *widgetAssests, *widgetCentral, *widgetScene, *widgetInspector, *widgetToolbar, *widgetSceneView;
    QScrollArea   *areaBottom;
    QFrame        *statusBar;

    QHBoxLayout   *horizontalLayout;
    QVBoxLayout   *verticalLayout, *verticalLayoutObject, *verticalLayoutAdvisor, *verticalLayoutAsset, *verticalLayoutView;
    ColorSplitter *splitterHorizontal, *splitterVertical;

    QDockWidget   *advisor, *assets, *inspector, *toolbar;
    QPushButton   *buttonAtlas, *buttonFonts, *buttonPlay, *buttonSettings, *buttonWorlds;

    // Labels to display info
    QLabel        *label_1,         *label_2,           *label_3,           *label_mouse_1,     *label_mouse_2;
    QLabel        *label_object_1,  *label_object_2,    *label_object_3,    *label_object_4,    *label_object_5;
    QLabel        *label_position,  *label_center,      *label_scale,       *label_rotate,       *label_z_order,    *label_pos_flag;
    QLabel        *label_bottom;

public:
    // Constructor and Destructor
    explicit FormMain(QWidget *parent = nullptr);
    ~FormMain();

    // Member functions
    virtual void    buildObjectInspector(QList<long> key_list);
    virtual void    buildTreeSceneList();
    virtual void    populateScene(long new_key);
    virtual void    setAdvisorInfo(HeaderBodyList header_body_list);
    virtual void    setAdvisorInfo(QString header, QString body);
    virtual void    setLabelText(Label_Names label_name, QString new_text);


private:
    // Form Building / Setup
    void        buildMenu();
    void        buildWindow(Form_Main_Mode new_layout);
    void        buildWindowModeEditScene();
    void        changePalette(Color_Scheme new_color_scheme);

    // Menu Bar Functions
    void        menuAbout();
    void        menuUndo();
    void        menuRedo();
    void        menuListChildren();

private slots:
    void        editMenuAboutToShow();
    void        editMenuAboutToHide();

signals:
    void        sendAdvisorInfo(QString header, QString body);              // Forwards info to MainWindow::changeAdvisor
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















