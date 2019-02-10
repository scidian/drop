//
//      Created by Stephens Nunnally on 12/10/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Calls to set up, initialize, build Form Main
//
//

#include "library.h"

#include "editor_scene.h"
#include "editor_tree_advisor.h"
#include "editor_tree_assets.h"
#include "editor_tree_inspector.h"
#include "editor_tree_stage.h"
#include "editor_view.h"

#include "form_main.h"

#include "project_world_stage.h"

//####################################################################################
//##        Setting up of form main
//####################################################################################
// Re-configures FormMain to new mode
void FormMain::buildWindow(Form_Main_Mode new_layout)
{
    QString widget_list;

    current_mode = new_layout;
    switch (current_mode)
    {
    case Form_Main_Mode::Edit_Stage:
        buildWindowModeEditStage();
        buildAssetList();
        buildTreeStageList();
        viewMain->setFocus(Qt::FocusReason::ActiveWindowFocusReason);
        scene->setSceneRect(-2000, -2000, 4000, 4000);
        scene->update();
        viewMain->update();
        centerViewOn(QPointF(0, 0));
        break;
    case Form_Main_Mode::Clear:  

        disconnectSignals();
        for (auto item : scene->selectedItems())
            item->setSelected(false);

        this->takeCentralWidget()->deleteLater();
        for (auto dock : findChildren<QDockWidget *>()) { dock->deleteLater(); }

        break;
    default:
        Dr::ShowMessageBox("Not set");
    }
}

