//
//      Created by Stephens Nunnally on 3/23/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QButtonGroup>

#include "editor_view.h"
#include "form_main.h"
#include "globals.h"

//####################################################################################
//##    buttonGroupMode SLOT and functions
//####################################################################################
void FormMain::buttonGroupModeClicked(int id)
{
    buttonGroupModeSetChecked(id);

    Form_Main_Mode new_id =     static_cast<Form_Main_Mode>(id);
    if (current_mode == new_id) return;

    if (new_id == Form_Main_Mode::World_Map) {
        this->setFormMainMode(Form_Main_Mode::Clear);
    } else if (new_id == Form_Main_Mode::World_Editor) {
        this->setFormMainMode(Form_Main_Mode::World_Editor);
    } else if (new_id == Form_Main_Mode::UI_Editor) {
        this->setFormMainMode(Form_Main_Mode::Clear);
    }
}

void FormMain::buttonGroupModeSetChecked(int id)
{
    QList<QAbstractButton*> buttons = buttonsGroupMode->buttons();
    for (auto button : buttons) {
        bool is_button = (buttonsGroupMode->button(id) == button);
        button->setChecked(is_button);
        button->setDown(!is_button);
    }
}



//####################################################################################
//##    buttonGroupLayering SLOT and functions
//####################################################################################
void FormMain::buttonGroupLayeringClicked(int id)
{
    Buttons_Layering clicked = static_cast<Buttons_Layering>(id);

    if (clicked == Buttons_Layering::Send_To_Back) {

    }
}


//####################################################################################
//##    buttonGroupReset SLOT and functions
//####################################################################################
void FormMain::buttonGroupResetClicked(int id)
{
    Buttons_Reset clicked = static_cast<Buttons_Reset>(id);

    if (clicked == Buttons_Reset::Reset_Object) {

    }
}


//####################################################################################
//##    buttonGroupReset SLOT and functions
//####################################################################################
void FormMain::buttonGroupGridClicked(int id)
{
    Buttons_Grid clicked = static_cast<Buttons_Grid>(id);
    Preferences preference;

    switch (clicked) {
    case Buttons_Grid::Snap_To_Grid:    preference = Preferences::World_Editor_Snap_To_Grid;        break;
    case Buttons_Grid::Resize_To_Grid:  preference = Preferences::World_Editor_Resize_To_Grid;      break;
    case Buttons_Grid::Grid_On_Top:     preference = Preferences::World_Editor_Grid_On_Top;         break;
    }
    Dr::SetPreference(preference, buttonsGroupGrid->button(id)->isChecked());
    viewEditor->updateGrid();
    viewEditor->updateSelectionBoundingBox(8);
}














