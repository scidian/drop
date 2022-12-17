//
//      Created by Stephens Nunnally on 5/24/2020, (c) 2020 Scidian Studios, All Rights Reserved
//
//  File:
//
//
//
#include <QCoreApplication>
#include <QDebug>
#include <QEvent>
#include <QMouseEvent>
#include <QPainter>

#include "editor/event_filters/event_filters.h"
#include "editor/helper_library.h"

// Local Constants
const   int     c_border_wiggle         = 10;            // Pixels to check if we're within in order to initiate resizing


//####################################################################################
//##    DrFilterClickAndDragWindow Class Functions
//####################################################################################
DrFilterClickAndDragWindow::DrFilterClickAndDragWindow(QObject *parent, bool resizable) : QObject(parent) {
    m_is_resizeable = resizable;
}

bool DrFilterClickAndDragWindow::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::Close) {
        event->accept();
        return QObject::eventFilter(obj, event);
    }

    QMouseEvent *mouse_event = nullptr;
    QHoverEvent *hover_event = nullptr;
    QPoint mouse_pos(0, 0);
    Qt::MouseButtons mouse_buttons;
    if (event->type() == QEvent::MouseButtonPress ||
        event->type() == QEvent::MouseMove ||
        event->type() == QEvent::MouseButtonRelease) {
        mouse_event = dynamic_cast<QMouseEvent*>(event);
        if (mouse_event == nullptr) return QObject::eventFilter(obj, event);
        mouse_pos = mouse_event->pos();
        mouse_buttons = mouse_event->buttons();
    } else if (event->type() == QEvent::HoverMove) {
        hover_event = dynamic_cast<QHoverEvent*>(event);
        if (hover_event == nullptr) return QObject::eventFilter(obj, event);
        mouse_pos = hover_event->position().toPoint();
    }

    // Force new transparent mask when form is done loading and shown on screen
    QResizeEvent *resize_event = nullptr;
    if (event->type() == QEvent::Polish) {
        // Check widget is valid
        QWidget *widget = dynamic_cast<QWidget*>(obj);
        if (widget == nullptr) return QObject::eventFilter(obj, event);

        // Raise event
        QSize new_size(widget->size());
        QSize old_size(widget->size());
        resize_event = new QResizeEvent(new_size, old_size);
        QCoreApplication::postEvent(this, resize_event);

        qDebug() << "Raised resize event";
    }

    if (event->type() == QEvent::MouseButtonPress ||
        event->type() == QEvent::MouseMove ||
        event->type() == QEvent::MouseButtonRelease ||
        event->type() == QEvent::HoverMove ||
        event->type() == QEvent::HoverLeave) {

        // Check widget is valid
        QWidget *widget = dynamic_cast<QWidget*>(obj);
        if (widget == nullptr) return QObject::eventFilter(obj, event);

        // If valid, process event
        if (event->type() == QEvent::HoverLeave) {
            if (m_is_moving == false && m_is_resizing == false) widget->window()->unsetCursor();

        } else if (event->type() == QEvent::MouseButtonPress) {
            if (m_is_resizeable && m_over_border != Over_Border::None) {
                m_press_window_rect = widget->window()->geometry();
                m_press_pos = mouse_event->globalPosition().toPoint();
                m_is_moving = false;
                m_is_resizing = true;
            } else {
                m_press_pos = mouse_pos;
                m_is_moving = true;
                m_is_resizing = false;
            }

        } else if (event->type() == QEvent::MouseButtonRelease) {
            m_is_moving = false;
            m_is_resizing = false;

        } else if (event->type() == QEvent::MouseMove) {
            if (mouse_buttons & Qt::NoButton) {
                m_is_moving = false;
                m_is_resizing = false;
            } else if (m_is_moving) {
                QPoint diff = mouse_pos - m_press_pos;
                widget->window()->move( widget->window()->pos() + diff );
            } else if (m_is_resizing) {

                // Adjust new window Rect
                QRect  window_rect = m_press_window_rect;
                QPoint diff = mouse_event->globalPosition().toPoint() - m_press_pos;
                if (m_over_border == Over_Border::Bottom_Right) {
                    window_rect.adjust(0, 0, diff.x(), diff.y());
                } else if (m_over_border == Over_Border::Bottom_Left) {
                    window_rect.adjust(diff.x(), 0, 0, diff.y());
                } else if (m_over_border == Over_Border::Top_Left) {
                    window_rect.adjust(diff.x(), diff.y(), 0, 0);
                } else if (m_over_border == Over_Border::Top_Right) {
                    window_rect.adjust(0, diff.y(), diff.x(), 0);
                } else if (m_over_border == Over_Border::Bottom) {
                    window_rect.adjust(0, 0, 0, diff.y());
                } else if (m_over_border == Over_Border::Left) {
                    window_rect.adjust(diff.x(), 0, 0, 0);
                } else if (m_over_border == Over_Border::Top) {
                    window_rect.adjust(0, diff.y(), 0, 0);
                } else if (m_over_border == Over_Border::Right) {
                    window_rect.adjust(0, 0, diff.x(), 0);
                }

                // Check that Window is not smaller than minimum size
                if (window_rect.width() < widget->window()->minimumWidth()) {
                    if (m_over_border == Over_Border::Left || m_over_border == Over_Border::Top_Left || m_over_border == Over_Border::Bottom_Left) {
                        window_rect.setX(m_press_window_rect.right() - widget->window()->minimumWidth());
                    } else {
                        window_rect.setX(m_press_window_rect.x());
                    }
                    window_rect.setWidth(widget->window()->minimumWidth());
                }
                if (window_rect.height() < widget->window()->minimumHeight()) {
                    if (m_over_border == Over_Border::Top || m_over_border == Over_Border::Top_Left || m_over_border == Over_Border::Top_Right) {
                        window_rect.setY(m_press_window_rect.bottom() - widget->window()->minimumHeight());
                    } else {
                        window_rect.setY(m_press_window_rect.y());
                    }
                    window_rect.setHeight(widget->window()->minimumHeight());
                }
                widget->window()->setGeometry(window_rect);
            }

        } else if (event->type() == QEvent::HoverMove) {
            if (m_is_moving == false && m_is_resizing == false && m_is_resizeable) {
                QRect bottom_right = QRect(widget->window()->rect().bottomRight() - QPoint(c_border_wiggle, c_border_wiggle), widget->window()->rect().bottomRight());
                QRect bottom_left =  QRect(QPoint(0, widget->window()->rect().bottom() - c_border_wiggle), QPoint(c_border_wiggle, widget->window()->rect().bottom()));
                QRect top_left =     QRect(QPoint(0, 0), QPoint(c_border_wiggle, c_border_wiggle));
                QRect top_right =    QRect(QPoint(widget->window()->rect().right() - c_border_wiggle, 0), QPoint(widget->window()->rect().right(), c_border_wiggle));

                QRect bottom =       QRect(QPoint(0, widget->window()->rect().bottom() - c_border_wiggle), widget->window()->rect().bottomRight());
                QRect left =         QRect(QPoint(0, 0), QPoint(c_border_wiggle, widget->window()->rect().bottom()));
                QRect top =          QRect(QPoint(0, 0), QPoint(widget->window()->rect().right(), c_border_wiggle));
                QRect right =        QRect(QPoint(widget->window()->rect().right() - c_border_wiggle, 0), widget->window()->rect().bottomRight());

                if (bottom_right.contains(mouse_pos)) {
                    widget->window()->setCursor(Qt::CursorShape::SizeFDiagCursor);
                    m_over_border = Over_Border::Bottom_Right;
                } else if (bottom_left.contains(mouse_pos)) {
                    widget->window()->setCursor(Qt::CursorShape::SizeBDiagCursor);
                    m_over_border = Over_Border::Bottom_Left;
                } else if (top_left.contains(mouse_pos)) {
                    widget->window()->setCursor(Qt::CursorShape::SizeFDiagCursor);
                    m_over_border = Over_Border::Top_Left;
                } else if (top_right.contains(mouse_pos)) {
                    widget->window()->setCursor(Qt::CursorShape::SizeBDiagCursor);
                    m_over_border = Over_Border::Top_Right;
                } else if (bottom.contains(mouse_pos)) {
                    widget->window()->setCursor(Qt::CursorShape::SizeVerCursor);
                    m_over_border = Over_Border::Bottom;
                } else if (left.contains(mouse_pos)) {
                    widget->window()->setCursor(Qt::CursorShape::SizeHorCursor);
                    m_over_border = Over_Border::Left;
                } else if (top.contains(mouse_pos)) {
                    widget->window()->setCursor(Qt::CursorShape::SizeVerCursor);
                    m_over_border = Over_Border::Top;
                } else if (right.contains(mouse_pos)) {
                    widget->window()->setCursor(Qt::CursorShape::SizeHorCursor);
                    m_over_border = Over_Border::Right;
                } else {
                    widget->window()->unsetCursor();
                    m_over_border = Over_Border::None;
                }
            }
        }

        event->accept();
    }

    return QObject::eventFilter(obj, event);
}















