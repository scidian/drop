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
#include "editor/view_node_map/node_map_scene.h"
#include "editor/view_node_map/node_map_view.h"
#include "editor/widgets/widgets_editor.h"
#include "engine/debug_flags.h"


//####################################################################################
//##    Builds shared widgets used for all modes of FormMain
//####################################################################################
void FormMain::buildCentralWidgetClear() {
    QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);

    // ***** Empty central widget
    widgetCentralClear = new QWidget();
    widgetCentralClear->setObjectName(QStringLiteral("widgetCentralClear"));
    widgetCentralClear->setSizePolicy(sizePolicy);
    QGridLayout *layout = new QGridLayout(widgetCentralClear);
        QLabel *label = new QLabel("Coming Soon...");
        label->setFont(Dr::CustomFontLarger());
        layout->addWidget(label, 0, 0, Qt::AlignmentFlag::AlignCenter);

}


//####################################################################################
//##    Builds Widgets used for FormMainMode "World Editor"
//####################################################################################
void FormMain::buildCentralWidgetWorldMap() {
    QSizePolicy sizePolicyPreferredHorizontal(  QSizePolicy::Preferred,         QSizePolicy::Preferred);
    sizePolicyPreferredHorizontal.setHorizontalStretch(1);      sizePolicyPreferredHorizontal.setVerticalStretch(0);

    // ***** Initialize scene used for showing stages Editor_Mode::World_Editor viewEditor widget
    sceneWorldMap = new NodeMapScene(this, m_project, this);

    // ***** Build central widgets
    widgetCentralWorldMap = new QWidget();
    widgetCentralWorldMap->setObjectName(QStringLiteral("widgetCentralWorldMap"));
    widgetCentralWorldMap->setSizePolicy(sizePolicyPreferredHorizontal);
        QVBoxLayout *verticalLayoutWorldMap = new QVBoxLayout(widgetCentralWorldMap);
        verticalLayoutWorldMap->setSpacing(0);
        verticalLayoutWorldMap->setObjectName(QStringLiteral("verticalLayoutWorldMap"));
        // This sets the border with for the main view area between middle and docks
        verticalLayoutWorldMap->setContentsMargins(0, 0, 0, 0);

        // ***** Load our EditorView to display our EditorScene collection of items
        viewWorldMap = new NodeMapView(widgetCentralWorldMap, m_project, sceneWorldMap, this);
        viewWorldMap->setObjectName(QStringLiteral("viewWorldMap"));
        viewWorldMap->setAcceptDrops(true);
        viewWorldMap->setFrameShape(QFrame::NoFrame);
        viewWorldMap->setDragMode(QGraphicsView::DragMode::NoDrag);
        viewWorldMap->setTransformationAnchor(QGraphicsView::ViewportAnchor::AnchorUnderMouse);
        viewWorldMap->setOptimizationFlags(QGraphicsView::OptimizationFlag::DontSavePainterState);
        viewWorldMap->zoomInOut( 0 );

        ///// This setting means we will decide when to call update(), controls recurssive paint events
        ///viewWorldMap->setViewportUpdateMode(QGraphicsView::ViewportUpdateMode::NoViewportUpdate);
        viewWorldMap->setViewportUpdateMode(QGraphicsView::ViewportUpdateMode::SmartViewportUpdate);

        if (!Dr::CheckDebugFlag(Debug_Flags::Turn_On_Antialiasing_in_Editor))
            viewWorldMap->setRenderHint(QPainter::Antialiasing, false);
        else
            viewWorldMap->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

        if (Dr::CheckDebugFlag(Debug_Flags::Turn_On_OpenGL_in_Editor)) {
            QOpenGLWidget *gl_widget = new QOpenGLWidget();
            gl_widget->setUpdateBehavior(QOpenGLWidget::UpdateBehavior::NoPartialUpdate);
            viewWorldMap->setViewport(gl_widget);
        }

        // ***** ToolBar above World Map
        toolbarWorldMap = new EditorViewToolbar(widgetCentralWorldMap, m_project, this, Editor_Mode::World_Map, nullptr, viewWorldMap);

    verticalLayoutWorldMap->addWidget(toolbarWorldMap);
    verticalLayoutWorldMap->addWidget(viewWorldMap);

}


