//
//
//      Calls to set up, initialize, build, style, and color main form
//
//

#include "form_main.h"


//####################################################################################
//##        Apply palette / coloring / styling to children widgets
//####################################################################################
void FormMain::applyColoring()
{
    QString style_sheet = QString(
        " QMainWindow { background: " + globals->getColor(Window_Colors::Background_Light).name() + "; }" +
        " QMainWindow::separator { border: 1px solid " + globals->getColor(Window_Colors::Background_Light).name() + "; }"

        " QSplitter { width: 4px; } "
        " QSplitter::handle:vertical { image: url(:/tree_icons/splitter_v.png); } "
        " QSplitter::handle:horizontal { image: url(:/tree_icons/splitter_h.png); } "

        " QScrollBar:vertical { width: 12px; margin: 0px; border-radius: 6px; "
        "       background: " + globals->getColor(Window_Colors::Button_Light).name() + "; } "
        " QScrollBar::handle:vertical { margin: 2px; border-radius: 4px; "
        "       background: qlineargradient(spread:pad, x1:0 y1:0, x2:0 y2:1, "
        "                   stop:0 " + globals->getColor(Window_Colors::Icon_Dark).name() + ", "
        "                   stop:1 " + globals->getColor(Window_Colors::Background_Dark).name() + "); } "
        " QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0px; } "
        " QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical { height: 0px; } "

        " QScrollBar:horizontal { height: 12px; margin: 0px; border-radius: 6px; "
        "       background: " + globals->getColor(Window_Colors::Button_Light).name() + " ;} "
        " QScrollBar::handle:horizontal {      margin: 2px; border-radius: 4px; "
        "       background: qlineargradient(spread:pad, x1:0 y1:0, x2:1 y2:0, "
        "                   stop:0 " + globals->getColor(Window_Colors::Icon_Dark).name() + ", "
        "                   stop:1 " + globals->getColor(Window_Colors::Background_Dark).name() + "); } "
        " QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal { width: 0px; } "
        " QScrollBar::add-page:horizontal, QScrollBar::sub-page:horizontal { width: 0px; } "

        " QPushButton { color: " + globals->getColor(Window_Colors::Text).name() + "; "
        "       background: qlineargradient(spread:pad, x1:0 y1:0, x2:0 y2:1, "
        "                   stop:0 " + globals->getColor(Window_Colors::Button_Light).name() + ", "
        "                   stop:1 " + globals->getColor(Window_Colors::Button_Dark).name() + "); "
        "       border: none; border-radius: 6px; }"
        " QPushButton:hover:!pressed { color: " + globals->getColor(Window_Colors::Highlight).name() + "; "
        "       background: " + globals->getColor(Window_Colors::Button_Light).name() + "; }"
        " QPushButton:pressed { color: " + globals->getColor(Window_Colors::Highlight).name() + "; "
        "       background: " + globals->getColor(Window_Colors::Shadow_Light).name() + "; }"

        " QTreeWidget { icon-size: 14px 14px; }"
        " QTreeWidget { color: " + globals->getColor(Window_Colors::Text).name() + ";  "
        "       background: " + globals->getColor(Window_Colors::Background_Dark).name() + "; "
        "       selection-background-color: " + globals->getColor(Window_Colors::Midlight).name() + "; }"
        " QTreeWidget::item:selected { color: " + globals->getColor(Window_Colors::Icon_Dark).name() + "; "
        "       background: " + globals->getColor(Window_Colors::Midlight).name() + "; }"
        " QTreeWidget::item:hover:selected { color: " + globals->getColor(Window_Colors::Icon_Light).name() + "; "
        "       background: " + globals->getColor(Window_Colors::Midlight).name() + "; }"
        " QTreeWidget::item:hover:!selected { color: " + globals->getColor(Window_Colors::Highlight).name() + "; "
        "       background: " + globals->getColor(Window_Colors::Background_Dark).name() + "; }"

        " QHeaderView::section { "
        "       background-color: " + globals->getColor(Window_Colors::Background_Dark).name() + "; "
        "       border: 0px; }"

        " QDockWidget { font-size: 11px; color: " + globals->getColor(Window_Colors::Text).name() + "; } "
        " QDockWidget::title { text-align: center; "
        "       background: qlineargradient(x1:0 y1:0, x2:0 y2:1, "
        "                   stop:0 " + globals->getColor(Window_Colors::Icon_Light).name() + ", "
        "                   stop:1 " + globals->getColor(Window_Colors::Background_Dark).name() + "); } "

        " QGraphicsView { background: " + globals->getColor(Window_Colors::Background_Light).name() + "; }"
        " QGraphicsView::corner { background: transparent; } "

        " QScrollArea { background: " + globals->getColor(Window_Colors::Background_Dark).name() + "; }"

        " QLabel { color : " + globals->getColor(Window_Colors::Text).name() + "; } "
    );

    this->setStyleSheet(style_sheet);

    applyDropShadow(buttonAtlas,    6, 0, 3, globals->getColor(Window_Colors::Shadow_Dark));
    applyDropShadow(buttonFonts,    6, 0, 3, globals->getColor(Window_Colors::Shadow_Dark));
    applyDropShadow(buttonPlay,     6, 0, 3, globals->getColor(Window_Colors::Shadow_Dark));
    applyDropShadow(buttonSettings, 6, 0, 3, globals->getColor(Window_Colors::Shadow_Dark));
    applyDropShadow(buttonWorlds,   6, 0, 3, globals->getColor(Window_Colors::Shadow_Dark));
}

