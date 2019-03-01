//
//      Created by Stephens Nunnally on 3/1/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QKeyEvent>

#include "editor_view.h"
#include "form_main.h"


//####################################################################################
//##        FormMain Wide Event Filter, catches all events
//####################################################################################
bool FormMain::eventFilter(QObject *obj, QEvent *event)
{

    // ********** Catch space bar for view to make sure we can drag even if view didnt have focus
    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->key() == Qt::Key::Key_Space && current_form_main_mode == Form_Main_Mode::World_Editor)
            if (viewMain->hasFocus() == false)
                viewMain->keyPressEvent(keyEvent);
    }
    if (event->type() == QEvent::KeyRelease)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->key() == Qt::Key::Key_Space && current_form_main_mode == Form_Main_Mode::World_Editor)
            if (viewMain->hasFocus() == false)
                viewMain->keyReleaseEvent(keyEvent);
    }

    return QObject::eventFilter(obj, event);
}
