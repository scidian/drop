//
//
//      Functions to color and style form widgets
//
//

#include <QGraphicsDropShadowEffect>

#include "form_coloring.h"
#include "form_styling.h"

//#######################################################################
//##
//##    Setting up and Coloring of main window
//##
//#######################################################################
void adjustMainWindowUi(MainWindow *main_window)
{
    // Set icon for program
    main_window->setWindowIcon(QIcon(":icon/icon256.png"));


    // ***** Load our custom MyTreeView for the Scene List
    main_window->setSceneListWidget(new MyTreeView(main_window->getUi()->splitterHorizontal, main_window));
    main_window->getSceneListWidget()->setStyle(new SceneTreeHighlightProxy(main_window->getSceneListWidget()->style(), main_window->getSceneListWidget()));

    QTreeWidgetItem *header_item = new QTreeWidgetItem();
    header_item->setIcon(1, QIcon(":/tree_icons/tree_lock_header.png"));
    main_window->getSceneListWidget()->setHeaderItem(header_item);
    main_window->getSceneListWidget()->setObjectName(QStringLiteral("treeScene"));
    main_window->getSceneListWidget()->setColumnCount(2);
    main_window->getSceneListWidget()->setColumnWidth(0, 150);
    main_window->getSceneListWidget()->setColumnWidth(1, 16);
    main_window->getSceneListWidget()->setMinimumSize(QSize(190, 0));
    QFont font;
    font.setPointSize(11);
    main_window->getSceneListWidget()->setFont(font);
    main_window->getSceneListWidget()->setProperty("showDropIndicator", QVariant(false));
    main_window->getSceneListWidget()->setDragEnabled(true);
    main_window->getSceneListWidget()->setDragDropOverwriteMode(false);
    main_window->getSceneListWidget()->setDragDropMode(QAbstractItemView::InternalMove);
    main_window->getSceneListWidget()->setDefaultDropAction(Qt::TargetMoveAction);
    main_window->getSceneListWidget()->setAlternatingRowColors(false);
    main_window->getSceneListWidget()->setSelectionMode(QAbstractItemView::ExtendedSelection);
    main_window->getSceneListWidget()->setSelectionBehavior(QAbstractItemView::SelectRows);
    main_window->getSceneListWidget()->setIndentation(15);
    main_window->getSceneListWidget()->setRootIsDecorated(true);
    main_window->getSceneListWidget()->setItemsExpandable(true);
    main_window->getSceneListWidget()->setExpandsOnDoubleClick(false);
    main_window->getSceneListWidget()->header()->setSectionResizeMode(0, QHeaderView::ResizeMode::Stretch);
    main_window->getSceneListWidget()->header()->setStretchLastSection(false);
    main_window->getSceneListWidget()->header()->setVisible(true);
    main_window->getUi()->splitterHorizontal->addWidget(main_window->getSceneListWidget());


    // ***** Load ScrollArea for use in showing the Scene
    QScrollArea *areaScene;
    QWidget *widgetScene;
    areaScene = new QScrollArea(main_window->getUi()->splitterHorizontal);
    areaScene->setObjectName(QStringLiteral("areaScene"));
    QSizePolicy sizePolicy1(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);
    sizePolicy1.setHorizontalStretch(1);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(areaScene->sizePolicy().hasHeightForWidth());
    areaScene->setSizePolicy(sizePolicy1);
    areaScene->setMinimumSize(QSize(100, 0));
    areaScene->setFont(font);
    areaScene->setWidgetResizable(true);
    widgetScene = new QWidget();
    widgetScene->setObjectName(QStringLiteral("widgetScene"));
    widgetScene->setGeometry(QRect(0, 0, 392, 522));
    areaScene->setWidget(widgetScene);
    main_window->getUi()->splitterHorizontal->addWidget(areaScene);


    // ********** Small tweaks to some of the widgets
    main_window->getUi()->toolbar->setTitleBarWidget(new QWidget());                    // Removes title bar from QDockWidget Toolbar

    main_window->getUi()->splitterHorizontal->setSizes(QList<int> { 150, 300 });        // Sets tree_scene (scene assests) startup width to 150
                                                                                        // NOTE: You can save and restore the sizes of the widgets from a QByteArray
                                                                                        //       using QSplitter.saveState() and QSplitter.restoreState() respectively

}

