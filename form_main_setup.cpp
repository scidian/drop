//
//      Created by Stephens Nunnally on 12/10/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Calls to set up, initialize, build Form Main
//
//
#include <QApplication>
#include <QDockWidget>
#include <QHeaderView>
#include <QOpenGLWidget>

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
//##        Setting up of form main
//####################################################################################
// Re-configures FormMain to new mode
void FormMain::buildWindow(Form_Main_Mode new_layout)
{
    Form_Main_Mode old_mode = static_cast<Form_Main_Mode>(Dr::GetPreference(Preferences::Form_Main_Mode).toInt());
    Dr::SetPreference(Preferences::Form_Main_Mode, static_cast<int>(new_layout));

    switch (new_layout) {
    case Form_Main_Mode::World_Editor:
        setWindowTitle( tr("Drop") + " - " + project->getOption(Project_Options::Name).toString() );
        buildWindowModeWorldEditor();
        buildAssetTree();
        buildProjectTree();
        viewMain->setFocus(Qt::FocusReason::ActiveWindowFocusReason);
        buildSceneAfterLoading( project->getOption(Project_Options::Current_Stage).toInt() );
        centerViewOnPoint( project->findStageFromKey(project->getOption(Project_Options::Current_Stage).toInt())->getViewCenterPoint() );
        break;
    case Form_Main_Mode::Clear:  
        if (old_mode == Form_Main_Mode::World_Editor)
            disconnectSignalsEditor();

        for (auto item : scene->selectedItems())
            item->setSelected(false);

        this->takeCentralWidget()->deleteLater();
        for (auto dock : findChildren<QDockWidget *>()) { dock->deleteLater(); }

        break;
    default:
        Dr::ShowMessageBox("Not set");
    }

    buttonGroupEditorSetChecked(int(new_layout));
}

