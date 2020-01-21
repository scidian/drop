//
//      Created by Stephens Nunnally on 1/7/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QButtonGroup>

#include "editor/form_main/form_main.h"
#include "editor/preferences.h"
#include "editor/view/editor_view.h"


//####################################################################################
//##    buttonGroupMouse SLOT and functions
//####################################################################################
void FormMain::buttonGroupMouseClicked(int id) {
    Mouse_Mode clicked = static_cast<Mouse_Mode>(id);

    if (viewEditor != nullptr) viewEditor->setMouseMode(clicked);
    if (clicked == Mouse_Mode::Pointer) {
        widgetGroupHandTool->setVisible(false);
        widgetGroupZoomTool->setVisible(false);

    } else if (clicked == Mouse_Mode::Hand) {
        widgetGroupZoomTool->setVisible(false);
        widgetGroupHandTool->setVisible(true);

    } else if (clicked == Mouse_Mode::Magnify) {
        widgetGroupHandTool->setVisible(false);
        widgetGroupZoomTool->setVisible(true);
    }

    updateToolbar();
    viewEditor->spaceBarUp();
}


//####################################################################################
//##    buttonGroupToggle SLOT and functions
//####################################################################################
void FormMain::buttonGroupToggleClicked(int id) {
    Buttons_Toggle clicked = static_cast<Buttons_Toggle>(id);

    if (clicked == Buttons_Toggle::CameraOnOff) {
        Dr::SetPreference(Preferences::World_Editor_Show_Camera_Boxes, buttonsGroupToggle->button(id)->isChecked());
        viewEditor->update();

    } else if (clicked == Buttons_Toggle::DebugOnOff) {
        Dr::SetPreference(Preferences::World_Editor_Show_Collision_Shapes, buttonsGroupToggle->button(id)->isChecked());
        viewEditor->update();
    }
}














