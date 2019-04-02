//
//      Created by Stephens Nunnally on 3/25/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QApplication>
#include <QPushButton>
#include <QGridLayout>
#include <QMouseEvent>

#include "colors.h"
#include "form_popup.h"
#include "globals.h"
#include "library.h"
#include "project.h"
#include "widgets_event_filters.h"

//####################################################################################
//##        Constructor
//####################################################################################
FormPopup::FormPopup(DrProject *project, QWidget *widget_to_use_for_mapToGlobal, QWidget *parent, int x_offset, int y_offset)
    : QWidget(parent), m_project(project), m_mapper(widget_to_use_for_mapToGlobal)
{
    m_offset = QPoint(x_offset, y_offset);

    // ***** Set up initial window
    this->setAttribute(Qt::WidgetAttribute::WA_DeleteOnClose, true);

    this->setWindowFlag(Qt::WindowType::FramelessWindowHint);
    this->setWindowFlag(Qt::WindowType::Popup);
    this->setFixedSize(QSize(50, 50));
    this->setObjectName(QStringLiteral("childForm"));

    Dr::ApplyCustomStyleSheetFormatting(this);

    // Create a layout for the form and add a button
    m_inner_widget = new QWidget(this);
    m_inner_widget->setObjectName(QStringLiteral("innerWidgetPopup"));

}

// Makes sure this form is closed and deleted when it loses focus
void FormPopup::focusOutEvent(QFocusEvent *) { this->close(); }


//####################################################################################
//##        Paints arrow at top of form
//####################################################################################
void FormPopup::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.setPen( QPen(Dr::GetColor(Window_Colors::Midlight)) );
    painter.setBrush( QBrush(Dr::GetColor(Window_Colors::Button_Dark) ) );
    painter.drawRoundedRect(1, 11, this->rect().width() - 3, 24, 9, 6);

    painter.setPen( Qt::NoPen );
    QPoint points[3];
    points[0] = QPoint( this->rect().width() / 2,        1);
    points[1] = QPoint((this->rect().width() / 2) - 20, 21);
    points[2] = QPoint((this->rect().width() / 2) + 20, 21);
    painter.drawPolygon(points, 3);

    painter.setPen( QPen(Dr::GetColor(Window_Colors::Midlight)) );
    painter.drawLine( points[0], QPoint((this->rect().width() / 2) - 10, 11) );
    painter.drawLine( points[0], QPoint((this->rect().width() / 2) + 10, 11) );
}

//####################################################################################
//##        Keeps container widget same size as form
//####################################################################################
void FormPopup::resizeEvent(QResizeEvent *event)
{
    Dr::ApplyPopupMask(this, 8, 8);
    m_inner_widget->setGeometry( 1, 15, this->geometry().width() - 2, this->geometry().height() - 16);

    QWidget::resizeEvent(event);
}



//####################################################################################
//##        Upon first showing, shape window
//####################################################################################
void FormPopup::showEvent(QShowEvent *event)
{
    // Find new desired popup location relative to parent button
    QRect  parent_rect = parentWidget()->geometry();
    QPoint center =      m_mapper->mapToGlobal(parent_rect.center());
    QPoint bot_left =    m_mapper->mapToGlobal(parent_rect.bottomLeft());

    int x = center.x() - this->geometry().width() / 2 + m_offset.x();
    int y = bot_left.y() + m_offset.y();

    // Make sure it is within the screen
    if (!QGuiApplication::screenAt(QPoint(x, y)) && x < 2) x = 2;

    QScreen *screen = QGuiApplication::screenAt( QCursor::pos() );
    if (screen) {
        if (x + this->geometry().width() +  2 > screen->availableGeometry().right())
            x = screen->availableGeometry().right() -  this->geometry().width() - 2;
        if (y + this->geometry().height() + 2 > screen->availableGeometry().bottom())
            y = screen->availableGeometry().bottom() - this->geometry().height() - 2;
    }
    this->move(x, y);

    QWidget::showEvent(event);
}


















