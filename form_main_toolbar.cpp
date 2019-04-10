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

#include "editor_item.h"
#include "editor_scene.h"
#include "editor_view.h"
#include "enums_form_main.h"
#include "form_main.h"
#include "globals.h"
#include "library.h"
#include "project.h"
#include "project_world_stage_object.h"
#include "widgets_event_filters.h"


//####################################################################################
//##    Updates status bar, enables / disables buttons as necessary
//####################################################################################
void FormMain::updateToolbar()
{
    QString selected;
    if (sceneEditor->getSelectionCount() == 0) {
        selected = "No Selection";
        for (auto button : buttonsGroupLayering->buttons())
            if (button->isEnabled()) button->setEnabled(false);
        for (auto button : buttonsGroupTransform->buttons())
            if (button->isEnabled()) button->setEnabled(false);

    } else {
        for (auto button : buttonsGroupLayering->buttons())
            if (!button->isEnabled()) button->setEnabled(true);
        for (auto button : buttonsGroupTransform->buttons())
            if (!button->isEnabled()) button->setEnabled(true);

        if (sceneEditor->getSelectionCount() == 1) {
            DrObject *object = dynamic_cast<DrItem*>( sceneEditor->getSelectionItems().first() )->getObject();
            if (dynamic_cast<DrItem*>( sceneEditor->getSelectionItems().first() )->getObject())
                selected = project->findSettingsFromKey(object->getAssetKey())->getName();

        } else if (sceneEditor->getSelectionCount() > 1) {
            selected = QString::number( sceneEditor->getSelectionCount() ) + " Objects";
        }
    }

    labelSelected->setText(selected);

}


//####################################################################################
//##    Clears and recreates the toolbar based on the new mode
//####################################################################################
void FormMain::clearToolbar()
{
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

void FormMain::setToolbar(Form_Main_Mode new_mode)
{
    switch (new_mode) {
    case Form_Main_Mode::World_Editor:
        addToolbarGroup( widgetGroupLayering,   true );
        addToolbarGroup( widgetGroupTransform,  true );
        addToolbarGroup( widgetGroupGrid,       true );
        addToolbarGroup( widgetGroupSettings,   false );

//        for (auto button : buttonsGroupLayering->buttons() ) {
//            button->setEnabled(false);
//        }

        break;

    case Form_Main_Mode::Clear:
        addToolbarGroup( widgetGroupSettings, false );
        break;


    // !!!!! TODO
    case Form_Main_Mode::World_Map:
    case Form_Main_Mode::Stage_Map:
    case Form_Main_Mode::UI_Editor:
    case Form_Main_Mode::Program_Loading:
        break;
    }
}

void FormMain::addToolbarGroup(QWidget *group, bool add_spacer)
{
    toolbarWidgets.append(group);
    widgetToolbarLayout->addWidget(group);
    group->show();

    if (add_spacer) {
        QSpacerItem *spacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);
        toolbarSpacers.append(spacer);
        widgetToolbarLayout->addSpacerItem(spacer);
    }
}












