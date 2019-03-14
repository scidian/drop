//
//      Created by Stephens Nunnally on 12/10/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Calls to set up, initialize, build Form Main
//
//
#include <QApplication>
#include <QDockWidget>

#include "colors.h"
#include "debug.h"
#include "editor_scene.h"
#include "editor_tree_advisor.h"
#include "editor_tree_assets.h"
#include "editor_tree_inspector.h"
#include "editor_tree_project.h"
#include "editor_view.h"
#include "form_main.h"
#include "globals.h"
#include "library.h"
#include "project.h"
#include "project_world_stage.h"


//####################################################################################
//##        Re-configures FormMain to new mode
//####################################################################################
void FormMain::setFormMainMode(Form_Main_Mode new_layout)
{
    Form_Main_Mode old_layout = static_cast<Form_Main_Mode>(Dr::GetPreference(Preferences::Form_Main_Mode).toInt());
    Dr::SetPreference(Preferences::Form_Main_Mode, static_cast<int>(new_layout));

    lockDockWidth( dockAdvisor, dockAdvisor->width() );
    lockDockWidth( dockInspector, dockInspector->width() );
    lockDockWidth( dockAssetsEditor, dockAssetsEditor->width() );

    // ***** If we aren't loading for the first time, clear previous layout and save central widgets for future use
    if (old_layout != new_layout) {
        switch (old_layout) {
        case Form_Main_Mode::World_Editor:
            widgetCentralEditor = takeCentralWidget();
            buildObjectInspector( {} );
            dockAssetsEditor->hide();
            break;
        case Form_Main_Mode::Clear:
            widgetCentral = takeCentralWidget();
            break;

        default:    Dr::ShowMessageBox("Not set");
        }
    }

    // ***** Set up new layout
    switch (new_layout) {
    case Form_Main_Mode::World_Editor:
        Dr::SetDoneLoading(false);
            setWindowTitle( tr("Drop") + " - " + project->getOption(Project_Options::Name).toString() );
            this->setCentralWidget( widgetCentralEditor );
            buildAssetTree();
            dockAssetsEditor->show();
            buildProjectTree();
            sceneEditor->setCurrentStageKeyShown(-1);
            buildSceneAfterLoading( project->getOption(Project_Options::Current_Stage).toInt() );    
        break;

    case Form_Main_Mode::Clear:  
        while (Dr::CheckDoneLoading() == false) QApplication::processEvents();
        Dr::SetDoneLoading(false);
            setWindowTitle( tr("Drop") );
            this->setCentralWidget( widgetCentral );
        break;

    default:    Dr::ShowMessageBox("Not set");
    }

    QApplication::processEvents();
    Dr::SetDoneLoading(true);

    unlockDockWidth( dockAdvisor );
    unlockDockWidth( dockInspector );
    unlockDockWidth( dockAssetsEditor );
    buttonGroupEditorSetChecked(int(new_layout));
}


//####################################################################################
//##        Builds shared widgets used for all modes of FormMain
//####################################################################################
void FormMain::buildWidgetsShared()
{
    QFont font, fontLarger;
    font.setPointSize(Dr::FontSize());
    fontLarger.setPointSize(Dr::FontSize() + 2);

    /// Other size policies to play with
    ///QSizePolicy sizePolicyNoChange(QSizePolicy::Policy::MinimumExpanding, QSizePolicy::Policy::MinimumExpanding);
    ///QSizePolicy sizePolicyNoChange(QSizePolicy::Policy::Ignored, QSizePolicy::Policy::Ignored);
    ///QSizePolicy sizePolicyNoChange(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);

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


    // ***** Build right Advisor Dock
    dockAdvisor = new QDockWidget();
    dockAdvisor->setWindowTitle( tr("Advisor") );
    dockAdvisor->setObjectName(QStringLiteral("dockAdvisor"));
    dockAdvisor->setSizePolicy(sizePolicyLess);
    dockAdvisor->setFont(font);
    dockAdvisor->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetClosable);
    dockAdvisor->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
        widgetAdvisor = new QWidget();
        widgetAdvisor->setObjectName(QStringLiteral("widgetAdvisor"));
        widgetAdvisor->setSizePolicy(sizePolicyLess);
        widgetAdvisor->setMaximumHeight(180);
            QVBoxLayout *verticalLayoutAdvisor = new QVBoxLayout(widgetAdvisor);
            verticalLayoutAdvisor->setObjectName(QStringLiteral("verticalLayoutAdvisor"));
            verticalLayoutAdvisor->setSpacing(0);
            verticalLayoutAdvisor->setContentsMargins(0, 0, 0, 0);

                // ***** Load our custom TreeAdvisor for the helpful advisor text
                treeAdvisor = new TreeAdvisor(widgetAdvisor, project, this);
                treeAdvisor->setObjectName(QStringLiteral("treeAdvisor"));
                treeAdvisor->setColumnCount(1);
                treeAdvisor->setFont(fontLarger);
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
    dockInspector->setFeatures(QDockWidget::DockWidgetMovable);
    dockInspector->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
        widgetInspector = new QWidget();
        widgetInspector->setObjectName(QStringLiteral("widgetInspector"));
        widgetInspector->setSizePolicy(sizePolicyPreferredVertical);
            QVBoxLayout *verticalLayoutObject = new QVBoxLayout(widgetInspector);
            verticalLayoutObject->setObjectName(QStringLiteral("verticalLayoutObject"));
            verticalLayoutObject->setSpacing(0);
            verticalLayoutObject->setContentsMargins(0, 0, 0, 0);

                // ***** Load our custom TreeObjectInspector for the Stage List
                treeInspector = new TreeInspector(widgetInspector, project, this);
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
                treeInspector->setIndentation(0);
                treeInspector->setRootIsDecorated(false);
                treeInspector->setItemsExpandable(true);
                treeInspector->setExpandsOnDoubleClick(false);
                treeInspector->setHeaderHidden(true);
                treeInspector->setFrameShape(QFrame::NoFrame);

            verticalLayoutObject->addWidget(treeInspector);
        dockInspector->setWidget(widgetInspector);


    // ***** Add QMainWindow Docks
    addDockWidget(static_cast<Qt::DockWidgetArea>(2), dockInspector);
    addDockWidget(static_cast<Qt::DockWidgetArea>(2), dockAdvisor);
    resizeDocks( { dockInspector  }, { 270 }, Qt::Horizontal);
    resizeDocks( { dockAdvisor, dockInspector }, { 140, 900 }, Qt::Vertical);

}


//####################################################################################
//##        Keeps docks from changing width while FormMain is changed around
//####################################################################################
void FormMain::lockDockWidth(QDockWidget *dock, int width) { dock->setFixedWidth( width ); }
void FormMain::unlockDockWidth(QDockWidget *dock) {
    dock->setMaximumSize(500, QWIDGETSIZE_MAX);
    dock->setMinimumSize(140, 80);
}











