//
//      Created by Stephens Nunnally on 11/25/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QHeaderView>

#include "editor/docks/docks.h"
#include "editor/forms/form_animation.h"
#include "editor/trees/tree_inspector.h"
#include "editor/trees/tree_project.h"
#include "editor/view/editor_scene.h"
#include "editor/view/editor_view.h"
#include "style/style.h"
#include "widgets/widgets_event_filters.h"

#include "debug.h"
#include "helper_qt.h"


//####################################################################################
//##    Sets initial settings of FormAnimation
//####################################################################################
void FormAnimation::initializeFormAnimation() {
    // ********* Initialize form and customize colors and styles
    this->setStyleSheet( Dr::CustomStyleSheetFormatting() );

    // ***** Main window settings
    this->setObjectName(QStringLiteral("formAnimation"));
    this->setWindowModality(Qt::NonModal);
    this->setMinimumSize(QSize(900, 500));
    this->setMouseTracking(true);
    this->setAcceptDrops(true);
    this->setWindowIcon(QIcon(":/assets/icon/icon256.png"));

    this->setWindowTitle( tr("Animation Editor") );

    this->setTabPosition(Qt::AllDockWidgetAreas, QTabWidget::North);
    this->setDockOptions(AnimatedDocks | AllowNestedDocks);    /// | AllowTabbedDocks | GroupedDragging);

    // ***** Center window on screen, sets initial size as a percentage of screen size
    Dr::CenterFormOnScreen(this, this, 70, 80);

    // ***** Initialize hover handler
    m_filter_hover = new DrFilterHoverHandler(this);
    connect(m_filter_hover, SIGNAL(signalMouseHover(QString, QString)), this, SLOT(setAdvisorInfo(QString, QString)));

    // ***** Build docks, widgets, etc
    buildCentralWidget();

    dockAdvisor =       Dr::buildDockAdvisor(  m_project, this, treeAdvisor);                                       // Build Advisor Dock
    dockAssetsEditor =  Dr::buildDockAssets(   m_project, this, treeAssetEditor, "Images", { DrType::Image });      // Build Assets Dock
    dockInspector =     Dr::buildDockInspector(m_project, this, treeInspector);                                     // Build Inspector Dock
    Dr::initializeDockWidgets(this, dockAdvisor, dockAssetsEditor, dockInspector);

    // ***** Set up FormAnimation for first time
    Dr::lockDockWidth( dockAdvisor, dockAdvisor->width() );
    Dr::lockDockWidth( dockAssetsEditor, dockAssetsEditor->width() );
    Dr::lockDockWidth( dockInspector, dockInspector->width() );

    this->setCentralWidget( widgetCentral );
    buildAssetTree();
    dockAssetsEditor->show();
    buildProjectTree();
    sceneEditor->clearStageShown();

    //buildSceneAfterLoading( m_project->getOption(Project_Options::Current_Stage).toInt() );

    Dr::unlockDockWidth( this, dockAdvisor );
    Dr::unlockDockWidth( this, dockAssetsEditor );
    Dr::unlockDockWidth( this, dockInspector );
}


