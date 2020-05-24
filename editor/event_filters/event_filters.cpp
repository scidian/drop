//
//      Created by Stephens Nunnally on 5/24/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QApplication>
#include <QDebug>
#include <QObject>
#include <QEvent>
#include <QPainter>
#include <QMouseEvent>
#include <QMenu>

#include "core/colors/colors.h"
#include "editor/event_filters/event_filters.h"
#include "editor/helper_library.h"
#include "editor/interface_editor_relay.h"
#include "editor/preferences.h"
#include "project/settings/settings_component_property.h"


//####################################################################################
//##    DrFilterHoverHandler Class Functions
//##        This exists as a member object inside some parent classes (instead of being created on the fly like other event filters).
//##        This is so the SIGNAL, signalMouseHover, only has to be CONNECTed once upon the parent class initialization.
//##        Otherwise we'd have to pass an IEditorRelay into every class that wanted to initialize a DrFilterHoverHandler.
//####################################################################################
bool DrFilterHoverHandler::eventFilter(QObject *obj, QEvent *event) {
    QWidget *hover_widget = dynamic_cast<QWidget*>(obj);
    if (!hover_widget) return QObject::eventFilter(obj, event);

    if (event->type() == QEvent::HoverEnter) {
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
void DrFilterHoverHandler::attachToHoverHandler(QWidget *widget, DrProperty *property) {
    widget->setProperty(User_Property::Header,  QString::fromStdString(property->getDisplayName()));
    widget->setProperty(User_Property::Body,    QString::fromStdString(property->getDescription()));
    widget->setAttribute(Qt::WidgetAttribute::WA_Hover, true);
    widget->installEventFilter(this);
}

void DrFilterHoverHandler::attachToHoverHandler(QWidget *widget, QString header, QString body) {
    widget->setProperty(User_Property::Header,  header);
    widget->setProperty(User_Property::Body,    body);
    widget->setAttribute(Qt::WidgetAttribute::WA_Hover, true);
    widget->installEventFilter(this);
}

void DrFilterHoverHandler::attachToHoverHandler(QWidget *widget, HeaderBodyList header_body_list) {
    widget->setProperty(User_Property::Header,  header_body_list[0]);
    widget->setProperty(User_Property::Body,    header_body_list[1]);
    widget->setAttribute(Qt::WidgetAttribute::WA_Hover, true);
    widget->installEventFilter(this);
}




//####################################################################################
//##    DrFilterMouseWheelAdjustmentGuard Class Functions
//####################################################################################
DrFilterMouseWheelAdjustmentGuard::DrFilterMouseWheelAdjustmentGuard(QObject *parent) : QObject(parent) { }

bool DrFilterMouseWheelAdjustmentGuard::eventFilter(QObject *obj, QEvent *event) {
    const QWidget* widget = static_cast<QWidget*>(obj);
    if (widget == nullptr) return QObject::eventFilter(obj, event);

    if (event->type() == QEvent::Wheel && !widget->hasFocus()) {
        event->ignore();
        return true;
    }

    return QObject::eventFilter(obj, event);
}


//####################################################################################
//##    DrFilterPopUpMenuRelocater Class Functions
//####################################################################################
DrFilterPopUpMenuRelocater::DrFilterPopUpMenuRelocater(QObject *parent, int top_offset, int left_offset) : QObject(parent) {
    m_top_offset =  top_offset;
    m_left_offset = left_offset;
}

bool DrFilterPopUpMenuRelocater::eventFilter(QObject *obj, QEvent *event) {
    QMenu* menu = dynamic_cast<QMenu*>(obj);
    if (menu == nullptr) return QObject::eventFilter(obj, event);

    if (event->type() == QEvent::Show) {
        bool oldAnimationEffects = qApp->isEffectEnabled(Qt::UI_AnimateCombo);
        qApp->setEffectEnabled(Qt::UI_AnimateCombo, false);

        QPoint pos = menu->pos();
        int new_x = pos.x() + m_left_offset;
        int new_y = pos.y() + m_top_offset;

        if (new_x < 5) new_x = 5;
        if (new_y < 5) new_y = 5;

        pos.setX(new_x);
        pos.setY(new_y);
        menu->move(pos);

        qApp->setEffectEnabled(Qt::UI_AnimateCombo, oldAnimationEffects);
        return true;
    }
    return QObject::eventFilter(obj, event);
}


















