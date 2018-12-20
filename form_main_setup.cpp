//
//
//      Calls to set up, initialize, build, style, and color main form
//
//

#include "form_main.h"


//####################################################################################
//##        Apply palette to main window
//####################################################################################
void FormMain::applyPalette(Color_Scheme new_scheme)
{
    QPalette palette = this->palette();
    Color_Scheme nc = new_scheme;
    palette.setColor(QPalette::ColorRole::Window, globals->color_schemes[nc][QPalette::ColorRole::Window]);                      // Background color
    palette.setColor(QPalette::ColorRole::Background, globals->color_schemes[nc][QPalette::ColorRole::Background]);              // Background color, obsolete?
    palette.setColor(QPalette::ColorRole::WindowText, globals->color_schemes[nc][QPalette::ColorRole::WindowText]);              // Header and label font color
    palette.setColor(QPalette::ColorRole::Foreground, globals->color_schemes[nc][QPalette::ColorRole::Foreground]);              // Header and label font color, obsolete?
    palette.setColor(QPalette::ColorRole::Base, globals->color_schemes[nc][QPalette::ColorRole::Base]);                          // Alternate background color
    palette.setColor(QPalette::ColorRole::AlternateBase, globals->color_schemes[nc][QPalette::ColorRole::AlternateBase]);        // Alternate background color in QTable
    palette.setColor(QPalette::ColorRole::Text, globals->color_schemes[nc][QPalette::ColorRole::Text]);                          // Button Text color
    palette.setColor(QPalette::ColorRole::Button, globals->color_schemes[nc][QPalette::ColorRole::Button]);                      // Header background gradient top color
    palette.setColor(QPalette::ColorRole::Dark, globals->color_schemes[nc][QPalette::ColorRole::Dark]);                          // Header background gradient bottom color
    palette.setColor(QPalette::ColorRole::Highlight, globals->color_schemes[nc][QPalette::ColorRole::Highlight]);                // List selection highlight color
    palette.setColor(QPalette::ColorRole::HighlightedText, globals->color_schemes[nc][QPalette::ColorRole::HighlightedText]);    // List selection highlight text color
    palette.setColor(QPalette::ColorRole::ButtonText, globals->color_schemes[nc][QPalette::ColorRole::ButtonText]);              // Button text, ** what is QPushButton background color??
    palette.setColor(QPalette::ColorRole::ToolTipBase, globals->color_schemes[nc][QPalette::ColorRole::ToolTipBase]);            // Tooltips
    palette.setColor(QPalette::ColorRole::ToolTipText, globals->color_schemes[nc][QPalette::ColorRole::ToolTipText]);            // Tooltips
    this->setPalette(palette);

    // More, currently unused, colors:
    //    palette.setColor(QPalette::ColorRole::Light, QColor(255, 0, 0));                  // ??
    //    palette.setColor(QPalette::ColorRole::Mid, QColor(255, 0, 0));                    // ??
    //    palette.setColor(QPalette::ColorRole::Midlight, QColor(255, 0, 0));               // ??
    //    palette.setColor(QPalette::ColorRole::Shadow, QColor(255, 0, 0));                 // ??
    //    palette.setColor(QPalette::ColorRole::Link, QColor(0, 150, 145));
    //    palette.setColor(QPalette::ColorRole::LinkVisited, QColor(0, 100, 95));

}



