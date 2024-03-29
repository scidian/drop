//
//      Created by Stephens Nunnally on 11/25/2019, (c) 2019 Scidian Studios, All Rights Reserved
//
//  File:
//      Docks used with our Forms
//
//
#ifndef DOCKS_H
#define DOCKS_H

#include <QDockWidget>
#include <QMainWindow>

// Forward Declarations
class DrProject;
class IEditorRelay;
class TreeAdvisor;
class TreeAssets;
class TreeInspector;
class TreeWaveForm;
enum class DrType;


//####################################################################################
//##    Dock Building / Handling
//############################
namespace Dr {


    // ***** Building Shared Dock Widgets
    QDockWidget*        BuildDockAdvisor(   DrProject *project, IEditorRelay *editor_relay, TreeAdvisor   *&tree_advisor);
    QDockWidget*        BuildDockAssets(    DrProject *project, IEditorRelay *editor_relay, TreeAssets    *&tree_assets);
    QDockWidget*        BuildDockInspector( DrProject *project, IEditorRelay *editor_relay, TreeInspector *&tree_inspector);
    QDockWidget*        BuildDockWaveForm(  DrProject *project, IEditorRelay *editor_relay, TreeWaveForm  *&tree_wave_form);

    // ***** Handling Shared Dock Widgets
    void                InitializeDockWidgets(QMainWindow *window);
    void                LockDockSize(QDockWidget *dock);
    void                UnlockDockSize(QMainWindow *window, QDockWidget *dock);


}   // namespace Dr




#endif // DOCKS_H



