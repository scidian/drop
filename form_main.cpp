//
//
//      Loads, styles and handles events for FormMain
//
//

#include "01_project.h"
#include "02_world.h"
#include "03_scene.h"
#include "04_object.h"

#include "30_settings.h"
#include "31_component.h"
#include "32_property.h"

#include "form_styling.h"
#include "form_main.h"

// Destructor for Main Window
FormMain::~FormMain()
{
    delete project;
}


// Constructor for Main Window
FormMain::FormMain(QWidget *parent, Globals *the_globals) : QMainWindow(parent), globals(the_globals)
{
    // Initialize form and customize colors and styles
    applyPalette(globals->current_color_scheme);
    buildWindow();
    applyColoring(globals->current_color_scheme);


    // Initialize new project, initialize local variables
    project = new DrProject();
    current_world = 0;





    // TEMP NEW PROJECT:
    //      Create a new project and add some stuff to it
    project->addWorld();
    project->getWorldWithName("World 2")->addScene();
    project->getWorldWithName("World 2")->addScene("asdfasdfasdfasdfasfdasdfasdfasdfasdfasdfasdfasdfasd");
    project->getWorldWithName("World 2")->addScene();
    project->getWorldWithName("World 2")->addScene();
    project->getWorldWithName("World 2")->addScene();
    project->getWorldWithName("World 2")->getSceneWithName("4")->addObject(DrTypes::Object);
    project->getWorldWithName("World 2")->getSceneWithName("4")->addObject(DrTypes::Object);
    project->addWorld();
    project->addWorld();




    // TEMP LIST POPULATE:
    //      Populates list based on project data
    for (auto world_pair: project->getWorldMap())
    {
        QTreeWidgetItem *topLevelItem = new QTreeWidgetItem(treeScene);                             // Create new item (top level item)

        topLevelItem->setIcon(0, QIcon(":/tree_icons/tree_world.png"));                                 // Loads icon for world
        topLevelItem->setText(0, "World: " + world_pair.second->getComponentPropertyValue(
                                 World_Components::settings, World_Properties::name).toString());       // Set text for item
        topLevelItem->setData(0, Qt::UserRole, QVariant::fromValue(world_pair.second->getKey()));
        treeScene->addTopLevelItem(topLevelItem);                                                   // Add it on our tree as the top item.

        for (auto scene_pair: world_pair.second->getSceneMap())
        {
            QTreeWidgetItem *sub_item = new QTreeWidgetItem(topLevelItem);                              // Create new item and add as child item
            sub_item->setIcon(0, QIcon(":/tree_icons/tree_scene.png"));                                 // Loads icon for scene
            sub_item->setText(0, "Scene: " + scene_pair.second->getComponentPropertyValue(
                                 Scene_Components::settings, Scene_Properties::name).toString());       // Set text for item
            sub_item->setData(0, Qt::UserRole, QVariant::fromValue(scene_pair.second->getKey()));

            for (auto object_pair: scene_pair.second->getObjectMap())
            {
                QTreeWidgetItem *sub_sub_item = new QTreeWidgetItem(sub_item);                                      // Create new item and add as child item
                switch (object_pair.second->getType())
                {
                    case DrTypes::Object:    sub_sub_item->setIcon(0, QIcon(":/tree_icons/tree_object.png")); break;
                    case DrTypes::Camera:    sub_sub_item->setIcon(0, QIcon(":/tree_icons/tree_camera.png")); break;
                    case DrTypes::Character: sub_sub_item->setIcon(0, QIcon(":/tree_icons/tree_character.png")); break;
                    default: break;
                }

                sub_sub_item->setText(0, object_pair.second->getComponentPropertyValue(
                                         Object_Components::settings, Object_Properties::name).toString());         // Set text for item
                sub_sub_item->setData(0, Qt::UserRole, QVariant::fromValue(object_pair.second->getKey()));          // Store item key in user data

                // Add lock box
                QString check_images = QString(" QCheckBox::indicator { width: 12px; height: 12px; }"
                                               " QCheckBox::indicator:checked { image: url(:/tree_icons/tree_lock.png); }"
                                               " QCheckBox::indicator:unchecked { image: url(:/tree_icons/tree_bullet.png); }");
                QCheckBox *lock_item = new QCheckBox();
                lock_item->setStyleSheet(check_images);
                treeScene->setItemWidget(sub_sub_item, 1, lock_item);

            }
        }
    }
    treeScene->expandAll();                                             // Expand all items



    // TEMP:
    //      Test loading data out from a pair, i.e. "POINT2D", stored as QList<QVariant>
    QList<QVariant> myPoint = project->getWorld(current_world)->getComponentProperty(World_Components::physics, World_Properties::gravity)->getValue().toList();
    label_1->setText("Gravity X:" + myPoint.first().toString());
    label_2->setText("Gravity Y:" + myPoint.last().toString());


    // DISPLAY MESSAGEBOX TO KNOW WE MADE IT HERE
    //ShowMessageBox("Made It 99");
}





