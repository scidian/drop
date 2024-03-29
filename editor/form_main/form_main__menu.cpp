//
//      Created by Stephens Nunnally on 1/3/2019, (c) 2019 Scidian Studios, All Rights Reserved
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

#include "core/colors/colors.h"
#include "editor/form_main/form_main.h"
#include "editor/forms/form_about.h"
#include "editor/helper_library.h"
#include "editor/project/project.h"
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
//##    File Menu Functions
//####################################################################################
void FormMain::menuNew(bool test_project) {
    // Ask to save
    if (!askShouldSave("creating a new project")) {
        return;
    }

    // Set up new Project
    setEditorMode( Editor_Mode::Clear );
    Dr::InitializeNewProject(m_project, "New Project", Orientation::Portrait, c_project_width, c_project_height, test_project);
    m_tree_assets->clear();
    m_tree_project->clear();
    setEditorMode( Editor_Mode::World_Creator );                    // Causes FormMain to rebuild itself
}


void FormMain::menuOpen() {
    // Ask to save
    if (!askShouldSave("opening a different project")) {
        return;
    }

    // Grab directory from current save file, if no save file yet, use Desktop location
    QString directory = QString::fromStdString(m_project->getOption(Project_Options::File_Name_Path).toString());
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
        setEditorMode( Editor_Mode::Clear );
        m_project->clearProject();
        Dr::AddBuiltInImages(m_project);
        Dr::OpenProjectFromFile(m_project, open_file.toStdString());
        m_tree_assets->clear();
        m_tree_project->clear();
        setEditorMode( Editor_Mode::World_Creator );
    }
}

// Returns true if we should continue doing what we were doing, false if we need to cancel
bool FormMain::askShouldSave(QString before_what) {
    if (m_project == nullptr) return true;
    if (m_project->hasSaved() == false && m_project->isTestOnly() == false) {
        QMessageBox::StandardButton ask = Dr::ShowMessageBox("Project has changed, would you like to save the changes before " + before_what.toStdString() + "?",
                                                             QMessageBox::Icon::Warning, this,
                                                             QMessageBox::StandardButton::Cancel |
                                                             QMessageBox::StandardButton::No |
                                                             QMessageBox::StandardButton::Yes);
        if (ask == QMessageBox::StandardButton::Cancel) return false;
        if (ask == QMessageBox::StandardButton::Yes) menuSave();
    }
    return true;
}

void FormMain::menuSave(bool save_as) {
    // Grab Filename from Project Settings, if is empty need Save As dialog anyways
    QString full_path = QString::fromStdString(m_project->getOption(Project_Options::File_Name_Path).toString());
    if (full_path == "") {
        QString directory = QStandardPaths::writableLocation(QStandardPaths::StandardLocation::DesktopLocation);
        QString filename =  QString::fromStdString(m_project->getOption(Project_Options::Name).toString());
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
    m_project->setOption(Project_Options::File_Name_Path, full_path.toStdString());
    Dr::SaveProjectToFile(m_project);
}




//####################################################################################
//##    Edit Menu Functions
//####################################################################################
void FormMain::menuUndo() {
    // !!!!!!!!!!
    // !!!!!!!!!! #FIXME: Need to get Undo / Redo working again !!!!!!!!!!
    // !!!!!!!!!!
    ///m_scene_editor->undoAction();
}
void FormMain::menuRedo() {
    // !!!!!!!!!!
    // !!!!!!!!!! #FIXME: Need to get Undo / Redo working again !!!!!!!!!!
    // !!!!!!!!!!
    ///m_scene_editor->redoAction();
}

// SLOT: Updates Undo / Redo text
void FormMain::editMenuAboutToShow() {

    m_action_undo->setText("Undo Disabled");
    m_action_redo->setText("Redo Disabled");
    m_action_undo->setEnabled(false);
    m_action_redo->setEnabled(false);
    return;

    // !!!!!!!!!!
    // !!!!!!!!!!
    // !!!!!!!!!! #FIXME: Need to get Undo / Redo working again !!!!!!!!!!
    // !!!!!!!!!!
    // !!!!!!!!!!

    QString undo_text = m_scene_editor->getCurrentUndo();
    QString redo_text = m_scene_editor->getCurrentRedo();
    m_action_undo->setText(undo_text);
    m_action_redo->setText(redo_text);

    if (undo_text.isEmpty()) {
        m_action_undo->setText("&Undo");
        m_action_undo->setEnabled(false);
    }
    if (redo_text.isEmpty()) {
        m_action_redo->setText("&Redo");
        m_action_redo->setEnabled(false);
    }
}

void FormMain::editMenuAboutToHide() {
    m_action_undo->setEnabled(true);
    m_action_redo->setEnabled(true);
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
    Dr::ShowMessageBox(widget_list.toStdString());
}









