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

#include "enums_form_main.h"
#include "form_main.h"
#include "globals.h"
#include "library.h"
#include "widgets_event_filters.h"


//####################################################################################
//##        Constructor / Destructor
//####################################################################################
DrToolBar::DrToolBar(QWidget *parent) : QToolBar(parent) { }
DrToolBar::~DrToolBar() { }


//####################################################################################
//##    DrToolBar Function Calls
//####################################################################################
// Save the press position (this is relative to the current widget)
void DrToolBar::mousePressEvent(QMouseEvent* event) {
    press_pos = event->pos();
    is_moving = true;
}

// Calculate difference between the press position and the new Mouse position, relative to the current widget
void DrToolBar::mouseMoveEvent(QMouseEvent* event) {
    if (is_moving) {
        QPoint diff = event->pos() - press_pos;
        window()->move( window()->pos() + diff );
    }
}
void DrToolBar::mouseReleaseEvent(QMouseEvent*) { is_moving = false; }


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
        addToolbarGroup( widgetGroupLayering, false);
        addToolbarGroup( widgetGroupReset, true );
        addToolbarGroup( widgetGroupSettings, false );

//        for (auto button : buttonsGroupLayering->buttons() ) {
//            button->setEnabled(false);
//        }

        break;

    case Form_Main_Mode::Clear:
        addToolbarGroup( widgetGroupSettings, false );
        break;

    default:    ;
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