//#######################################################################
//##
//##    Setting up and Coloring of main window
//##
//#######################################################################
void FormMain::buildWindow()
{
    QFont font;
    font.setPointSize(11);

    if (this->objectName().isEmpty())
        this->setObjectName(QStringLiteral("MainWindow"));
    this->setWindowModality(Qt::NonModal);
    this->resize(1100, 700);
    this->setMinimumSize(QSize(780, 400));
    this->setFont(font);
    this->setMouseTracking(true);
    this->setAcceptDrops(true);

    widgetCentral = new QWidget(this);
    widgetCentral->setObjectName(QStringLiteral("widgetCentral"));
    QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    sizePolicy.setHorizontalStretch(1);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(widgetCentral->sizePolicy().hasHeightForWidth());
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

    horizontalLayout->addWidget(splitterHorizontal);

    splitterVertical->addWidget(widgetInner);
    areaBottom = new QScrollArea(splitterVertical);
    areaBottom->setObjectName(QStringLiteral("areaBottom"));
    QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(areaBottom->sizePolicy().hasHeightForWidth());
    areaBottom->setSizePolicy(sizePolicy1);
    areaBottom->setMinimumSize(QSize(0, 100));
    areaBottom->setFont(font);
    areaBottom->setWidgetResizable(true);
    widgetBottom = new QWidget();
    widgetBottom->setObjectName(QStringLiteral("widgetBottom"));
    widgetBottom->setGeometry(QRect(0, 0, 652, 622));
    label_2 = new QLabel(widgetBottom);
    label_2->setObjectName(QStringLiteral("label_2"));
    label_2->setGeometry(QRect(10, 30, 141, 21));
    label_2->setFont(font);
    label_1 = new QLabel(widgetBottom);
    label_1->setObjectName(QStringLiteral("label_1"));
    label_1->setGeometry(QRect(10, 10, 141, 21));
    label_1->setFont(font);
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
    label_3 = new QLabel(widgetBottom);
    label_3->setObjectName(QStringLiteral("label_3"));
    label_3->setGeometry(QRect(10, 70, 631, 21));
    label_3->setFont(font);
    areaBottom->setWidget(widgetBottom);
    splitterVertical->addWidget(areaBottom);

    verticalLayout->addWidget(splitterVertical);

    this->setCentralWidget(widgetCentral);
    menuBar = new QMenuBar(this);
    menuBar->setObjectName(QStringLiteral("menuBar"));
    menuBar->setGeometry(QRect(0, 0, 1100, 22));
    menuDrop = new QMenu(menuBar);
    menuDrop->setObjectName(QStringLiteral("menuDrop"));
    this->setMenuBar(menuBar);
    assets = new QDockWidget(this);
    assets->setObjectName(QStringLiteral("assets"));
    assets->setMinimumSize(QSize(180, 35));
    assets->setFont(font);
    assets->setFeatures(QDockWidget::DockWidgetMovable);
    assets->setAllowedAreas(Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea);
    widgetAssests = new QWidget();
    widgetAssests->setObjectName(QStringLiteral("widgetAssests"));
    assets->setWidget(widgetAssests);
    this->addDockWidget(static_cast<Qt::DockWidgetArea>(1), assets);
    inspector = new QDockWidget(this);
    inspector->setObjectName(QStringLiteral("inspector"));
    QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Preferred);
    sizePolicy2.setHorizontalStretch(0);
    sizePolicy2.setVerticalStretch(0);
    sizePolicy2.setHeightForWidth(inspector->sizePolicy().hasHeightForWidth());
    inspector->setSizePolicy(sizePolicy2);
    inspector->setMinimumSize(QSize(260, 139));
    inspector->setFont(font);
    inspector->setFeatures(QDockWidget::DockWidgetMovable);
    inspector->setAllowedAreas(Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea);
    widgetInspector = new QWidget();
    widgetInspector->setObjectName(QStringLiteral("widgetInspector"));
    sizePolicy2.setHeightForWidth(widgetInspector->sizePolicy().hasHeightForWidth());
    widgetInspector->setSizePolicy(sizePolicy2);
    tableWidget = new QTableWidget(widgetInspector);
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
    inspector->setWidget(widgetInspector);
    this->addDockWidget(static_cast<Qt::DockWidgetArea>(2), inspector);
    toolbar = new QDockWidget(this);
    toolbar->setObjectName(QStringLiteral("toolbar"));
    toolbar->setMinimumSize(QSize(449, 45));
    toolbar->setMaximumSize(QSize(524287, 45));
    toolbar->setFeatures(QDockWidget::NoDockWidgetFeatures);
    toolbar->setAllowedAreas(Qt::TopDockWidgetArea);
    toolbar->setWindowTitle(QStringLiteral(""));
    widgetToolbar = new QWidget();
    widgetToolbar->setObjectName(QStringLiteral("widgetToolbar"));
    buttonWorlds = new QPushButton(widgetToolbar);
    buttonWorlds->setObjectName(QStringLiteral("buttonWorlds"));
    buttonWorlds->setGeometry(QRect(20, 8, 121, 31));
    QFont font1;
    font1.setPointSize(11);
    font1.setBold(false);
    font1.setWeight(50);
    buttonWorlds->setFont(font1);
    buttonWorlds->setCheckable(false);
    buttonPlay = new QPushButton(widgetToolbar);
    buttonPlay->setObjectName(QStringLiteral("buttonPlay"));
    buttonPlay->setGeometry(QRect(660, 8, 111, 31));
    buttonPlay->setFont(font);
    buttonSettings = new QPushButton(widgetToolbar);
    buttonSettings->setObjectName(QStringLiteral("buttonSettings"));
    buttonSettings->setGeometry(QRect(780, 8, 111, 31));
    buttonSettings->setFont(font);
    buttonAtlas = new QPushButton(widgetToolbar);
    buttonAtlas->setObjectName(QStringLiteral("buttonAtlas"));
    buttonAtlas->setGeometry(QRect(250, 8, 111, 31));
    buttonAtlas->setFont(font);
    buttonFonts = new QPushButton(widgetToolbar);
    buttonFonts->setObjectName(QStringLiteral("buttonFonts"));
    buttonFonts->setGeometry(QRect(370, 8, 111, 31));
    buttonFonts->setFont(font);
    toolbar->setWidget(widgetToolbar);
    this->addDockWidget(static_cast<Qt::DockWidgetArea>(4), toolbar);

    menuBar->addAction(menuDrop->menuAction());



    // Set icon for program
    this->setWindowIcon(QIcon(":icon/icon256.png"));


    // ***** Load our custom MyTreeView for the Scene List
    treeScene = new TreeSceneView(splitterHorizontal, this);
    treeScene->setStyle(new SceneTreeHighlightProxy(treeScene->style(), treeScene));

    QTreeWidgetItem *header_item = new QTreeWidgetItem();
    header_item->setIcon(1, QIcon(":/tree_icons/tree_lock_header.png"));
    treeScene->setHeaderItem(header_item);
    treeScene->setObjectName(QStringLiteral("treeScene"));
    treeScene->setColumnCount(2);
    treeScene->setColumnWidth(0, 150);
    treeScene->setColumnWidth(1, 16);
    treeScene->setMinimumSize(QSize(190, 0));
    treeScene->setFont(font);
    treeScene->setProperty("showDropIndicator", QVariant(false));
    treeScene->setDragEnabled(true);
    treeScene->setDragDropOverwriteMode(false);
    treeScene->setDragDropMode(QAbstractItemView::InternalMove);
    treeScene->setDefaultDropAction(Qt::TargetMoveAction);
    treeScene->setAlternatingRowColors(false);
    treeScene->setSelectionMode(QAbstractItemView::ExtendedSelection);
    treeScene->setSelectionBehavior(QAbstractItemView::SelectRows);
    treeScene->setIndentation(15);
    treeScene->setRootIsDecorated(true);
    treeScene->setItemsExpandable(true);
    treeScene->setExpandsOnDoubleClick(false);
    treeScene->header()->setSectionResizeMode(0, QHeaderView::ResizeMode::Stretch);
    treeScene->header()->setStretchLastSection(false);
    treeScene->header()->setVisible(true);
    splitterHorizontal->addWidget(treeScene);


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


    setWindowTitle(QApplication::translate("MainWindow", "Drop", nullptr));
    label_2->setText(QApplication::translate("MainWindow", "TextLabel", nullptr));
    label_1->setText(QApplication::translate("MainWindow", "TextLabel", nullptr));
    label_object->setText(QApplication::translate("MainWindow", "Object ID, Type", nullptr));
    label_object_2->setText(QApplication::translate("MainWindow", "Object ID, Type", nullptr));
    label_object_3->setText(QApplication::translate("MainWindow", "Object ID, Type", nullptr));
    label_3->setText(QApplication::translate("MainWindow", "TextLabel", nullptr));
    menuDrop->setTitle(QApplication::translate("MainWindow", "File", nullptr));
    assets->setWindowTitle(QApplication::translate("MainWindow", "Assets", nullptr));
    inspector->setWindowTitle(QApplication::translate("MainWindow", "Inspector", nullptr));
    buttonWorlds->setToolTip(QApplication::translate("MainWindow", "Project View", nullptr));
    buttonWorlds->setText(QApplication::translate("MainWindow", "Worlds / UI", nullptr));
    buttonPlay->setText(QApplication::translate("MainWindow", "Play", nullptr));
    buttonSettings->setText(QApplication::translate("MainWindow", "App Settings", nullptr));
    buttonAtlas->setText(QApplication::translate("MainWindow", "Atlases", nullptr));
    buttonFonts->setText(QApplication::translate("MainWindow", "Fonts", nullptr));


    // ********** Small tweaks to some of the widgets
    toolbar->setTitleBarWidget(new QWidget());                    // Removes title bar from QDockWidget Toolbar

    splitterHorizontal->setSizes(QList<int> { 150, 300 });        // Sets tree_scene (scene assests) startup width to 150
                                                                  // NOTE: You can save and restore the sizes of the widgets from a QByteArray
                                                                  //       using QSplitter.saveState() and QSplitter.restoreState() respectively
}



