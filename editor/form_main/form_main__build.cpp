//
//      Created by Stephens Nunnally on 12/10/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Calls to set up, initialize, build Form Main
//
//
#include <QApplication>
#include <QDebug>
#include <QDockWidget>
#include <QScreen>
#include <QScrollBar>

#include "core/colors/colors.h"
#include "editor/docks/docks.h"
#include "editor/event_filters/event_filters.h"
#include "editor/form_main/form_main.h"
#include "editor/helper_library.h"
#include "editor/preferences.h"
#include "editor/style/style.h"
#include "editor/trees/tree_advisor.h"
#include "editor/trees/tree_assets.h"
#include "editor/trees/tree_inspector.h"
#include "editor/trees/tree_project.h"
#include "editor/view_editor/editor_scene.h"
#include "editor/view_editor/editor_view.h"
#include "editor/view_node/node_scene.h"
#include "editor/view_node/node_view.h"
#include "engine/debug_flags.h"
#include "project/dr_project.h"
#include "project/entities/dr_stage.h"


//####################################################################################
//##    Sets initial settings of FormMain
//####################################################################################
void FormMain::initializeFormMain() {
    // ***** Main window settings
    this->setObjectName(QStringLiteral("formMain"));
    this->setWindowModality(Qt::NonModal);
    this->setMinimumSize(QSize(1000, 500));
    this->setMouseTracking(true);
    this->setAcceptDrops(true);
    this->setWindowIcon(QIcon(":/assets/icon/icon256.png"));

    this->setTabPosition(Qt::AllDockWidgetAreas, QTabWidget::North);
    this->setDockOptions(AnimatedDocks | AllowNestedDocks);    /// | AllowTabbedDocks | GroupedDragging);
    ///this->setDocumentMode(true);         // Allows for left alignemnt of dock tabs, but not responsive to style sheets

    // ***** Center window on screen, sets initial size as a percentage of screen size
    Dr::CenterFormOnScreen(this, this, 80, 100);

    // ***** Initialize hover handler
    m_filter_hover = new DrFilterHoverHandler(this);
    connect(m_filter_hover, SIGNAL(signalMouseHover(QString, QString)), this, SLOT(setAdvisorInfo(QString, QString)));

    // ***** Build menus, toolbars, widgets, etc
    buildMenu();
    buildToolBar();

    buildCentralWidgetClear();
    buildCentralWidgetEditor();
    buildCentralWidgetWorldGraph();

    // Build Docks
    dockAdvisor =       Dr::BuildDockAdvisor(   m_project, this, treeAdvisor);
    dockAssetsEditor =  Dr::BuildDockAssets(    m_project, this, treeAssetEditor);
    dockInspector =     Dr::BuildDockInspector( m_project, this, treeInspector);
    Dr::InitializeDockWidgets(this, dockAdvisor, dockAssetsEditor, dockInspector);
}


//####################################################################################
//##    Re-configures FormMain to new mode
//####################################################################################
void FormMain::rebuildFormMain(Editor_Mode new_mode) {

    Editor_Mode old_mode = m_current_mode;
    m_current_mode = new_mode;

    Dr::LockDockWidth( dockAdvisor, dockAdvisor->width() );
    Dr::LockDockWidth( dockAssetsEditor, dockAssetsEditor->width() );
    Dr::LockDockWidth( dockInspector, dockInspector->width() );

    // ***** Clear Current Layout ***** (if we aren't loading for the first time) and save central widgets for future use
    if ((old_mode != new_mode) && (old_mode != Editor_Mode::Program_Loading)) {
        clearToolBar();
        switch (old_mode) {
            case Editor_Mode::World_Graph:
                widgetCentralWorldGraph = takeCentralWidget();
                buildInspector( { } );
                dockAssetsEditor->hide();
                break;
            case Editor_Mode::World_Editor:
                widgetCentralEditor = takeCentralWidget();
                buildInspector( { } );
                dockAssetsEditor->hide();
                break;
            case Editor_Mode::Clear:
                widgetCentralClear = takeCentralWidget();
                break;
            default:
                Dr::ShowMessageBox("rebuildFormMain, clearing - Mode not known", QMessageBox::Icon::Warning, this);
        }
    }

    // Wait for possible finish loading
    if (new_mode != Editor_Mode::World_Editor) {
        while (Dr::CheckDoneLoading() == false) QApplication::processEvents();
    }
    Dr::SetDoneLoading(false);

    // ***** Set New Layout *****
    switch (new_mode) {
        case Editor_Mode::World_Graph:
            sceneWorldGraph->clearSceneOverride();
            setWindowTitle( tr("Drop") + " - " + QString::fromStdString(m_project->getOption(Project_Options::Name).toString()) );
            setCentralWidget( widgetCentralWorldGraph );
            dockAssetsEditor->setWindowTitle( QMainWindow::tr(QString("Nodes").toUtf8()) );
            treeAssetEditor->setShowTypes({ DrType::Block });
            buildAssetTree();
            dockAssetsEditor->show();
            buildSceneAfterLoading( c_no_key );
            break;

        case Editor_Mode::World_Editor:
            sceneEditor->clearSceneOverride();
            setWindowTitle( tr("Drop") + " - " + QString::fromStdString(m_project->getOption(Project_Options::Name).toString()) );
            setCentralWidget( widgetCentralEditor );
            dockAssetsEditor->setWindowTitle( QMainWindow::tr(QString("Assets").toUtf8()) );
            treeAssetEditor->setShowTypes({ DrType::Asset, DrType::Device, DrType::Effect, DrType::Item, DrType::Prefab, DrType::Font });
            buildAssetTree();
            dockAssetsEditor->show();
            buildProjectTree();
            buildSceneAfterLoading( m_project->getOption(Project_Options::Current_Stage).toInt() );
            break;

        case Editor_Mode::Clear:
            setWindowTitle( tr("Drop") );
            setCentralWidget( widgetCentralClear );
            break;

        default:    Dr::ShowMessageBox("rebuildFormMain, setting - Mode not known", QMessageBox::Icon::Warning, this);
    }

    // ***** Sets ToolBar Widgets ***** for the new mode selected
    updateToolBar();
    setToolBar(new_mode);

    // ***** Wait until widgets are done being moved around
    QApplication::processEvents();
    Dr::SetDoneLoading(true);

    Dr::UnlockDockWidth( this, dockAdvisor );
    Dr::UnlockDockWidth( this, dockAssetsEditor );
    Dr::UnlockDockWidth( this, dockInspector );
    buttonGroupModeSetChecked(int(new_mode));
}













