//
//      Created by Stephens Nunnally on 3/14/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QButtonGroup>
#include <QDockWidget>
#include <QHeaderView>
#include <QOpenGLWidget>
#include <QScrollBar>

#include "editor/docks/docks.h"
#include "editor/form_main/form_main.h"
#include "editor/helper_library.h"
#include "editor/preferences.h"
#include "editor/style/style.h"
#include "editor/trees/tree_assets.h"
#include "editor/trees/tree_inspector.h"
#include "editor/trees/tree_project.h"
#include "editor/view_editor/editor_item.h"
#include "editor/view_editor/editor_scene.h"
#include "editor/view_editor/editor_view.h"
#include "editor/view_mixer/mixer_item.h"
#include "editor/view_mixer/mixer_scene.h"
#include "editor/view_mixer/mixer_view.h"
#include "editor/view_node/node_scene.h"
#include "editor/view_node/node_view.h"
#include "editor/widgets/widgets_view_toolbar.h"
#include "editor/widgets/widgets_view.h"
#include "engine/debug_flags.h"


//####################################################################################
//##    Builds shared widgets used for all modes of FormMain
//####################################################################################
void FormMain::buildCentralWidgetClear() {
    QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);

    // ***** Empty central widget
    m_widget_central_clear = new QWidget();
    m_widget_central_clear->setObjectName(QStringLiteral("widgetCentralClear"));
    m_widget_central_clear->setSizePolicy(sizePolicy);
    QGridLayout *layout = new QGridLayout(m_widget_central_clear);
        QLabel *label = new QLabel("Coming Soon...");
                label->setFont(Dr::CustomFontLarger());
        layout->addWidget(label, 0, 0, Qt::AlignmentFlag::AlignCenter);

}


//####################################################################################
//##    Builds Widgets used for FormMainMode "World Graph"
//####################################################################################
void FormMain::buildCentralWidgetWorldGraph() {
    QSizePolicy size_policy_preferred_horizontal(QSizePolicy::Preferred, QSizePolicy::Preferred);
    size_policy_preferred_horizontal.setHorizontalStretch(1);
    size_policy_preferred_horizontal.setVerticalStretch(0);

    // ***** Initialize scene used for showing nodes in Editor_Mode::World_Graph m_view_world_graph widget
    m_scene_world_graph = new NodeScene(this, m_project, this);

    // ***** Build central widgets
    m_widget_central_world_graph = new QWidget();
    m_widget_central_world_graph->setObjectName(QStringLiteral("widgetCentralWorldGraph"));
    m_widget_central_world_graph->setSizePolicy(size_policy_preferred_horizontal);
        QVBoxLayout *vertical_layout_world_graph = new QVBoxLayout(m_widget_central_world_graph);
        vertical_layout_world_graph->setSpacing(0);
        vertical_layout_world_graph->setObjectName(QStringLiteral("verticalLayoutWorldGraph"));
        // This sets the border with for the main view area between middle and docks
        vertical_layout_world_graph->setContentsMargins(0, 0, 0, 0);

        // ***** Load our EditorView to display our EditorScene collection of items
        m_view_world_graph = new NodeView(m_widget_central_world_graph, m_project, m_scene_world_graph, this);
        m_view_world_graph->setObjectName(QStringLiteral("viewWorldGraph"));
        m_view_world_graph->setAcceptDrops(true);
        m_view_world_graph->setFrameShape(QFrame::NoFrame);
        m_view_world_graph->setDragMode(QGraphicsView::DragMode::NoDrag);
        m_view_world_graph->setTransformationAnchor(QGraphicsView::ViewportAnchor::AnchorUnderMouse);
        m_view_world_graph->setOptimizationFlags(QGraphicsView::OptimizationFlag::DontSavePainterState);
        m_view_world_graph->zoomInOut( 0 );

        ///// This setting means we will decide when to call update(), controls recurssive paint events
        ///m_view_world_graph->setViewportUpdateMode(QGraphicsView::ViewportUpdateMode::NoViewportUpdate);
        m_view_world_graph->setViewportUpdateMode(QGraphicsView::ViewportUpdateMode::SmartViewportUpdate);

        if (!Dr::CheckDebugFlag(Debug_Flags::Turn_On_Antialiasing_in_Editor))
            m_view_world_graph->setRenderHint(QPainter::Antialiasing, false);
        else
            m_view_world_graph->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

        if (Dr::CheckDebugFlag(Debug_Flags::Turn_On_OpenGL_in_Editor)) {
            QOpenGLWidget *gl_widget = new QOpenGLWidget();
            gl_widget->setUpdateBehavior(QOpenGLWidget::UpdateBehavior::NoPartialUpdate);
            m_view_world_graph->setViewport(gl_widget);
        }

        // ***** ToolBar above World Graph
        m_toolbar_world_graph = new ViewToolbar(m_widget_central_world_graph, m_project, this, Editor_Mode::World_Graph, nullptr, m_view_world_graph);

    vertical_layout_world_graph->addWidget(m_toolbar_world_graph);
    vertical_layout_world_graph->addWidget(m_view_world_graph);

}


