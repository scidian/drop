//
//      Created by Stephens Nunnally on 1/10/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QPainter>

#include "core/colors/colors.h"
#include "editor/helper_library.h"
#include "editor/widgets/widgets.h"
#include "editor/globals_editor.h"


//####################################################################################
//##    Check Box Paint Event
//####################################################################################
void DrQCheckBox::paintEvent(QPaintEvent *) {
    /// default: m_draw_left = 6, m_draw_top = 1
    QRect  checkbox_indicator(m_draw_left - 1, m_draw_top - 1, 22, 26);
    QPoint mouse_position = property(User_Property::Mouse_Pos).toPoint();

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QColor top, middle;
    bool hover = (property(User_Property::Mouse_Over).toBool() && checkbox_indicator.contains(mouse_position));
    if (hover) {
        top =    Dr::ToQColor(Dr::GetColor(Window_Colors::Background_Dark)).darker(110);
        middle = Dr::ToQColor(Dr::GetColor(Window_Colors::Background_Light));
    } else {
        top =    Dr::ToQColor(Dr::GetColor(Window_Colors::Background_Dark)).darker(150);
        middle = Dr::ToQColor(Dr::GetColor(Window_Colors::Background_Dark));
    }

    if (this->isEnabled()) {
        // Draw bottom highlight
        painter.setPen( QPen(Dr::ToQColor(Dr::GetColor(Window_Colors::Background_Dark)).lighter(200), Dr::BorderWidthAsInt()) );
        painter.setBrush( Qt::NoBrush );
        painter.drawRoundedRect(m_draw_left, m_draw_top, 21, 21, 4, 4);

        // Draw main square
        QLinearGradient gradient(m_draw_left, m_draw_top, m_draw_left, 21);
        gradient.setColorAt(0.00, top);
        gradient.setColorAt(0.14, top);
        gradient.setColorAt(0.18, middle);
        gradient.setColorAt(1.00, middle);
        painter.setBrush(gradient);
        painter.setPen( QPen(Dr::ToQColor(Dr::GetColor(Window_Colors::Background_Dark)).darker(200), Dr::BorderWidthAsInt()) );
        painter.drawRoundedRect(m_draw_left, m_draw_top, 21, 20, 4, 4);

        // Draw check mark
        if (checkState()) {
            painter.setPen( QPen(Dr::ToQColor(Dr::GetColor(Window_Colors::Text)), 2, Qt::PenStyle::SolidLine, Qt::PenCapStyle::RoundCap) );
            QVector<QLineF> check;
            ///check.append( QLineF( 11, 13, 13, 16) );
            ///check.append( QLineF( 14, 16, 21,  8) );
            check.append( QLineF( m_draw_left + 5, m_draw_top + 12, m_draw_left + 7,  m_draw_top + 15) );
            check.append( QLineF( m_draw_left + 8, m_draw_top + 15, m_draw_left + 15, m_draw_top +  7) );
            painter.drawLines(check);
        }
    } else {
        // Draw check mark
        if (checkState()) {
            painter.setPen( QPen(Dr::ToQColor(Dr::GetColor(Window_Colors::Text_Dark)), 2, Qt::PenStyle::SolidLine, Qt::PenCapStyle::RoundCap) );
            QVector<QLineF> check;
            ///check.append( QLineF( 11, 13, 13, 16) );
            ///check.append( QLineF( 14, 16, 21,  8) );
            check.append( QLineF( m_draw_left + 5, m_draw_top + 12, m_draw_left + 7,  m_draw_top + 15) );
            check.append( QLineF( m_draw_left + 8, m_draw_top + 15, m_draw_left + 15, m_draw_top +  7) );
            painter.drawLines(check);
        }
    }

    // Draws Outline of CheckBox
    ///painter.setBrush(Qt::NoBrush);
    ///painter.setPen(QPen(Qt::red, 1));
    ///painter.drawRect(this->rect());
}











