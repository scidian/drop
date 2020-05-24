//
//      Created by Stephens Nunnally on 5/24/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef EVENT_FILTERS_H
#define EVENT_FILTERS_H

#include <QWidget>

#include "editor/constants_advisor_info.h"

// Forward Declarations
class DrProperty;

// Local Enum
enum class Over_Border {
    Top,
    Top_Right,
    Right,
    Bottom_Right,
    Bottom,
    Bottom_Left,
    Left,
    Top_Left,
    None,
};


//####################################################################################
//##    DrFilterHoverHandler
//##        Catches hover events for widgets without needing subclassing
//############################
class DrFilterHoverHandler : public QObject
{
    Q_OBJECT

public:
    DrFilterHoverHandler(QObject *parent) : QObject(parent) { }
    virtual ~DrFilterHoverHandler() { }

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
    // Local Variables
    bool            m_is_resizeable         { false };

    // Internal Variables
    Over_Border     m_over_border           { Over_Border::None };
    bool            m_is_moving             { false };
    bool            m_is_resizing           { false };

    QPoint          m_press_pos             { 0, 0 };
    QRect           m_press_window_rect;

public:
    explicit        DrFilterClickAndDragWindow(QObject *parent = nullptr, bool resizable = false);

protected:
    bool            eventFilter(QObject *obj, QEvent *event) override;
};


#endif // EVENT_FILTERS_H




