void FormMain::buildWindowModeEditStage()
{
    QFont font, fontLarger;
    font.setPointSize(Dr::FontSize());
    fontLarger.setPointSize(Dr::FontSize() + 2);

    /// Other size policies to play with
    ///QSizePolicy sizePolicyNoChange(QSizePolicy::Policy::MinimumExpanding, QSizePolicy::Policy::MinimumExpanding);
    ///QSizePolicy sizePolicyNoChange(QSizePolicy::Policy::Ignored, QSizePolicy::Policy::Ignored);
    ///QSizePolicy sizePolicyNoChange(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);

    QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);

    QSizePolicy sizePolicyLess(QSizePolicy::Preferred, QSizePolicy::Preferred);
    sizePolicy.setHorizontalStretch(1);
    sizePolicy.setVerticalStretch(1);

    QSizePolicy sizePolicyPreferredHorizontal(QSizePolicy::Preferred, QSizePolicy::Preferred);
    sizePolicyPreferredHorizontal.setHorizontalStretch(1);
    sizePolicyPreferredHorizontal.setVerticalStretch(0);

    QSizePolicy sizePolicyPreferredVertical(QSizePolicy::Preferred, QSizePolicy::Preferred);
    sizePolicyPreferredHorizontal.setHorizontalStretch(0);
    sizePolicyPreferredHorizontal.setVerticalStretch(1);

    QSizePolicy sizePolicyMinimum(QSizePolicy::Policy::MinimumExpanding, QSizePolicy::Policy::MinimumExpanding);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);

    QSizePolicy sizePolicyView(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);
    sizePolicyView.setHorizontalStretch(1);
    sizePolicyView.setVerticalStretch(0);


    // ***** Build central widgets
    widgetCentral = new QWidget(this);
    widgetCentral->setObjectName(QStringLiteral("widgetCentral"));
    widgetCentral->setSizePolicy(sizePolicyPreferredHorizontal);
        verticalLayout = new QVBoxLayout(widgetCentral);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(4, 0, 4, 0);

        splitterVertical = new ColorSplitter(widgetCentral);
        splitterVertical->setObjectName(QStringLiteral("splitterVertical"));
        splitterVertical->setOrientation(Qt::Vertical);
        splitterVertical->setHandleWidth(4);

            widgetStage = new QWidget(splitterVertical);
            widgetStage->setObjectName(QStringLiteral("widgetStage"));
                horizontalLayout = new QHBoxLayout(widgetStage);
                horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
                horizontalLayout->setSpacing(0);
                horizontalLayout->setContentsMargins(0, 0, 0, 0);

                splitterHorizontal = new ColorSplitter(widgetStage);
                splitterHorizontal->setObjectName(QStringLiteral("splitterHorizontal"));
                splitterHorizontal->setLineWidth(0);
                splitterHorizontal->setOrientation(Qt::Horizontal);
                splitterHorizontal->setHandleWidth(4);

                    // ***** Load our custom TreeStageView for the Scene List
                    treeStage = new TreeStage(splitterHorizontal, project, this);
                    treeStage->setStyle(new StageTreeHighlightProxy(treeStage->style(), treeStage, this));
                        QTreeWidgetItem *header_item_stage = new QTreeWidgetItem();
                        header_item_stage->setIcon(1, QIcon(":/tree_icons/tree_lock_header.png"));
                        treeStage->setHeaderItem(header_item_stage);
                    treeStage->setObjectName(QStringLiteral("treeStage"));
                    treeStage->setColumnCount(2);
                    treeStage->setColumnWidth(0, 150);
                    treeStage->setColumnWidth(1, 16);
                    treeStage->setMinimumSize(QSize(190, 0));
                    treeStage->setMaximumWidth(400);
                    treeStage->setFont(font);
                    treeStage->setProperty("showDropIndicator", QVariant(false));
                    treeStage->setDragEnabled(true);
                    treeStage->setDragDropOverwriteMode(false);
                    treeStage->setDragDropMode(QAbstractItemView::DragDropMode::InternalMove);
                    treeStage->setDefaultDropAction(Qt::DropAction::TargetMoveAction);
                    treeStage->setAlternatingRowColors(false);
                    treeStage->setSelectionMode(QAbstractItemView::SelectionMode::ExtendedSelection);
                    treeStage->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
                    treeStage->setIndentation(15);
                    treeStage->setRootIsDecorated(true);
                    treeStage->setItemsExpandable(true);
                    treeStage->setExpandsOnDoubleClick(false);
                    treeStage->header()->setSectionResizeMode(0, QHeaderView::ResizeMode::Stretch);
                    treeStage->header()->setStretchLastSection(false);
                    treeStage->header()->setVisible(true);
                    treeStage->setFrameShape(QFrame::NoFrame);

                splitterHorizontal->addWidget(treeStage);


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

                        if (!Dr::CheckDebugFlag(Debug_Flags::Turn_Off_Antialiasing))
                            viewMain->setRenderHint(QPainter::Antialiasing, false);
                        else
                            viewMain->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::HighQualityAntialiasing);

                        viewMain->setDragMode(QGraphicsView::DragMode::NoDrag);
                        viewMain->setOptimizationFlags(QGraphicsView::OptimizationFlag::DontSavePainterState);
                        viewMain->setTransformationAnchor(QGraphicsView::ViewportAnchor::AnchorUnderMouse);

                        /// This setting means we will decide when to call update(), controls recurssive paint events
                        ///viewMain->setViewportUpdateMode(QGraphicsView::ViewportUpdateMode::NoViewportUpdate);
                        viewMain->setViewportUpdateMode(QGraphicsView::ViewportUpdateMode::SmartViewportUpdate);
                        viewMain->setFrameShape(QFrame::NoFrame);


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
                label_1 = new QLabel(areaBottom);
                label_1->setObjectName(QStringLiteral("label_1"));
                label_1->setGeometry(QRect(10, 5, 220, 21));
                label_1->setFont(font);
                label_2 = new QLabel(areaBottom);
                label_2->setObjectName(QStringLiteral("label_2"));
                label_2->setGeometry(QRect(10, 20, 220, 21));
                label_2->setFont(font);
                label_3 = new QLabel(areaBottom);
                label_3->setObjectName(QStringLiteral("label_2"));
                label_3->setGeometry(QRect(10, 35, 220, 21));
                label_3->setFont(font);
                label_mouse_1 = new QLabel(areaBottom);
                label_mouse_1->setObjectName(QStringLiteral("label_mouse_1"));
                label_mouse_1->setGeometry(QRect(10, 50, 220, 21));
                label_mouse_1->setFont(font);
                label_mouse_2 = new QLabel(areaBottom);
                label_mouse_2->setObjectName(QStringLiteral("label_mouse_2"));
                label_mouse_2->setGeometry(QRect(10, 65, 220, 21));
                label_mouse_2->setFont(font);

                label_object_1 = new QLabel(areaBottom);
                label_object_1->setObjectName(QStringLiteral("label_object"));
                label_object_1->setGeometry(QRect(240, 5, 400, 21));
                label_object_1->setFont(font);
                label_object_2 = new QLabel(areaBottom);
                label_object_2->setObjectName(QStringLiteral("label_object_2"));
                label_object_2->setGeometry(QRect(240, 20, 400, 21));
                label_object_2->setFont(font);
                label_object_3 = new QLabel(areaBottom);
                label_object_3->setObjectName(QStringLiteral("label_object_3"));
                label_object_3->setGeometry(QRect(240, 35, 400, 21));
                label_object_3->setFont(font);
                label_object_4 = new QLabel(areaBottom);
                label_object_4->setObjectName(QStringLiteral("label_object_4"));
                label_object_4->setGeometry(QRect(240, 50, 400, 21));
                label_object_4->setFont(font);
                label_object_5 = new QLabel(areaBottom);
                label_object_5->setObjectName(QStringLiteral("label_object_4"));
                label_object_5->setGeometry(QRect(240, 65, 400, 21));
                label_object_5->setFont(font);

                label_position = new QLabel(areaBottom);
                label_position->setObjectName(QStringLiteral("label_position"));
                label_position->setGeometry(QRect(560, 5, 400, 21));
                label_position->setFont(font);
                label_center = new QLabel(areaBottom);
                label_center->setObjectName(QStringLiteral("label_center"));
                label_center->setGeometry(QRect(560, 20, 400, 21));
                label_center->setFont(font);
                label_scale = new QLabel(areaBottom);
                label_scale->setObjectName(QStringLiteral("label_scale"));
                label_scale->setGeometry(QRect(560, 35, 400, 21));
                label_scale->setFont(font);
                label_rotate = new QLabel(areaBottom);
                label_rotate->setObjectName(QStringLiteral("label_rotate"));
                label_rotate->setGeometry(QRect(560, 50, 400, 21));
                label_rotate->setFont(font);
                label_z_order = new QLabel(areaBottom);
                label_z_order->setObjectName(QStringLiteral("label_z_order"));
                label_z_order->setGeometry(QRect(560, 65, 400, 21));
                label_z_order->setFont(font);
                label_pos_flag = new QLabel(areaBottom);
                label_pos_flag->setObjectName(QStringLiteral("label_pos_flag"));
                label_pos_flag->setGeometry(QRect(560, 80, 400, 21));
                label_pos_flag->setFont(font);

                label_bottom = new QLabel(areaBottom);
                label_bottom->setObjectName(QStringLiteral("label_bottom"));
                label_bottom->setGeometry(QRect(10, 80, 700, 21));
                label_bottom->setFont(font);
        splitterVertical->addWidget(areaBottom);

        splitterVertical->setStretchFactor(0, 1);           // widgetStage (index 0) should stretch (1)
        splitterVertical->setStretchFactor(1, 0);           // areaBottom  (index 1) should not stretch (0)

    verticalLayout->addWidget(splitterVertical);
    this->setCentralWidget(widgetCentral);



    // ***** Build left Assets Dock
    assets = new QDockWidget(this);
    assets->setObjectName(QStringLiteral("assets"));
    assets->setMinimumSize(QSize(160, 35));
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
                treeAsset = new TreeAssetList(widgetAssests, project, this);
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


    // ***** Build top Toolbar Dock
    toolbar = new QDockWidget(this);
    toolbar->setObjectName(QStringLiteral("toolbar"));
    toolbar->setMinimumSize(QSize(449, 50));
    toolbar->setMaximumSize(QSize(524287, 50));
    toolbar->setFeatures(QDockWidget::NoDockWidgetFeatures);
    toolbar->setAllowedAreas(Qt::TopDockWidgetArea);
    toolbar->setWindowTitle(QStringLiteral(""));
    toolbar->setContentsMargins(0, 0, 0, 4);
        widgetToolbar = new QWidget();
        widgetToolbar->setObjectName(QStringLiteral("widgetToolbar"));
            buttonAtlas = new QPushButton(widgetToolbar);
            buttonAtlas->setObjectName(QStringLiteral("buttonAtlas"));
            buttonAtlas->setGeometry(QRect(250, 8, 111, 31));
            buttonAtlas->setFont(font);
            buttonFonts = new QPushButton(widgetToolbar);
            buttonFonts->setObjectName(QStringLiteral("buttonFonts"));
            buttonFonts->setGeometry(QRect(370, 8, 111, 31));
            buttonFonts->setFont(font);
            buttonPlay = new QPushButton(widgetToolbar);
            buttonPlay->setObjectName(QStringLiteral("buttonPlay"));
            buttonPlay->setGeometry(QRect(660, 8, 111, 31));
            buttonPlay->setFont(font);
            buttonSettings = new QPushButton(widgetToolbar);
            buttonSettings->setObjectName(QStringLiteral("buttonSettings"));
            buttonSettings->setGeometry(QRect(780, 8, 111, 31));
            buttonSettings->setFont(font);
            buttonWorlds = new QPushButton(widgetToolbar);
            buttonWorlds->setObjectName(QStringLiteral("buttonWorlds"));
            buttonWorlds->setGeometry(QRect(20, 8, 121, 31));
            buttonWorlds->setFont(font);
        toolbar->setWidget(widgetToolbar);
        toolbar->setTitleBarWidget(new QWidget());                                      // Removes title bar from QDockWidget Toolbar


    // ***** Add QMainWindow Docks
    addDockWidget(static_cast<Qt::DockWidgetArea>(1), assets);
    addDockWidget(static_cast<Qt::DockWidgetArea>(2), inspector);
    addDockWidget(static_cast<Qt::DockWidgetArea>(2), advisor);
    addDockWidget(static_cast<Qt::DockWidgetArea>(4), toolbar);

    // Forces resize of docks
    resizeDocks( { assets, inspector  }, { 160, 300 }, Qt::Horizontal);
    resizeDocks( { advisor, inspector }, { 140, 900 }, Qt::Vertical);



    // ***** Signals emitted by FormMain
    connectSignals();



    // ***** Apply shadow effects to buttons
    Dr::ApplyDropShadowByType(buttonAtlas,    Shadow_Types::Button_Shadow);
    Dr::ApplyDropShadowByType(buttonFonts,    Shadow_Types::Button_Shadow);
    Dr::ApplyDropShadowByType(buttonPlay,     Shadow_Types::Button_Shadow);
    Dr::ApplyDropShadowByType(buttonSettings, Shadow_Types::Button_Shadow);
    Dr::ApplyDropShadowByType(buttonWorlds,   Shadow_Types::Button_Shadow);


    // ***** Set titles and button texts
    setWindowTitle(QApplication::translate("MainWindow", "Drop", nullptr));
    advisor->setWindowTitle(QApplication::translate("MainWindow", "Advisor", nullptr));
    assets->setWindowTitle(QApplication::translate("MainWindow", "Assets", nullptr));
    inspector->setWindowTitle(QApplication::translate("MainWindow", "Inspector", nullptr));
    label_object_1->setText(QApplication::translate("MainWindow", "Object ID, Type", nullptr));
    label_object_2->setText(QApplication::translate("MainWindow", "Object ID, Type", nullptr));
    label_object_3->setText(QApplication::translate("MainWindow", "Object ID, Type", nullptr));
    buttonAtlas->setText(QApplication::translate("MainWindow", "Atlases", nullptr));
    buttonFonts->setText(QApplication::translate("MainWindow", "Fonts", nullptr));
    buttonPlay->setText(QApplication::translate("MainWindow", "Play", nullptr));
    buttonSettings->setText(QApplication::translate("MainWindow", "App Settings", nullptr));
    buttonWorlds->setText(QApplication::translate("MainWindow", "Worlds / UI", nullptr));

}



//####################################################################################
//##        Connect / disconnect signals emitted by FormMain
//####################################################################################
void FormMain::connectSignals()
{
    connect(this, SIGNAL(sendAdvisorInfo(QString, QString)), treeAdvisor, SLOT(changeAdvisor(QString, QString)) , Qt::QueuedConnection);

    // Connects signal used to populate scene
    connect(this, SIGNAL(newStageSelected(DrProject*, DrScene*, long, long)),
            scene,  SLOT(newStageSelected(DrProject*, DrScene*, long, long)) );
}

// Disconnect signals emitted by FormMain
void FormMain::disconnectSignals()
{
    disconnect(this, SIGNAL(sendAdvisorInfo(QString, QString)), treeAdvisor, SLOT(changeAdvisor(QString, QString)) );

    // Connects signal used to populate scene
    disconnect(this, SIGNAL(newStageSelected(DrProject*, DrScene*, long, long)),
               scene,  SLOT(newStageSelected(DrProject*, DrScene*, long, long)) );
}

















