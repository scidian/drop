//
//      Created by Stephens Nunnally on 3/16/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QObject>
#include <QEvent>
#include <QPainter>
#include <QMouseEvent>
#include <QMenu>
#include <QTime>

#include "colors.h"
#include "interface_editor_relay.h"
#include "settings_component_property.h"
#include "widgets_event_filters.h"


//####################################################################################
//##    WidgetHoverHandler Class Functions
//##        This exists as a member object inside some parent classes (instead of being created on the fly like other event filters).
//##        This is so the SIGNAL, signalMouseHover, only has to be CONNECTed once upon the parent class initialization.
//##        Otherwise we'd have to pass an IEditorRelay into every class that wanted to initialize a WidgetHoverHandler.
//####################################################################################
bool WidgetHoverHandler::eventFilter(QObject *obj, QEvent *event)
{
    QWidget *hover_widget = dynamic_cast<QWidget*>(obj);
    if (!hover_widget) return QObject::eventFilter(obj, event);

    if (event->type() == QEvent::HoverEnter)
    {
        hover_widget->setProperty(User_Property::Mouse_Over, true);
        hover_widget->update();

        QString header = hover_widget->property(User_Property::Header).toString();
        QString body = hover_widget->property(User_Property::Body).toString();

        emit signalMouseHover(header, body);

    } else if (event->type() == QEvent::HoverLeave) {
        hover_widget->setProperty(User_Property::Mouse_Over, false);
        hover_widget->update();

    } else if (event->type() == QEvent::MouseMove) {
        QMouseEvent *mouse_event = dynamic_cast<QMouseEvent*>(event);
        hover_widget->setProperty(User_Property::Mouse_Pos, mouse_event->pos());
        hover_widget->update();
    }


    return QObject::eventFilter(obj, event);
}

//      Sets AdvisorInfo widget user properties
void WidgetHoverHandler::attachToHoverHandler(QWidget *widget, DrProperty *property)
{
    widget->setProperty(User_Property::Header, property->getDisplayName());
    widget->setProperty(User_Property::Body, property->getDescription());
    widget->setAttribute(Qt::WidgetAttribute::WA_Hover, true);
    widget->installEventFilter(this);
}

void WidgetHoverHandler::attachToHoverHandler(QWidget *widget, QString header, QString body)
{
    widget->setProperty(User_Property::Header, header);
    widget->setProperty(User_Property::Body, body);
    widget->setAttribute(Qt::WidgetAttribute::WA_Hover, true);
    widget->installEventFilter(this);
}

void WidgetHoverHandler::attachToHoverHandler(QWidget *widget, HeaderBodyList header_body_list)
{
    widget->setProperty(User_Property::Header, header_body_list[0]);
    widget->setProperty(User_Property::Body, header_body_list[1]);
    widget->setAttribute(Qt::WidgetAttribute::WA_Hover, true);
    widget->installEventFilter(this);
}




//####################################################################################
//##    MouseWheelAdjustmentGuard Class Functions
//####################################################################################
MouseWheelAdjustmentGuard::MouseWheelAdjustmentGuard(QObject *parent) : QObject(parent) {}

bool MouseWheelAdjustmentGuard::eventFilter(QObject *obj, QEvent *event)
{
    const QWidget* widget = static_cast<QWidget*>(obj);
    if (!widget) return QObject::eventFilter(obj, event);

    if (event->type() == QEvent::Wheel && !widget->hasFocus())
    {
        event->ignore();
        return true;
    }

    return QObject::eventFilter(obj, event);
}


//####################################################################################
//##    PopUpMenuRelocater Class Functions
//####################################################################################
PopUpMenuRelocater::PopUpMenuRelocater(QObject *parent) : QObject(parent) {}

bool PopUpMenuRelocater::eventFilter(QObject *obj, QEvent *event)
{
    QMenu* menu = dynamic_cast<QMenu*>(obj);
    if (!menu) return QObject::eventFilter(obj, event);

    if (event->type() == QEvent::Show)
    {
        QPoint pos = menu->pos();
        pos.setY(pos.y() + 2);
        menu->move(pos);
        return true;
    }
    return QObject::eventFilter(obj, event);
}















