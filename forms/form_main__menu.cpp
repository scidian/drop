//
//      Created by Stephens Nunnally on 1/3/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Calls to set up, initialize, build Form Main menu system
//
//
#include <QApplication>
#include <QActionGroup>
#include <QFileDialog>
#include <QMenuBar>
#include <QMessageBox>
#include <QStandardPaths>

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
//##    File Menu Functions
//####################################################################################
void FormMain::menuNew() {
    setFormMainMode( Form_Main_Mode::Clear );
    m_project->clearProject();
    m_project->initializeNewProject("New Project: "+ Dr::CurrentTimeAsString(), Orientation::Portrait, 800, 1600);
    setFormMainMode( Form_Main_Mode::World_Editor );            // Causes FormMain to rebuild itself
}


void FormMain::menuOpen() {
    QString caption =   "Open Project";
    QString directory  = QStandardPaths::writableLocation(QStandardPaths::StandardLocation::DesktopLocation);
    QString filter =    tr("Drop Project File (*.drop)");
    QString selected =  tr("Drop Project File (*.drop)");
    QFileDialog::Options dialog_options;
    QString open_file = QFileDialog::getOpenFileName(nullptr, caption, directory, filter, &selected, dialog_options);
    setFormMainMode( Form_Main_Mode::Clear );
    m_project->clearProject();
    m_project->openProjectFromFile(open_file);
    setFormMainMode( Form_Main_Mode::World_Editor );
}


void FormMain::menuSave(bool save_as) {
    // Grab Filename from Project Settings, if is empty need Save As dialog anyways
    QString full_path = m_project->getOption(Project_Options::File_Name_Path).toString();
    if (full_path == "") {
        QString directory  = QStandardPaths::writableLocation(QStandardPaths::StandardLocation::DesktopLocation);
        QString filename =   m_project->getOption(Project_Options::Name).toString();
        full_path = directory + "/" + filename + ".drop";
        save_as = true;
    }

    // Show dialog if we need a new filename
    if (save_as) {
        QString caption =   "Save Project As";
        QString filter =    tr("Drop Project File (*.drop)");
        QString selected =  tr("Drop Project File (*.drop)");
        full_path =  QFileDialog::getSaveFileName(nullptr, caption, full_path, filter, &selected);
    }

    // Save the file
    m_project->setOption(Project_Options::File_Name_Path, full_path);
    m_project->saveProjectToFile();
}




//####################################################################################
//##    Edit Menu Functions
//####################################################################################
void FormMain::menuUndo() {
    sceneEditor->undoAction();
}
void FormMain::menuRedo() {
    sceneEditor->redoAction();
}

// SLOT: Updates Undo / Redo text
void FormMain::editMenuAboutToShow() {
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
    QMessageBox::about(this, tr("About Drop"), tr("<b>Drop Creator</b> A Drag and Drop Game Maker by Stephens Nunnally"));
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