//####################################################################################
//##        Apply palette / coloring / styling to children widgets
//####################################################################################
void FormMain::applyColoring(Color_Scheme new_color)
{
    // ********** Custom blue dot image for QSplitter (horizontal)
    QString splitColor = QString(" QSplitter { width: 4px; } QSplitter::handle { image: url(:/tree_icons/splitter_h.png); } ");
    this->splitterHorizontal->setStyleSheet(splitColor);

    QColor temp1, temp2, temp3, temp4, temp5, temp6;

    // ********** Custom coloring for QPushButtons
    temp1 = globals->color_schemes[new_color][QPalette::ColorRole::Mid];
    temp2 = globals->color_schemes[new_color][QPalette::ColorRole::Midlight];
    temp3 = globals->color_schemes[new_color][QPalette::ColorRole::Base];
    temp4 = globals->color_schemes[new_color][QPalette::ColorRole::Button];
    temp5 = globals->color_schemes[new_color][QPalette::ColorRole::BrightText];
    QString buttonColor = QString(" QPushButton { color: " + temp1.name() + "; "
                                                " background: qlineargradient(spread:pad, x1:0 y1:0, x2:0 y2:1, stop:0 " + temp2.name() + ", stop:1 " + temp3.name() + "); "
                                                " border: none; border-radius: 6px; }"
                                  " QPushButton:hover:!pressed { color: " + temp5.name() + "; background: " + temp2.name() + "; }"
                                  " QPushButton:pressed { color: " + temp5.name() + "; background: " + temp4.name() + "; }");
    this->buttonAtlas->setStyleSheet(buttonColor);
    this->buttonFonts->setStyleSheet(buttonColor);
    this->buttonPlay->setStyleSheet(buttonColor);
    this->buttonSettings->setStyleSheet(buttonColor);
    this->buttonWorlds->setStyleSheet(buttonColor);

    // Shadow effect for buttons
    QGraphicsDropShadowEffect* effect[5];
    for (int i = 0; i < 5; i++)
    {
        effect[i] = new QGraphicsDropShadowEffect();
        effect[i]->setBlurRadius(6);
        effect[i]->setOffset(0,3);
        effect[i]->setColor(globals->color_schemes[new_color][QPalette::ColorRole::Shadow]);
    }
    this->buttonAtlas->setGraphicsEffect(effect[0]);
    this->buttonFonts->setGraphicsEffect(effect[1]);
    this->buttonPlay->setGraphicsEffect(effect[2]);
    this->buttonSettings->setGraphicsEffect(effect[3]);
    this->buttonWorlds->setGraphicsEffect(effect[4]);


    // ********** Custom coloring for TreeSceneView
    temp1 = globals->color_schemes[new_color][QPalette::ColorRole::Foreground];
    temp2 = globals->color_schemes[new_color][QPalette::ColorRole::Base];
    temp3 = globals->color_schemes[new_color][QPalette::ColorRole::Highlight];
    temp4 = globals->color_schemes[new_color][QPalette::ColorRole::HighlightedText];
    temp5 = globals->color_schemes[new_color][QPalette::ColorRole::Dark];
    temp6 = globals->color_schemes[new_color][QPalette::ColorRole::BrightText];
    QString listColor = QString(" QTreeWidget             { color: " + temp1.name() + ";  background: " + temp2.name() + "; selection-background-color: " + temp3.name() + "; }"
                                " QTreeWidget::item:selected { color: " + temp4.name() + "; background: " + temp3.name() + "; }"
                                " QTreeWidget::item:hover:selected { color: " + temp5.name() + ";  background: " + temp3.name() + "; }"
                                " QTreeWidget::item:hover:!selected { color: " + temp6.name() + ";  background: " + temp2.name() + "; }"
                                " QHeaderView::section { background-color: " + temp2.name() + "; border: 0px; }"
                                " QScrollBar:vertical { border: 2px solid grey; background: #32CC99; width: 14px; margin: 10px 0 10px 0; }");
    treeScene->header()->setStyleSheet(listColor);
    treeScene->setStyleSheet(listColor);

    // ********** Custom coloring for TreeObjectInspector
    temp1 = globals->color_schemes[new_color][QPalette::ColorRole::Foreground];
    temp2 = globals->color_schemes[new_color][QPalette::ColorRole::Base];
    temp3 = globals->color_schemes[new_color][QPalette::ColorRole::Highlight];
    temp4 = globals->color_schemes[new_color][QPalette::ColorRole::HighlightedText];
    temp5 = globals->color_schemes[new_color][QPalette::ColorRole::Dark];
    temp6 = globals->color_schemes[new_color][QPalette::ColorRole::BrightText];
    QString listObjColor = QString(" QTreeWidget             { color: " + temp1.name() + ";  background: " + temp2.name() + "; selection-background-color: " + temp3.name() + "; }"
                                   " QTreeWidget::item:selected { color: " + temp4.name() + "; background: " + temp3.name() + "; }"
                                   " QTreeWidget::item:hover:selected { color: " + temp5.name() + ";  background: " + temp3.name() + "; }"
                                   " QTreeWidget::item:hover:!selected { color: " + temp6.name() + ";  background: " + temp2.name() + "; }"
                                   " QHeaderView::section { background-color: " + temp2.name() + "; border: 0px; }");
    treeObject->header()->setStyleSheet(listObjColor);
    treeObject->setStyleSheet(listColor);

}




