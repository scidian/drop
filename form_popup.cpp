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

    // Create a layout for the form
    m_layout = new QVBoxLayout(this);
    m_layout->setContentsMargins(1, 15, 1, 1);

        m_inner_stacked_widget = new QStackedWidget();
        m_inner_stacked_widget->setObjectName(QStringLiteral("innerWidgetPopupBelow"));

    m_layout->addWidget(m_inner_stacked_widget);
}

// Makes sure this form is closed and deleted when it loses focus
void FormPopup::focusOutEvent(QFocusEvent *) { this->close(); }


//####################################################################################
//##        Paints arrow at top / bottom of form
//####################################################################################
void FormPopup::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.setPen( QPen(Dr::GetColor(Window_Colors::Midlight)) );
    painter.setBrush( QBrush(Dr::GetColor(Window_Colors::Button_Dark) ) );

    if (m_below) {
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
    } else {

        painter.drawRoundedRect(1, this->rect().height() - 24, this->rect().width() - 3, 12, 6, 4);
        painter.setPen( Qt::NoPen );
        QPoint points[3];
        points[0] = QPoint( this->rect().width() / 2,       this->rect().height() - 2);
        points[1] = QPoint((this->rect().width() / 2) - 20, this->rect().height() - 22);
        points[2] = QPoint((this->rect().width() / 2) + 20, this->rect().height() - 22);
        painter.drawPolygon(points, 3);

        painter.setPen( QPen(Dr::GetColor(Window_Colors::Midlight)) );
        painter.drawLine( points[0], QPoint((this->rect().width() / 2) - 10, this->rect().height() - 12) );
        painter.drawLine( points[0], QPoint((this->rect().width() / 2) + 10, this->rect().height() - 12) );

    }
    painter.end();
}

//####################################################################################
//##        Keeps container widget same size as form
//####################################################################################
void FormPopup::resizeEvent(QResizeEvent *event)
{
    Dr::ApplyPopupMask(this, 8, 8, m_below);

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
    QPoint top_left =    m_mapper->mapToGlobal(parent_rect.topLeft());

    int x = center.x() - this->geometry().width() / 2 + m_offset.x();
    int y = bot_left.y() + m_offset.y();

    // Make sure it is within the screen
    if (!QGuiApplication::screenAt(QPoint(x, y)) && x < 2) x = 2;

    QScreen *screen = QGuiApplication::screenAt( QCursor::pos() );
    if (screen) {
        if (x + this->geometry().width() +  2 > screen->availableGeometry().right()) {
            x = screen->availableGeometry().right() -  this->geometry().width() - 2;
        }
        if (y + this->geometry().height() + 2 > screen->availableGeometry().bottom()) {
            y = top_left.y() - m_offset.y() - this->geometry().height();
            m_below = false;
            m_layout->setContentsMargins(1, 1, 1, 15);
            m_inner_stacked_widget->setObjectName(QStringLiteral("innerWidgetPopupAbove"));     // Change style sheet for new location
            Dr::ApplyCustomStyleSheetFormatting(m_inner_stacked_widget);                        // Force style sheet to update
            Dr::ApplyPopupMask(this, 8, 8, m_below);
        }
    }
    this->move(x, y);

    qApp->processEvents();              // Make sure we finish moving around widgets if margins changed before we make form visible
    QWidget::showEvent(event);
}


















