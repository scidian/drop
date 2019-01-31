//
//      Created by Stephens Nunnally on 1/31/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//

#include <QObject>
#include <QEvent>

#include "editor_hover_handler.h"

#include "enums.h"


//####################################################################################
//##    WidgetHoverHandler Class Functions
//####################################################################################
void WidgetHoverHandler::attach(QWidget *widget)
{
    widget->setAttribute(Qt::WidgetAttribute::WA_Hover, true);
    widget->installEventFilter(this);
}

bool WidgetHoverHandler::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::HoverEnter)
    {
        QWidget *hover_widget = dynamic_cast<QWidget*>(obj);

        QString header = hover_widget->property(User_Property::Header).toString();
        QString body = hover_widget->property(User_Property::Body).toString();

        emit signalMouseHover(header, body);
    }

    return false;
}
