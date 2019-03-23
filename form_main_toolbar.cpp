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
        addToolbarGroup( widgetGroupGrid, true );
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












