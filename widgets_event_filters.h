//
//      Created by Stephens Nunnally on 3/16/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef WIDGETS_EVENT_FILTERS_H
#define WIDGETS_EVENT_FILTERS_H

#include <QWidget>

#include <constants.h>

// Forward declarations
class DrProperty;


//####################################################################################
//##    WidgetHoverHandler
//##        Catches hover events for widgets on Object Inspector without needing subclassing
//############################
class WidgetHoverHandler : public QObject
{
    Q_OBJECT

public:
    WidgetHoverHandler(QObject *parent) : QObject(parent) {}
    virtual ~WidgetHoverHandler() {}

    void            attachToHoverHandler(QWidget *widget, DrProperty *property);
    void            attachToHoverHandler(QWidget *widget, QString header, QString body);
    void            attachToHoverHandler(QWidget *widget, HeaderBodyList header_body_list);

protected:
    bool            eventFilter(QObject *obj, QEvent *event);

signals:
    void            signalMouseHover(QString header, QString body);
};



//####################################################################################
//##    MouseWheelAdjustmentGuard
//##        Stops widget from stealing focus on mouse wheel
//############################
class MouseWheelAdjustmentGuard : public QObject
{
public:
    explicit        MouseWheelAdjustmentGuard(QObject *parent);
protected:
    bool            eventFilter(QObject *obj, QEvent *event) override;
};


//####################################################################################
//##    PopUpMenuRelocater
//##        Changes starting position of popup menu
//############################
class PopUpMenuRelocater : public QObject
{
private:
    int         m_top_offset;
    int         m_left_offset;
public:
    explicit        PopUpMenuRelocater(QObject *parent, int top_offset, int left_offset);
protected:
    bool            eventFilter(QObject *obj, QEvent *event) override;
};


//####################################################################################
//##    ClickAndDragWindow
//##        Clicking and dragging will move parent window
//############################
class ClickAndDragWindow : public QObject
{
private:
    bool            m_is_moving = false;
    QPoint          m_press_pos;
public:
    explicit        ClickAndDragWindow(QObject *parent);
protected:
    bool            eventFilter(QObject *obj, QEvent *event) override;
};


#endif // WIDGETS_EVENT_FILTERS_H










