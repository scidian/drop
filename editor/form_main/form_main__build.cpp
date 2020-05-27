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
    m_dock_advisor =        Dr::BuildDockAdvisor(   m_project, this, m_tree_advisor);
    m_dock_assets =         Dr::BuildDockAssets(    m_project, this, m_tree_assets);
    m_dock_inspector =      Dr::BuildDockInspector( m_project, this, m_tree_inspector);
    Dr::InitializeDockWidgets(this, m_dock_advisor, m_dock_assets, m_dock_inspector);
}


//####################################################################################
//##    Re-configures FormMain to new mode
//####################################################################################
void FormMain::rebuildFormMain(Editor_Mode new_mode) {

    Editor_Mode old_mode = m_current_mode;
    m_current_mode = new_mode;

    Dr::LockDockWidth( m_dock_advisor, m_dock_advisor->width() );
    Dr::LockDockWidth( m_dock_assets, m_dock_assets->width() );
    Dr::LockDockWidth( m_dock_inspector, m_dock_inspector->width() );

    // ***** Clear Current Layout ***** (if we aren't loading for the first time) and save central widgets for future use
    if ((old_mode != new_mode) && (old_mode != Editor_Mode::Program_Loading)) {
        clearToolBar();
        switch (old_mode) {
            case Editor_Mode::World_Graph:
                m_widget_central_world_graph = takeCentralWidget();
                buildInspector( { } );
                m_dock_assets->hide();
                break;
            case Editor_Mode::World_Creator:
                m_widget_central_editor = takeCentralWidget();
                buildInspector( { } );
                m_dock_assets->hide();
                break;
            case Editor_Mode::Clear:
                m_widget_central_clear = takeCentralWidget();
                break;
            default:
                Dr::ShowMessageBox("rebuildFormMain, clearing - Mode not known", QMessageBox::Icon::Warning, this);
        }
    }

    // Wait for possible finish loading
    if (new_mode != Editor_Mode::World_Creator) {
        while (Dr::CheckDoneLoading() == false) QApplication::processEvents();
    }
    Dr::SetDoneLoading(false);

    // ***** Set New Layout *****
    switch (new_mode) {
        case Editor_Mode::World_Graph:
            m_scene_world_graph->clearSceneOverride();
            setWindowTitle( tr("Drop") + " - " + QString::fromStdString(m_project->getOption(Project_Options::Name).toString()) );
            setCentralWidget( m_widget_central_world_graph );
            m_dock_assets->setWindowTitle( QMainWindow::tr(QString("Nodes").toUtf8()) );
            m_tree_assets->setShowTypes({ DrType::Block });
            buildAssetTree();
            m_dock_assets->show();
            buildSceneAfterLoading( c_no_key );
            break;

        case Editor_Mode::World_Creator:
            m_scene_editor->clearSceneOverride();
            setWindowTitle( tr("Drop") + " - " + QString::fromStdString(m_project->getOption(Project_Options::Name).toString()) );
            setCentralWidget( m_widget_central_editor );
            m_dock_assets->setWindowTitle( QMainWindow::tr(QString("Assets").toUtf8()) );
            m_tree_assets->setShowTypes({ DrType::Asset, DrType::Device, DrType::Effect, DrType::Item, DrType::Prefab, DrType::Font });
            buildAssetTree();
            m_dock_assets->show();
            buildProjectTree();
            buildSceneAfterLoading( m_project->getOption(Project_Options::Current_Stage).toInt() );
            break;

        case Editor_Mode::Clear:
            setWindowTitle( tr("Drop") );
            setCentralWidget( m_widget_central_clear );
            break;

        default:    Dr::ShowMessageBox("rebuildFormMain, setting - Mode not known", QMessageBox::Icon::Warning, this);
    }

    // ***** Sets ToolBar Widgets ***** for the new mode selected
    updateToolBar();
    setToolBar(new_mode);

    // ***** Wait until widgets are done being moved around
    QApplication::processEvents();
    Dr::SetDoneLoading(true);

    Dr::UnlockDockWidth( this, m_dock_advisor );
    Dr::UnlockDockWidth( this, m_dock_assets );
    Dr::UnlockDockWidth( this, m_dock_inspector );
    buttonGroupModeSetChecked(int(new_mode));
}