void FormMain::applyPalette(Color_Scheme new_color)
{
    // ********** Apply palette to main window
    QPalette palette = this->palette();
    palette.setColor(QPalette::ColorRole::Window, (globals->color_schemes[new_color])[QPalette::ColorRole::Window]);                      // Background color
    palette.setColor(QPalette::ColorRole::Background, (globals->color_schemes[new_color])[QPalette::ColorRole::Background]);              // Background color, obsolete?
    palette.setColor(QPalette::ColorRole::WindowText, (globals->color_schemes[new_color])[QPalette::ColorRole::WindowText]);              // Header and label font color
    palette.setColor(QPalette::ColorRole::Foreground, (globals->color_schemes[new_color])[QPalette::ColorRole::Foreground]);              // Header and label font color, obsolete?
    palette.setColor(QPalette::ColorRole::Base, (globals->color_schemes[new_color])[QPalette::ColorRole::Base]);                          // Alternate background color
    palette.setColor(QPalette::ColorRole::AlternateBase, (globals->color_schemes[new_color])[QPalette::ColorRole::AlternateBase]);        // Alternate background color in QTable
    palette.setColor(QPalette::ColorRole::Text, (globals->color_schemes[new_color])[QPalette::ColorRole::Text]);                          // Button Text color
    palette.setColor(QPalette::ColorRole::Button, (globals->color_schemes[new_color])[QPalette::ColorRole::Button]);                      // Header background gradient top color
    palette.setColor(QPalette::ColorRole::Dark, (globals->color_schemes[new_color])[QPalette::ColorRole::Dark]);                          // Header background gradient bottom color
    palette.setColor(QPalette::ColorRole::Highlight, (globals->color_schemes[new_color])[QPalette::ColorRole::Highlight]);                // List selection highlight color
    palette.setColor(QPalette::ColorRole::HighlightedText, (globals->color_schemes[new_color])[QPalette::ColorRole::HighlightedText]);    // List selection highlight text color
    palette.setColor(QPalette::ColorRole::ButtonText, (globals->color_schemes[new_color])[QPalette::ColorRole::ButtonText]);              // Button text, ** what is QPushButton background color??
    palette.setColor(QPalette::ColorRole::ToolTipBase, (globals->color_schemes[new_color])[QPalette::ColorRole::ToolTipBase]);            // Tooltips
    palette.setColor(QPalette::ColorRole::ToolTipText, (globals->color_schemes[new_color])[QPalette::ColorRole::ToolTipText]);            // Tooltips
    this->setPalette(palette);
}

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


    // ********** Custom coloring for QTreeWidget
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
                                " QHeaderView::section { background-color: " + temp2.name() + "; border: 0px; }");
    treeScene->header()->setStyleSheet(listColor);
    treeScene->setStyleSheet(listColor);

}