//####################################################################################
//##    Builds Widgets used for FormMainMode "Sound Creator"
//####################################################################################
void FormMain::buildCentralWidgetSoundCreator() {
    QSizePolicy size_policy_preferred_horizontal(QSizePolicy::Preferred, QSizePolicy::Preferred);
    size_policy_preferred_horizontal.setHorizontalStretch(1);
    size_policy_preferred_horizontal.setVerticalStretch(0);

    // ***** Visual Timer
    m_visual_timer = new QTimer(this);
    m_visual_timer->setInterval(20);
    m_visual_timer->setTimerType(Qt::PreciseTimer);
    connect(m_visual_timer, SIGNAL(timeout()), this, SLOT(drawVisuals()));

    // ***** Initialize scene used for mixing DrMix
    m_scene_mixer = new MixerScene(this, m_project, this);

//    // ***** Build central widgets
//    m_widget_central_world_graph = new QWidget();
//    m_widget_central_world_graph->setObjectName(QStringLiteral("widgetCentralWorldGraph"));
//    m_widget_central_world_graph->setSizePolicy(size_policy_preferred_horizontal);
//        QVBoxLayout *vertical_layout_world_graph = new QVBoxLayout(m_widget_central_world_graph);
//        vertical_layout_world_graph->setSpacing(0);
//        vertical_layout_world_graph->setObjectName(QStringLiteral("verticalLayoutWorldGraph"));
//        // This sets the border with for the main view area between middle and docks
//        vertical_layout_world_graph->setContentsMargins(0, 0, 0, 0);

//        // ***** Load our EditorView to display our EditorScene collection of items
//        m_view_world_graph = new NodeView(m_widget_central_world_graph, m_project, m_scene_world_graph, this);
//        m_view_world_graph->setObjectName(QStringLiteral("viewWorldGraph"));
//        m_view_world_graph->setAcceptDrops(true);
//        m_view_world_graph->setFrameShape(QFrame::NoFrame);
//        m_view_world_graph->setDragMode(QGraphicsView::DragMode::NoDrag);
//        m_view_world_graph->setTransformationAnchor(QGraphicsView::ViewportAnchor::AnchorUnderMouse);
//        m_view_world_graph->setOptimizationFlags(QGraphicsView::OptimizationFlag::DontSavePainterState);
//        m_view_world_graph->zoomInOut( 0 );

//        ///// This setting means we will decide when to call update(), controls recurssive paint events
//        ///m_view_world_graph->setViewportUpdateMode(QGraphicsView::ViewportUpdateMode::NoViewportUpdate);
//        m_view_world_graph->setViewportUpdateMode(QGraphicsView::ViewportUpdateMode::SmartViewportUpdate);

//        if (!Dr::CheckDebugFlag(Debug_Flags::Turn_On_Antialiasing_in_Editor))
//            m_view_world_graph->setRenderHint(QPainter::Antialiasing, false);
//        else
//            m_view_world_graph->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

//        if (Dr::CheckDebugFlag(Debug_Flags::Turn_On_OpenGL_in_Editor)) {
//            QOpenGLWidget *gl_widget = new QOpenGLWidget();
//            gl_widget->setUpdateBehavior(QOpenGLWidget::UpdateBehavior::NoPartialUpdate);
//            m_view_world_graph->setViewport(gl_widget);
//        }

//        // ***** ToolBar above World Graph
//        m_toolbar_world_graph = new ViewToolbar(m_widget_central_world_graph, m_project, this, Editor_Mode::World_Graph, nullptr, m_view_world_graph);

//    vertical_layout_world_graph->addWidget(m_toolbar_world_graph);
//    vertical_layout_world_graph->addWidget(m_view_world_graph);
}