//#######################################################################
//      Set's parent window colors to be inherited by others
//#######################################################################
void loadPalettes()
{
    //    palette.setColor(QPalette::ColorRole::Light, QColor(255, 0, 0));                  // ??
    //    palette.setColor(QPalette::ColorRole::Mid, QColor(255, 0, 0));                    // ??
    //    palette.setColor(QPalette::ColorRole::Midlight, QColor(255, 0, 0));               // ??
    //    palette.setColor(QPalette::ColorRole::Shadow, QColor(255, 0, 0));                 // ??
    //    palette.setColor(QPalette::ColorRole::Link, QColor(0, 150, 145));
    //    palette.setColor(QPalette::ColorRole::LinkVisited, QColor(0, 100, 95));

    color_schemes[Color_Scheme::Dark].insert(std::make_pair(QPalette::ColorRole::Window, QColor(32, 32, 32)));
    color_schemes[Color_Scheme::Dark].insert(std::make_pair(QPalette::ColorRole::Background, QColor(32, 32, 32)));
    color_schemes[Color_Scheme::Dark].insert(std::make_pair(QPalette::ColorRole::WindowText, QColor(205, 205, 205)));
    color_schemes[Color_Scheme::Dark].insert(std::make_pair(QPalette::ColorRole::Foreground, QColor(205, 205, 205)));
    color_schemes[Color_Scheme::Dark].insert(std::make_pair(QPalette::ColorRole::Base, QColor(24, 24, 24)));
    color_schemes[Color_Scheme::Dark].insert(std::make_pair(QPalette::ColorRole::AlternateBase, QColor(32, 32, 32)));
    color_schemes[Color_Scheme::Dark].insert(std::make_pair(QPalette::ColorRole::Text, QColor(205, 205 ,205)));
    color_schemes[Color_Scheme::Dark].insert(std::make_pair(QPalette::ColorRole::Button, QColor(16, 16, 16)));
    color_schemes[Color_Scheme::Dark].insert(std::make_pair(QPalette::ColorRole::Dark, QColor(0, 180, 175)));
    color_schemes[Color_Scheme::Dark].insert(std::make_pair(QPalette::ColorRole::Highlight, QColor(64, 64, 64)));
    color_schemes[Color_Scheme::Dark].insert(std::make_pair(QPalette::ColorRole::HighlightedText, QColor(0, 150, 145)));
    color_schemes[Color_Scheme::Dark].insert(std::make_pair(QPalette::ColorRole::ButtonText, QColor(0, 0 ,0)));
    color_schemes[Color_Scheme::Dark].insert(std::make_pair(QPalette::ColorRole::ToolTipBase, QColor(98, 98, 98)));
    color_schemes[Color_Scheme::Dark].insert(std::make_pair(QPalette::ColorRole::ToolTipText, QColor(205, 205 , 205)));
    color_schemes[Color_Scheme::Dark].insert(std::make_pair(QPalette::ColorRole::BrightText, QColor(255, 255 , 255)));
    color_schemes[Color_Scheme::Dark].insert(std::make_pair(QPalette::ColorRole::Mid, QColor(215, 215 , 215)));
    color_schemes[Color_Scheme::Dark].insert(std::make_pair(QPalette::ColorRole::Midlight, QColor(48, 48, 48)));
    color_schemes[Color_Scheme::Dark].insert(std::make_pair(QPalette::ColorRole::Shadow, QColor(0, 0, 0)));

    color_schemes[Color_Scheme::Light].insert(std::make_pair(QPalette::ColorRole::Window, QColor(200, 200, 200)));
    color_schemes[Color_Scheme::Light].insert(std::make_pair(QPalette::ColorRole::Background, QColor(234, 234, 234)));
    color_schemes[Color_Scheme::Light].insert(std::make_pair(QPalette::ColorRole::WindowText, QColor(55, 55, 55)));
    color_schemes[Color_Scheme::Light].insert(std::make_pair(QPalette::ColorRole::Foreground, QColor(55, 55, 55)));
    color_schemes[Color_Scheme::Light].insert(std::make_pair(QPalette::ColorRole::Base, QColor(225, 225, 225)));
    color_schemes[Color_Scheme::Light].insert(std::make_pair(QPalette::ColorRole::AlternateBase, QColor(240, 240, 240)));
    color_schemes[Color_Scheme::Light].insert(std::make_pair(QPalette::ColorRole::Text, QColor(55, 55, 55)));
    color_schemes[Color_Scheme::Light].insert(std::make_pair(QPalette::ColorRole::Button, QColor(220, 220, 220)));
    color_schemes[Color_Scheme::Light].insert(std::make_pair(QPalette::ColorRole::Dark, QColor(190, 190, 190)));
    color_schemes[Color_Scheme::Light].insert(std::make_pair(QPalette::ColorRole::Highlight, QColor(64, 64, 64)));
    color_schemes[Color_Scheme::Light].insert(std::make_pair(QPalette::ColorRole::HighlightedText, QColor(240, 240, 240)));
    color_schemes[Color_Scheme::Light].insert(std::make_pair(QPalette::ColorRole::ButtonText, QColor(255, 255,255)));
    color_schemes[Color_Scheme::Light].insert(std::make_pair(QPalette::ColorRole::ToolTipBase, QColor(205, 205, 205)));
    color_schemes[Color_Scheme::Light].insert(std::make_pair(QPalette::ColorRole::ToolTipText, QColor(98, 98, 98)));
    color_schemes[Color_Scheme::Light].insert(std::make_pair(QPalette::ColorRole::BrightText, QColor(0, 0, 0)));
    color_schemes[Color_Scheme::Light].insert(std::make_pair(QPalette::ColorRole::Mid, QColor(48, 48 , 48)));
    color_schemes[Color_Scheme::Light].insert(std::make_pair(QPalette::ColorRole::Midlight, QColor(200, 200, 200)));
    color_schemes[Color_Scheme::Light].insert(std::make_pair(QPalette::ColorRole::Shadow, QColor(130, 130, 130)));

    color_schemes[Color_Scheme::Blue].insert(std::make_pair(QPalette::ColorRole::Window, QColor(20,55,103)));
    color_schemes[Color_Scheme::Blue].insert(std::make_pair(QPalette::ColorRole::Background, QColor(20,55,103)));
    color_schemes[Color_Scheme::Blue].insert(std::make_pair(QPalette::ColorRole::WindowText, QColor(112, 170, 229)));
    color_schemes[Color_Scheme::Blue].insert(std::make_pair(QPalette::ColorRole::Foreground, QColor(112, 170, 229)));
    color_schemes[Color_Scheme::Blue].insert(std::make_pair(QPalette::ColorRole::Base, QColor(11, 32, 55)));
    color_schemes[Color_Scheme::Blue].insert(std::make_pair(QPalette::ColorRole::AlternateBase, QColor(212, 234, 254)));
    color_schemes[Color_Scheme::Blue].insert(std::make_pair(QPalette::ColorRole::Text, QColor(112, 170, 229)));
    color_schemes[Color_Scheme::Blue].insert(std::make_pair(QPalette::ColorRole::Button, QColor(11, 32, 55)));
    color_schemes[Color_Scheme::Blue].insert(std::make_pair(QPalette::ColorRole::Dark, QColor(0,105,200)));
    color_schemes[Color_Scheme::Blue].insert(std::make_pair(QPalette::ColorRole::Highlight, QColor(29, 77, 144)));
    color_schemes[Color_Scheme::Blue].insert(std::make_pair(QPalette::ColorRole::HighlightedText, QColor(0, 150, 145)));
    color_schemes[Color_Scheme::Blue].insert(std::make_pair(QPalette::ColorRole::ButtonText, QColor(212, 234 ,254)));
    color_schemes[Color_Scheme::Blue].insert(std::make_pair(QPalette::ColorRole::ToolTipBase, QColor(20, 55, 103)));
    color_schemes[Color_Scheme::Blue].insert(std::make_pair(QPalette::ColorRole::ToolTipText, QColor(112, 234 , 254)));
    color_schemes[Color_Scheme::Blue].insert(std::make_pair(QPalette::ColorRole::BrightText, QColor(212, 234, 254)));
    color_schemes[Color_Scheme::Blue].insert(std::make_pair(QPalette::ColorRole::Mid, QColor(112, 170, 229)));
    color_schemes[Color_Scheme::Blue].insert(std::make_pair(QPalette::ColorRole::Midlight, QColor(20,55,103)));
    color_schemes[Color_Scheme::Blue].insert(std::make_pair(QPalette::ColorRole::Shadow, QColor(5, 16, 28)));
}


