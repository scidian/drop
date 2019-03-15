//
//      Created by Stephens Nunnally on 1/3/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Calls to set up, initialize, build Form Main menu system
//
//
#include <QApplication>
#include <QActionGroup>
#include <QMenuBar>

#include "colors.h"
#include "debug.h"

#include "editor_item.h"
#include "editor_scene.h"
#include "editor_tree_advisor.h"
#include "editor_tree_assets.h"
#include "editor_tree_inspector.h"
#include "editor_tree_project.h"
#include "editor_view.h"

#include "form_main.h"

#include "library.h"


//####################################################################################
//##        Main Menu Bar Functions
//####################################################################################
void FormMain::menuAbout() {
    QMessageBox::about(this, tr("About Drop"), tr("<b>Drop Creator</b> A Drag and Drop Game Maker by Stephens Nunnally"));
}

void FormMain::menuUndo() {
        sceneEditor->undoAction();
}
void FormMain::menuRedo() {
        sceneEditor->redoAction();
}

// Pops up a message box listing all child widgets of FormMain
void FormMain::menuListChildren() {
    QString widget_list;
    for (auto widget : findChildren<QWidget *>()) {
        widget_list += widget->objectName() + ", ";
    }
    Dr::ShowMessageBox(widget_list);
}

// SLOT: Updates Undo / Redo text
void FormMain::editMenuAboutToShow()
{
    QString undo_text = sceneEditor->getCurrentUndo();
    QString redo_text = sceneEditor->getCurrentRedo();
    actionUndo->setText(undo_text);
    actionRedo->setText(redo_text);

    if (undo_text.isEmpty()) {
        actionUndo->setText("&Undo");
        actionUndo->setEnabled(false);
    }
    if (redo_text.isEmpty()) {
        actionRedo->setText("&Redo");
        actionRedo->setEnabled(false);
    }
}

void FormMain::editMenuAboutToHide() {
    actionUndo->setEnabled(true);
    actionRedo->setEnabled(true);
}

//####################################################################################
//##        Sets up FormMain menu system
//####################################################################################
void FormMain::buildMenu()
{
    // ***** Create menu bar
    menuBar = new QMenuBar(this);
    menuBar->setObjectName(QStringLiteral("menuBar"));
    menuBar->setGeometry(QRect(0, 0, 1100, 22));


    QMenu *menuFile;
    QAction *actionExit;
    menuFile = new QMenu(menuBar);
    menuFile->setObjectName(QStringLiteral("menuFile"));
    actionExit = new QAction(this);    actionExit->setObjectName(QStringLiteral("actionExit"));
    actionExit->setShortcuts(QKeySequence::Quit);
    connect(actionExit, SIGNAL(triggered()), this, SLOT(close()));
    menuBar->addAction(menuFile->menuAction());
    menuFile->addAction(actionExit);

    QMenu *menuEdit;
    menuEdit = new QMenu(menuBar);
    menuEdit->setObjectName(QStringLiteral("menuEdit"));
    connect(menuEdit, SIGNAL(aboutToShow()), this, SLOT(editMenuAboutToShow()));
    connect(menuEdit, SIGNAL(aboutToHide()), this, SLOT(editMenuAboutToHide()));
    actionUndo = new QAction(this);     actionUndo->setObjectName(QStringLiteral("actionUndo"));
    actionRedo = new QAction(this);     actionRedo->setObjectName(QStringLiteral("actionRedo"));
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

        QAction *actionListChildren =  new QAction(this); actionListChildren->setObjectName(QStringLiteral("actionListChildren"));

        menuBar->addAction(menuDebug->menuAction());
        menuDebug->addAction(actionListChildren);

        connect(actionListChildren, &QAction::triggered, [this]() { this->menuListChildren(); });

        menuDebug->setTitle(QApplication::translate("MainWindow", "Debug", nullptr));
        actionListChildren->setText(QApplication::translate("MainWindow", "List Children", nullptr));
    }
    // !!!!!

    // ***** Set menu titles and sub menu texts
    menuFile->setTitle(QApplication::translate("MainWindow", "&File", nullptr));
    actionExit->setText(QApplication::translate("MainWindow", "E&xit", nullptr));

    menuEdit->setTitle(QApplication::translate("MainWindow", "&Edit", nullptr));
    actionUndo->setText(QApplication::translate("MainWindow", "&Undo", nullptr));
    actionRedo->setText(QApplication::translate("MainWindow", "&Redo", nullptr));

    menuColor_Schemes->setTitle(QApplication::translate("MainWindow", "Color Schemes", nullptr));
    actionDark->setText(QApplication::translate("MainWindow", "Dark", nullptr));
    actionLight->setText(QApplication::translate("MainWindow", "Light", nullptr));
    actionBlue->setText(QApplication::translate("MainWindow", "Blue", nullptr));
    actionAutumn->setText(QApplication::translate("MainWindow", "Autumn", nullptr));

    menuHelp->setTitle(QApplication::translate("MainWindow", "&Help", nullptr));
    actionAbout->setText(QApplication::translate("MainWindow", "&About", nullptr));

    this->setMenuBar(menuBar);
}