//####################################################################################
//##    Builds Widgets used for Animation Editor
//####################################################################################
void FormAnimation::buildCentralWidget() {
    QFont font = Dr::CustomFont();

    QSizePolicy sizePolicy(                     QSizePolicy::Preferred,         QSizePolicy::Preferred);
    QSizePolicy sizePolicyPreferredHorizontal(  QSizePolicy::Preferred,         QSizePolicy::Preferred);
    QSizePolicy sizePolicyPreferredVertical(    QSizePolicy::Preferred,         QSizePolicy::Preferred);
    QSizePolicy sizePolicyMinimum(              QSizePolicy::MinimumExpanding,  QSizePolicy::MinimumExpanding);
    QSizePolicy sizePolicyView(                 QSizePolicy::MinimumExpanding,  QSizePolicy::Expanding);

    sizePolicy.setHorizontalStretch(0);                         sizePolicy.setVerticalStretch(0);
    sizePolicyPreferredHorizontal.setHorizontalStretch(1);      sizePolicyPreferredHorizontal.setVerticalStretch(0);
    sizePolicyPreferredVertical.setHorizontalStretch(0);        sizePolicyPreferredVertical.setVerticalStretch(1);
    sizePolicyMinimum.setHorizontalStretch(0);                  sizePolicyMinimum.setVerticalStretch(0);
    sizePolicyView.setHorizontalStretch(1);                     sizePolicyView.setVerticalStretch(0);

    // ***** Initialize scene used for showing stages World Editor Mode viewEditor widget
    sceneEditor = new DrScene(this, m_project, this);


    // Connects signal used to populate scene
//    connect(this,       SIGNAL(newStageSelected(DrProject*, DrScene*, long, long)),
//            sceneEditor,  SLOT(newStageSelected(DrProject*, DrScene*, long, long)) );


    // ***** Build central widgets
    widgetCentral = new QWidget();
    widgetCentral->setObjectName(QStringLiteral("widgetCentral"));
    widgetCentral->setSizePolicy(sizePolicyPreferredHorizontal);
        QVBoxLayout *verticalLayoutCentral = new QVBoxLayout(widgetCentral);
        verticalLayoutCentral->setSpacing(0);
        verticalLayoutCentral->setObjectName(QStringLiteral("verticalLayout"));
        // This sets the border with for the main view area between middle and docks
        verticalLayoutCentral->setContentsMargins(0, 0, 0, 0);

        QSplitter *splitterVertical = new QSplitter(widgetCentral);
        splitterVertical->setObjectName(QStringLiteral("splitterVertical"));
        splitterVertical->setOrientation(Qt::Vertical);
        splitterVertical->setHandleWidth(4);

            QWidget *widgetStage = new QWidget(splitterVertical);
            widgetStage->setObjectName(QStringLiteral("widgetStage"));
                QHBoxLayout *horizontalLayout = new QHBoxLayout(widgetStage);
                horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
                horizontalLayout->setSpacing(0);
                horizontalLayout->setContentsMargins(0, 0, 0, 0);

                QSplitter *splitterHorizontal = new QSplitter(widgetStage);
                splitterHorizontal->setObjectName(QStringLiteral("splitterHorizontal"));
                splitterHorizontal->setLineWidth(0);
                splitterHorizontal->setOrientation(Qt::Horizontal);
                splitterHorizontal->setHandleWidth(4);

                    // ***** Load our custom TreeProject for the Scene List
                    treeProjectEditor = new TreeProject(splitterHorizontal, m_project, this);
                    treeProjectEditor->setStyle(new TreeStageHighlightProxy(treeProjectEditor->style(), treeProjectEditor));
                        QTreeWidgetItem *header_item_stage = new QTreeWidgetItem();
                        header_item_stage->setIcon(1, QIcon(":/assets/tree_icons/tree_lock_header.png"));
                        treeProjectEditor->setHeaderItem(header_item_stage);
                    treeProjectEditor->setObjectName(QStringLiteral("treeProjectEditor"));
                    treeProjectEditor->setColumnCount(3);
                    treeProjectEditor->setColumnWidth(COLUMN_TITLE,     150);
                    treeProjectEditor->setColumnWidth(COLUMN_LOCK,       16);
                    treeProjectEditor->setColumnWidth(COLUMN_Z_ORDER,    30);
                    treeProjectEditor->setColumnHidden(COLUMN_Z_ORDER, true);
                    treeProjectEditor->setMinimumSize(QSize(190, 0));
                    treeProjectEditor->setMaximumWidth(400);
                    treeProjectEditor->setFont(font);
                    treeProjectEditor->setProperty("showDropIndicator", QVariant(false));
                    treeProjectEditor->setDragEnabled(true);
                    treeProjectEditor->setDragDropOverwriteMode(false);
                    treeProjectEditor->setDragDropMode(QAbstractItemView::DragDropMode::InternalMove);
                    treeProjectEditor->setDefaultDropAction(Qt::DropAction::TargetMoveAction);
                    treeProjectEditor->setAlternatingRowColors(false);
                    treeProjectEditor->setSelectionMode(QAbstractItemView::SelectionMode::ExtendedSelection);
                    treeProjectEditor->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
                    treeProjectEditor->setIndentation(15);
                    treeProjectEditor->setRootIsDecorated(true);
                    treeProjectEditor->setItemsExpandable(true);
                    treeProjectEditor->setExpandsOnDoubleClick(false);
                    treeProjectEditor->header()->setSectionResizeMode(0, QHeaderView::ResizeMode::Stretch);
                    treeProjectEditor->header()->setStretchLastSection(false);
                    treeProjectEditor->header()->setVisible(true);
                    treeProjectEditor->setFrameShape(QFrame::NoFrame);
                    treeProjectEditor->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

                splitterHorizontal->addWidget(treeProjectEditor);


                    QWidget *widgetStageView = new QWidget(splitterHorizontal);
                    widgetStageView->setObjectName(QStringLiteral("widgetStageView"));
                    widgetStageView->setSizePolicy(sizePolicyView);
                    widgetStageView->setMinimumSize(QSize(100, 0));
                    widgetStageView->setFont(font);
                        QVBoxLayout *verticalLayoutView = new QVBoxLayout(widgetStageView);
                        verticalLayoutView->setObjectName(QStringLiteral("verticalLayoutView"));
                        verticalLayoutView->setSpacing(0);
                        verticalLayoutView->setContentsMargins(0, 0, 0, 0);

                        // ***** Load our DrView to display our DrScene collection of items
                        viewEditor = new DrView(widgetStageView, m_project, sceneEditor, this);
                        viewEditor->setObjectName(QStringLiteral("viewEditor"));
                        viewEditor->setAcceptDrops(true);
                        viewEditor->setFrameShape(QFrame::NoFrame);
                        viewEditor->setDragMode(QGraphicsView::DragMode::NoDrag);
                        viewEditor->setTransformationAnchor(QGraphicsView::ViewportAnchor::AnchorUnderMouse);
                        viewEditor->setOptimizationFlags(QGraphicsView::OptimizationFlag::DontSavePainterState);
                        viewEditor->zoomInOut( 0 );

                        ///// This setting means we will decide when to call update(), controls recurssive paint events
                        ///viewEditor->setViewportUpdateMode(QGraphicsView::ViewportUpdateMode::NoViewportUpdate);
                        viewEditor->setViewportUpdateMode(QGraphicsView::ViewportUpdateMode::SmartViewportUpdate);

                        if (!Dr::CheckDebugFlag(Debug_Flags::Turn_On_Antialiasing))
                            viewEditor->setRenderHint(QPainter::Antialiasing, false);
                        else
                            viewEditor->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

                    verticalLayoutView->addWidget(viewEditor);

                splitterHorizontal->addWidget(widgetStageView);
                splitterHorizontal->setSizes(QList<int> { 150, 300 });      // Sets tree_stage (stage assests) startup width to 150
                                                                            // #NOTE: You can save and restore the sizes of the widgets from a QByteArray
                                                                            //        using QSplitter.saveState() and QSplitter.restoreState() respectively
            horizontalLayout->addWidget(splitterHorizontal);
        splitterVertical->addWidget(widgetStage);

            areaBottom = new QScrollArea(splitterVertical);
            areaBottom->setObjectName(QStringLiteral("areaBottom"));
            areaBottom->setSizePolicy(sizePolicyMinimum);
            areaBottom->setMinimumSize(QSize(0, 100));
            areaBottom->setFont(font);
            areaBottom->setWidgetResizable(true);
            areaBottom->setFrameShape(QFrame::NoFrame);
                QLabel *label_1 = new QLabel(areaBottom);
                label_1->setObjectName("label_1");
                label_1->setFont(font);

        splitterVertical->addWidget(areaBottom);


        splitterVertical->setStretchFactor(0, 1);           // widgetStage (index 0) should stretch (1)
        splitterVertical->setStretchFactor(1, 0);           // areaBottom  (index 1) should not stretch (0)

    verticalLayoutCentral->addWidget(splitterVertical);


}