void applyMainWindowPalette(MainWindow *main_window, Color_Scheme new_color)
{
    // ********** Apply palette to main window
    QPalette palette = main_window->palette();
    palette.setColor(QPalette::ColorRole::Window, (color_schemes[new_color])[QPalette::ColorRole::Window]);                      // Background color
    palette.setColor(QPalette::ColorRole::Background, (color_schemes[new_color])[QPalette::ColorRole::Background]);              // Background color, obsolete?
    palette.setColor(QPalette::ColorRole::WindowText, (color_schemes[new_color])[QPalette::ColorRole::WindowText]);              // Header and label font color
    palette.setColor(QPalette::ColorRole::Foreground, (color_schemes[new_color])[QPalette::ColorRole::Foreground]);              // Header and label font color, obsolete?
    palette.setColor(QPalette::ColorRole::Base, (color_schemes[new_color])[QPalette::ColorRole::Base]);                          // Alternate background color
    palette.setColor(QPalette::ColorRole::AlternateBase, (color_schemes[new_color])[QPalette::ColorRole::AlternateBase]);        // Alternate background color in QTable
    palette.setColor(QPalette::ColorRole::Text, (color_schemes[new_color])[QPalette::ColorRole::Text]);                          // Button Text color
    palette.setColor(QPalette::ColorRole::Button, (color_schemes[new_color])[QPalette::ColorRole::Button]);                      // Header background gradient top color
    palette.setColor(QPalette::ColorRole::Dark, (color_schemes[new_color])[QPalette::ColorRole::Dark]);                          // Header background gradient bottom color
    palette.setColor(QPalette::ColorRole::Highlight, (color_schemes[new_color])[QPalette::ColorRole::Highlight]);                // List selection highlight color
    palette.setColor(QPalette::ColorRole::HighlightedText, (color_schemes[new_color])[QPalette::ColorRole::HighlightedText]);    // List selection highlight text color
    palette.setColor(QPalette::ColorRole::ButtonText, (color_schemes[new_color])[QPalette::ColorRole::ButtonText]);              // Button text, ** what is QPushButton background color??
    palette.setColor(QPalette::ColorRole::ToolTipBase, (color_schemes[new_color])[QPalette::ColorRole::ToolTipBase]);            // Tooltips
    palette.setColor(QPalette::ColorRole::ToolTipText, (color_schemes[new_color])[QPalette::ColorRole::ToolTipText]);            // Tooltips
    main_window->setPalette(palette);


    // ********** Custom blue dot image for QSplitter (horizontal)
    QString splitColor = QString(" QSplitter { width: 4px; } QSplitter::handle { image: url(:/tree_icons/splitter_h.png); } ");
    main_window->getUi()->splitterHorizontal->setStyleSheet(splitColor);

    QColor temp1, temp2, temp3, temp4, temp5, temp6;

    // ********** Custom coloring for QPushButtons
    temp1 = color_schemes[current_color_scheme][QPalette::ColorRole::Mid];
    temp2 = color_schemes[current_color_scheme][QPalette::ColorRole::Midlight];
    temp3 = color_schemes[current_color_scheme][QPalette::ColorRole::Base];
    temp4 = color_schemes[current_color_scheme][QPalette::ColorRole::Button];
    temp5 = color_schemes[current_color_scheme][QPalette::ColorRole::BrightText];
    QString buttonColor = QString(" QPushButton { color: " + temp1.name() + "; "
                                                " background: qlineargradient(spread:pad, x1:0 y1:0, x2:0 y2:1, stop:0 " + temp2.name() + ", stop:1 " + temp3.name() + "); "
                                                " border: none; border-radius: 6px; }"
                                  " QPushButton:hover:!pressed { color: " + temp5.name() + "; background: " + temp2.name() + "; }"
                                  " QPushButton:pressed { color: " + temp5.name() + "; background: " + temp4.name() + "; }");
    main_window->getUi()->buttonAtlas->setStyleSheet(buttonColor);
    main_window->getUi()->buttonFonts->setStyleSheet(buttonColor);
    main_window->getUi()->buttonPlay->setStyleSheet(buttonColor);
    main_window->getUi()->buttonSettings->setStyleSheet(buttonColor);
    main_window->getUi()->buttonWorlds->setStyleSheet(buttonColor);

    // Shadow effect for buttons
    QGraphicsDropShadowEffect* effect[5];
    for (int i = 0; i < 5; i++)
    {
        effect[i] = new QGraphicsDropShadowEffect();
        effect[i]->setBlurRadius(6);
        effect[i]->setOffset(0,3);
        effect[i]->setColor(color_schemes[current_color_scheme][QPalette::ColorRole::Shadow]);
    }
    main_window->getUi()->buttonAtlas->setGraphicsEffect(effect[0]);
    main_window->getUi()->buttonFonts->setGraphicsEffect(effect[1]);
    main_window->getUi()->buttonPlay->setGraphicsEffect(effect[2]);
    main_window->getUi()->buttonSettings->setGraphicsEffect(effect[3]);
    main_window->getUi()->buttonWorlds->setGraphicsEffect(effect[4]);


    // ********** Custom coloring for QTreeWidget
    temp1 = color_schemes[current_color_scheme][QPalette::ColorRole::Foreground];
    temp2 = color_schemes[current_color_scheme][QPalette::ColorRole::Base];
    temp3 = color_schemes[current_color_scheme][QPalette::ColorRole::Highlight];
    temp4 = color_schemes[current_color_scheme][QPalette::ColorRole::HighlightedText];
    temp5 = color_schemes[current_color_scheme][QPalette::ColorRole::Dark];
    temp6 = color_schemes[current_color_scheme][QPalette::ColorRole::BrightText];
    QString listColor = QString(" QTreeWidget             { color: " + temp1.name() + ";  background: " + temp2.name() + "; selection-background-color: " + temp3.name() + "; }"
                                " QTreeWidget::item:selected { color: " + temp4.name() + "; background: " + temp3.name() + "; }"
                                " QTreeWidget::item:hover:selected { color: " + temp5.name() + ";  background: " + temp3.name() + "; }"
                                " QTreeWidget::item:hover:!selected { color: " + temp6.name() + ";  background: " + temp2.name() + "; }"
                                " QHeaderView::section { background-color: " + temp2.name() + "; border: 0px; }");
    main_window->getSceneListWidget()->header()->setStyleSheet(listColor);
    main_window->getSceneListWidget()->setStyleSheet(listColor);


}










