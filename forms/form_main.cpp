//
//      Created by Stephens Nunnally on 12/7/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Loads, styles and handles events for FormMain
//
//
#include <QApplication>
#include <QDockWidget>
#include <QKeyEvent>
#include <QTimer>

#include "colors/colors.h"
#include "debug.h"
#include "enums.h"
#include "editor/tree_advisor.h"
#include "editor/tree_assets.h"
#include "editor/tree_inspector.h"
#include "editor/tree_project.h"
#include "editor_view/editor_item.h"
#include "editor_view/editor_scene.h"
#include "editor_view/editor_view.h"
#include "form_main.h"
#include "globals.h"
#include "helper.h"
#include "project/project.h"
#include "project/project_asset.h"
#include "project/project_world.h"
#include "project/project_world_stage.h"
#include "project/project_world_stage_thing.h"
#include "settings/settings.h"
#include "settings/settings_component.h"
#include "settings/settings_component_property.h"
#include "style/style.h"


//####################################################################################
//##    Constructor / Destructor for Main Window
//####################################################################################
FormMain::FormMain(QWidget *parent) : QMainWindow(parent) {

    // ********** Initialize new project, load DrProject options
    m_project = new DrProject();
    m_project->initializeNewProject("Rocky Rover", Orientation::Portrait, 800, 1600, true);

    // ********* Initialize form and customize colors and styles
    this->setStyleSheet( Dr::CustomStyleSheetFormatting() );
    initializeFormMainSettings();
    buildMenu();
    buildToolBar();
    buildWidgetsShared();
    buildWidgetsEditor();
    initializeDockWidgets();

    // ********** Loads project into FormMain
    setFormMainMode( Form_Main_Mode::World_Editor );
}


FormMain::~FormMain() {
    // Wait until scene is not being changed, then delete view and scene
    qApp->blockSignals(true);
    while (sceneEditor->scene_mutex.tryLock() == false)
        qApp->processEvents();
    sceneEditor->clearStageShown();
    sceneEditor->scene_mutex.unlock();
    sceneEditor->deleteLater();

    // Delete widgets not currently attached to main form
    if (m_current_mode != Form_Main_Mode::World_Editor)   widgetCentralEditor->deleteLater();
    if (m_current_mode != Form_Main_Mode::Clear)          widgetCentral->deleteLater();
    delete m_project;
}


//####################################################################################
//##    Populates DrScene with passed in DrStage when Program is done loading
//####################################################################################
void FormMain::buildSceneAfterLoading(long stage_key) {
    if (Dr::CheckDoneLoading() == false) {
        QTimer::singleShot( 100, this, [this, stage_key] { this->buildSceneAfterLoading(stage_key); } );
        return;
    }
    buildScene( stage_key );
}


//####################################################################################
//##    Sets the new palette to the style sheets
//####################################################################################
void FormMain::changePalette(Color_Scheme new_color_scheme) {
    Dr::SetColorScheme(new_color_scheme);

    // When changing from Qt 5.12 to Qt 5.13.1 style sheet propagation seemed to stop working, tried the following things:
    ///     QCoreApplication::setAttribute(Qt::AA_UseStyleSheetPropagationInWidgetStyles, true);
    ///     this->setAttribute(Qt::WA_WindowPropagation, true);
    ///     this->ensurePolished();
    // but didn't work, so now we loop through all the children QWidgets and Polish / Update them...

    this->setStyleSheet( Dr::CustomStyleSheetFormatting() );

    for (auto widget : findChildren<QWidget *>()) {
        widget->style()->unpolish(widget);
        widget->style()->polish(widget);
        widget->update();
    }

    if (m_current_mode == Form_Main_Mode::World_Editor) {
        buildAssetTree();
        buildProjectTree();
        treeInspector->buildInspectorFromKeys( { treeInspector->getSelectedKey() }, true );
        viewEditor->updateGrid();
    }
}



//####################################################################################
//##    FormMain Wide Event Filter, catches all events
//####################################################################################
bool FormMain::eventFilter(QObject *obj, QEvent *event) {

    // ********** Catch space bar for view to make sure we can drag even if view didnt have focus
    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->key() == Qt::Key::Key_Space && m_current_mode == Form_Main_Mode::World_Editor)
            if (viewEditor->hasFocus() == false)
                viewEditor->spaceBarDown();
    }
    if (event->type() == QEvent::KeyRelease)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->key() == Qt::Key::Key_Space && m_current_mode == Form_Main_Mode::World_Editor)
            if (viewEditor->hasFocus() == false)
                viewEditor->spaceBarUp();
    }

    return QObject::eventFilter(obj, event);
}

// Overrides close event to make sure all rogue windows are closed
void FormMain::closeEvent(QCloseEvent *event) {
    qApp->closeAllWindows();
    event->accept();
}

// Overrides resize event to keep toolbar proper width
void FormMain::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);
    widgetToolbar->setFixedWidth( this->width() );
}


//####################################################################################
//##    Sets the text of a label on FormMain, can be called globally from Dr::SetLabelText
//####################################################################################
void FormMain::setLabelText(Label_Names label_name, QString new_text) {
    if (Dr::CheckDoneLoading() == false) return;

    switch (label_name) {
        case Label_Names::Label_1:          label1->setText(new_text);         break;
        case Label_Names::Label_2:          label2->setText(new_text);         break;
        case Label_Names::Label_3:          label3->setText(new_text);         break;
        case Label_Names::Label_Mouse_1:    labelMouse1->setText(new_text);   break;
        case Label_Names::Label_Mouse_2:    labelMouse2->setText(new_text);   break;

        case Label_Names::Label_Object_1:   labelObject1->setText(new_text);  break;
        case Label_Names::Label_Object_2:   labelObject2->setText(new_text);  break;
        case Label_Names::Label_Object_3:   labelObject3->setText(new_text);  break;
        case Label_Names::Label_Object_4:   labelObject4->setText(new_text);  break;
        case Label_Names::Label_Object_5:   labelObject5->setText(new_text);  break;

        case Label_Names::Label_Position:   labelPosition->setText(new_text);  break;
        case Label_Names::Label_Center:     labelCenter->setText(new_text);    break;
        case Label_Names::Label_Scale:      labelScale->setText(new_text);     break;
        case Label_Names::Label_Rotate:     labelRotate->setText(new_text);    break;
        case Label_Names::Label_Z_Order:    labelZOrder->setText(new_text);   break;
        case Label_Names::Label_Pos_Flag:   labelPosFlag->setText(new_text);  break;

        case Label_Names::Label_Bottom:     labelBottom->setText(new_text);    break;
    }
}












