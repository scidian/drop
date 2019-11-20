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

#include "colors/colors.h"
#include "editor/tree_advisor.h"
#include "editor/tree_assets.h"
#include "editor/tree_inspector.h"
#include "editor/tree_project.h"
#include "editor_view/editor_scene.h"
#include "editor_view/editor_view.h"
#include "forms/form_main.h"
#include "project/project.h"
#include "project/project_stage.h"
#include "style/style.h"
#include "widgets/widgets_event_filters.h"

#include "debug.h"
#include "globals.h"
#include "helper.h"
#include "helper_qt.h"


//####################################################################################
//##    Re-configures FormMain to new mode
//####################################################################################
void FormMain::setFormMainMode(Form_Main_Mode new_mode) {

    Form_Main_Mode old_mode = m_current_mode;
    m_current_mode = new_mode;

    lockDockWidth( dockAdvisor, dockAdvisor->width() );
    lockDockWidth( dockInspector, dockInspector->width() );
    lockDockWidth( dockAssetsEditor, dockAssetsEditor->width() );

    // ***** If we aren't loading for the first time, clear previous layout and save central widgets for future use
    if ((old_mode != new_mode) && (old_mode != Form_Main_Mode::Program_Loading)) {
        clearToolbar();
        switch (old_mode) {
            case Form_Main_Mode::World_Editor:
                widgetCentralEditor = takeCentralWidget();
                buildInspector( {} );
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
                setWindowTitle( tr("Drop") + " - " + m_project->getOption(Project_Options::Name).toString() );
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

    unlockDockWidth( dockAdvisor );
    unlockDockWidth( dockInspector );
    unlockDockWidth( dockAssetsEditor );
    buttonGroupModeSetChecked(int(new_mode));
}


//####################################################################################
//##    Sets initial settings of FormMain
//####################################################################################
void FormMain::initializeFormMainSettings() {
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
}


//####################################################################################
//##    Builds shared widgets used for all modes of FormMain
//####################################################################################
void FormMain::buildWidgetsShared() {

    QFont font = Dr::CustomFont();
    QFont font_larger = Dr::CustomFont();
    font_larger.setPointSize(Dr::FontSize() + 2);

    QSizePolicy sizePolicy(                     QSizePolicy::Preferred,         QSizePolicy::Preferred);
    QSizePolicy sizePolicyLess(                 QSizePolicy::Preferred,         QSizePolicy::Preferred);
    QSizePolicy sizePolicyPreferredVertical(    QSizePolicy::Preferred,         QSizePolicy::Preferred);
    sizePolicy.setHorizontalStretch(0);                         sizePolicy.setVerticalStretch(0);
    sizePolicyLess.setHorizontalStretch(1);                     sizePolicyLess.setVerticalStretch(1);
    sizePolicyPreferredVertical.setHorizontalStretch(0);        sizePolicyPreferredVertical.setVerticalStretch(1);


    // ***** Empty central widget
    widgetCentral = new QWidget();
    widgetCentral->setObjectName(QStringLiteral("widgetCentral"));
    widgetCentral->setSizePolicy(sizePolicy);
    QGridLayout *layout = new QGridLayout(widgetCentral);
        QLabel *label = new QLabel("Coming Soon...");
        label->setFont(font_larger);
        layout->addWidget(label, 0, 0, Qt::AlignmentFlag::AlignCenter);


    // ***** Build right Advisor Dock
    dockAdvisor = new QDockWidget(nullptr, Qt::WindowType::Window | Qt::WindowType::FramelessWindowHint);
    dockAdvisor->setWindowTitle( tr("Advisor") );
    dockAdvisor->setObjectName(QStringLiteral("dockAdvisor"));
    dockAdvisor->setSizePolicy(sizePolicyLess);
    dockAdvisor->setFont(font);
    dockAdvisor->setFeatures(QDockWidget::DockWidgetMovable); /// | QDockWidget::DockWidgetFloatable); /// | QDockWidget::DockWidgetClosable);
    dockAdvisor->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
        widgetAdvisor = new QWidget();
        widgetAdvisor->setObjectName(QStringLiteral("widgetAdvisor"));
        widgetAdvisor->setSizePolicy(sizePolicyLess);
        //widgetAdvisor->setMaximumHeight(180);
            QVBoxLayout *verticalLayoutAdvisor = new QVBoxLayout(widgetAdvisor);
            verticalLayoutAdvisor->setObjectName(QStringLiteral("verticalLayoutAdvisor"));
            verticalLayoutAdvisor->setSpacing(0);
            verticalLayoutAdvisor->setContentsMargins(0, 0, 0, 0);

                // ***** Load our custom TreeAdvisor for the helpful advisor text
                treeAdvisor = new TreeAdvisor(widgetAdvisor, m_project, this);
                treeAdvisor->setObjectName(QStringLiteral("treeAdvisor"));
                treeAdvisor->setColumnCount(1);
                treeAdvisor->setFont(font_larger);
                treeAdvisor->setProperty("showDropIndicator", QVariant(false));
                treeAdvisor->setDragEnabled(false);
                treeAdvisor->setDragDropOverwriteMode(false);
                treeAdvisor->setDragDropMode(QAbstractItemView::DragDropMode::NoDragDrop);
                treeAdvisor->setDefaultDropAction(Qt::DropAction::IgnoreAction);
                treeAdvisor->setAlternatingRowColors(false);
                treeAdvisor->setSelectionMode(QAbstractItemView::SelectionMode::NoSelection);
                treeAdvisor->setIndentation(12);
                treeAdvisor->setRootIsDecorated(false);
                treeAdvisor->setItemsExpandable(false);
                treeAdvisor->setExpandsOnDoubleClick(false);
                treeAdvisor->setHeaderHidden(true);
                treeAdvisor->setFrameShape(QFrame::NoFrame);
            verticalLayoutAdvisor->addWidget(treeAdvisor);

        dockAdvisor->setWidget(widgetAdvisor);


    // ***** Build right Inspector Dock
    dockInspector = new QDockWidget();
    dockInspector->setWindowTitle( tr("Inspector") );
    dockInspector->setObjectName(QStringLiteral("dockInspector"));
    dockInspector->setSizePolicy(sizePolicyPreferredVertical);
    dockInspector->setFont(font);
    dockInspector->setFeatures(QDockWidget::DockWidgetMovable); /// | QDockWidget::DockWidgetFloatable);
    dockInspector->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
        widgetInspector = new QWidget();
        widgetInspector->setObjectName(QStringLiteral("widgetInspector"));
        widgetInspector->setSizePolicy(sizePolicyPreferredVertical);
            QVBoxLayout *verticalLayoutObject = new QVBoxLayout(widgetInspector);
            verticalLayoutObject->setObjectName(QStringLiteral("verticalLayoutObject"));
            verticalLayoutObject->setSpacing(0);
            verticalLayoutObject->setContentsMargins(0, 0, 0, 0);

                // ***** Load our custom TreeInspector for the Stage List
                treeInspector = new TreeInspector(widgetInspector, m_project, this);
                treeInspector->setObjectName(QStringLiteral("treeObject"));
                treeInspector->setColumnCount(1);
                treeInspector->setFont(font);
                treeInspector->setProperty("showDropIndicator", QVariant(false));
                treeInspector->setDragEnabled(false);
                treeInspector->setDragDropOverwriteMode(false);
                treeInspector->setDragDropMode(QAbstractItemView::DragDropMode::NoDragDrop);
                treeInspector->setDefaultDropAction(Qt::DropAction::TargetMoveAction);
                treeInspector->setAlternatingRowColors(false);
                treeInspector->setSelectionMode(QAbstractItemView::SelectionMode::NoSelection);
                treeInspector->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectItems);
                ///treeInspector->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
                treeInspector->setIndentation(0);
                treeInspector->setRootIsDecorated(false);
                treeInspector->setItemsExpandable(true);
                treeInspector->setExpandsOnDoubleClick(false);
                treeInspector->setHeaderHidden(true);
                treeInspector->setFrameShape(QFrame::NoFrame);
                treeInspector->setUniformRowHeights(false);
                treeInspector->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
                treeInspector->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

            verticalLayoutObject->addWidget(treeInspector);
        dockInspector->setWidget(widgetInspector);

}


//####################################################################################
//##    Lays out docks for the first time
//####################################################################################
void FormMain::initializeDockWidgets() {

    // ***** Starting width, if screen size is small, decrease starting width of assets to make more room (to single column)
    if (this->geometry().width() < 1500)
        dockAssetsEditor->setFixedWidth( 124 );
    else
        dockAssetsEditor->setFixedWidth( 221 );
    addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, dockAssetsEditor);
    dockAssetsEditor->hide();


    // ***** Add QMainWindow Docks, set starting widths
    dockInspector->setFixedWidth( 310 );
    addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, dockAdvisor);
    addDockWidget(Qt::DockWidgetArea::RightDockWidgetArea, dockInspector);
    resizeDocks( { dockAdvisor, dockAssetsEditor }, { 160, 900 }, Qt::Vertical);
    resizeDocks( { dockInspector }, { 900 }, Qt::Vertical);


    ///// Helpful QDockWidget commands
    ///void                  addDockWidget(Qt::DockWidgetArea area, QDockWidget *dockwidget);
    ///void                  addDockWidget(Qt::DockWidgetArea area, QDockWidget *dockwidget, Qt::Orientation orientation);
    ///void                  splitDockWidget(QDockWidget *after, QDockWidget *dockwidget, Qt::Orientation orientation);
    ///void                  tabifyDockWidget(QDockWidget *first, QDockWidget *second);
    ///QList<QDockWidget*>   tabifiedDockWidgets(QDockWidget *dockwidget) const;
    ///void                  removeDockWidget(QDockWidget *dockwidget);
    ///bool                  restoreDockWidget(QDockWidget *dockwidget);
}


//####################################################################################
//##    Keeps docks from changing width while FormMain is changed around
//####################################################################################
void FormMain::lockDockWidth(QDockWidget *dock, int width) {
    dock->setFixedWidth( width );
}
void FormMain::unlockDockWidth(QDockWidget *dock) {
    int pre_width = dock->width();
    dock->setMaximumSize(622, QWIDGETSIZE_MAX);

    QString dock_name = dock->objectName();
    if (dock_name == "dockInspector")
        dock->setMinimumSize(260, 80);
    else
        dock->setMinimumSize(124, 80);

    QApplication::processEvents();
    resizeDocks( { dock }, { pre_width }, Qt::Horizontal);
}











