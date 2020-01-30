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
#include "editor/event_filters.h"
#include "editor/form_main/form_main.h"
#include "editor/helper_library.h"
#include "editor/preferences.h"
#include "editor/style/style.h"
#include "editor/trees/tree_advisor.h"
#include "editor/trees/tree_assets.h"
#include "editor/trees/tree_inspector.h"
#include "editor/trees/tree_project.h"
#include "editor/view/editor_scene.h"
#include "editor/view/editor_view.h"
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

    buildCentralWidgetMain();
    buildCentralWidgetEditor();

    dockAdvisor =       Dr::BuildDockAdvisor(  m_project, this, treeAdvisor);                                           // Build Advisor Dock
    dockAssetsEditor =  Dr::BuildDockAssets(   m_project, this, treeAssetEditor, "Assets",
                                             { DrType::Asset, DrType::Device, DrType::Effect, DrType::Font });          // Build Assets Dock
    dockInspector =     Dr::BuildDockInspector(m_project, this, treeInspector);                                         // Build Inspector Dock
    Dr::InitializeDockWidgets(this, dockAdvisor, dockAssetsEditor, dockInspector);
}


//####################################################################################
//##    Re-configures FormMain to new mode
//####################################################################################
void FormMain::setFormMainMode(Form_Main_Mode new_mode) {

    Form_Main_Mode old_mode = m_current_mode;
    m_current_mode = new_mode;

    Dr::LockDockWidth( dockAdvisor, dockAdvisor->width() );
    Dr::LockDockWidth( dockAssetsEditor, dockAssetsEditor->width() );
    Dr::LockDockWidth( dockInspector, dockInspector->width() );

    // ***** If we aren't loading for the first time, clear previous layout and save central widgets for future use
    if ((old_mode != new_mode) && (old_mode != Form_Main_Mode::Program_Loading)) {
        clearToolbar();
        switch (old_mode) {
            case Form_Main_Mode::World_Editor:
                widgetCentralEditor = takeCentralWidget();
                buildInspector( { } );
                dockAssetsEditor->hide();
                break;
            case Form_Main_Mode::Clear:
                widgetCentral = takeCentralWidget();
                break;

            default:    Dr::ShowMessageBox("setFormMainMode, clearing - Mode not known", QMessageBox::Icon::Warning, "Warning!", this);
        }
    }
    setToolbar(new_mode);           // Sets toolbar widgets for the new mode selected

    // ***** Set up new layout
    switch (new_mode) {
        case Form_Main_Mode::World_Editor:
            Dr::SetDoneLoading(false);
                setWindowTitle( tr("Drop") + " - " + QString::fromStdString(m_project->getOption(Project_Options::Name).toString()) );
                this->setCentralWidget( widgetCentralEditor );
                buildAssetTree();
                dockAssetsEditor->show();
                buildProjectTree();
                sceneEditor->clearStageShown();
                buildSceneAfterLoading( m_project->getOption(Project_Options::Current_Stage).toInt() );
            break;

        case Form_Main_Mode::Clear:
            while (Dr::CheckDoneLoading() == false) QApplication::processEvents();
            Dr::SetDoneLoading(false);
                setWindowTitle( tr("Drop") );
                this->setCentralWidget( widgetCentral );
            break;

        default:    Dr::ShowMessageBox("setFormMainMode, setting - Mode not known", QMessageBox::Icon::Warning, "Warning!", this);
    }

    QApplication::processEvents();
    Dr::SetDoneLoading(true);

    Dr::UnlockDockWidth( this, dockAdvisor );
    Dr::UnlockDockWidth( this, dockAssetsEditor );
    Dr::UnlockDockWidth( this, dockInspector );
    buttonGroupModeSetChecked(int(new_mode));
}













