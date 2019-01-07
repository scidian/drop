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

#include "01_project.h"
#include "globals.h"
#include "interface_relay.h"


// Necessary forward declarations
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
    Globals        *globals;                                            // Holds project globals
    Form_Main_Mode  current_mode;                                       // Holds current editing mode of FormMain
    Form_Main_Focus current_focus;                                      // Holds Widget that currently has focus

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
    QWidget       *widgetAdvisor, *widgetAssests, *widgetCentral, *widgetScene, *widgetInspector, *widgetToolbar;
    QScrollArea   *areaBottom;

    QHBoxLayout   *horizontalLayout;
    QVBoxLayout   *verticalLayout, *verticalLayoutObject, *verticalLayoutAdvisor, *verticalLayoutAsset;
    QSplitter     *splitterHorizontal, *splitterVertical;

    QDockWidget   *advisor, *assets, *inspector, *toolbar;
    QPushButton   *buttonAtlas, *buttonFonts, *buttonPlay, *buttonSettings, *buttonWorlds;

    // Labels to display info
    QLabel        *label_1,         *label_2,           *label_3;
    QLabel        *label_object_1,  *label_object_2,    *label_object_3;

public:
    // Constructor and Destructor
    explicit FormMain(QWidget *parent = nullptr, Globals *the_globals = nullptr);
    ~FormMain();

    // Member functions
    virtual void    buildObjectInspector(QList<long> key_list);
    virtual void    buildTreeSceneList();
    virtual QColor  getColor(Window_Colors color_role);
    virtual void    setAdvisorInfo(HeaderBodyList header_body_list);
    virtual void    setLabelText(Label_Names label_name, QString new_text);
    virtual void    showMessageBox(QString message);

    void            populateScene();                                        // TEMP generic fill of scene

private:
    // Form setup
    void        applyColoring();
    void        applyDropShadow(QWidget *target_widget, qreal blur_radius, qreal offset_x, qreal offset_y, QColor shadow_color);
    void        applyDropShadowByType(QWidget *target_widget, Shadow_Types shadow_type);
    void        buildMenu();
    void        buildWindow(Form_Main_Mode new_layout);
    void        buildWindowModeEditScene();
    void        changePalette(Color_Scheme new_color_scheme);
    void        listChildren();
    void        refreshMainView();

signals:
    void        sendAdvisorInfo(HeaderBodyList header_body_list);           // Forwards info to MainWindow::changeAdvisor
};



#endif // MAINWINDOW_H