void FormMain::buildWindowModeWorldEditor()
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
    QSizePolicy sizePolicyPreferredHorizontal(  QSizePolicy::Preferred,         QSizePolicy::Preferred);
    QSizePolicy sizePolicyPreferredVertical(    QSizePolicy::Preferred,         QSizePolicy::Preferred);
    QSizePolicy sizePolicyMinimum(              QSizePolicy::MinimumExpanding,  QSizePolicy::MinimumExpanding);
    QSizePolicy sizePolicyView(                 QSizePolicy::MinimumExpanding,  QSizePolicy::Expanding);

    sizePolicy.setHorizontalStretch(0);                         sizePolicy.setVerticalStretch(0);
    sizePolicyLess.setHorizontalStretch(1);                     sizePolicyLess.setVerticalStretch(1);
    sizePolicyPreferredHorizontal.setHorizontalStretch(1);      sizePolicyPreferredHorizontal.setVerticalStretch(0);
    sizePolicyPreferredVertical.setHorizontalStretch(0);        sizePolicyPreferredVertical.setVerticalStretch(1);
    sizePolicyMinimum.setHorizontalStretch(0);                  sizePolicyMinimum.setVerticalStretch(0);
    sizePolicyView.setHorizontalStretch(1);                     sizePolicyView.setVerticalStretch(0);


    // ***** Build central widgets
    widgetCentral = new QWidget(this);
    widgetCentral->setObjectName(QStringLiteral("widgetCentral"));
    widgetCentral->setSizePolicy(sizePolicyPreferredHorizontal);
        verticalLayout = new QVBoxLayout(widgetCentral);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));

        // This sets the border with for the main vontrol area between middle and docks
        //verticalLayout->setContentsMargins(2, 0, 2, 0);
        verticalLayout->setContentsMargins(0, 0, 0, 0);

        splitterVertical = new ColorSplitter(widgetCentral);
        splitterVertical->setObjectName(QStringLiteral("splitterVertical"));
        splitterVertical->setOrientation(Qt::Vertical);
        splitterVertical->setHandleWidth(4);

            widgetStage = new QWidget(splitterVertical);
            widgetStage->setObjectName(QStringLiteral("widgetStage"));
                QHBoxLayout *horizontalLayout = new QHBoxLayout(widgetStage);
                horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
                horizontalLayout->setSpacing(0);
                horizontalLayout->setContentsMargins(0, 0, 0, 0);

                splitterHorizontal = new ColorSplitter(widgetStage);
                splitterHorizontal->setObjectName(QStringLiteral("splitterHorizontal"));
                splitterHorizontal->setLineWidth(0);
                splitterHorizontal->setOrientation(Qt::Horizontal);
                splitterHorizontal->setHandleWidth(4);

                    // ***** Load our custom TreeProject for the Scene List
                    treeProject = new TreeProject(splitterHorizontal, project, this);
                    treeProject->setStyle(new StageTreeHighlightProxy(treeProject->style(), treeProject));
                        QTreeWidgetItem *header_item_stage = new QTreeWidgetItem();
                        header_item_stage->setIcon(1, QIcon(":/tree_icons/tree_lock_header.png"));
                        treeProject->setHeaderItem(header_item_stage);
                    treeProject->setObjectName(QStringLiteral("treeProject"));
                    treeProject->setColumnCount(2);
                    treeProject->setColumnWidth(0, 150);
                    treeProject->setColumnWidth(1, 16);
                    treeProject->setMinimumSize(QSize(190, 0));
                    treeProject->setMaximumWidth(400);
                    treeProject->setFont(font);
                    treeProject->setProperty("showDropIndicator", QVariant(false));
                    treeProject->setDragEnabled(true);
                    treeProject->setDragDropOverwriteMode(false);
                    treeProject->setDragDropMode(QAbstractItemView::DragDropMode::InternalMove);
                    treeProject->setDefaultDropAction(Qt::DropAction::TargetMoveAction);
                    treeProject->setAlternatingRowColors(false);
                    treeProject->setSelectionMode(QAbstractItemView::SelectionMode::ExtendedSelection);
                    treeProject->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
                    treeProject->setIndentation(15);
                    treeProject->setRootIsDecorated(true);
                    treeProject->setItemsExpandable(true);
                    treeProject->setExpandsOnDoubleClick(false);
                    treeProject->header()->setSectionResizeMode(0, QHeaderView::ResizeMode::Stretch);
                    treeProject->header()->setStretchLastSection(false);
                    treeProject->header()->setVisible(true);
                    treeProject->setFrameShape(QFrame::NoFrame);

                splitterHorizontal->addWidget(treeProject);


                    widgetStageView = new QWidget(splitterHorizontal);
                    widgetStageView->setObjectName(QStringLiteral("widgetStageView"));
                    widgetStageView->setSizePolicy(sizePolicyView);
                    widgetStageView->setMinimumSize(QSize(100, 0));
                    widgetStageView->setFont(font);
                        verticalLayoutView = new QVBoxLayout(widgetStageView);
                        verticalLayoutView->setObjectName(QStringLiteral("verticalLayoutView"));
                        verticalLayoutView->setSpacing(0);
                        verticalLayoutView->setContentsMargins(0, 0, 0, 0);

                        // ***** Load our DrView to display our DrScene collection of items
                        viewMain = new DrView(widgetStageView, project, scene, this);
                        viewMain->setObjectName(QStringLiteral("viewMain"));

                        if (!Dr::CheckDebugFlag(Debug_Flags::Turn_On_Antialiasing))
                            viewMain->setRenderHint(QPainter::Antialiasing, false);
                        else {
                            viewMain->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::HighQualityAntialiasing);
                            QSurfaceFormat format;
                            format.setSamples(4);
                            QSurfaceFormat::setDefaultFormat(format);                   // Set antialiasing samples to 4
                        }

                        if (Dr::CheckDebugFlag(Debug_Flags::Turn_On_OpenGL)) {
                            QOpenGLWidget *gl_widget = new QOpenGLWidget();
                            gl_widget->setUpdateBehavior(QOpenGLWidget::UpdateBehavior::NoPartialUpdate);
                            viewMain->setViewport(gl_widget);
                        }

                        viewMain->setDragMode(QGraphicsView::DragMode::NoDrag);
                        viewMain->setOptimizationFlags(QGraphicsView::OptimizationFlag::DontSavePainterState);
                        viewMain->setTransformationAnchor(QGraphicsView::ViewportAnchor::AnchorUnderMouse);

                        /// This setting means we will decide when to call update(), controls recurssive paint events
                        ///viewMain->setViewportUpdateMode(QGraphicsView::ViewportUpdateMode::NoViewportUpdate);
                        viewMain->setViewportUpdateMode(QGraphicsView::ViewportUpdateMode::SmartViewportUpdate);
                        viewMain->setFrameShape(QFrame::NoFrame);
                        viewMain->zoomInOut( 0 );



                        // ***** View area status bar
                        statusBar = new QFrame(widgetStageView);
                        statusBar->setObjectName("statusBar");
                        statusBar->setFixedHeight(20);


                    verticalLayoutView->addWidget(viewMain);
                    ///verticalLayoutView->addWidget(statusBar);

                splitterHorizontal->addWidget(widgetStageView);
                splitterHorizontal->setSizes(QList<int> { 150, 300 });      // Sets tree_stage (stage assests) startup width to 150
                                                                            // NOTE: You can save and restore the sizes of the widgets from a QByteArray
                                                                            //       using QSplitter.saveState() and QSplitter.restoreState() respectively
            horizontalLayout->addWidget(splitterHorizontal);
        splitterVertical->addWidget(widgetStage);

            areaBottom = new QScrollArea(splitterVertical);
            areaBottom->setObjectName(QStringLiteral("areaBottom"));
            areaBottom->setSizePolicy(sizePolicyMinimum);
            areaBottom->setMinimumSize(QSize(0, 100));
            areaBottom->setFont(font);
            areaBottom->setWidgetResizable(true);
            areaBottom->setFrameShape(QFrame::NoFrame);
                label_1 =       createLabel(areaBottom,   "label_1",            QRect( 10,  5, 220, 21),    font);
                label_2 =       createLabel(areaBottom,   "label_2",            QRect( 10, 20, 220, 21),    font);
                label_3 =       createLabel(areaBottom,   "label_3",            QRect( 10, 35, 220, 21),    font);
                label_mouse_1 = createLabel(areaBottom,   "label_mouse_1",      QRect( 10, 50, 220, 21),    font);
                label_mouse_2 = createLabel(areaBottom,   "label_mouse_2",      QRect( 10, 65, 220, 21),    font);

                label_object_1 = createLabel(areaBottom,  "label_object_1",     QRect(240,  5, 400, 21),    font);
                label_object_2 = createLabel(areaBottom,  "label_object_2",     QRect(240, 20, 400, 21),    font);
                label_object_3 = createLabel(areaBottom,  "label_object_3",     QRect(240, 35, 400, 21),    font);
                label_object_4 = createLabel(areaBottom,  "label_object_4",     QRect(240, 50, 400, 21),    font);
                label_object_5 = createLabel(areaBottom,  "label_object_5",     QRect(240, 65, 400, 21),    font);

                label_position = createLabel(areaBottom,  "label_position",     QRect(560,  5, 400, 21),    font);
                label_center =   createLabel(areaBottom,  "label_center",       QRect(560, 20, 400, 21),    font);
                label_scale =    createLabel(areaBottom,  "label_scale",        QRect(560, 35, 400, 21),    font);
                label_rotate =   createLabel(areaBottom,  "label_rotate",       QRect(560, 50, 400, 21),    font);
                label_z_order =  createLabel(areaBottom,  "label_z_order",      QRect(560, 65, 400, 21),    font);
                label_pos_flag = createLabel(areaBottom,  "label_pos_flag",     QRect(560, 80, 400, 21),    font);

                label_bottom =   createLabel(areaBottom,  "label_bottom",       QRect( 10, 80, 700, 21),    font);
        splitterVertical->addWidget(areaBottom);

        splitterVertical->setStretchFactor(0, 1);           // widgetStage (index 0) should stretch (1)
        splitterVertical->setStretchFactor(1, 0);           // areaBottom  (index 1) should not stretch (0)

    verticalLayout->addWidget(splitterVertical);
    this->setCentralWidget(widgetCentral);



    // ***** Build left Assets Dock
    assets = new QDockWidget(this);
    assets->setWindowTitle( tr("Assets") );
    assets->setObjectName(QStringLiteral("assets"));
    assets->setMinimumSize(QSize(140, 35));
    assets->setMaximumWidth(300);
    assets->setFont(font);
    assets->setFeatures(QDockWidget::DockWidgetMovable);  // | QDockWidget::DockWidgetClosable);
    assets->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
        widgetAssests = new QWidget();
        widgetAssests->setObjectName(QStringLiteral("widgetAssests"));
        widgetAssests->setSizePolicy(sizePolicyPreferredVertical);
            verticalLayoutAsset = new QVBoxLayout(widgetAssests);
            verticalLayoutAsset->setObjectName(QStringLiteral("verticalLayoutAsset"));
            verticalLayoutAsset->setSpacing(0);
            verticalLayoutAsset->setContentsMargins(0, 0, 0, 0);

                // ***** Load our custom TreeObjectInspector for the Stage List
                treeAsset = new TreeAssets(widgetAssests, project, this);
                treeAsset->setObjectName(QStringLiteral("treeAsset"));
                treeAsset->setColumnCount(1);
                treeAsset->setFont(font);
                treeAsset->setProperty("showDropIndicator", QVariant(false));
                treeAsset->setDragEnabled(false);
                treeAsset->setDragDropOverwriteMode(false);
                treeAsset->setDragDropMode(QAbstractItemView::DragDropMode::NoDragDrop);
                treeAsset->setDefaultDropAction(Qt::DropAction::TargetMoveAction);
                treeAsset->setAlternatingRowColors(false);
                treeAsset->setSelectionMode(QAbstractItemView::SelectionMode::NoSelection);
                treeAsset->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectItems);
                treeAsset->setIndentation(0);
                treeAsset->setRootIsDecorated(false);
                treeAsset->setItemsExpandable(true);
                treeAsset->setExpandsOnDoubleClick(false);
                treeAsset->setHeaderHidden(true);
                treeAsset->setFrameShape(QFrame::NoFrame);

            verticalLayoutAsset->addWidget(treeAsset);

        assets->setWidget(widgetAssests);


    // ***** Build right Advisor Dock
    advisor = new QDockWidget(this);
    advisor->setWindowTitle( tr("Advisor") );
    advisor->setObjectName(QStringLiteral("advisor"));
    advisor->setMinimumSize(QSize(100, 80));
    advisor->setSizePolicy(sizePolicyLess);
    advisor->setFont(font);
    advisor->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetClosable);
    advisor->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
        widgetAdvisor = new QWidget();
        widgetAdvisor->setObjectName(QStringLiteral("widgetAdvisor"));
        widgetAdvisor->setSizePolicy(sizePolicyLess);
        widgetAdvisor->setMaximumHeight(180);
            verticalLayoutAdvisor = new QVBoxLayout(widgetAdvisor);
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

        advisor->setWidget(widgetAdvisor);


    // ***** Build right Inspector Dock
    inspector = new QDockWidget(this);
    inspector->setWindowTitle( tr("Inspector") );
    inspector->setObjectName(QStringLiteral("inspector"));
    inspector->setSizePolicy(sizePolicyPreferredVertical);
    inspector->setMinimumSize(QSize(250, 250));
    inspector->setMaximumWidth(450);
    inspector->setFont(font);
    inspector->setFeatures(QDockWidget::DockWidgetMovable);
    inspector->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
        widgetInspector = new QWidget();
        widgetInspector->setObjectName(QStringLiteral("widgetInspector"));
        widgetInspector->setSizePolicy(sizePolicyPreferredVertical);
            verticalLayoutObject = new QVBoxLayout(widgetInspector);
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
        inspector->setWidget(widgetInspector);


    // ***** Add QMainWindow Docks
    addDockWidget(static_cast<Qt::DockWidgetArea>(1), assets);
    addDockWidget(static_cast<Qt::DockWidgetArea>(2), inspector);
    addDockWidget(static_cast<Qt::DockWidgetArea>(2), advisor);

    // Forces resize of docks
    resizeDocks( { assets, inspector  }, { 140, 270 }, Qt::Horizontal);
    resizeDocks( { advisor, inspector }, { 140, 900 }, Qt::Vertical);


    // ***** Signals emitted by FormMain
    connectSignalsEditor();


}


