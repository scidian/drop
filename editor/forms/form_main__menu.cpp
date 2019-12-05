//
//      Created by Stephens Nunnally on 1/3/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Calls to set up, initialize, build Form Main menu system
//
//
#include <QApplication>
#include <QActionGroup>
#include <QDebug>
#include <QFileDialog>
#include <QMenuBar>
#include <QMessageBox>
#include <QStandardPaths>

#include "colors/colors.h"
#include "editor/forms/form_about.h"
#include "editor/forms/form_main.h"
#include "editor/trees/tree_advisor.h"
#include "editor/trees/tree_assets.h"
#include "editor/trees/tree_inspector.h"
#include "editor/trees/tree_project.h"
#include "editor/view/editor_item.h"
#include "editor/view/editor_scene.h"
#include "editor/view/editor_view.h"
#include "project/project.h"

#include "debug.h"
#include "helper.h"
#include "helper_qt.h"


//####################################################################################
//##    File Menu Functions
//####################################################################################
void FormMain::menuNew(bool test_project) {
    // Ask to save
    if (!askShouldSave("creating a new project")) {
        return;
    }

    // Set up new Project
    setFormMainMode( Form_Main_Mode::Clear );
    m_project->initializeNewProject("New Project", Orientation::Portrait, 800, 1600, test_project);
    treeAssetEditor->clear();
    treeProjectEditor->clear();
    setFormMainMode( Form_Main_Mode::World_Editor );            // Causes FormMain to rebuild itself
}


void FormMain::menuOpen() {
    // Ask to save
    if (!askShouldSave("opening a different project")) {
        return;
    }

    // Grab directory from current save file, if no save file yet, use Desktop location
    QString directory = m_project->getOption(Project_Options::File_Name_Path).toString();
    if (directory == "") {
        directory = QStandardPaths::writableLocation(QStandardPaths::StandardLocation::DesktopLocation);
    }

    // Show dialog for opening a file
    QString caption =   tr("Open Project");
    QString filter =    tr("All Files (*);;Drop Project File (*.drop)");
    QString selected =  tr("Drop Project File (*.drop)");
    QFileDialog::Options dialog_options;
    QString open_file = QFileDialog::getOpenFileName(this, caption, directory, filter, &selected, dialog_options);

    if (!open_file.isEmpty()) {
        setFormMainMode( Form_Main_Mode::Clear );
        m_project->clearProject();
        m_project->openProjectFromFile(open_file);
        treeAssetEditor->clear();
        treeProjectEditor->clear();
        setFormMainMode( Form_Main_Mode::World_Editor );
    }
}

// Returns true if we should continue doing what we were doing, false if we need to cancel
bool FormMain::askShouldSave(QString before_what) {
    if (m_project == nullptr) return true;
    if (m_project->hasSaved() == false && m_project->isTestOnly() == false) {
        QMessageBox::StandardButton ask = Dr::ShowMessageBox("Project has changed, would you like to save the changes before " + before_what + "?",
                                                             QMessageBox::Icon::Warning, "Save Changes?", this,
                                                             QMessageBox::Button::Cancel | QMessageBox::No | QMessageBox::Yes);
        if (ask == QMessageBox::StandardButton::Cancel) return false;
        if (ask == QMessageBox::StandardButton::Yes) menuSave();
    }
    return true;
}

void FormMain::menuSave(bool save_as) {
    // Grab Filename from Project Settings, if is empty need Save As dialog anyways
    QString full_path = m_project->getOption(Project_Options::File_Name_Path).toString();
    if (full_path == "") {
        QString directory = QStandardPaths::writableLocation(QStandardPaths::StandardLocation::DesktopLocation);
        QString filename =  m_project->getOption(Project_Options::Name).toString();
        full_path = directory + "/" + filename + ".drop";
        save_as = true;
    }

    // Show dialog if we need a new filename
    if (save_as) {
        QString caption =   tr("Save Project As");
        QString filter =    tr("All Files (*);;Drop Project File (*.drop)");
        QString selected =  tr("Drop Project File (*.drop)");
        full_path =  QFileDialog::getSaveFileName(this, caption, full_path, filter, &selected);
    }

    // Save the file
    m_project->setOption(Project_Options::File_Name_Path, full_path);
    m_project->saveProjectToFile();
}




//####################################################################################
//##    Edit Menu Functions
//####################################################################################
void FormMain::menuUndo() {
    // !!!!!!!!!!
    // !!!!!!!!!! #FIXME: Need to get Undo / Redo working again !!!!!!!!!!
    // !!!!!!!!!!
    ///sceneEditor->undoAction();
}
void FormMain::menuRedo() {
    // !!!!!!!!!!
    // !!!!!!!!!! #FIXME: Need to get Undo / Redo working again !!!!!!!!!!
    // !!!!!!!!!!
    ///sceneEditor->redoAction();
}

// SLOT: Updates Undo / Redo text
void FormMain::editMenuAboutToShow() {

    actionUndo->setText("Undo Disabled");
    actionRedo->setText("Redo Disabled");
    actionUndo->setEnabled(false);
    actionRedo->setEnabled(false);
    return;

    // !!!!!!!!!!
    // !!!!!!!!!!
    // !!!!!!!!!! #FIXME: Need to get Undo / Redo working again !!!!!!!!!!
    // !!!!!!!!!!
    // !!!!!!!!!!

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
//##    Help Menu Functions
//####################################################################################
void FormMain::menuAbout() {    
    FormAbout *about = new FormAbout(m_project, this);
    about->show();
}


//####################################################################################
//##    Debug Menu Functions
//####################################################################################
// Pops up a message box listing all child widgets of FormMain
void FormMain::menuListChildren() {
    QString widget_list;
    for (auto widget : findChildren<QWidget *>()) {
        widget_list += widget->objectName() + ", ";
    }
    Dr::ShowMessageBox(widget_list);
}









