//
//      Created by Stephens Nunnally on 3/11/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QButtonGroup>
#include <QToolBar>
#include <QToolButton>

#include "editor/event_filters.h"
#include "editor/form_main/form_main.h"
#include "editor/preferences.h"
#include "editor/trees/tree_assets.h"
#include "editor/trees/tree_project.h"
#include "editor/view/editor_item.h"
#include "editor/view/editor_scene.h"
#include "editor/view/editor_view.h"
#include "editor/enums_editor.h"
#include "project/dr_project.h"
#include "project/entities/dr_asset.h"
#include "project/entities/dr_thing.h"


//####################################################################################
//##    Updates status bar, enables / disables buttons as necessary
//####################################################################################
void FormMain::updateToolbar() {

    if (m_current_mode == Form_Main_Mode::World_Editor) {
        for (auto button : buttonsGroupLayering->buttons())     if (button->isEnabled()) button->setEnabled(false);
        for (auto button : buttonsGroupEdit->buttons())         if (button->isEnabled()) button->setEnabled(false);
        for (auto button : buttonsGroupTransform->buttons())    if (button->isEnabled()) button->setEnabled(false);

        QString selected = "No Selection";
        if (getActiveWidget() == Editor_Widgets::Project_Tree || getActiveWidget() == Editor_Widgets::Stage_View) {
            // ***** Things are selected
            if (sceneEditor->getSelectionCount() > 0) {
                for (auto button : buttonsGroupLayering->buttons())     if (!button->isEnabled()) button->setEnabled(true);
                for (auto button : buttonsGroupEdit->buttons())         if (!button->isEnabled()) button->setEnabled(true);
                for (auto button : buttonsGroupTransform->buttons())    if (!button->isEnabled()) button->setEnabled(true);

                if (sceneEditor->getSelectionCount() == 1) {
                    DrThing *thing = dynamic_cast<DrItem*>( sceneEditor->getSelectionItems().first() )->getThing();
                    if (thing != nullptr) {
                        DrSettings *settings = m_project->findSettingsFromKey(thing->getAssetKey());
                        if (settings != nullptr) selected = QString::fromStdString(settings->getName());
                    }

                } else if (sceneEditor->getSelectionCount() > 1) {
                    selected = QString::number( sceneEditor->getSelectionCount() ) + " Things";
                }

            // ***** World or Stage is selected
            } else if (treeProjectEditor->selectedItems().count() > 0) {
                long first_key = treeProjectEditor->selectedItems().first()->data(0, User_Roles::Key).toLongLong();
                DrSettings *settings = m_project->findSettingsFromKey(first_key);
                if (settings != nullptr) {
                    int selection_count = treeProjectEditor->selectedItems().count();
                    if (settings->getType() == DrType::Stage || settings->getType() == DrType::World) {
                        for (auto button : buttonsGroupEdit->buttons())     if (!button->isEnabled()) button->setEnabled(true);

                        if (selection_count == 1) {
                            if (settings->getType() == DrType::Stage) selected = "Stage: " + QString::fromStdString(settings->getName());
                            if (settings->getType() == DrType::World) selected = "World: " + QString::fromStdString(settings->getName());
                        } else {
                            if (settings->getType() == DrType::Stage) selected = QString::number( selection_count ) + " Stages";
                            if (settings->getType() == DrType::World) selected = QString::number( selection_count ) + " Worlds";
                        }
                    }
                }
            }

        } else if (getActiveWidget() == Editor_Widgets::Asset_Tree && treeAssetEditor->getSelectedKey() != c_no_key) {
            // ***** Asset is selected
            DrSettings *asset = m_project->findSettingsFromKey(treeAssetEditor->getSelectedKey(), false);
            if (asset != nullptr) {
                if (asset->getType() == DrType::Asset || asset->getType() == DrType::Font) {
                    for (auto button : buttonsGroupEdit->buttons()) {
                        if (!button->isEnabled()) button->setEnabled(true);
                    }
                }
            }
        }

        buttonAdd->setEnabled(true);
        if (viewEditor->getMouseMode() == Mouse_Mode::Pointer) {
            labelSelected->setText(selected);
        } else if (viewEditor->getMouseMode() == Mouse_Mode::Hand) {
            labelSelected->setText("Left click and drag to <b>Move View</b>");
        } else if (viewEditor->getMouseMode() == Mouse_Mode::Magnify) {
            labelSelected->setText("Left click to <b>Zoom In</b>, right click to <b>Zoom Out</b>");
        }
    }
}


//####################################################################################
//##    Clears and recreates the toolbar based on the new mode
//####################################################################################
void FormMain::clearToolbar() {
    for (auto widget : toolbarWidgets) {
        widgetToolbarLayout->removeWidget( widget );
        widget->hide();
    }
    // Use iterator to delete items in list
    for(auto it = toolbarSpacers.begin(); it != toolbarSpacers.end(); ) {
        widgetToolbarLayout->removeItem( *it );
        it = toolbarSpacers.erase(it);
    }
}

void FormMain::setToolbar(Form_Main_Mode new_mode) {
    switch (new_mode) {
        case Form_Main_Mode::World_Editor:
            addToolbarGroup( widgetGroupEdit,       true );
            addToolbarGroup( widgetGroupLayering,   true );
            addToolbarGroup( widgetGroupTransform,  true );
            addToolbarGroup( widgetGroupGrid,       true );
            addToolbarGroup( widgetGroupPlay,       false );
            addToolbarGroup( widgetGroupSettings,   false );
            break;

        case Form_Main_Mode::Clear:
            addToolbarGroup( widgetGroupPlay,       false );
            addToolbarGroup( widgetGroupSettings,   false );
            break;


        // !!!!! #TODO:
        case Form_Main_Mode::World_Map:
            addToolbarGroup( widgetGroupPlay,       false );
            addToolbarGroup( widgetGroupSettings,   false );
            break;

        case Form_Main_Mode::Stage_Map:
            addToolbarGroup( widgetGroupPlay,       false );
            addToolbarGroup( widgetGroupSettings,   false );
            break;

        case Form_Main_Mode::UI_Editor:
            addToolbarGroup( widgetGroupPlay,       false );
            addToolbarGroup( widgetGroupSettings,   false );
            break;

        case Form_Main_Mode::Program_Loading:
            break;
    }
}

void FormMain::addToolbarGroup(QWidget *group, bool add_spacer) {
    toolbarWidgets.append(group);
    widgetToolbarLayout->addWidget(group);
    group->show();

    if (add_spacer) {
        QSpacerItem *spacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);
        toolbarSpacers.append(spacer);
        widgetToolbarLayout->addSpacerItem(spacer);
    }
}











