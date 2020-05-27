//
//      Created by Stephens Nunnally on 5/27/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QHeaderView>

#include "editor/docks/docks.h"
#include "editor/event_filters/event_filters.h"
#include "editor/form_animation/form_animation.h"
#include "editor/helper_library.h"
#include "editor/style/style.h"
#include "editor/trees/tree_assets.h"
#include "editor/trees/tree_inspector.h"
#include "editor/trees/tree_project.h"
#include "editor/view_editor/editor_scene.h"
#include "editor/view_editor/editor_view.h"
#include "engine/debug_flags.h"


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

    m_dock_advisor =    Dr::BuildDockAdvisor(  m_project, this, m_tree_advisor);
    m_dock_assets =     Dr::BuildDockAssets(   m_project, this, m_tree_assets);
    m_dock_inspector =  Dr::BuildDockInspector(m_project, this, m_tree_inspector);
    Dr::InitializeDockWidgets(this, m_dock_advisor, m_dock_assets, m_dock_inspector);

    // ***** Set up FormAnimation for first time
    Dr::LockDockWidth( m_dock_advisor, m_dock_advisor->width() );
    Dr::LockDockWidth( m_dock_assets, m_dock_assets->width() );
    Dr::LockDockWidth( m_dock_inspector, m_dock_inspector->width() );

    this->setCentralWidget( m_widget_central );
    m_dock_assets->setWindowTitle( QMainWindow::tr(QString("Images").toUtf8()) );
    m_tree_assets->setShowTypes({ DrType::Image });
    buildAssetTree();
    m_dock_assets->show();
    buildProjectTree();
    m_scene_editor->clearSceneOverride();

    //buildSceneAfterLoading( m_project->getOption(Project_Options::Current_Stage).toInt() );

    Dr::UnlockDockWidth( this, m_dock_advisor );
    Dr::UnlockDockWidth( this, m_dock_assets );
    Dr::UnlockDockWidth( this, m_dock_inspector );
}