//####################################################################################
//##    Builds Widgets used for FormMainMode "World Creator"
//####################################################################################
void FormMain::buildCentralWidgetWorldCreator() {
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
    connect(this,           SIGNAL(newStageSelected(DrProject*, EditorScene*, long, long)),
            m_scene_editor,   SLOT(newStageSelected(DrProject*, EditorScene*, long, long)) );


    // ***** Build central widgets
    m_widget_central_editor = new QWidget();
    m_widget_central_editor->setObjectName(QStringLiteral("widgetCentralEditor"));
    m_widget_central_editor->setSizePolicy(size_policy_preferred_horizontal);
        QVBoxLayout *vertical_layout_central = new QVBoxLayout(m_widget_central_editor);
        vertical_layout_central->setSpacing(0);
        vertical_layout_central->setObjectName(QStringLiteral("verticalLayoutCentral"));
        // This sets the border with for the main view area between middle and docks
        vertical_layout_central->setContentsMargins(0, 0, 0, 0);

        QSplitter *splitter_vertical = new QSplitter(m_widget_central_editor);
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

                        ///// Possible Optimizations
                        ///m_view_editor->setHorizontalScrollBarPolicy ( Qt::ScrollBarAlwaysOff );
                        ///m_view_editor->setVerticalScrollBarPolicy ( Qt::ScrollBarAlwaysOff );
                        ///m_view_editor->setCacheMode(QGraphicsView::CacheModeFlag::CacheBackground);

                        ///// This setting means we will decide when to call update(), controls recurssive paint events
                        ///m_view_editor->setViewportUpdateMode(QGraphicsView::ViewportUpdateMode::NoViewportUpdate);
                        m_view_editor->setViewportUpdateMode(QGraphicsView::ViewportUpdateMode::SmartViewportUpdate);

                        if (!Dr::CheckDebugFlag(Debug_Flags::Turn_On_Antialiasing_in_Editor))
                            m_view_editor->setRenderHint(QPainter::Antialiasing, false);
                        else
                            m_view_editor->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

                        if (Dr::CheckDebugFlag(Debug_Flags::Turn_On_OpenGL_in_Editor)) {
                            QOpenGLWidget *gl_widget = new QOpenGLWidget();
                            gl_widget->setUpdateBehavior(QOpenGLWidget::UpdateBehavior::NoPartialUpdate);
                            m_view_editor->setViewport(gl_widget);
                        }


                        // ***** ToolBar above World Editor View
                        m_toolbar_editor = new ViewToolbar(widget_editor_view, m_project, this, Editor_Mode::World_Creator, m_view_editor, nullptr);


                        // ***** Lower View area Status Bar
                        m_status_bar = new QFrame(widget_editor_view);
                        m_status_bar->setObjectName("statusBar");
                            QHBoxLayout *status_layout = new QHBoxLayout(m_status_bar);
                            status_layout->setObjectName(QStringLiteral("statusLayout"));
                            status_layout->setSpacing(6);
                            status_layout->setContentsMargins(6, 0, 6, 0);

                            m_label_selected = new QLabel("No Selection");
                            m_label_selected->setObjectName(QStringLiteral("labelSelected"));
                            m_label_selected->setAlignment(Qt::AlignmentFlag::AlignCenter);
                            m_label_selected->setFont(font);
                            status_layout->addWidget(m_label_selected);
                            status_layout->addWidget(createToolBarSpacer(18));

                            m_label_info = new QLabel("");
                            m_label_info->setObjectName(QStringLiteral("labelInfo"));
                            m_label_info->setAlignment(Qt::AlignmentFlag::AlignCenter);
                            m_label_info->setFont(font);
                            status_layout->addWidget(m_label_info);
                            status_layout->addStretch();

                            m_label_mouse_position = new QLabel(" ");
                            m_label_mouse_position->setObjectName(QStringLiteral("labelMousePosition"));
                            m_label_mouse_position->setAlignment(Qt::AlignmentFlag::AlignRight | Qt::AlignmentFlag::AlignVCenter);
                            m_label_mouse_position->setFont(font);
                            status_layout->addWidget(m_label_mouse_position);

                    vertical_layout_view->addWidget(m_toolbar_editor);
                    vertical_layout_view->addWidget(m_view_editor);
                    vertical_layout_view->addWidget(m_status_bar);

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
                label_1 =           createLabel(m_area_bottom,  "label_1",            QRect( 10,  5, 220, 21),    font);
                label_2 =           createLabel(m_area_bottom,  "label_2",            QRect( 10, 20, 220, 21),    font);
                label_3 =           createLabel(m_area_bottom,  "label_3",            QRect( 10, 35, 220, 21),    font);
                label_mouse_1 =     createLabel(m_area_bottom,  "label_mouse_1",      QRect( 10, 50, 220, 21),    font);
                label_mouse_2 =     createLabel(m_area_bottom,  "label_mouse_2",      QRect( 10, 65, 220, 21),    font);

                label_object_1 =    createLabel(m_area_bottom,  "label_object_1",     QRect(240,  5, 400, 21),    font);
                label_object_2 =    createLabel(m_area_bottom,  "label_object_2",     QRect(240, 20, 400, 21),    font);
                label_object_3 =    createLabel(m_area_bottom,  "label_object_3",     QRect(240, 35, 400, 21),    font);
                label_object_4 =    createLabel(m_area_bottom,  "label_object_4",     QRect(240, 50, 400, 21),    font);
                label_object_5 =    createLabel(m_area_bottom,  "label_object_5",     QRect(240, 65, 400, 21),    font);

                label_position =    createLabel(m_area_bottom,  "label_position",     QRect(560,  5, 400, 21),    font);
                label_center =      createLabel(m_area_bottom,  "label_center",       QRect(560, 20, 400, 21),    font);
                label_scale =       createLabel(m_area_bottom,  "label_scale",        QRect(560, 35, 400, 21),    font);
                label_rotate =      createLabel(m_area_bottom,  "label_rotate",       QRect(560, 50, 400, 21),    font);
                label_z_order =     createLabel(m_area_bottom,  "label_z_order",      QRect(560, 65, 400, 21),    font);
                label_pos_flag =    createLabel(m_area_bottom,  "label_pos_flag",     QRect(560, 80, 400, 21),    font);

                label_bottom =      createLabel(m_area_bottom,  "label_bottom",       QRect( 10, 80, 700, 21),    font);
        splitter_vertical->addWidget(m_area_bottom);

        m_area_bottom->setVisible( Dr::CheckDebugFlag(Debug_Flags::Show_Bottom_Debug_Labels) );

        splitter_vertical->setStretchFactor(0, 1);           // widgetStage (index 0) should stretch (1)
        splitter_vertical->setStretchFactor(1, 0);           // areaBottom  (index 1) should not stretch (0)

    vertical_layout_central->addWidget(splitter_vertical);

}


QLabel* FormMain::createLabel(QWidget *parent, QString object_name, QRect label_rect, QFont &label_font) {
    QLabel *new_label;
            new_label = new QLabel(parent);
            new_label->setObjectName(object_name);
            new_label->setGeometry(label_rect);
            new_label->setFont(label_font);
    return new_label;
}