void FormMain::applyDropShadow(QWidget *target_widget, qreal blur_radius, qreal offset_x, qreal offset_y, QColor shadow_color)
{
    QGraphicsDropShadowEffect *shadow_effect;
    shadow_effect = new QGraphicsDropShadowEffect();
    shadow_effect->setBlurRadius(blur_radius);
    shadow_effect->setOffset(offset_x, offset_y);
    shadow_effect->setColor(shadow_color);
    target_widget->setGraphicsEffect(shadow_effect);
}




//####################################################################################
//##        Setting up of form main
//####################################################################################
void FormMain::buildWindow()
{
    QFont font, fontLarger;
    font.setPointSize(11);
    fontLarger.setPointSize(13);

    // Other size policies to play with
    //QSizePolicy sizePolicyNoChange(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Minimum);
    //QSizePolicy sizePolicyNoChange(QSizePolicy::Policy::MinimumExpanding, QSizePolicy::Policy::MinimumExpanding);
    //QSizePolicy sizePolicyNoChange(QSizePolicy::Policy::Ignored, QSizePolicy::Policy::Ignored);
    //QSizePolicy sizePolicyNoChange(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);

    QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);

    QSizePolicy sizePolicyPreferredHorizontal(QSizePolicy::Preferred, QSizePolicy::Preferred);
    sizePolicyPreferredHorizontal.setHorizontalStretch(1);
    sizePolicyPreferredHorizontal.setVerticalStretch(0);

    QSizePolicy sizePolicyPreferredVertical(QSizePolicy::Preferred, QSizePolicy::Preferred);
    sizePolicyPreferredHorizontal.setHorizontalStretch(0);
    sizePolicyPreferredHorizontal.setVerticalStretch(1);


    // ***** Main window settings
    this->setObjectName(QStringLiteral("formMain"));
    this->setWindowModality(Qt::NonModal);
    this->resize(1300, 800);
    this->setMinimumSize(QSize(780, 400));
    this->setFont(font);
    this->setMouseTracking(true);
    this->setAcceptDrops(true);
    this->setWindowIcon(QIcon(":icon/icon256.png"));                        // Set icon


    // ***** Build left Assets Dock
    assets = new QDockWidget(this);
    assets->setObjectName(QStringLiteral("assets"));
    assets->setMinimumSize(QSize(180, 35));
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

            // ***** Load our custom TreeObjectInspector for the Scene List
            treeAsset = new TreeAssetList(widgetAssests, this);
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
        verticalLayoutAsset->addWidget(treeAsset);

        assets->setWidget(widgetAssests);
    addDockWidget(static_cast<Qt::DockWidgetArea>(1), assets);


    // ***** Build right Advisor Dock
    advisor = new QDockWidget(this);
    advisor->setObjectName(QStringLiteral("advisor"));
    advisor->setMinimumSize(QSize(300, 80));
    advisor->setSizePolicy(sizePolicy);
    advisor->setFont(font);
    advisor->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetClosable);
    advisor->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
        widgetAdvisor = new QWidget();
        widgetAdvisor->setObjectName(QStringLiteral("widgetAdvisor"));
        widgetAdvisor->setSizePolicy(sizePolicy);
        widgetAdvisor->setMaximumHeight(140);
        verticalLayoutAdvisor = new QVBoxLayout(widgetAdvisor);
        verticalLayoutAdvisor->setObjectName(QStringLiteral("verticalLayoutAdvisor"));
        verticalLayoutAdvisor->setSpacing(2);
        verticalLayoutAdvisor->setContentsMargins(1, 1, 1, 1);
            treeAdvisor = new TreeAdvisorList(widgetInspector, this);
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
        verticalLayoutAdvisor->addWidget(treeAdvisor);
        advisor->setWidget(widgetAdvisor);

    // ***** Build right Inspector Dock
    inspector = new QDockWidget(this);
    inspector->setObjectName(QStringLiteral("inspector"));
    inspector->setSizePolicy(sizePolicyPreferredVertical);
    inspector->setMinimumSize(QSize(300, 250));
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

            // ***** Load our custom TreeObjectInspector for the Scene List
            treeObject = new TreeObjectInspector(widgetInspector, this);
            treeObject->setObjectName(QStringLiteral("treeObject"));
            treeObject->setColumnCount(1);
            treeObject->setFont(font);
            treeObject->setProperty("showDropIndicator", QVariant(false));
            treeObject->setDragEnabled(false);
            treeObject->setDragDropOverwriteMode(false);
            treeObject->setDragDropMode(QAbstractItemView::DragDropMode::NoDragDrop);
            treeObject->setDefaultDropAction(Qt::DropAction::TargetMoveAction);
            treeObject->setAlternatingRowColors(false);
            treeObject->setSelectionMode(QAbstractItemView::SelectionMode::NoSelection);
            treeObject->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectItems);
            treeObject->setIndentation(0);
            treeObject->setRootIsDecorated(false);
            treeObject->setItemsExpandable(true);
            treeObject->setExpandsOnDoubleClick(false);
            treeObject->setHeaderHidden(true);
        verticalLayoutObject->addWidget(treeObject);

        inspector->setWidget(widgetInspector);

    addDockWidget(static_cast<Qt::DockWidgetArea>(2), inspector);
    addDockWidget(static_cast<Qt::DockWidgetArea>(2), advisor);


    // ***** Can force adjust size of docks with QMainWindow::resizeDocks call
    //resizeDocks({advisor, inspector}, {100 , 300}, Qt::Vertical);


    // ***** Build top Toolbar Dock
    toolbar = new QDockWidget(this);
    toolbar->setObjectName(QStringLiteral("toolbar"));
    toolbar->setMinimumSize(QSize(449, 45));
    toolbar->setMaximumSize(QSize(524287, 45));
    toolbar->setFeatures(QDockWidget::NoDockWidgetFeatures);
    toolbar->setAllowedAreas(Qt::TopDockWidgetArea);
    toolbar->setWindowTitle(QStringLiteral(""));
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
            buttonWorlds->setCheckable(false);
        toolbar->setWidget(widgetToolbar);
    addDockWidget(static_cast<Qt::DockWidgetArea>(4), toolbar);


    // ***** Build central widgets
    widgetCentral = new QWidget(this);
    widgetCentral->setObjectName(QStringLiteral("widgetCentral"));
    widgetCentral->setSizePolicy(sizePolicyPreferredHorizontal);
    verticalLayout = new QVBoxLayout(widgetCentral);
    verticalLayout->setSpacing(2);
    verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
    verticalLayout->setContentsMargins(2, 2, 2, 2);
        splitterVertical = new QSplitter(widgetCentral);
        splitterVertical->setObjectName(QStringLiteral("splitterVertical"));
        splitterVertical->setOrientation(Qt::Vertical);
        splitterVertical->setHandleWidth(4);

            widgetInner = new QWidget(splitterVertical);
            widgetInner->setObjectName(QStringLiteral("widgetInner"));
            horizontalLayout = new QHBoxLayout(widgetInner);
            horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
            horizontalLayout->setSpacing(0);
            horizontalLayout->setContentsMargins(0, 0, 0, 0);
                splitterHorizontal = new QSplitter(widgetInner);
                splitterHorizontal->setObjectName(QStringLiteral("splitterHorizontal"));
                splitterHorizontal->setLineWidth(0);
                splitterHorizontal->setOrientation(Qt::Horizontal);
                splitterHorizontal->setHandleWidth(4);

                    // ***** Load our custom TreeSceneView for the Scene List
                    treeScene = new TreeSceneView(splitterHorizontal, this);
                    treeScene->setStyle(new SceneTreeHighlightProxy(treeScene->style(), treeScene));
                        QTreeWidgetItem *header_item_scene = new QTreeWidgetItem();
                        header_item_scene->setIcon(1, QIcon(":/tree_icons/tree_lock_header.png"));
                        treeScene->setHeaderItem(header_item_scene);
                    treeScene->setObjectName(QStringLiteral("treeScene"));
                    treeScene->setColumnCount(2);
                    treeScene->setColumnWidth(0, 150);
                    treeScene->setColumnWidth(1, 16);
                    treeScene->setMinimumSize(QSize(190, 0));
                    treeScene->setFont(font);
                    treeScene->setProperty("showDropIndicator", QVariant(false));
                    treeScene->setDragEnabled(true);
                    treeScene->setDragDropOverwriteMode(false);
                    treeScene->setDragDropMode(QAbstractItemView::DragDropMode::InternalMove);
                    treeScene->setDefaultDropAction(Qt::DropAction::TargetMoveAction);
                    treeScene->setAlternatingRowColors(false);
                    treeScene->setSelectionMode(QAbstractItemView::SelectionMode::ExtendedSelection);
                    treeScene->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
                    treeScene->setIndentation(15);
                    treeScene->setRootIsDecorated(true);
                    treeScene->setItemsExpandable(true);
                    treeScene->setExpandsOnDoubleClick(false);
                    treeScene->header()->setSectionResizeMode(0, QHeaderView::ResizeMode::Stretch);
                    treeScene->header()->setStretchLastSection(false);
                    treeScene->header()->setVisible(true);
                splitterHorizontal->addWidget(treeScene);


                    // ***** Load our Graphics View
                    viewMain = new SceneGraphicsView(splitterHorizontal, this);
                    viewMain->setObjectName(QStringLiteral("viewMain"));
                    viewMain->setRenderHint(QPainter::Antialiasing, false);
                    viewMain->setDragMode(QGraphicsView::DragMode::RubberBandDrag);
                    viewMain->setOptimizationFlags(QGraphicsView::DontSavePainterState);
                    viewMain->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
                    viewMain->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
                    viewMain->setScene(scene);
                        QSizePolicy sizePolicyView(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);
                        sizePolicyView.setHorizontalStretch(1);
                        sizePolicyView.setVerticalStretch(0);
                    viewMain->setSizePolicy(sizePolicyView);
                    viewMain->setMinimumSize(QSize(100, 0));
                    viewMain->setFont(font);
                    //viewMain->setBackgroundBrush(QBrush(globals->getColor(Window_Colors::Background_Light)));
                    viewMain->setCacheMode(QGraphicsView::CacheBackground);
                splitterHorizontal->addWidget(viewMain);


            horizontalLayout->addWidget(splitterHorizontal);
        splitterVertical->addWidget(widgetInner);

            areaBottom = new QScrollArea(splitterVertical);
            areaBottom->setObjectName(QStringLiteral("areaBottom"));
            areaBottom->setSizePolicy(sizePolicy);
            areaBottom->setMinimumSize(QSize(0, 100));
            areaBottom->setFont(font);
            areaBottom->setWidgetResizable(true);
                label_1 = new QLabel(areaBottom);
                label_1->setObjectName(QStringLiteral("label_1"));
                label_1->setGeometry(QRect(10, 10, 141, 21));
                label_1->setFont(font);
                label_2 = new QLabel(areaBottom);
                label_2->setObjectName(QStringLiteral("label_2"));
                label_2->setGeometry(QRect(10, 30, 141, 21));
                label_2->setFont(font);
                label_3 = new QLabel(areaBottom);
                label_3->setObjectName(QStringLiteral("label_3"));
                label_3->setGeometry(QRect(10, 70, 631, 21));
                label_3->setFont(font);
                label_object_1 = new QLabel(areaBottom);
                label_object_1->setObjectName(QStringLiteral("label_object"));
                label_object_1->setGeometry(QRect(180, 10, 461, 21));
                label_object_1->setFont(font);
                label_object_2 = new QLabel(areaBottom);
                label_object_2->setObjectName(QStringLiteral("label_object_2"));
                label_object_2->setGeometry(QRect(180, 30, 461, 21));
                label_object_2->setFont(font);
                label_object_3 = new QLabel(areaBottom);
                label_object_3->setObjectName(QStringLiteral("label_object_3"));
                label_object_3->setGeometry(QRect(180, 50, 461, 21));
                label_object_3->setFont(font);
        splitterVertical->addWidget(areaBottom);

    verticalLayout->addWidget(splitterVertical);
    this->setCentralWidget(widgetCentral);


    // ***** Build Menu Bar
    menuBar = new QMenuBar(this);
    menuBar->setObjectName(QStringLiteral("menuBar"));
    menuBar->setGeometry(QRect(0, 0, 1100, 22));
        // ***** Color Schemes sub menu
        QMenu *menuColor_Schemes;
        QAction *actionDark, *actionLight, *actionBlue, *actionAutumn;
        actionDark =   new QAction(this);   actionDark->setObjectName(QStringLiteral("actionDark"));
        actionLight =  new QAction(this);   actionLight->setObjectName(QStringLiteral("actionLight"));
        actionBlue =   new QAction(this);   actionBlue->setObjectName(QStringLiteral("actionBlue"));
        actionAutumn = new QAction(this);   actionAutumn->setObjectName(QStringLiteral("actionAutumn"));
            QActionGroup *alignmentGroup;
            alignmentGroup = new QActionGroup(this);
            alignmentGroup->addAction(actionDark);
            alignmentGroup->addAction(actionLight);
            alignmentGroup->addAction(actionBlue);
            alignmentGroup->addAction(actionAutumn);
            alignmentGroup->setExclusive(true);
            actionDark->setCheckable(true);
            actionLight->setCheckable(true);
            actionBlue->setCheckable(true);
            actionAutumn->setCheckable(true);
            switch (globals->current_color_scheme)
            {
            case Color_Scheme::Dark:    actionDark->setChecked(true);    break;
            case Color_Scheme::Light:   actionLight->setChecked(true);   break;
            case Color_Scheme::Blue:    actionBlue->setChecked(true);    break;
            case Color_Scheme::Autumn:  actionAutumn->setChecked(true);  break;
            }
        // Instead of traditional SIGNAL to SLOT connect, we can "connect" inline lamda functions directly
        //      to signals. This allows for passing of variables not included in the SIGNAL that was emitted.
        // Such as in this instance, passing a new Color_Scheme to FormMain::changePalette)
        connect(actionDark,   &QAction::triggered, [this]() { changePalette(Color_Scheme::Dark); });
        connect(actionLight,  &QAction::triggered, [this]() { changePalette(Color_Scheme::Light); });
        connect(actionBlue,   &QAction::triggered, [this]() { changePalette(Color_Scheme::Blue); });
        connect(actionAutumn, &QAction::triggered, [this]() { changePalette(Color_Scheme::Autumn); });

        menuColor_Schemes = new QMenu(menuBar);
        menuColor_Schemes->setObjectName(QStringLiteral("menuColor_Schemes"));
        menuBar->addAction(menuColor_Schemes->menuAction());
        menuColor_Schemes->addAction(actionDark);
        menuColor_Schemes->addAction(actionLight);
        menuColor_Schemes->addAction(actionBlue);
        menuColor_Schemes->addAction(actionAutumn);
    this->setMenuBar(menuBar);


    // ***** Set menu titles and sub menu texts
    menuColor_Schemes->setTitle(QApplication::translate("MainWindow", "Color Schemes", nullptr));
    actionDark->setText(QApplication::translate("MainWindow", "Dark", nullptr));
    actionLight->setText(QApplication::translate("MainWindow", "Light", nullptr));
    actionBlue->setText(QApplication::translate("MainWindow", "Blue", nullptr));
    actionAutumn->setText(QApplication::translate("MainWindow", "Autumn", nullptr));


    // ***** Set titles and button texts
    setWindowTitle(QApplication::translate("MainWindow", "Drop", nullptr));
    advisor->setWindowTitle(QApplication::translate("MainWindow", "Advisor", nullptr));
    assets->setWindowTitle(QApplication::translate("MainWindow", "Assets", nullptr));
    inspector->setWindowTitle(QApplication::translate("MainWindow", "Inspector", nullptr));
    label_1->setText(QApplication::translate("MainWindow", "TextLabel", nullptr));
    label_2->setText(QApplication::translate("MainWindow", "TextLabel", nullptr));
    label_3->setText(QApplication::translate("MainWindow", "TextLabel", nullptr));
    label_object_1->setText(QApplication::translate("MainWindow", "Object ID, Type", nullptr));
    label_object_2->setText(QApplication::translate("MainWindow", "Object ID, Type", nullptr));
    label_object_3->setText(QApplication::translate("MainWindow", "Object ID, Type", nullptr));
    buttonAtlas->setText(QApplication::translate("MainWindow", "Atlases", nullptr));
    buttonFonts->setText(QApplication::translate("MainWindow", "Fonts", nullptr));
    buttonPlay->setText(QApplication::translate("MainWindow", "Play", nullptr));
    buttonSettings->setText(QApplication::translate("MainWindow", "App Settings", nullptr));
    buttonWorlds->setText(QApplication::translate("MainWindow", "Worlds / UI", nullptr));


    // ***** Small tweaks to some of the widgets
    toolbar->setTitleBarWidget(new QWidget());                    // Removes title bar from QDockWidget Toolbar

    splitterHorizontal->setSizes(QList<int> { 150, 300 });        // Sets tree_scene (scene assests) startup width to 150
                                                                  // NOTE: You can save and restore the sizes of the widgets from a QByteArray
                                                                  //       using QSplitter.saveState() and QSplitter.restoreState() respectively
}










