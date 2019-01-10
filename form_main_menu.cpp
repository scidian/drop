//
//      Created by Stephens Nunnally on 1/3/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Calls to set up, initialize, build Form Main menu system
//
//

#include "editor_scene_scene.h"
#include "editor_tree_advisor.h"
#include "editor_tree_assets.h"
#include "editor_tree_inspector.h"
#include "editor_tree_scene.h"
#include "editor_scene_view.h"

#include "form_main.h"



//####################################################################################
//##        Sets up FormMain menu system
//####################################################################################
void FormMain::buildMenu()
{
    // ***** Main window settings
    this->setObjectName(QStringLiteral("formMain"));
    this->setWindowModality(Qt::NonModal);
    this->resize(1300, 800);
    this->setMinimumSize(QSize(780, 400));
    this->setMouseTracking(true);
    this->setAcceptDrops(true);
    this->setWindowIcon(QIcon(":icon/icon256.png"));                        // Set icon

    // ***** Create menu bar
    menuBar = new QMenuBar(this);
    menuBar->setObjectName(QStringLiteral("menuBar"));
    menuBar->setGeometry(QRect(0, 0, 1100, 22));

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

    menuBar->addAction(menuColor_Schemes->menuAction());
    menuColor_Schemes->addAction(actionDark);
    menuColor_Schemes->addAction(actionLight);
    menuColor_Schemes->addAction(actionBlue);
    menuColor_Schemes->addAction(actionAutumn);

    // ***** Debug Menu
    if (globals->show_debug) {
        QMenu *menuDebug;
        menuDebug = new QMenu(menuBar);
        menuDebug->setObjectName(QStringLiteral("menuDebug"));
        QAction *actionClearMain, *actionSceneEditMode, *actionListChildren;
        actionClearMain =   new QAction(this);    actionClearMain->setObjectName(QStringLiteral("actionClearMain"));
        actionSceneEditMode =  new QAction(this); actionSceneEditMode->setObjectName(QStringLiteral("actionSceneEditMode"));
        actionListChildren =  new QAction(this);  actionSceneEditMode->setObjectName(QStringLiteral("actionListChildren"));

        menuBar->addAction(menuDebug->menuAction());
        menuDebug->addAction(actionClearMain);
        menuDebug->addAction(actionSceneEditMode);
        menuDebug->addAction(actionListChildren);

        connect(actionClearMain, &QAction::triggered, [this]() { buildWindow(Form_Main_Mode::Clear); });
        connect(actionSceneEditMode, &QAction::triggered, [this]() { buildWindow(Form_Main_Mode::Edit_Scene); });
        connect(actionListChildren, &QAction::triggered, [this]() { listChildren(); });

        menuDebug->setTitle(QApplication::translate("MainWindow", "Debug", nullptr));
        actionClearMain->setText(QApplication::translate("MainWindow", "Clear Form Main Widgets", nullptr));
        actionSceneEditMode->setText(QApplication::translate("MainWindow", "Set Form Main Mode: Edit Scene", nullptr));
        actionListChildren->setText(QApplication::translate("MainWindow", "List Children", nullptr));
    }

    // ***** Set menu titles and sub menu texts
    menuColor_Schemes->setTitle(QApplication::translate("MainWindow", "Color Schemes", nullptr));
    actionDark->setText(QApplication::translate("MainWindow", "Dark", nullptr));
    actionLight->setText(QApplication::translate("MainWindow", "Light", nullptr));
    actionBlue->setText(QApplication::translate("MainWindow", "Blue", nullptr));
    actionAutumn->setText(QApplication::translate("MainWindow", "Autumn", nullptr));

    this->setMenuBar(menuBar);
}










