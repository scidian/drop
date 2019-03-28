//
//      Created by Stephens Nunnally on 3/14/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QDockWidget>
#include <QHeaderView>
#include <QOpenGLWidget>

#include "debug.h"
#include "editor_scene.h"
#include "editor_tree_assets.h"
#include "editor_tree_inspector.h"
#include "editor_tree_project.h"
#include "editor_view.h"
#include "form_main.h"
#include "globals.h"
#include "library.h"


//####################################################################################
//##        Builds Widgets used for FormMainMode "World Editor"
//####################################################################################
void FormMain::buildWidgetsEditor()
{
    QFont font;
    font.setPointSize(Dr::FontSize());

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
    sceneEditor = new DrScene(this, project, this);

    // Connects signal used to populate scene
    connect(this,       SIGNAL(newStageSelected(DrProject*, DrScene*, long, long)),
            sceneEditor,  SLOT(newStageSelected(DrProject*, DrScene*, long, long)) );


    // ***** Build central widgets
    widgetCentralEditor = new QWidget();
    widgetCentralEditor->setObjectName(QStringLiteral("widgetCentralEditor"));
    widgetCentralEditor->setSizePolicy(sizePolicyPreferredHorizontal);
        QVBoxLayout *verticalLayoutCentral = new QVBoxLayout(widgetCentralEditor);
        verticalLayoutCentral->setSpacing(0);
        verticalLayoutCentral->setObjectName(QStringLiteral("verticalLayout"));
        // This sets the border with for the main view area between middle and docks
        verticalLayoutCentral->setContentsMargins(0, 0, 0, 0);

        ColorSplitter *splitterVertical = new ColorSplitter(widgetCentralEditor);
        splitterVertical->setObjectName(QStringLiteral("splitterVertical"));
        splitterVertical->setOrientation(Qt::Vertical);
        splitterVertical->setHandleWidth(4);

            widgetStage = new QWidget(splitterVertical);
            widgetStage->setObjectName(QStringLiteral("widgetStage"));
                QHBoxLayout *horizontalLayout = new QHBoxLayout(widgetStage);
                horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
                horizontalLayout->setSpacing(0);
                horizontalLayout->setContentsMargins(0, 0, 0, 0);

                ColorSplitter *splitterHorizontal = new ColorSplitter(widgetStage);
                splitterHorizontal->setObjectName(QStringLiteral("splitterHorizontal"));
                splitterHorizontal->setLineWidth(0);
                splitterHorizontal->setOrientation(Qt::Horizontal);
                splitterHorizontal->setHandleWidth(4);

                    // ***** Load our custom TreeProject for the Scene List
                    treeProjectEditor = new TreeProject(splitterHorizontal, project, this);
                    treeProjectEditor->setStyle(new StageTreeHighlightProxy(treeProjectEditor->style(), treeProjectEditor));
                        QTreeWidgetItem *header_item_stage = new QTreeWidgetItem();
                        header_item_stage->setIcon(1, QIcon(":/tree_icons/tree_lock_header.png"));
                        treeProjectEditor->setHeaderItem(header_item_stage);
                    treeProjectEditor->setObjectName(QStringLiteral("treeProjectEditor"));
                    treeProjectEditor->setColumnCount(2);
                    treeProjectEditor->setColumnWidth(0, 150);
                    treeProjectEditor->setColumnWidth(1, 16);
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

                splitterHorizontal->addWidget(treeProjectEditor);


                    widgetStageView = new QWidget(splitterHorizontal);
                    widgetStageView->setObjectName(QStringLiteral("widgetStageView"));
                    widgetStageView->setSizePolicy(sizePolicyView);
                    widgetStageView->setMinimumSize(QSize(100, 0));
                    widgetStageView->setFont(font);
                        QVBoxLayout *verticalLayoutView = new QVBoxLayout(widgetStageView);
                        verticalLayoutView->setObjectName(QStringLiteral("verticalLayoutView"));
                        verticalLayoutView->setSpacing(0);
                        verticalLayoutView->setContentsMargins(0, 0, 0, 0);

                        // ***** Load our DrView to display our DrScene collection of items
                        viewEditor = new DrView(widgetStageView, project, sceneEditor, this);
                        viewEditor->setObjectName(QStringLiteral("viewEditor"));
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

                        if (!Dr::CheckDebugFlag(Debug_Flags::Turn_On_Antialiasing))
                            viewEditor->setRenderHint(QPainter::Antialiasing, false);
                        else {
                            viewEditor->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::HighQualityAntialiasing);
                            ///// Enables multisampling
                            ///QSurfaceFormat format;
                            ///format.setSamples(2);
                            ///QSurfaceFormat::setDefaultFormat(format);                   // Set antialiasing samples to 2
                        }

                        if (Dr::CheckDebugFlag(Debug_Flags::Turn_On_OpenGL)) {
                            QOpenGLWidget *gl_widget = new QOpenGLWidget();
                            gl_widget->setUpdateBehavior(QOpenGLWidget::UpdateBehavior::NoPartialUpdate);
                            viewEditor->setViewport(gl_widget);
                        }




                        // ***** View area status bar
                        statusBar = new QFrame(widgetStageView);
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
                            status_layout->addWidget(createToolbarSpacer(18));
                            status_layout->addStretch();




                    verticalLayoutView->addWidget(statusBar);
                    verticalLayoutView->addWidget(viewEditor);

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

        splitterVertical->setStretchFactor(0, 1);           // widgetStage (index 0) should stretch (1)
        splitterVertical->setStretchFactor(1, 0);           // areaBottom  (index 1) should not stretch (0)

    verticalLayoutCentral->addWidget(splitterVertical);




    // ***** Build left Assets Dock
    dockAssetsEditor = new QDockWidget(this);
    dockAssetsEditor->setWindowTitle( tr("Assets") );
    dockAssetsEditor->setObjectName(QStringLiteral("dockAssetsEditor"));
    dockAssetsEditor->setFont(font);
    dockAssetsEditor->setFeatures(QDockWidget::DockWidgetMovable);  // | QDockWidget::DockWidgetClosable);
    dockAssetsEditor->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
        widgetAssestEditor = new QWidget();
        widgetAssestEditor->setObjectName(QStringLiteral("widgetAssestsEditor"));
        widgetAssestEditor->setSizePolicy(sizePolicyPreferredVertical);
            QVBoxLayout *verticalLayoutAsset = new QVBoxLayout(widgetAssestEditor);
            verticalLayoutAsset->setObjectName(QStringLiteral("verticalLayoutAsset"));
            verticalLayoutAsset->setSpacing(0);
            verticalLayoutAsset->setContentsMargins(0, 0, 0, 0);

                // ***** Load our custom TreeObjectInspector for the Stage List
                treeAssetEditor = new TreeAssets(widgetAssestEditor, project, this);
                treeAssetEditor->setObjectName(QStringLiteral("treeAssetEditor"));
                treeAssetEditor->setColumnCount(1);
                treeAssetEditor->setFont(font);
                treeAssetEditor->setProperty("showDropIndicator", QVariant(false));
                treeAssetEditor->setDragEnabled(false);
                treeAssetEditor->setDragDropOverwriteMode(false);
                treeAssetEditor->setDragDropMode(QAbstractItemView::DragDropMode::NoDragDrop);
                treeAssetEditor->setDefaultDropAction(Qt::DropAction::TargetMoveAction);
                treeAssetEditor->setAlternatingRowColors(false);
                treeAssetEditor->setSelectionMode(QAbstractItemView::SelectionMode::NoSelection);
                treeAssetEditor->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectItems);
                treeAssetEditor->setIndentation(0);
                treeAssetEditor->setRootIsDecorated(false);
                treeAssetEditor->setItemsExpandable(true);
                treeAssetEditor->setExpandsOnDoubleClick(false);
                treeAssetEditor->setHeaderHidden(true);
                treeAssetEditor->setFrameShape(QFrame::NoFrame);
                treeAssetEditor->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
                treeAssetEditor->setUniformRowHeights(false);

            verticalLayoutAsset->insertWidget(0, treeAssetEditor);

        dockAssetsEditor->setWidget(widgetAssestEditor);

    // Starting width, if screen size is small, decrease starting width of assets to make more room
    if (this->geometry().width() < 1500)
        dockAssetsEditor->setFixedWidth( 124 );
    else
        dockAssetsEditor->setFixedWidth( 221 );
    addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, dockAssetsEditor);
    dockAssetsEditor->hide();

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















