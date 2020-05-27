//
//      Created by Stephens Nunnally on 10/10/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QApplication>
#include <QActionGroup>
#include <QDesktopServices>
#include <QFileDialog>
#include <QMenuBar>
#include <QMessageBox>

#include "core/colors/colors.h"
#include "editor/form_main/form_main.h"
#include "editor/helper_library.h"
#include "editor/trees/tree_advisor.h"
#include "editor/trees/tree_assets.h"
#include "editor/trees/tree_inspector.h"
#include "editor/trees/tree_project.h"
#include "editor/view_editor/editor_item.h"
#include "editor/view_editor/editor_scene.h"
#include "editor/view_editor/editor_view.h"
#include "engine/debug_flags.h"
#include "project/dr_project.h"


//####################################################################################
//##    Sets up FormMain menu system
//####################################################################################
void FormMain::buildMenu() {

    // ***** Create menu bar
    m_menu_bar = new QMenuBar(this);
    m_menu_bar->setObjectName(QStringLiteral("menuBar"));
    m_menu_bar->setGeometry(QRect(0, 0, 1100, 22));


    QMenu *menu_file;
    QAction *action_new, *action_open, *action_save, *action_save_as, *action_exit;
    menu_file = new QMenu(m_menu_bar);
    menu_file->setObjectName(QStringLiteral("menuFile"));
    action_new =        new QAction(this);  action_new->setObjectName(QStringLiteral("actionNew"));
    action_open =       new QAction(this);  action_open->setObjectName(QStringLiteral("actionOpen"));
    action_save =       new QAction(this);  action_new->setObjectName(QStringLiteral("actionSave"));
    action_save_as =    new QAction(this);  action_new->setObjectName(QStringLiteral("actionSaveAs"));
    action_exit =       new QAction(this);  action_exit->setObjectName(QStringLiteral("actionExit"));
    action_new->setShortcuts(QKeySequence::New);
    action_open->setShortcuts(QKeySequence::Open);
    action_save->setShortcuts(QKeySequence::Save);
    action_save_as->setShortcuts(QKeySequence::SaveAs);
    action_exit->setShortcuts(QKeySequence::Quit);
    connect(action_new,     &QAction::triggered, [this]() { this->menuNew(); });
    connect(action_open,    &QAction::triggered, [this]() { this->menuOpen(); });
    connect(action_save,    &QAction::triggered, [this]() { this->menuSave(); });
    connect(action_save_as, &QAction::triggered, [this]() { this->menuSave(true); });
    connect(action_exit,       SIGNAL(triggered()), this, SLOT(close()));
    m_menu_bar->addAction(menu_file->menuAction());
    menu_file->addAction(action_new);
    menu_file->addAction(action_open);
    menu_file->addAction(action_save);
    menu_file->addAction(action_save_as);
    menu_file->addAction(action_exit);

    QMenu *menu_edit;
    menu_edit = new QMenu(m_menu_bar);
    menu_edit->setObjectName(QStringLiteral("menuEdit"));
    connect(menu_edit, SIGNAL(aboutToShow()), this, SLOT(editMenuAboutToShow()));
    connect(menu_edit, SIGNAL(aboutToHide()), this, SLOT(editMenuAboutToHide()));
    m_action_undo = new QAction(this);  m_action_undo->setObjectName(QStringLiteral("actionUndo"));
    m_action_redo = new QAction(this);  m_action_redo->setObjectName(QStringLiteral("actionRedo"));
    m_action_undo->setShortcuts(QKeySequence::Undo);
    m_action_redo->setShortcuts(QKeySequence::Redo);
    connect(m_action_undo,   &QAction::triggered, [this]() { this->menuUndo(); });
    connect(m_action_redo,   &QAction::triggered, [this]() { this->menuRedo(); });
    m_menu_bar->addAction(menu_edit->menuAction());
    menu_edit->addAction(m_action_undo);
    menu_edit->addAction(m_action_redo);

    // ***** Color Schemes sub menu
    QMenu *menu_color_schemes;
    menu_color_schemes = new QMenu(m_menu_bar);
    menu_color_schemes->setObjectName(QStringLiteral("menuColor_Schemes"));
    QAction *action_dark, *action_mid, *action_light, *action_navy, *action_grape, *action_rust, *action_coffee, *action_emerald;
    action_dark =    new QAction(this);  action_dark->setObjectName(QStringLiteral("actionDark"));
    action_mid =     new QAction(this);  action_mid->setObjectName(QStringLiteral("actionMid"));
    action_light =   new QAction(this);  action_light->setObjectName(QStringLiteral("actionLight"));
    action_navy =    new QAction(this);  action_navy->setObjectName(QStringLiteral("actionNavy"));
    action_grape =   new QAction(this);  action_grape->setObjectName(QStringLiteral("actionGrape"));
    action_rust =    new QAction(this);  action_rust->setObjectName(QStringLiteral("actionRust"));
    action_coffee =  new QAction(this);  action_coffee->setObjectName(QStringLiteral("actionCoffee"));
    action_emerald = new QAction(this);  action_emerald->setObjectName(QStringLiteral("actionEmerald"));
        QActionGroup *alignmentGroup;
        alignmentGroup = new QActionGroup(this);
        alignmentGroup->addAction(action_dark);
        alignmentGroup->addAction(action_mid);
        alignmentGroup->addAction(action_light);
        alignmentGroup->addAction(action_navy);
        alignmentGroup->addAction(action_grape);
        alignmentGroup->addAction(action_rust);
        alignmentGroup->addAction(action_coffee);
        alignmentGroup->addAction(action_emerald);
        alignmentGroup->setExclusive(true);
        action_dark->setCheckable(true);
        action_mid->setCheckable(true);
        action_light->setCheckable(true);
        action_navy->setCheckable(true);
        action_grape->setCheckable(true);
        action_rust->setCheckable(true);
        action_coffee->setCheckable(true);
        action_emerald->setCheckable(true);
        switch (Dr::GetColorScheme()) {
            case Color_Scheme::Dark:    action_dark->setChecked(true);       break;
            case Color_Scheme::Mid:     action_mid->setChecked(true);        break;
            case Color_Scheme::Light:   action_light->setChecked(true);      break;
            case Color_Scheme::Navy:    action_navy->setChecked(true);       break;
            case Color_Scheme::Grape:   action_grape->setChecked(true);      break;
            case Color_Scheme::Rust:    action_rust->setChecked(true);       break;
            case Color_Scheme::Coffee:  action_coffee->setChecked(true);     break;
            case Color_Scheme::Emerald: action_emerald->setChecked(true);    break;
        }
    // Instead of traditional SIGNAL to SLOT connect, we can "connect" inline lamda functions directly
    //      to signals. This allows for passing of variables not included in the SIGNAL that was fired.
    // Such as in this instance, passing a new Color_Scheme to FormMain::changePalette)
    connect(action_dark,     &QAction::triggered, [this]() {     changePalette(Color_Scheme::Dark);      });
    connect(action_mid,      &QAction::triggered, [this]() {     changePalette(Color_Scheme::Mid);       });
    connect(action_light,    &QAction::triggered, [this]() {     changePalette(Color_Scheme::Light);     });
    connect(action_navy,     &QAction::triggered, [this]() {     changePalette(Color_Scheme::Navy);      });
    connect(action_grape,    &QAction::triggered, [this]() {     changePalette(Color_Scheme::Grape);     });
    connect(action_rust,     &QAction::triggered, [this]() {     changePalette(Color_Scheme::Rust);      });
    connect(action_coffee,   &QAction::triggered, [this]() {     changePalette(Color_Scheme::Coffee);    });
    connect(action_emerald,  &QAction::triggered, [this]() {     changePalette(Color_Scheme::Emerald);   });

    m_menu_bar->addAction(menu_color_schemes->menuAction());
    menu_color_schemes->addAction(action_dark);
    menu_color_schemes->addAction(action_mid);
    menu_color_schemes->addAction(action_light);
    menu_color_schemes->addSeparator();
    menu_color_schemes->addAction(action_navy);
    menu_color_schemes->addAction(action_grape);
    menu_color_schemes->addAction(action_rust);
    menu_color_schemes->addAction(action_coffee);
    menu_color_schemes->addAction(action_emerald);

    QMenu *menu_help;
    QAction *action_help, *action_about;
    menu_help = new QMenu(m_menu_bar);
    menu_help->setObjectName(QStringLiteral("menuHelp"));
    action_help =   new QAction(this);  action_help->setObjectName(QStringLiteral("actionHelp"));
    action_about =  new QAction(this);  action_about->setObjectName(QStringLiteral("actionAbout"));
    action_help->setShortcut(QKeySequence::HelpContents);
    action_about->setShortcuts( { QKeySequence("Ctrl+A"), QKeySequence("Ctrl+B") } );
    connect(action_help, &QAction::triggered, []() { QDesktopServices::openUrl(QUrl("http://docs.drop.scidian.com")); });
    connect(action_about, &QAction::triggered, [this]() { this->menuAbout(); });
    m_menu_bar->addAction(menu_help->menuAction());
    menu_help->addAction(action_help);
    menu_help->addAction(action_about);

    // !!!!! #DEBUG:    Load hidden debug menu into menu bar
    if (Dr::CheckDebugFlag(Debug_Flags::Show_Secret_Menu)) {
        QMenu *menu_debug;
        menu_debug = new QMenu(m_menu_bar);
        menu_debug->setObjectName(QStringLiteral("menuDebug"));

        QAction *action_hidden, *action_test_project, *action_debug_labels, *action_list_children;
        action_hidden =         new QAction(this); action_hidden->setObjectName(QStringLiteral("actionHiddenProperties"));
        action_test_project =   new QAction(this); action_test_project->setObjectName(QStringLiteral("actionTestProject"));
        action_debug_labels =   new QAction(this); action_debug_labels->setObjectName(QStringLiteral("actionToggleLabels"));
        action_list_children =  new QAction(this); action_list_children->setObjectName(QStringLiteral("actionListChildren"));

        m_menu_bar->addAction(menu_debug->menuAction());
        menu_debug->addAction(action_hidden);
        menu_debug->addSeparator();
        menu_debug->addAction(action_test_project);
        menu_debug->addSeparator();
        menu_debug->addAction(action_debug_labels);
        menu_debug->addAction(action_list_children);

        action_hidden->setCheckable(true);
        action_hidden->setChecked( Dr::CheckDebugFlag(Debug_Flags::Show_Hidden_Component) );
        action_debug_labels->setCheckable(true);
        action_debug_labels->setChecked( Dr::CheckDebugFlag(Debug_Flags::Show_Hidden_Component) );

        connect(action_hidden, &QAction::triggered, []() { Dr::FlipDebugFlag(Debug_Flags::Show_Hidden_Component); });
        connect(action_test_project, &QAction::triggered, [this]() { this->menuNew(true); });
        connect(action_debug_labels, &QAction::triggered, [this]() {
            Dr::FlipDebugFlag(Debug_Flags::Show_Bottom_Debug_Labels);
            m_area_bottom->setVisible( Dr::CheckDebugFlag(Debug_Flags::Show_Bottom_Debug_Labels));
        });
        connect(action_list_children, &QAction::triggered, [this]() { this->menuListChildren(); });

        menu_debug->setTitle(QApplication::translate("MainWindow",       "Debug", nullptr));
        action_hidden->setText(QApplication::translate("MainWindow",         "Show Hidden Droplet Properties", nullptr));
        action_test_project->setText(QApplication::translate("MainWindow",    "Load Test Project", nullptr));
        action_debug_labels->setText(QApplication::translate("MainWindow",    "Show Scene View Debug Labels", nullptr));
        action_list_children->setText(QApplication::translate("MainWindow",   "List FormMain Children Widgets", nullptr));
    }
    // !!!!!

    // ***** Set menu titles and sub menu texts
    menu_file->setTitle(QApplication::translate("MainWindow",            "&File", nullptr));
    action_new->setText(QApplication::translate("MainWindow",                "&New Project", nullptr));
    action_open->setText(QApplication::translate("MainWindow",               "&Open Project", nullptr));
    action_save->setText(QApplication::translate("MainWindow",               "&Save Project", nullptr));
    action_save_as->setText(QApplication::translate("MainWindow",             "Save Project &As", nullptr));
    action_exit->setText(QApplication::translate("MainWindow",               "E&xit", nullptr));

    menu_edit->setTitle(QApplication::translate("MainWindow",            "&Edit", nullptr));
    m_action_undo->setText(QApplication::translate("MainWindow",               "&Undo", nullptr));
    m_action_redo->setText(QApplication::translate("MainWindow",               "&Redo", nullptr));

    menu_color_schemes->setTitle(QApplication::translate("MainWindow",   "Color Schemes", nullptr));
    action_dark->setText(QApplication::translate("MainWindow",               "Dark", nullptr));
    action_mid->setText(QApplication::translate("MainWindow",                "Mid", nullptr));
    action_light->setText(QApplication::translate("MainWindow",              "Light", nullptr));
    action_navy->setText(QApplication::translate("MainWindow",               "Navy", nullptr));
    action_grape->setText(QApplication::translate("MainWindow",              "Grape", nullptr));
    action_rust->setText(QApplication::translate("MainWindow",               "Rust", nullptr));
    action_coffee->setText(QApplication::translate("MainWindow",             "Coffee", nullptr));
    action_emerald->setText(QApplication::translate("MainWindow",            "Emerald", nullptr));

    menu_help->setTitle(QApplication::translate("MainWindow",            "&Help", nullptr));
    action_help->setText(QApplication::translate("MainWindow",               "&Drop Documentation", nullptr));
    action_about->setText(QApplication::translate("MainWindow",              "&About", nullptr));

    this->setMenuBar(m_menu_bar);
}





