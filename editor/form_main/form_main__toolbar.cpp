//
//      Created by Stephens Nunnally on 3/11/2019, (c) 2019 Scidian Studios, All Rights Reserved
//
//  File:
//
//
//
#include <QButtonGroup>
#include <QDebug>
#include <QToolBar>
#include <QToolButton>

#include "editor/event_filters/event_filters.h"
#include "editor/form_main/form_main.h"
#include "editor/helper_library.h"
#include "editor/preferences.h"
#include "editor/trees/tree_assets.h"
#include "editor/trees/tree_project.h"
#include "editor/view_editor/editor_item.h"
#include "editor/view_editor/editor_scene.h"
#include "editor/view_editor/editor_view.h"
#include "editor/enums_editor.h"
#include "project/dr_project.h"
#include "project/entities/dr_thing.h"
#include "project/entities_physics_2d/dr_asset.h"


//####################################################################################
//##    Updates status bar, enables / disables buttons as necessary
//####################################################################################
void FormMain::updateToolBar() {

    Editor_Widgets active_widget = getActiveWidget();

    // ***** Start by disabling buttons, we we re-enable them as necessary in this function
    for (auto button : m_buttons_group_layering->buttons())     if (button->isEnabled()) button->setEnabled(false);
    for (auto button : m_buttons_group_edit->buttons())         if (button->isEnabled()) button->setEnabled(false);
    for (auto button : m_buttons_group_transform->buttons())    if (button->isEnabled()) button->setEnabled(false);
    m_button_add->setEnabled(true);


    // ********** World Graph Toolbar Update
    if (getEditorMode() == Editor_Mode::World_Graph) {
        QAbstractButton *snap_grid = m_buttons_group_grid_simple->button(int(Buttons_Grid::Snap_To_Grid));
        if (snap_grid) snap_grid->setChecked(Dr::GetPreference(Preferences::Editor_Snap_To_Grid).toBool());


    // ********** World Creator Toolbar Update
    } else if (getEditorMode() == Editor_Mode::World_Creator) {
        QAbstractButton *snap_grid = m_buttons_group_grid_full->button(int(Buttons_Grid::Snap_To_Grid));
        if (snap_grid) snap_grid->setChecked(Dr::GetPreference(Preferences::Editor_Snap_To_Grid).toBool());

        QString selected = "No Selection";
        if (active_widget == Editor_Widgets::Project_Tree || active_widget == Editor_Widgets::View) {
            // ***** Things are selected
            if (m_scene_editor->getSelectionCount() > 0) {
                for (auto button : m_buttons_group_layering->buttons())     if (!button->isEnabled()) button->setEnabled(true);
                for (auto button : m_buttons_group_edit->buttons())         if (!button->isEnabled()) button->setEnabled(true);
                for (auto button : m_buttons_group_transform->buttons())    if (!button->isEnabled()) button->setEnabled(true);

                if (m_scene_editor->getSelectionCount() == 1) {
                    DrThing *thing = dynamic_cast<EditorItem*>(m_scene_editor->getSelectionItems().first())->getThing();
                    if (thing != nullptr) {
                        DrSettings *settings = m_project->findSettingsFromKey(thing->getAssetKey());
                        if (settings != nullptr) selected = QString::fromStdString(settings->getName());
                    }

                } else if (m_scene_editor->getSelectionCount() > 1) {
                    selected = QString::number(m_scene_editor->getSelectionCount()) + " Things";
                }

            // ***** World or Stage is selected
            } else if (m_tree_project->selectedItems().count() > 0) {
                long first_key = m_tree_project->selectedItems().first()->data(0, User_Roles::Key).toLongLong();
                DrSettings *settings = m_project->findSettingsFromKey(first_key);
                if (settings != nullptr) {
                    int selection_count = m_tree_project->selectedItems().count();
                    if (settings->getType() == DrType::Stage || settings->getType() == DrType::World) {
                        for (auto button : m_buttons_group_edit->buttons())     if (!button->isEnabled()) button->setEnabled(true);

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

        // ***** Asset is selected
        } else if (active_widget == Editor_Widgets::Asset_Tree || active_widget == Editor_Widgets::Inspector_Tree) {
            DrSettings *entity = m_project->findSettingsFromKey(m_tree_assets->getSelectedKey(), false);
            if (entity != nullptr) {
                if (entity->getType() == DrType::Asset || entity->getType() == DrType::Font) {
                    for (auto button : m_buttons_group_edit->buttons()) {
                        if (!button->isEnabled()) button->setEnabled(true);
                    }
                }
            }
        }

        // ***** Update Status Bar
        if (m_view_editor->getMouseMode() == Mouse_Mode::Pointer) {
            m_label_selected->setText(selected);
        } else if (m_view_editor->getMouseMode() == Mouse_Mode::Hand) {
            m_label_selected->setText("Left click and drag to <b>Move View</b>");
        } else if (m_view_editor->getMouseMode() == Mouse_Mode::Magnify) {
            m_label_selected->setText("Left click to <b>Zoom In</b>, right click to <b>Zoom Out</b>");
        }


    // ********** Sound Creator Toolbar Update
    } else if (getEditorMode() == Editor_Mode::Sound_Creator) {

        // ***** Asset is selected
        if (active_widget == Editor_Widgets::Asset_Tree || active_widget == Editor_Widgets::Inspector_Tree) {
            DrSettings *entity = m_project->findSettingsFromKey(m_tree_assets->getSelectedKey(), false);
            if (entity != nullptr) {
                if (entity->getType() == DrType::Mix || entity->getType() == DrType::Sound) {
                    for (auto button : m_buttons_group_edit->buttons()) {
                        if (!button->isEnabled()) button->setEnabled(true);
                    }
                }
            }
        }

    }   // End Editor_Mode::Sound_Creator
}


//####################################################################################
//##    Clears and recreates the toolbar based on the new mode
//####################################################################################
void FormMain::clearToolBar() {
    for (auto widget : m_toolbar_widgets) {
        m_widget_toolbar_layout->removeWidget( widget );
        widget->hide();
    }
    // Use iterator to delete Spacer Items
    for(auto it = m_toolbar_spacers.begin(); it != m_toolbar_spacers.end(); ) {
        m_widget_toolbar_layout->removeItem( *it );
        it = m_toolbar_spacers.erase(it);
    }
}

void FormMain::setToolBar(Editor_Mode new_mode) {
    switch (new_mode) {
        case Editor_Mode::World_Graph:
            addToolBarSpacer(true, 70);
            addToolBarGroup( m_widget_group_grid_simple,    false, 35);
            addToolBarGroup( m_widget_group_play,           false, 35);
            addToolBarGroup( m_widget_group_settings,       false);
            break;
        case Editor_Mode::World_Creator:
            addToolBarSpacer(false, 70);
            addToolBarGroup( m_widget_group_edit,           true);
            addToolBarGroup( m_widget_group_layering,       true);
            addToolBarGroup( m_widget_group_transform,      true);
            addToolBarGroup( m_widget_group_grid_full,      false, 35);
            addToolBarGroup( m_widget_group_play,           false, 35);
            addToolBarGroup( m_widget_group_settings,       false);
            break;
        case Editor_Mode::UI_Creator:
            addToolBarSpacer(true);
            addToolBarGroup( m_widget_group_play,           false, 35);
            addToolBarGroup( m_widget_group_settings,       false);
            break;
        case Editor_Mode::Sound_Creator:
            addToolBarSpacer(false, 70);
            addToolBarGroup( m_widget_group_edit,           true);
            addToolBarGroup( m_widget_group_visual,         false, 35);
            addToolBarGroup( m_widget_group_play,           false, 35);
            addToolBarGroup( m_widget_group_settings,       false);
            break;
        case Editor_Mode::Clear:
            addToolBarSpacer(true);
            addToolBarGroup( m_widget_group_play,           false, 35);
            addToolBarGroup( m_widget_group_settings,       false);
            break;


        // !!!!! #TODO:
        case Editor_Mode::Animation_Creator:
        case Editor_Mode::Component_Graph:
        case Editor_Mode::Program_Loading:
            break;
    }
}

void FormMain::addToolBarGroup(QWidget *group, bool spacer_expands, int min_desired_spacer_size) {
    m_toolbar_widgets.append(group);
    m_widget_toolbar_layout->addWidget(group);
    group->show();
    addToolBarSpacer(spacer_expands, min_desired_spacer_size);
}

void FormMain::addToolBarSpacer(bool spacer_expands, int min_desired_size) {
    QSpacerItem *spacer;
    if (spacer_expands)
        spacer = new QSpacerItem(min_desired_size, 0, QSizePolicy::Policy::Expanding, QSizePolicy::Minimum);
    else
        spacer = new QSpacerItem(min_desired_size, 0, QSizePolicy::Policy::Preferred, QSizePolicy::Minimum);
    m_toolbar_spacers.append(spacer);
    m_widget_toolbar_layout->addSpacerItem(spacer);
}























