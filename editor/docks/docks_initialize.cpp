//
//      Created by Stephens Nunnally on 11/25/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QApplication>

#include "editor/docks/docks.h"
#include "editor/interface_editor_relay.h"
#include "editor/trees/tree_advisor.h"
#include "editor/trees/tree_assets.h"
#include "editor/trees/tree_inspector.h"

// Local Enums
enum class Advisor_Side {
    Start_On_Left,
    Start_On_Right,
};

// Local Constants
const   Advisor_Side    c_advisor_start_side    { Advisor_Side::Start_On_Right };


namespace Dr {

//####################################################################################
//##    Lays out docks for the first time
//####################################################################################
void InitializeDockWidgets(QMainWindow *window) {
    IEditorRelay *editor = dynamic_cast<IEditorRelay*>(window);
    if (editor == nullptr) return;

    QDockWidget *dock_advisor =     editor->getDock(Editor_Widgets::Advisor);
    QDockWidget *dock_assets =      editor->getDock(Editor_Widgets::Asset_Tree);
    QDockWidget *dock_inspector =   editor->getDock(Editor_Widgets::Inspector_Tree);
    QDockWidget *dock_wave_form =   editor->getDock(Editor_Widgets::Wave_Form);

    // Starting width, if screen size is small, decrease starting width of assets to make more room (to single column)
    int asset_start_width =    (window->geometry().width() < 1300 ? 124 : 221);
    int advisor_start_height = (window->geometry().width() < 1300 ? 280 : 160);

    // Add Left Side Docks (Assets, Wave Form)
    if (dock_assets) {
        dock_assets->setFixedWidth(asset_start_width);
        window->addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea,       dock_assets);
    }
    if (dock_wave_form) {
        dock_wave_form->setFixedWidth(asset_start_width);
        dock_wave_form->setFixedHeight(120);
        window->addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea,       dock_wave_form);
    }

    // Add Right Side Docks (Inspector, maybe Advisor)
    if (dock_inspector) {
        dock_inspector->setFixedWidth( 310 );
        window->addDockWidget(Qt::DockWidgetArea::RightDockWidgetArea,      dock_inspector);
    }
    if (dock_advisor) {
        if (c_advisor_start_side == Advisor_Side::Start_On_Left) {
            window->addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea,   dock_advisor);
        } else {
            window->addDockWidget(Qt::DockWidgetArea::RightDockWidgetArea,   dock_advisor);
        }
    }

    if (dock_advisor)   window->resizeDocks( { dock_advisor },      { advisor_start_height }, Qt::Vertical);
    if (dock_assets)    window->resizeDocks( { dock_assets },       { 900 }, Qt::Vertical);
    if (dock_inspector) window->resizeDocks( { dock_inspector },    { 900 }, Qt::Vertical);
///    if (dock_wave_form) window->resizeDocks( { dock_assets },    { 900 }, Qt::Vertical);

//    if (c_advisor_start_side == Advisor_Side::Start_On_Left) {
//        window->addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea,   dock_advisor);
//        if (window->geometry().width() < 1300) {
//            window->resizeDocks( { dock_advisor, dock_assets }, { advisor_start_height, 900 }, Qt::Vertical);
//            window->resizeDocks( { dock_inspector }, { 900 }, Qt::Vertical);
//        } else {
//            window->resizeDocks( { dock_advisor, dock_assets }, { advisor_start_height, 900 }, Qt::Vertical);
//            window->resizeDocks( { dock_inspector }, { 900 }, Qt::Vertical);
//        }
//    } else {
//        window->addDockWidget(Qt::DockWidgetArea::RightDockWidgetArea,  dock_advisor);
//        window->resizeDocks( { dock_assets }, { 900 }, Qt::Vertical);
//        window->resizeDocks( { dock_advisor, dock_inspector }, { 160, 900 }, Qt::Vertical);
//    }


    // ***** Hide Docks that aren't used in every mode
    if (dock_assets) dock_assets->hide();
    if (dock_wave_form) dock_wave_form->hide();


    // ***** Helpful QDockWidget commands
    ///void                  addDockWidget(Qt::DockWidgetArea area, QDockWidget *dockwidget);
    ///void                  addDockWidget(Qt::DockWidgetArea area, QDockWidget *dockwidget, Qt::Orientation orientation);
    ///void                  splitDockWidget(QDockWidget *after, QDockWidget *dockwidget, Qt::Orientation orientation);
    ///void                  tabifyDockWidget(QDockWidget *first, QDockWidget *second);
    ///QList<QDockWidget*>   tabifiedDockWidgets(QDockWidget *dockwidget) const;
    ///void                  removeDockWidget(QDockWidget *dockwidget);
    ///bool                  restoreDockWidget(QDockWidget *dockwidget);
}



}   // namespace Dr









