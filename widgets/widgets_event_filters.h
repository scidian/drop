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
//##    DrFilterHoverHandler
//##        Catches hover events for widgets without needing subclassing
//############################
class DrFilterHoverHandler : public QObject
{
    Q_OBJECT

public:
    DrFilterHoverHandler(QObject *parent) : QObject(parent) {}
    virtual ~DrFilterHoverHandler() {}

    void            attachToHoverHandler(QWidget *widget, DrProperty *property);
    void            attachToHoverHandler(QWidget *widget, QString header, QString body);
    void            attachToHoverHandler(QWidget *widget, HeaderBodyList header_body_list);

protected:
    bool            eventFilter(QObject *obj, QEvent *event);

signals:
    void            signalMouseHover(QString header, QString body);
};



//####################################################################################
//##    DrFilterMouseWheelAdjustmentGuard
//##        Stops widget from stealing focus on mouse wheel
//############################
class DrFilterMouseWheelAdjustmentGuard : public QObject
{
public:
    explicit        DrFilterMouseWheelAdjustmentGuard(QObject *parent);
protected:
    bool            eventFilter(QObject *obj, QEvent *event) override;
};


//####################################################################################
//##    DrFilterPopUpMenuRelocater
//##        Changes starting position of popup menu
//############################
class DrFilterPopUpMenuRelocater : public QObject
{
private:
    int         m_top_offset;
    int         m_left_offset;
public:
    explicit        DrFilterPopUpMenuRelocater(QObject *parent, int top_offset, int left_offset);
protected:
    bool            eventFilter(QObject *obj, QEvent *event) override;
};


//####################################################################################
//##    DrFilterClickAndDragWindow
//##        Clicking and dragging will move parent window
//############################
class DrFilterClickAndDragWindow : public QObject
{
private:
    bool            m_is_moving = false;
    QPoint          m_press_pos;
public:
    explicit        DrFilterClickAndDragWindow(QObject *parent);
protected:
    bool            eventFilter(QObject *obj, QEvent *event) override;
};




#endif // WIDGETS_EVENT_FILTERS_H










