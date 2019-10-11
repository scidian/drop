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
#include "editor/editor_tree_advisor.h"
#include "editor/editor_tree_assets.h"
#include "editor/editor_tree_inspector.h"
#include "editor/editor_tree_project.h"
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
        switch (Dr::GetColorScheme()) {
            case Color_Scheme::Dark:    actionDark->setChecked(true);    break;
            case Color_Scheme::Light:   actionLight->setChecked(true);   break;
            case Color_Scheme::Blue:    actionBlue->setChecked(true);    break;
            case Color_Scheme::Autumn:  actionAutumn->setChecked(true);  break;
        }
    // Instead of traditional SIGNAL to SLOT connect, we can "connect" inline lamda functions directly
    //      to signals. This allows for passing of variables not included in the SIGNAL that was fired.
    // Such as in this instance, passing a new Color_Scheme to FormMain::changePalette)
    connect(actionDark,   &QAction::triggered, [this]() { changePalette(Color_Scheme::Dark); });
    connect(actionLight,  &QAction::triggered, [this]() { changePalette(Color_Scheme::Light); });
    connect(actionBlue,   &QAction::triggered, [this]() { changePalette(Color_Scheme::Blue); });
    connect(actionAutumn, &QAction::triggered, [this]() { changePalette(Color_Scheme::Autumn); });

    menuBar->addAction(menuColor_Schemes->menuAction());
    menuColor_Schemes->addAction(actionDark);
    menuColor_Schemes->addAction(actionLight);
    menuColor_Schemes->addAction(actionBlue);
    menuColor_Schemes->addAction(actionAutumn);

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
    actionBlue->setText(QApplication::translate("MainWindow",               "Blue", nullptr));
    actionAutumn->setText(QApplication::translate("MainWindow",             "Autumn", nullptr));

    menuHelp->setTitle(QApplication::translate("MainWindow",            "&Help", nullptr));
    actionAbout->setText(QApplication::translate("MainWindow",              "&About", nullptr));

    this->setMenuBar(menuBar);
}