//##############################################################
//
//  Testing drag event
//

// This removes the item from under the mouse
void TreeSceneView::startDrag(Qt::DropActions supportedActions)
{
    // Partly copied from Qt 5.5.5 sources
    QModelIndexList indexes = selectedIndexes();
    if (indexes.count() > 0) {
        // Get a list of the supported MIMEs of the selected indexes
        QMimeData* data = model()->mimeData(indexes);
        if (!data) {
            return;
        }

        // Create the drag object
        QDrag* drag = new QDrag(this);
        drag->setMimeData(data);
        drag->setPixmap(QPixmap());

        // Execute the drag
        drag->exec(supportedActions, Qt::MoveAction);
    }
}


// Fires when we start dragging
void TreeSceneView::dragMoveEvent(QDragMoveEvent *event)
{
    m_mouse_x = event->pos().x();
    m_mouse_y = event->pos().y();
    getMainWindow()->label_1->setText(QString::fromStdString("MX: ") + QString::number(m_mouse_x) +
                                      QString::fromStdString(", MY: ") + QString::number(m_mouse_y) );

    // Get item under mouse, if not null lets process it to see if we will allow dropping there
    QTreeWidgetItem *item_at = this->itemAt(event->pos());
    m_can_drop = false;
    if (item_at != nullptr)
    {
        long        check_key = item_at->data(0, Qt::UserRole).toLongLong();
        getMainWindow()->label_object_3->setText(QString::fromStdString("Selected: " + std::to_string(m_selected_key) +
                                                                      ", Checking: " + std::to_string(check_key)) );
        // Check if its the same type as already selected, if so allow possible drop
        if (m_is_dragging && m_selected_key != 0 && check_key != 0) {
            DrSettings *check_settings = getMainWindow()->project->findSettingsFromKey(check_key);
            DrSettings *selected_settings = getMainWindow()->project->findSettingsFromKey(m_selected_key);

            if ( CheckTypesAreSame(check_settings->getType(), selected_settings->getType()) ) { m_can_drop = true; }
        }
    }

    setDropIndicatorShown(true);
    m_is_dragging = true;
    QTreeWidget::dragMoveEvent(event);
}