QLabel* FormMain::createLabel(QWidget *parent, QString object_name, QRect label_rect, QFont &label_font)
{
    QLabel *new_label;
    new_label = new QLabel(parent);
    new_label->setObjectName(object_name);
    new_label->setGeometry(label_rect);
    new_label->setFont(label_font);
    return new_label;
}


//####################################################################################
//##        Connect / disconnect signals emitted by FormMain
//####################################################################################
void FormMain::connectSignalsEditor()
{
    connect(this, SIGNAL(sendAdvisorInfo(QString, QString)), treeAdvisor, SLOT(changeAdvisor(QString, QString)) , Qt::QueuedConnection);

    // Connects signal used to populate scene
    connect(this, SIGNAL(newStageSelected(DrProject*, DrScene*, long, long)),
            scene,  SLOT(newStageSelected(DrProject*, DrScene*, long, long)) );
}

// Disconnect signals emitted by FormMain
void FormMain::disconnectSignalsEditor()
{
    disconnect(this, SIGNAL(sendAdvisorInfo(QString, QString)), treeAdvisor, SLOT(changeAdvisor(QString, QString)) );

    // Connects signal used to populate scene
    disconnect(this, SIGNAL(newStageSelected(DrProject*, DrScene*, long, long)),
               scene,  SLOT(newStageSelected(DrProject*, DrScene*, long, long)) );
}











