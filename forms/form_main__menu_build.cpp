//
//      Created by Stephens Nunnally on 10/10/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QApplication>
#include <QActionGroup>
#include <QFileDialog>
#include <QMenuBar>
#include <QMessageBox>

#include "colors/colors.h"
#include "debug.h"
#include "editor/tree_advisor.h"
#include "editor/tree_assets.h"
#include "editor/tree_inspector.h"
#include "editor/tree_project.h"
#include "editor_view/editor_item.h"
#include "editor_view/editor_scene.h"
#include "editor_view/editor_view.h"
#include "form_main.h"
#include "helper.h"
#include "helper_qt.h"
#include "project/project.h"


//####################################################################################
//##    Sets up FormMain menu system
//####################################################################################
void FormMain::buildMenu() {

    // ***** Create menu bar
    menuBar = new QMenuBar(this);
    menuBar->setObjectName(QStringLiteral("menuBar"));
    menuBar->setGeometry(QRect(0, 0, 1100, 22));


    QMenu *menuFile;
    QAction *actionNew, *actionOpen, *actionSave, *actionSaveAs, *actionExit;
    menuFile = new QMenu(menuBar);
    menuFile->setObjectName(QStringLiteral("menuFile"));
    actionNew =     new QAction(this);  actionNew->setObjectName(QStringLiteral("actionNew"));
    actionOpen =    new QAction(this);  actionOpen->setObjectName(QStringLiteral("actionOpen"));
    actionSave =    new QAction(this);  actionNew->setObjectName(QStringLiteral("actionSave"));
    actionSaveAs =  new QAction(this);  actionNew->setObjectName(QStringLiteral("actionSaveAs"));
    actionExit =    new QAction(this);  actionExit->setObjectName(QStringLiteral("actionExit"));
    actionNew->setShortcuts(QKeySequence::New);
    actionOpen->setShortcuts(QKeySequence::Open);
    actionSave->setShortcuts(QKeySequence::Save);
    actionSaveAs->setShortcuts(QKeySequence::SaveAs);
    actionExit->setShortcuts(QKeySequence::Quit);
    connect(actionNew,      &QAction::triggered, [this]() { this->menuNew(); });
    connect(actionOpen,     &QAction::triggered, [this]() { this->menuOpen(); });
    connect(actionSave,     &QAction::triggered, [this]() { this->menuSave(); });
    connect(actionSaveAs,   &QAction::triggered, [this]() { this->menuSave(true); });
    connect(actionExit,     SIGNAL(triggered()), this, SLOT(close()));
    menuBar->addAction(menuFile->menuAction());
    menuFile->addAction(actionNew);
    menuFile->addAction(actionOpen);
    menuFile->addAction(actionSave);
    menuFile->addAction(actionSaveAs);
    menuFile->addAction(actionExit);

    QMenu *menuEdit;
    menuEdit = new QMenu(menuBar);
    menuEdit->setObjectName(QStringLiteral("menuEdit"));
    connect(menuEdit, SIGNAL(aboutToShow()), this, SLOT(editMenuAboutToShow()));
    connect(menuEdit, SIGNAL(aboutToHide()), this, SLOT(editMenuAboutToHide()));
    actionUndo =    new QAction(this);  actionUndo->setObjectName(QStringLiteral("actionUndo"));
    actionRedo =    new QAction(this);  actionRedo->setObjectName(QStringLiteral("actionRedo"));
    actionUndo->setShortcuts(QKeySequence::Undo);
    actionRedo->setShortcuts(QKeySequence::Redo);
    connect(actionUndo,   &QAction::triggered, [this]() { this->menuUndo(); });
    connect(actionRedo,   &QAction::triggered, [this]() { this->menuRedo(); });
    menuBar->addAction(menuEdit->menuAction());
    menuEdit->addAction(actionUndo);
    menuEdit->addAction(actionRedo);

    // ***** Color Schemes sub menu
    QMenu *menuColor_Schemes;
    menuColor_Schemes = new QMenu(menuBar);
    menuColor_Schemes->setObjectName(QStringLiteral("menuColor_Schemes"));
    QAction *actionDark, *actionLight, *actionNavy, *actionGrape, *actionRust, *actionCoffee, *actionEmerald;
    actionDark =    new QAction(this);  actionDark->setObjectName(QStringLiteral("actionDark"));
    actionLight =   new QAction(this);  actionLight->setObjectName(QStringLiteral("actionLight"));
    actionNavy =    new QAction(this);  actionNavy->setObjectName(QStringLiteral("actionNavy"));
    actionGrape =   new QAction(this);  actionGrape->setObjectName(QStringLiteral("actionGrape"));
    actionRust =    new QAction(this);  actionRust->setObjectName(QStringLiteral("actionRust"));
    actionCoffee =  new QAction(this);  actionCoffee->setObjectName(QStringLiteral("actionCoffee"));
    actionEmerald = new QAction(this);  actionEmerald->setObjectName(QStringLiteral("actionEmerald"));
        QActionGroup *alignmentGroup;
        alignmentGroup = new QActionGroup(this);
        alignmentGroup->addAction(actionDark);
        alignmentGroup->addAction(actionLight);
        alignmentGroup->addAction(actionNavy);
        alignmentGroup->addAction(actionGrape);
        alignmentGroup->addAction(actionRust);
        alignmentGroup->addAction(actionCoffee);
        alignmentGroup->addAction(actionEmerald);
        alignmentGroup->setExclusive(true);
        actionDark->setCheckable(true);
        actionLight->setCheckable(true);
        actionNavy->setCheckable(true);
        actionGrape->setCheckable(true);
        actionRust->setCheckable(true);
        actionCoffee->setCheckable(true);
        actionEmerald->setCheckable(true);
        switch (Dr::GetColorScheme()) {
            case Color_Scheme::Dark:    actionDark->setChecked(true);       break;
            case Color_Scheme::Light:   actionLight->setChecked(true);      break;
            case Color_Scheme::Navy:    actionNavy->setChecked(true);       break;
            case Color_Scheme::Grape:   actionGrape->setChecked(true);      break;
            case Color_Scheme::Rust:    actionRust->setChecked(true);       break;
            case Color_Scheme::Coffee:  actionCoffee->setChecked(true);     break;
            case Color_Scheme::Emerald: actionEmerald->setChecked(true);    break;
        }
    // Instead of traditional SIGNAL to SLOT connect, we can "connect" inline lamda functions directly
    //      to signals. This allows for passing of variables not included in the SIGNAL that was fired.
    // Such as in this instance, passing a new Color_Scheme to FormMain::changePalette)
    connect(actionDark,     &QAction::triggered, [this]() {     changePalette(Color_Scheme::Dark);      });
    connect(actionLight,    &QAction::triggered, [this]() {     changePalette(Color_Scheme::Light);     });
    connect(actionNavy,     &QAction::triggered, [this]() {     changePalette(Color_Scheme::Navy);      });
    connect(actionGrape,    &QAction::triggered, [this]() {     changePalette(Color_Scheme::Grape);     });
    connect(actionRust,     &QAction::triggered, [this]() {     changePalette(Color_Scheme::Rust);      });
    connect(actionCoffee,   &QAction::triggered, [this]() {     changePalette(Color_Scheme::Coffee);    });
    connect(actionEmerald,  &QAction::triggered, [this]() {     changePalette(Color_Scheme::Emerald);   });

    menuBar->addAction(menuColor_Schemes->menuAction());
    menuColor_Schemes->addAction(actionDark);
    menuColor_Schemes->addAction(actionLight);
    menuColor_Schemes->addSeparator();
    menuColor_Schemes->addAction(actionNavy);
    menuColor_Schemes->addAction(actionGrape);
    menuColor_Schemes->addAction(actionRust);
    menuColor_Schemes->addAction(actionCoffee);
    menuColor_Schemes->addAction(actionEmerald);

    QMenu *menuHelp;
    QAction *actionAbout;
    menuHelp = new QMenu(menuBar);
    menuHelp->setObjectName(QStringLiteral("menuHelp"));
    actionAbout = new QAction(this);    actionAbout->setObjectName(QStringLiteral("actionAbout"));
    QList<QKeySequence> aboutShortcuts;
    aboutShortcuts << tr("Ctrl+A") << tr("Ctrl+B");
    actionAbout->setShortcuts(aboutShortcuts);
    connect(actionAbout, &QAction::triggered, [this]() { this->menuAbout(); });
    menuBar->addAction(menuHelp->menuAction());
    menuHelp->addAction(actionAbout);

    // !!!!! #DEBUG:    Load hidden debug menu into menu bar
    if (Dr::CheckDebugFlag(Debug_Flags::Show_Secret_Menu)) {
        QMenu *menuDebug;
        menuDebug = new QMenu(menuBar);
        menuDebug->setObjectName(QStringLiteral("menuDebug"));

        QAction *actionHidden, *actionDebugLabels, *actionListChildren;
        actionHidden =          new QAction(this); actionHidden->setObjectName(QStringLiteral("actionHiddenProperties"));
        actionDebugLabels =     new QAction(this); actionDebugLabels->setObjectName(QStringLiteral("actionToggleLabels"));
        actionListChildren =    new QAction(this); actionListChildren->setObjectName(QStringLiteral("actionListChildren"));

        menuBar->addAction(menuDebug->menuAction());
        menuDebug->addAction(actionHidden);
        menuDebug->addSeparator();
        menuDebug->addAction(actionDebugLabels);
        menuDebug->addAction(actionListChildren);

        actionHidden->setCheckable(true);
        actionHidden->setChecked( Dr::CheckDebugFlag(Debug_Flags::Show_Hidden_Component) );
        actionDebugLabels->setCheckable(true);
        actionDebugLabels->setChecked( Dr::CheckDebugFlag(Debug_Flags::Show_Hidden_Component) );

        connect(actionHidden, &QAction::triggered, []() { Dr::FlipDebugFlag(Debug_Flags::Show_Hidden_Component); });
        connect(actionDebugLabels, &QAction::triggered, [this]() {
            Dr::FlipDebugFlag(Debug_Flags::Show_Bottom_Debug_Labels);
            areaBottom->setVisible( Dr::CheckDebugFlag(Debug_Flags::Show_Bottom_Debug_Labels));
        });
        connect(actionListChildren, &QAction::triggered, [this]() { this->menuListChildren(); });

        menuDebug->setTitle(QApplication::translate("MainWindow",       "Debug", nullptr));
        actionHidden->setText(QApplication::translate("MainWindow",         "Show Hidden Droplet Properties", nullptr));
        actionDebugLabels->setText(QApplication::translate("MainWindow",    "Show Scene View Debug Labels", nullptr));
        actionListChildren->setText(QApplication::translate("MainWindow",   "List FormMain Children Widgets", nullptr));
    }
    // !!!!!

    // ***** Set menu titles and sub menu texts
    menuFile->setTitle(QApplication::translate("MainWindow",            "&File", nullptr));
    actionNew->setText(QApplication::translate("MainWindow",                "&New Project", nullptr));
    actionOpen->setText(QApplication::translate("MainWindow",               "&Open Project", nullptr));
    actionSave->setText(QApplication::translate("MainWindow",               "&Save Project", nullptr));
    actionSaveAs->setText(QApplication::translate("MainWindow",             "Save Project &As", nullptr));
    actionExit->setText(QApplication::translate("MainWindow",               "E&xit", nullptr));

    menuEdit->setTitle(QApplication::translate("MainWindow",            "&Edit", nullptr));
    actionUndo->setText(QApplication::translate("MainWindow",               "&Undo", nullptr));
    actionRedo->setText(QApplication::translate("MainWindow",               "&Redo", nullptr));

    menuColor_Schemes->setTitle(QApplication::translate("MainWindow",   "Color Schemes", nullptr));
    actionDark->setText(QApplication::translate("MainWindow",               "Dark", nullptr));
    actionLight->setText(QApplication::translate("MainWindow",              "Light", nullptr));
    actionNavy->setText(QApplication::translate("MainWindow",               "Navy", nullptr));
    actionGrape->setText(QApplication::translate("MainWindow",              "Grape", nullptr));
    actionRust->setText(QApplication::translate("MainWindow",               "Rust", nullptr));
    actionCoffee->setText(QApplication::translate("MainWindow",             "Coffee", nullptr));
    actionEmerald->setText(QApplication::translate("MainWindow",            "Emerald", nullptr));

    menuHelp->setTitle(QApplication::translate("MainWindow",            "&Help", nullptr));
    actionAbout->setText(QApplication::translate("MainWindow",              "&About", nullptr));

    this->setMenuBar(menuBar);
}