//####################################################################################
//##    Builds Widgets used for FormMainMode "World Editor"
//####################################################################################
void FormMain::buildCentralWidgetEditor() {
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
    sceneEditor = new EditorScene(this, m_project, this);

    // Connects signal used to populate scene
    connect(this,       SIGNAL(newStageSelected(DrProject*, EditorScene*, long, long)),
            sceneEditor,  SLOT(newStageSelected(DrProject*, EditorScene*, long, long)) );


    // ***** Build central widgets
    widgetCentralEditor = new QWidget();
    widgetCentralEditor->setObjectName(QStringLiteral("widgetCentralEditor"));
    widgetCentralEditor->setSizePolicy(sizePolicyPreferredHorizontal);
        QVBoxLayout *verticalLayoutCentral = new QVBoxLayout(widgetCentralEditor);
        verticalLayoutCentral->setSpacing(0);
        verticalLayoutCentral->setObjectName(QStringLiteral("verticalLayoutCentral"));
        // This sets the border with for the main view area between middle and docks
        verticalLayoutCentral->setContentsMargins(0, 0, 0, 0);

        QSplitter *splitterVertical = new QSplitter(widgetCentralEditor);
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
                    treeProjectEditor->setProperty("showDropIndicator", false);
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


                    QWidget *widgetEditorView = new QWidget(splitterHorizontal);
                    widgetEditorView->setObjectName(QStringLiteral("widgetEditorView"));
                    widgetEditorView->setSizePolicy(sizePolicyView);
                    widgetEditorView->setMinimumSize(QSize(100, 0));
                    widgetEditorView->setFont(font);

                        QVBoxLayout *verticalLayoutView = new QVBoxLayout(widgetEditorView);
                        verticalLayoutView->setObjectName(QStringLiteral("verticalLayoutView"));
                        verticalLayoutView->setSpacing(0);
                        verticalLayoutView->setContentsMargins(0, 0, 0, 0);

                        // ***** Load our EditorView to display our EditorScene collection of items
                        viewEditor = new EditorView(widgetEditorView, m_project, this, sceneEditor);
                        viewEditor->setObjectName(QStringLiteral("viewEditor"));
                        viewEditor->setAcceptDrops(true);
                        viewEditor->setFrameShape(QFrame::NoFrame);
                        viewEditor->setDragMode(QGraphicsView::DragMode::NoDrag);
                        viewEditor->setTransformationAnchor(QGraphicsView::ViewportAnchor::AnchorUnderMouse);
                        viewEditor->setOptimizationFlags(QGraphicsView::OptimizationFlag::DontSavePainterState);
                        viewEditor->zoomInOut( 0 );

                        ///// Possible Optimizations
                        ///viewEditor->setHorizontalScrollBarPolicy ( Qt::ScrollBarAlwaysOff );
                        ///viewEditor->setVerticalScrollBarPolicy ( Qt::ScrollBarAlwaysOff );
                        ///viewEditor->setCacheMode(QGraphicsView::CacheModeFlag::CacheBackground);

                        ///// This setting means we will decide when to call update(), controls recurssive paint events
                        ///viewEditor->setViewportUpdateMode(QGraphicsView::ViewportUpdateMode::NoViewportUpdate);
                        viewEditor->setViewportUpdateMode(QGraphicsView::ViewportUpdateMode::SmartViewportUpdate);

                        if (!Dr::CheckDebugFlag(Debug_Flags::Turn_On_Antialiasing_in_Editor))
                            viewEditor->setRenderHint(QPainter::Antialiasing, false);
                        else
                            viewEditor->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

                        if (Dr::CheckDebugFlag(Debug_Flags::Turn_On_OpenGL_in_Editor)) {
                            QOpenGLWidget *gl_widget = new QOpenGLWidget();
                            gl_widget->setUpdateBehavior(QOpenGLWidget::UpdateBehavior::NoPartialUpdate);
                            viewEditor->setViewport(gl_widget);
                        }


                        // ***** ToolBar above World Editor
                        toolbarEditor = new EditorViewToolbar(widgetEditorView, m_project, this, Editor_Mode::World_Editor, viewEditor, nullptr);


                        // ***** Lower View area Status Bar
                        statusBar = new QFrame(widgetEditorView);
                        statusBar->setObjectName("statusBar");
                        statusBar->setFixedHeight(26);
                            QHBoxLayout *status_layout = new QHBoxLayout(statusBar);
                            status_layout->setObjectName(QStringLiteral("statusLayout"));
                            status_layout->setSpacing(6);
                            status_layout->setContentsMargins(6, 0, 6, 0);

                            labelSelected = new QLabel("No Selection");
                            labelSelected->setObjectName(QStringLiteral("labelSelected"));
                            labelSelected->setAlignment(Qt::AlignmentFlag::AlignCenter);
                            labelSelected->setFont(font);
                            status_layout->addWidget(labelSelected);
                            status_layout->addWidget(createToolBarSpacer(18));

                            labelInfo = new QLabel("");
                            labelInfo->setObjectName(QStringLiteral("labelInfo"));
                            labelInfo->setAlignment(Qt::AlignmentFlag::AlignCenter);
                            labelInfo->setFont(font);
                            status_layout->addWidget(labelInfo);
                            status_layout->addStretch();

                            labelMousePosition = new QLabel(" ");
                            labelMousePosition->setObjectName(QStringLiteral("labelMousePosition"));
                            labelMousePosition->setAlignment(Qt::AlignmentFlag::AlignRight | Qt::AlignmentFlag::AlignVCenter);
                            labelMousePosition->setFont(font);
                            status_layout->addWidget(labelMousePosition);

                    verticalLayoutView->addWidget(toolbarEditor);
                    verticalLayoutView->addWidget(viewEditor);
                    verticalLayoutView->addWidget(statusBar);

                splitterHorizontal->addWidget(widgetEditorView);
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
                label1 =        createLabel(areaBottom,   "label_1",            QRect( 10,  5, 220, 21),    font);
                label2 =        createLabel(areaBottom,   "label_2",            QRect( 10, 20, 220, 21),    font);
                label3 =        createLabel(areaBottom,   "label_3",            QRect( 10, 35, 220, 21),    font);
                labelMouse1 =   createLabel(areaBottom,   "label_mouse_1",      QRect( 10, 50, 220, 21),    font);
                labelMouse2 =   createLabel(areaBottom,   "label_mouse_2",      QRect( 10, 65, 220, 21),    font);

                labelObject1 =   createLabel(areaBottom,  "label_object_1",     QRect(240,  5, 400, 21),    font);
                labelObject2 =   createLabel(areaBottom,  "label_object_2",     QRect(240, 20, 400, 21),    font);
                labelObject3 =   createLabel(areaBottom,  "label_object_3",     QRect(240, 35, 400, 21),    font);
                labelObject4 =   createLabel(areaBottom,  "label_object_4",     QRect(240, 50, 400, 21),    font);
                labelObject5 =   createLabel(areaBottom,  "label_object_5",     QRect(240, 65, 400, 21),    font);

                labelPosition =  createLabel(areaBottom,  "label_position",     QRect(560,  5, 400, 21),    font);
                labelCenter =    createLabel(areaBottom,  "label_center",       QRect(560, 20, 400, 21),    font);
                labelScale =     createLabel(areaBottom,  "label_scale",        QRect(560, 35, 400, 21),    font);
                labelRotate =    createLabel(areaBottom,  "label_rotate",       QRect(560, 50, 400, 21),    font);
                labelZOrder =    createLabel(areaBottom,  "label_z_order",      QRect(560, 65, 400, 21),    font);
                labelPosFlag =   createLabel(areaBottom,  "label_pos_flag",     QRect(560, 80, 400, 21),    font);

                labelBottom =    createLabel(areaBottom,  "label_bottom",       QRect( 10, 80, 700, 21),    font);
        splitterVertical->addWidget(areaBottom);

        areaBottom->setVisible( Dr::CheckDebugFlag(Debug_Flags::Show_Bottom_Debug_Labels) );

        splitterVertical->setStretchFactor(0, 1);           // widgetStage (index 0) should stretch (1)
        splitterVertical->setStretchFactor(1, 0);           // areaBottom  (index 1) should not stretch (0)

    verticalLayoutCentral->addWidget(splitterVertical);

}


QLabel* FormMain::createLabel(QWidget *parent, QString object_name, QRect label_rect, QFont &label_font) {
    QLabel *new_label;
    new_label = new QLabel(parent);
    new_label->setObjectName(object_name);
    new_label->setGeometry(label_rect);
    new_label->setFont(label_font);
    return new_label;
}















