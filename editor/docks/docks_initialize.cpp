//
//      Created by Stephens Nunnally on 11/25/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "editor/docks/docks.h"
#include "editor/trees/tree_advisor.h"
#include "editor/trees/tree_assets.h"
#include "editor/trees/tree_inspector.h"


namespace Dr {

//####################################################################################
//##    Lays out docks for the first time
//####################################################################################
void InitializeDockWidgets(QMainWindow *window, QDockWidget *&dock_advisor, QDockWidget *&dock_assets, QDockWidget *&dock_inspector) {
    // Starting width, if screen size is small, decrease starting width of assets to make more room (to single column)
    if (window->geometry().width() < 1300) {
        dock_assets->setFixedWidth( 124 );
    } else {
        dock_assets->setFixedWidth( 221 );
    }
    window->addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, dock_assets);
    dock_assets->hide();

    // Add QMainWindow Docks, set starting widths
    dock_inspector->setFixedWidth( 310 );
    window->addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, dock_advisor);
    window->addDockWidget(Qt::DockWidgetArea::RightDockWidgetArea, dock_inspector);

    if (window->geometry().width() < 1300) {
        window->resizeDocks( { dock_advisor, dock_assets }, { 280, 900 }, Qt::Vertical);
        window->resizeDocks( { dock_inspector }, { 900 }, Qt::Vertical);
    } else {
        window->resizeDocks( { dock_advisor, dock_assets }, { 160, 900 }, Qt::Vertical);
        window->resizeDocks( { dock_inspector }, { 900 }, Qt::Vertical);
    }

    ///// Helpful QDockWidget commands
    ///void                  addDockWidget(Qt::DockWidgetArea area, QDockWidget *dockwidget);
    ///void                  addDockWidget(Qt::DockWidgetArea area, QDockWidget *dockwidget, Qt::Orientation orientation);
    ///void                  splitDockWidget(QDockWidget *after, QDockWidget *dockwidget, Qt::Orientation orientation);
    ///void                  tabifyDockWidget(QDockWidget *first, QDockWidget *second);
    ///QList<QDockWidget*>   tabifiedDockWidgets(QDockWidget *dockwidget) const;
    ///void                  removeDockWidget(QDockWidget *dockwidget);
    ///bool                  restoreDockWidget(QDockWidget *dockwidget);
}



}   // namespace Dr









