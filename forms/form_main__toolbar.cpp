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

#include "editor/editor_tree_assets.h"
#include "editor_view/editor_item.h"
#include "editor_view/editor_scene.h"
#include "editor_view/editor_view.h"
#include "enums_form_main.h"
#include "form_main.h"
#include "globals.h"
#include "helper.h"
#include "project/project.h"
#include "project/project_asset.h"
#include "project/project_world_stage_thing.h"
#include "widgets/widgets_event_filters.h"


//####################################################################################
//##    Updates status bar, enables / disables buttons as necessary
//####################################################################################
void FormMain::updateToolbar() {

    if (m_current_mode == Form_Main_Mode::World_Editor) {
        for (auto button : buttonsGroupLayering->buttons())     if (button->isEnabled()) button->setEnabled(false);
        for (auto button : buttonsGroupEdit->buttons())         if (button->isEnabled()) button->setEnabled(false);
        for (auto button : buttonsGroupTransform->buttons())    if (button->isEnabled()) button->setEnabled(false);

        QString selected = "No Selection";
        if (getActiveWidget() == Editor_Widgets::Project_Tree || getActiveWidget() == Editor_Widgets::Scene_View) {
            if (sceneEditor->getSelectionCount() > 0) {
                for (auto button : buttonsGroupLayering->buttons())     if (!button->isEnabled()) button->setEnabled(true);
                for (auto button : buttonsGroupEdit->buttons())         if (!button->isEnabled()) button->setEnabled(true);
                for (auto button : buttonsGroupTransform->buttons())    if (!button->isEnabled()) button->setEnabled(true);

                if (sceneEditor->getSelectionCount() == 1) {
                    DrThing *thing = dynamic_cast<DrItem*>( sceneEditor->getSelectionItems().first() )->getThing();
                    if (dynamic_cast<DrItem*>( sceneEditor->getSelectionItems().first() )->getThing())
                        selected = m_project->findSettingsFromKey(thing->getAssetKey())->getName();

                } else if (sceneEditor->getSelectionCount() > 1) {
                    selected = QString::number( sceneEditor->getSelectionCount() ) + " Things";
                }
            }

        } else if (getActiveWidget() == Editor_Widgets::Asset_Tree && treeAssetEditor->getSelectedKey() != c_no_key) {
            // Enable Trash Can
            DrAsset *asset = m_project->findAssetFromKey(treeAssetEditor->getSelectedKey());
            if (asset != nullptr) {
                if (asset->getAssetType() != DrAssetType::Effect) {
                    for (auto button : buttonsGroupEdit->buttons())     if (!button->isEnabled()) button->setEnabled(true);
                }
            }
        }

        labelSelected->setText(selected);
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
            addToolbarGroup( widgetGroupAdd,        true );
            addToolbarGroup( widgetGroupLayering,   true );
            addToolbarGroup( widgetGroupEdit,       true );
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