void TreeSceneView::dropEvent(QDropEvent* event)
{
    bool dropOK = false;
    DropIndicatorPosition dropIndicator = dropIndicatorPosition();

    switch (dropIndicator)
    {
        case QAbstractItemView::AboveItem:  dropOK = true;  break;
        case QAbstractItemView::BelowItem:  dropOK = true;  break;
        case QAbstractItemView::OnItem:     dropOK = false; break;
        case QAbstractItemView::OnViewport: dropOK = false; break;
    }

    if (dropOK && m_can_drop)
    {
        // ########## !!!!!!!!! Here we need manage the case of dropping an item

        QTreeWidget::dropEvent(event);              // Pass event through to base
    }
    else
    {
        event->ignore();
    }
    setDropIndicatorShown(false);                   // hide the drop indicator once the drop is done
    m_is_dragging = false;
}


//##############################################################
//
// Updates selection, checks to make sure if more than one item is selected all new items
//                    not matching original type are not selected
//
void TreeSceneView::selectionChanged (const QItemSelection &selected, const QItemSelection &deselected)
{
    getMainWindow()->listSelectionChanged(this->selectedItems());
    QTreeWidget::selectionChanged(selected, deselected);
}

void FormMain::listSelectionChanged(QList<QTreeWidgetItem*> item_list)
{
    label_3->setText("Selected Items: " + QString::number(item_list.size()));

    // If size of list is zero, clear selected_key and exit function
    if (item_list.size() == 0) {
        treeScene->setSelectedKey(0);
        return;
    }

    // Otherwise add first item to label, if size is one, reset first selected item
    label_3->setText(label_3->text() + ", First Item: " + item_list.first()->text(0));

    if (item_list.size() == 1){
        treeScene->setSelectedKey(item_list.first()->data(0, Qt::UserRole).toLongLong());           // grab stored key from list view user data
        buildObjectInspector();
    }
    else {
        DrTypes selected_type = project->findTypeFromKey( treeScene->getSelectedKey() );

        // Check if newly selected items are same type, if not, do not allow select
        for (auto check_item: item_list) {
            // Get key from each item so we can compare it to first selected item
            long    check_key = check_item->data(0, Qt::UserRole).toLongLong();
            DrTypes check_type = project->findTypeFromKey(check_key);

            // If we are over item that was first selected, skip to next
            if (check_key == treeScene->getSelectedKey()) { continue; }

            if (CheckTypesAreSame(check_type, selected_type) == false) {
                check_item->setSelected(false);
            }
        }
    }
}