//####################################################################################
//##    Builds Widgets used for Animation Editor
//####################################################################################
void FormAnimation::buildCentralWidget() {
    QFont font = Dr::CustomFont();

    QSizePolicy size_policy(                        QSizePolicy::Preferred,         QSizePolicy::Preferred);
    QSizePolicy size_policy_preferred_horizontal(   QSizePolicy::Preferred,         QSizePolicy::Preferred);
    QSizePolicy size_policy_preferred_vertical(     QSizePolicy::Preferred,         QSizePolicy::Preferred);
    QSizePolicy size_policy_minimum(                QSizePolicy::MinimumExpanding,  QSizePolicy::MinimumExpanding);
    QSizePolicy size_policy_view(                   QSizePolicy::MinimumExpanding,  QSizePolicy::Expanding);

    size_policy.setHorizontalStretch(0);                        size_policy.setVerticalStretch(0);
    size_policy_preferred_horizontal.setHorizontalStretch(1);   size_policy_preferred_horizontal.setVerticalStretch(0);
    size_policy_preferred_vertical.setHorizontalStretch(0);     size_policy_preferred_vertical.setVerticalStretch(1);
    size_policy_minimum.setHorizontalStretch(0);                size_policy_minimum.setVerticalStretch(0);
    size_policy_view.setHorizontalStretch(1);                   size_policy_view.setVerticalStretch(0);

    // ***** Initialize scene used for showing stages World Creator Mode m_view_editor widget
    m_scene_editor = new EditorScene(this, m_project, this);


    // Connects signal used to populate scene
//    connect(this,           SIGNAL(newStageSelected(DrProject*, EditorScene*, long, long)),
//            m_scene_editor,   SLOT(newStageSelected(DrProject*, EditorScene*, long, long)) );


    // ***** Build central widgets
    m_widget_central = new QWidget();
    m_widget_central->setObjectName(QStringLiteral("widgetCentral"));
    m_widget_central->setSizePolicy(size_policy_preferred_horizontal);
        QVBoxLayout *vertical_layout_central = new QVBoxLayout(m_widget_central);
        vertical_layout_central->setSpacing(0);
        vertical_layout_central->setObjectName(QStringLiteral("verticalLayoutCentral"));
        // This sets the border with for the main view area between middle and docks
        vertical_layout_central->setContentsMargins(0, 0, 0, 0);

        QSplitter *splitter_vertical = new QSplitter(m_widget_central);
        splitter_vertical->setObjectName(QStringLiteral("splitterVertical"));
        splitter_vertical->setOrientation(Qt::Vertical);
        splitter_vertical->setHandleWidth(4);

            QWidget *widget_stage = new QWidget(splitter_vertical);
            widget_stage->setObjectName(QStringLiteral("widgetStage"));
                QHBoxLayout *horizontal_layout = new QHBoxLayout(widget_stage);
                horizontal_layout->setObjectName(QStringLiteral("horizontalLayout"));
                horizontal_layout->setSpacing(0);
                horizontal_layout->setContentsMargins(0, 0, 0, 0);

                QSplitter *splitter_horizontal = new QSplitter(widget_stage);
                splitter_horizontal->setObjectName(QStringLiteral("splitterHorizontal"));
                splitter_horizontal->setLineWidth(0);
                splitter_horizontal->setOrientation(Qt::Horizontal);
                splitter_horizontal->setHandleWidth(4);

                    // ***** Load our custom TreeProject for the Scene List
                    m_tree_project = new TreeProject(splitter_horizontal, m_project, this);
                    m_tree_project->setStyle(new TreeStageHighlightProxy(m_tree_project->style(), m_tree_project));
                        QTreeWidgetItem *header_item_stage = new QTreeWidgetItem();
                        header_item_stage->setIcon(1, QIcon(":/assets/tree_icons/tree_lock_header.png"));
                        m_tree_project->setHeaderItem(header_item_stage);
                    m_tree_project->setObjectName(QStringLiteral("treeProjectEditor"));
                    m_tree_project->setColumnCount(3);
                    m_tree_project->setColumnWidth(COLUMN_TITLE,     150);
                    m_tree_project->setColumnWidth(COLUMN_LOCK,       16);
                    m_tree_project->setColumnWidth(COLUMN_Z_ORDER,    30);
                    m_tree_project->setColumnHidden(COLUMN_Z_ORDER, true);
                    m_tree_project->setMinimumSize(QSize(190, 0));
                    m_tree_project->setMaximumWidth(400);
                    m_tree_project->setFont(font);
                    m_tree_project->setProperty("showDropIndicator", false);
                    m_tree_project->setDragEnabled(true);
                    m_tree_project->setDragDropOverwriteMode(false);
                    m_tree_project->setDragDropMode(QAbstractItemView::DragDropMode::InternalMove);
                    m_tree_project->setDefaultDropAction(Qt::DropAction::TargetMoveAction);
                    m_tree_project->setAlternatingRowColors(false);
                    m_tree_project->setSelectionMode(QAbstractItemView::SelectionMode::ExtendedSelection);
                    m_tree_project->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
                    m_tree_project->setIndentation(15);
                    m_tree_project->setRootIsDecorated(true);
                    m_tree_project->setItemsExpandable(true);
                    m_tree_project->setExpandsOnDoubleClick(false);
                    m_tree_project->header()->setSectionResizeMode(0, QHeaderView::ResizeMode::Stretch);
                    m_tree_project->header()->setStretchLastSection(false);
                    m_tree_project->header()->setVisible(true);
                    m_tree_project->setFrameShape(QFrame::NoFrame);
                    m_tree_project->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

                splitter_horizontal->addWidget(m_tree_project);


                    QWidget *widget_editor_view = new QWidget(splitter_horizontal);
                    widget_editor_view->setObjectName(QStringLiteral("widgetEditorView"));
                    widget_editor_view->setSizePolicy(size_policy_view);
                    widget_editor_view->setMinimumSize(QSize(100, 0));
                    widget_editor_view->setFont(font);
                        QVBoxLayout *vertical_layout_view = new QVBoxLayout(widget_editor_view);
                        vertical_layout_view->setObjectName(QStringLiteral("verticalLayoutView"));
                        vertical_layout_view->setSpacing(0);
                        vertical_layout_view->setContentsMargins(0, 0, 0, 0);

                        // ***** Load our EditorView to display our EditorScene collection of items
                        m_view_editor = new EditorView(widget_editor_view, m_project, this, m_scene_editor);
                        m_view_editor->setObjectName(QStringLiteral("viewEditor"));
                        m_view_editor->setAcceptDrops(true);
                        m_view_editor->setFrameShape(QFrame::NoFrame);
                        m_view_editor->setDragMode(QGraphicsView::DragMode::NoDrag);
                        m_view_editor->setTransformationAnchor(QGraphicsView::ViewportAnchor::AnchorUnderMouse);
                        m_view_editor->setOptimizationFlags(QGraphicsView::OptimizationFlag::DontSavePainterState);
                        m_view_editor->zoomInOut( 0 );

                        ///// This setting means we will decide when to call update(), controls recurssive paint events
                        ///m_view_editor->setViewportUpdateMode(QGraphicsView::ViewportUpdateMode::NoViewportUpdate);
                        m_view_editor->setViewportUpdateMode(QGraphicsView::ViewportUpdateMode::SmartViewportUpdate);

                        if (!Dr::CheckDebugFlag(Debug_Flags::Turn_On_Antialiasing_in_Editor))
                            m_view_editor->setRenderHint(QPainter::Antialiasing, false);
                        else
                            m_view_editor->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

                    vertical_layout_view->addWidget(m_view_editor);

                splitter_horizontal->addWidget(widget_editor_view);
                splitter_horizontal->setSizes(QList<int> { 150, 300 });      // Sets tree_stage (stage assests) startup width to 150
                                                                            // #NOTE: You can save and restore the sizes of the widgets from a QByteArray
                                                                            //        using QSplitter.saveState() and QSplitter.restoreState() respectively
            horizontal_layout->addWidget(splitter_horizontal);
        splitter_vertical->addWidget(widget_stage);

            m_area_bottom = new QScrollArea(splitter_vertical);
            m_area_bottom->setObjectName(QStringLiteral("areaBottom"));
            m_area_bottom->setSizePolicy(size_policy_minimum);
            m_area_bottom->setMinimumSize(QSize(0, 100));
            m_area_bottom->setFont(font);
            m_area_bottom->setWidgetResizable(true);
            m_area_bottom->setFrameShape(QFrame::NoFrame);
                QLabel *label_1 = new QLabel(m_area_bottom);
                label_1->setObjectName("label_1");
                label_1->setFont(font);

        splitter_vertical->addWidget(m_area_bottom);


        splitter_vertical->setStretchFactor(0, 1);           // widgetStage (index 0) should stretch (1)
        splitter_vertical->setStretchFactor(1, 0);           // areaBottom  (index 1) should not stretch (0)

    vertical_layout_central->addWidget(splitter_vertical);


}