//####################################################################################
//##        Setting up of form main
//####################################################################################
void FormMain::buildWindow()
{
    QFont font;
    font.setPointSize(11);

    QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    sizePolicy.setHorizontalStretch(1);
    sizePolicy.setVerticalStretch(0);

    QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);

    QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Preferred);
    sizePolicy2.setHorizontalStretch(0);
    sizePolicy2.setVerticalStretch(0);

    // ***** Main window settings
    this->setObjectName(QStringLiteral("formMain"));
    this->setWindowModality(Qt::NonModal);
    this->resize(1100, 700);
    this->setMinimumSize(QSize(780, 400));
    this->setFont(font);
    this->setMouseTracking(true);
    this->setAcceptDrops(true);
    this->setWindowIcon(QIcon(":icon/icon256.png"));                        // Set icon


    // ***** Build Menu Bar
    menuBar = new QMenuBar(this);
    menuBar->setObjectName(QStringLiteral("menuBar"));
    menuBar->setGeometry(QRect(0, 0, 1100, 22));
        menuDrop = new QMenu(menuBar);
        menuDrop->setObjectName(QStringLiteral("menuDrop"));
        menuBar->addAction(menuDrop->menuAction());
    this->setMenuBar(menuBar);


    // ***** Build left Assets Dock
    assets = new QDockWidget(this);
    assets->setObjectName(QStringLiteral("assets"));
    assets->setMinimumSize(QSize(180, 35));
    assets->setFont(font);
    assets->setFeatures(QDockWidget::DockWidgetMovable);
    assets->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
        widgetAssests = new QWidget();
        widgetAssests->setObjectName(QStringLiteral("widgetAssests"));

            // TEMP TABLE
            tableWidget = new QTableWidget(widgetAssests);
            if (tableWidget->columnCount() < 2)
                tableWidget->setColumnCount(2);
            tableWidget->setObjectName(QStringLiteral("tableWidget"));
            tableWidget->setGeometry(QRect(0, 0, 251, 601));
            tableWidget->setFont(font);
            tableWidget->setLineWidth(1);
            tableWidget->setMidLineWidth(0);
            tableWidget->setSizeAdjustPolicy(QAbstractScrollArea::AdjustIgnored);
            tableWidget->setAutoScrollMargin(6);
            tableWidget->setAlternatingRowColors(true);
            tableWidget->setSelectionMode(QAbstractItemView::NoSelection);
            tableWidget->setShowGrid(false);
            tableWidget->setRowCount(0);
            tableWidget->setColumnCount(2);
            tableWidget->horizontalHeader()->setVisible(false);
            tableWidget->horizontalHeader()->setMinimumSectionSize(10);
            tableWidget->verticalHeader()->setVisible(false);
            tableWidget->verticalHeader()->setDefaultSectionSize(20);
            tableWidget->verticalHeader()->setMinimumSectionSize(10);

        assets->setWidget(widgetAssests);
    addDockWidget(static_cast<Qt::DockWidgetArea>(1), assets);


    // ***** Build right Inspector Dock
    inspector = new QDockWidget(this);
    inspector->setObjectName(QStringLiteral("inspector"));
    inspector->setSizePolicy(sizePolicy2);
    inspector->setMinimumSize(QSize(300, 139));
    inspector->setFont(font);
    inspector->setFeatures(QDockWidget::DockWidgetMovable);
    inspector->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
        widgetInspector = new QWidget();
        widgetInspector->setObjectName(QStringLiteral("widgetInspector"));
        widgetInspector->setSizePolicy(sizePolicy2);
        verticalLayoutObject = new QVBoxLayout(widgetInspector);
        verticalLayoutObject->setSpacing(2);
        verticalLayoutObject->setContentsMargins(11, 11, 11, 11);
        verticalLayoutObject->setObjectName(QStringLiteral("verticalLayoutObject"));
        verticalLayoutObject->setContentsMargins(2, 2, 2, 2);

            // ***** Load our custom TreeObjectInspector for the Scene List
            treeObject = new TreeObjectInspector(widgetInspector, this);
                QTreeWidgetItem *header_item_obj = new QTreeWidgetItem();
                treeObject->setHeaderItem(header_item_obj);
            treeObject->setObjectName(QStringLiteral("treeObject"));
            treeObject->setColumnCount(2);
            treeObject->setColumnWidth(0, 135);
            treeObject->setMinimumSize(QSize(135, 150));
            //treeObject->setSizeAdjustPolicy(QAbstractScrollArea::SizeAdjustPolicy::AdjustToContents);
            treeObject->setFont(font);
            treeObject->setProperty("showDropIndicator", QVariant(false));
            treeObject->setDragEnabled(false);
            treeObject->setDragDropOverwriteMode(false);
            treeObject->setDragDropMode(QAbstractItemView::DragDropMode::NoDragDrop);
            treeObject->setDefaultDropAction(Qt::DropAction::TargetMoveAction);
            treeObject->setAlternatingRowColors(false);
            treeObject->setSelectionMode(QAbstractItemView::SelectionMode::NoSelection);
            treeObject->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectItems);
            treeObject->setIndentation(5);
            treeObject->setRootIsDecorated(true);
            treeObject->setItemsExpandable(true);
            treeObject->setExpandsOnDoubleClick(true);
            treeObject->header()->setSectionResizeMode(1, QHeaderView::ResizeMode::Stretch);
            treeObject->header()->setStretchLastSection(true);
            treeObject->header()->setVisible(false);
            treeObject->setHeaderHidden(true);
        verticalLayoutObject->addWidget(treeObject);

        inspector->setWidget(widgetInspector);
    addDockWidget(static_cast<Qt::DockWidgetArea>(2), inspector);


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
    widgetCentral->setSizePolicy(sizePolicy);
    verticalLayout = new QVBoxLayout(widgetCentral);
    verticalLayout->setSpacing(2);
    verticalLayout->setContentsMargins(11, 11, 11, 11);
    verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
    verticalLayout->setContentsMargins(2, 2, 2, 2);
        splitterVertical = new QSplitter(widgetCentral);
        splitterVertical->setObjectName(QStringLiteral("splitterVertical"));
        splitterVertical->setOrientation(Qt::Vertical);
        splitterVertical->setHandleWidth(4);

            widgetInner = new QWidget(splitterVertical);
            widgetInner->setObjectName(QStringLiteral("widgetInner"));
            horizontalLayout = new QHBoxLayout(widgetInner);
            horizontalLayout->setSpacing(0);
            horizontalLayout->setContentsMargins(11, 11, 11, 11);
            horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
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

            horizontalLayout->addWidget(splitterHorizontal);
        splitterVertical->addWidget(widgetInner);

            areaBottom = new QScrollArea(splitterVertical);
            areaBottom->setObjectName(QStringLiteral("areaBottom"));
            areaBottom->setSizePolicy(sizePolicy1);
            areaBottom->setMinimumSize(QSize(0, 100));
            areaBottom->setFont(font);
            areaBottom->setWidgetResizable(true);
                widgetBottom = new QWidget();
                widgetBottom->setObjectName(QStringLiteral("widgetBottom"));
                widgetBottom->setGeometry(QRect(0, 0, 652, 622));
                label_1 = new QLabel(widgetBottom);
                label_1->setObjectName(QStringLiteral("label_1"));
                label_1->setGeometry(QRect(10, 10, 141, 21));
                label_1->setFont(font);
                label_2 = new QLabel(widgetBottom);
                label_2->setObjectName(QStringLiteral("label_2"));
                label_2->setGeometry(QRect(10, 30, 141, 21));
                label_2->setFont(font);
                label_3 = new QLabel(widgetBottom);
                label_3->setObjectName(QStringLiteral("label_3"));
                label_3->setGeometry(QRect(10, 70, 631, 21));
                label_3->setFont(font);
                label_object = new QLabel(widgetBottom);
                label_object->setObjectName(QStringLiteral("label_object"));
                label_object->setGeometry(QRect(180, 10, 461, 21));
                label_object->setFont(font);
                label_object_2 = new QLabel(widgetBottom);
                label_object_2->setObjectName(QStringLiteral("label_object_2"));
                label_object_2->setGeometry(QRect(180, 30, 461, 21));
                label_object_2->setFont(font);
                label_object_3 = new QLabel(widgetBottom);
                label_object_3->setObjectName(QStringLiteral("label_object_3"));
                label_object_3->setGeometry(QRect(180, 50, 461, 21));
                label_object_3->setFont(font);
            areaBottom->setWidget(widgetBottom);
        splitterVertical->addWidget(areaBottom);

    verticalLayout->addWidget(splitterVertical);
    this->setCentralWidget(widgetCentral);





    // ***** Load ScrollArea for use in showing the Scene
    QScrollArea *areaScene;
    QWidget *widgetScene;
    areaScene = new QScrollArea(this->splitterHorizontal);
    areaScene->setObjectName(QStringLiteral("areaScene"));
    QSizePolicy sizePolicy3(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);
    sizePolicy3.setHorizontalStretch(1);
    sizePolicy3.setVerticalStretch(0);
    sizePolicy3.setHeightForWidth(areaScene->sizePolicy().hasHeightForWidth());
    areaScene->setSizePolicy(sizePolicy3);
    areaScene->setMinimumSize(QSize(100, 0));
    areaScene->setFont(font);
    areaScene->setWidgetResizable(true);
    widgetScene = new QWidget();
    widgetScene->setObjectName(QStringLiteral("widgetScene"));
    widgetScene->setGeometry(QRect(0, 0, 392, 522));
    areaScene->setWidget(widgetScene);
    splitterHorizontal->addWidget(areaScene);


    // ***** Set titles and button texts
    setWindowTitle(QApplication::translate("MainWindow", "Drop", nullptr));
    assets->setWindowTitle(QApplication::translate("MainWindow", "Assets", nullptr));
    inspector->setWindowTitle(QApplication::translate("MainWindow", "Inspector", nullptr));
    menuDrop->setTitle(QApplication::translate("MainWindow", "File", nullptr));
    label_1->setText(QApplication::translate("MainWindow", "TextLabel", nullptr));
    label_2->setText(QApplication::translate("MainWindow", "TextLabel", nullptr));
    label_3->setText(QApplication::translate("MainWindow", "TextLabel", nullptr));
    label_object->setText(QApplication::translate("MainWindow", "Object ID, Type", nullptr));
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