//#######################################################################
//
//  Need to finish dynamically building object inspector
//
void FormMain::buildObjectInspector()
{
    // First, retrieve unique key of item clicked in list
    DrTypes     selected_type = project->findTypeFromKey( treeScene->getSelectedKey() );
    std::string type_string = StringFromType(selected_type);
    label_object->setText("KEY: " + QString::number( treeScene->getSelectedKey() ) + ", TYPE: " + QString::fromStdString(type_string));
    label_object_2->setText("");
    label_object_3->setText("");


    // Reset / clear tableWidget
    tableWidget->clear();

    ComponentMap components = project->findSettingsFromKey( treeScene->getSelectedKey() )->getComponentList();


    //####**** Build Object Inspector!!!!! ****####


    // Build tables in Object Inspector
    int rowCount = 0;
    for (auto i: components)
    {

        for (auto j: i.second->getPropertyList())
        {
            QTableWidgetItem *new_item = new QTableWidgetItem(QString::fromStdString(j.second->getDisplayName()));
            new_item->setData(Qt::UserRole, QVariant::fromValue(j.second->getPropertyKey()));

            // If no rows yet, create some, otherwise don't
            if (tableWidget->rowCount() < (rowCount + 1))
            {
                tableWidget->insertRow(rowCount);
            }

            tableWidget->setItem(rowCount, 0, new_item);

            rowCount++;
        }
    }
}




//##############################################################
//
//  On object inspector click show info about object and property
//
void FormMain::on_tableWidget_itemClicked(QTableWidgetItem *item)
{
    // If no item is selected in tree view, exit function
    if (treeScene->getSelectedKey() == 0) { return; }

    // First, retrieve property key of item clicked in tableWidget list
    long        property_key = item->data(Qt::UserRole).toLongLong();

    // Grab a pointer to the component list of the first selected item from treeScene (stored in selected_list)
    DrSettings  *selected_item_settings = project->findSettingsFromKey( treeScene->getSelectedKey() );
    DrComponent *clicked_component = selected_item_settings->findComponentFromPropertyKey(property_key);
    DrProperty  *clicked_property = clicked_component->getProperty(property_key);

    std::string property_name = clicked_property->getDisplayName();
    std::string component_name = clicked_component->getDisplayName();
    long        component_key = clicked_component->getComponentKey();

    // Grab type of main selected item in selected tree list
    std::string type_string2 = StringFromType(project->findTypeFromKey( treeScene->getSelectedKey() ));
    std::string type_string = StringFromType(selected_item_settings->getType());

    label_object->setText("KEY: " + QString::number( treeScene->getSelectedKey() ) + ", TYPE: " + QString::fromStdString(type_string));
    label_object_2->setText("COMPONENT: " + QString::number(component_key) +   ", NAME: " + QString::fromStdString(component_name));
    label_object_3->setText("PROPERTY: " + QString::number(property_key) +   ", NAME: " + QString::fromStdString(property_name));

}








