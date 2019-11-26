//
//      Created by Stephens Nunnally on 11/25/2019, (c) 2019 Scidian Software, All Rights Reserved
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


namespace Dr {

    // Building Shared Dock Widgets
    QDockWidget*        buildDockAdvisor(   DrProject *project, IEditorRelay *editor_relay, TreeAdvisor *&tree_advisor);
    QDockWidget*        buildDockAssets(    DrProject *project, IEditorRelay *editor_relay, TreeAssets *&tree_assets);
    QDockWidget*        buildDockInspector( DrProject *project, IEditorRelay *editor_relay, TreeInspector *&tree_inspector) ;

    // Handling Shared Dock Widgets
    void        initializeDockWidgets(QMainWindow *window, QDockWidget *&dock_advisor, QDockWidget *&dock_assets, QDockWidget *&dock_inspector);
    void        lockDockWidth(QDockWidget *&dock, int width);
    void        unlockDockWidth(QMainWindow *window, QDockWidget *&dock);


}   // namespace Dr




#endif // DOCKS_H



