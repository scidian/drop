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

#include "editor/colors/colors.h"
#include "editor/debug.h"
#include "editor/docks/docks.h"
#include "editor/forms/form_main.h"
#include "editor/globals_editor.h"
#include "editor/helper_library.h"
#include "editor/project/project.h"
#include "editor/style/style.h"
#include "editor/trees/tree_advisor.h"
#include "editor/trees/tree_assets.h"
#include "editor/trees/tree_inspector.h"
#include "editor/trees/tree_project.h"
#include "editor/view/editor_item.h"
#include "editor/view/editor_scene.h"
#include "editor/view/editor_view.h"
#include "model/enums_model_types.h"
#include "model/project/project.h"
#include "model/project/project_asset.h"
#include "model/project/project_world.h"
#include "model/project/project_stage.h"
#include "model/project/project_thing.h"
#include "model/settings/settings.h"
#include "model/settings/settings_component.h"
#include "model/settings/settings_component_property.h"


//####################################################################################
//##    Constructor / Destructor for Main Window
//####################################################################################
FormMain::FormMain(QWidget *parent, std::string file_to_open) : QMainWindow(parent) {

    // ********** Initialize new project, load DrProject options
    m_project = new DrProject();

    // Try to open command line file, otherwise initialize empty Project
    bool init_new = (file_to_open == "");
    if (init_new == false) {
        Dr::AddBuiltInImages(m_project);
        init_new = (m_project->openProjectFromFile(file_to_open) == false);
    }
    if (init_new) Dr::InitializeNewProject(m_project, "New Project", Orientation::Portrait, 800, 1600, Dr::CheckDebugFlag(Debug_Flags::Load_Test_Project));

    // ********* Initialize form and customize colors and styles
    this->setStyleSheet( Dr::CustomStyleSheetFormatting() );
    initializeFormMain();

    // ********** Loads project into FormMain
    setFormMainMode( Form_Main_Mode::World_Editor );
}

// Destructor Deletes all Widgets so that it can then safely delete Project
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
    this->setStyleSheet( Dr::CustomStyleSheetFormatting() );

    // When changing from Qt 5.12 to Qt 5.13.1 style sheet propagation seemed to stop working, tried the following things:
    ///     QCoreApplication::setAttribute(Qt::AA_UseStyleSheetPropagationInWidgetStyles, true);
    ///     this->setAttribute(Qt::WA_WindowPropagation, true);
    ///     this->ensurePolished();
    // But didn't work, so now we loop through all the children QWidgets and Polish / Update them...
    ///     for (auto widget : findChildren<QWidget *>()) {
    ///         widget->style()->unpolish(widget);
    ///         widget->style()->polish(widget);
    ///         widget->update();
    ///     }
    // !!!!! UPDATE: Fixed in Qt 5.14

    if (m_current_mode == Form_Main_Mode::World_Editor) {
        buildAssetTree();
        treeProjectEditor->buildProjectTree(true);
        buildInspector( { treeInspector->getSelectedKey() }, true );
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
    // Ask to save
    if (!askShouldSave("closing the application")) {
        event->ignore();
        return;
    }

    // Accept close event
    event->accept();

    // Close everything and terminate app
    qApp->closeAllWindows();
    qApp->quit();
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












