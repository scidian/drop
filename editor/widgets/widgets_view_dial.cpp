//
//      Created by Stephens Nunnally on 6/8/2020, (c) 2020 Scidian Studios, All Rights Reserved
//
//  File:
//
//
//
#include "core/colors/colors.h"
#include "core/dr_math.h"
#include "editor/helper_library.h"
#include "editor/widgets/widgets_view.h"

// Local Constants
const double    c_knob_radius       =  6.0;             // Determines size of Circle Knotch for Dial_Style::Knotch_Circle dial
const double    c_draw_arc_span     = 16.0;             // QPainter drawArc() spanAngle must be specified in 1/16th of a degree


//####################################################################################
//##    Constructor / Destructor
//####################################################################################
ViewDial::ViewDial(QWidget *parent, Dial_Style style, bool show_ticks, int dial_size)
    : QDial(parent), m_dial_size(dial_size) {

    this->setFixedSize(m_dial_size, m_dial_size);
    m_dial_style = style;
    m_show_ticks = show_ticks;
}

ViewDial::~ViewDial() { }


//####################################################################################
//##    Custom Style Sheet Properties
//####################################################################################
void ViewDial::setDialSize(int size) {
    m_dial_size = size;
}
int ViewDial::getDialSize() const {
    return m_dial_size;
}


//####################################################################################
//##    PAINT: Draw custom Dial
//####################################################################################
void ViewDial::paintEvent(QPaintEvent*) {

    // Update size based on custom Style Sheet property "qproperty-dialSize"
    this->setFixedSize(m_dial_size, m_dial_size);

    // Initialize Painter
    QPainter painter(this);
    painter.setBackgroundMode(Qt::OpaqueMode);
    painter.setRenderHint(QPainter::Antialiasing);

    QColor use_icon_color = Dr::ToQColor(Dr::GetColor(Dr::GetColorScheme() == Color_Scheme::Light ? Window_Colors::Icon_Dark : Window_Colors::Icon_Light));

    // Calculate Angle of Dial
    double ratio = double(QDial::value()) / double(QDial::maximum());           // Find ratio between current value and maximum to calculate angle
    double angle = (ratio * 270.0) + 45.0;                                      // Angle between 0 and 270, starting at lower left (0 = east)

    // Get Diameter of QDial
    double diameter = Dr::Min(QDial::width(), QDial::height());
    double mid = static_cast<double>(QDial::width()) / 2.0;

    // Show Ticks if turned on
    double dial_shrink = 0;
    double y_offset = 0;
    if (m_show_ticks) {
        dial_shrink = Dr::Scale(4.0);
        y_offset = dial_shrink / 2.0;

        // Circular Line
        double full_shrink = Dr::Scale(1.0);
        QRectF full_rect(full_shrink, full_shrink, diameter - (2.0*full_shrink), diameter - (2.0*full_shrink));
               full_rect.moveCenter(QPointF(full_rect.center().x(), full_rect.center().y() + y_offset));

        painter.setBrush(Qt::NoBrush);
        painter.setPen(QPen(use_icon_color, Dr::Scale(1.5), Qt::SolidLine, Qt::RoundCap));
        painter.drawArc(full_rect, (-135 - (270 * ratio)) * c_draw_arc_span, (270 * ratio) * c_draw_arc_span);
        painter.setPen(QPen(Dr::ToQColor(Dr::GetColor(Window_Colors::Shadow)), Dr::Scale(1.5), Qt::SolidLine, Qt::RoundCap));
        painter.drawArc(full_rect, -45 * c_draw_arc_span, (270 * (1.0 - ratio)) * c_draw_arc_span);

        // Circular Ticks
        /**
        painter.setPen(Qt::NoPen);
        painter.setBrush(use_icon_color);
        DrPointF top(0, (diameter / 2.0) - Dr::Scale(1.0));
        double tick_radius = (c_knob_radius / (100.0 / m_dial_size) * 0.5);
        for (int i = 0; i <= 270; i += 30) {
            painter.translate(mid, mid + y_offset);
            painter.rotate(static_cast<double>(i + 45));
            painter.drawEllipse(Dr::ToQPointF(top), tick_radius, tick_radius);
            painter.resetTransform();
        } */

    } else {
        dial_shrink = Dr::Scale(1.0);
        y_offset = 0.0;
    }

    // Calculate Rect for Dial
    QRectF circle_rect(dial_shrink, dial_shrink, diameter - (2.0*dial_shrink), diameter - (2.0*dial_shrink));
           circle_rect.moveCenter(QPointF(circle_rect.center().x(), circle_rect.center().y() + y_offset));

    // Draw Background Circle
    QColor light_color, dark_color;
    light_color = Dr::ToQColor(Dr::GetColor(Window_Colors::Text));
    dark_color  = Dr::ToQColor(Dr::GetColor(Window_Colors::Text_Dark));

    QConicalGradient gradient;
    gradient.setAngle(30);
    gradient.setColorAt(0.0000, light_color);
    gradient.setColorAt(0.1666, dark_color);
    gradient.setColorAt(0.3333, light_color);
    gradient.setColorAt(0.4999, dark_color);
    gradient.setColorAt(0.6666, light_color);
    gradient.setColorAt(0.8333, dark_color);
    gradient.setColorAt(1.0000, light_color);

    painter.translate(mid, mid + y_offset);
    painter.rotate(angle);
    painter.setPen(QPen(Qt::NoPen));
    painter.setBrush(gradient);
    QRectF offset_center = circle_rect;
           offset_center.moveTo(-circle_rect.width() / 2.0, -circle_rect.height() / 2.0);
    painter.drawEllipse(offset_center);
    painter.resetTransform();

    // Draw Background Arcs
    painter.setPen(QPen(Dr::ToQColor(Dr::GetColor(Window_Colors::Text)), Dr::Scale(1.0), Qt::SolidLine, Qt::RoundCap));
    painter.setBrush( Dr::ToQColor(Dr::GetColor(Window_Colors::Text)) );
    painter.drawArc(circle_rect, -45 * c_draw_arc_span, 180 * c_draw_arc_span);
    painter.setPen(QPen(Dr::ToQColor(Dr::GetColor(Window_Colors::Text_Dark)), Dr::Scale(1.0), Qt::SolidLine, Qt::RoundCap));
    painter.setBrush( Dr::ToQColor(Dr::GetColor(Window_Colors::Text_Dark)) );
    painter.drawArc(circle_rect, 135 * c_draw_arc_span, 180 * c_draw_arc_span);

    // Draw Knotch Line
    if (m_dial_style == Dial_Style::Knotch_Line) {
        double scale = static_cast<double>(circle_rect.width()) / 6.0;
        DrPointF top(0, scale * (2.25));
        DrPointF bot(0, scale * (1.00 + Dr::Scale(0.25)));

        painter.translate(mid, mid + y_offset);
        painter.rotate(angle);
        painter.setPen(QPen(Dr::ToQColor(Dr::GetColor(Window_Colors::Seperator)), Dr::Scale(2.0), Qt::SolidLine, Qt::RoundCap));
        painter.drawLine(Dr::ToQPointF(top), Dr::ToQPointF(bot));
        painter.resetTransform();

    // Draw the Value Circle
    } else if (m_dial_style == Dial_Style::Knotch_Circle) {
        double scale = static_cast<double>(circle_rect.width()) / 6.0;
        DrPointF top(0, scale * 2.0);

        double knob_radius = c_knob_radius / (100.0 / m_dial_size);
        painter.translate(mid, mid + y_offset);
        painter.rotate(angle);
        painter.setPen(Qt::NoPen);
        painter.setBrush(Dr::ToQColor(Dr::GetColor(Window_Colors::Seperator)));
        painter.drawEllipse(Dr::ToQPointF(top), knob_radius, knob_radius);
        painter.resetTransform();
    }

}































