//
//      Created by Stephens Nunnally on 6/8/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "core/colors/colors.h"
#include "core/dr_math.h"
#include "editor/helper_library.h"
#include "editor/widgets/widgets_view.h"

// Local Constans
const double    c_knob_margin       =  5.0;
const double    c_knob_radius       = 10.0;


//####################################################################################
//##    PAINT: Draw our Dial with custom colors
//####################################################################################
void ViewDial::paintEvent(QPaintEvent*) {

    static const double degree270 = 1.50 * DR_PI;
    static const double degree225 = 1.25 * DR_PI;

    QPainter painter(this);
    painter.setBackgroundMode(Qt::OpaqueMode);
    painter.setRenderHint(QPainter::Antialiasing);

    // Draw Background Circle
    painter.setPen(QPen(Qt::NoPen));
    painter.setBrush( Dr::ToQColor(Dr::GetColor(Window_Colors::Background_Light)) );
    painter.drawEllipse(0, 0, QDial::height(), QDial::height());

    // Draw dial arc
    int width =  this->rect().width();
    int height = this->rect().height();
    double r = Dr::Min(width, height) / 2.0;
    double d = r / 6.0;
    double dx = this->rect().x() + d + (width - 2  * r) / 2.0 + 1;
    double dy = this->rect().y() + d + (height - 2 * r) / 2.0 + 1;
    QRect br = QRect(int(dx), int(dy), int(r * 2 - 2 * d - 2), int(r * 2 - 2 * d - 2));

    painter.setPen(QPen(Dr::ToQColor(Dr::GetColor(Window_Colors::Background_Dark))));
    painter.setBrush( Dr::ToQColor(Dr::GetColor(Window_Colors::Background_Dark)) );
    painter.drawArc(br, 60 * 16, 180 * 16);
    painter.setPen(QPen(Dr::ToQColor(Dr::GetColor(Window_Colors::Button_Light))));
    painter.setBrush( Dr::ToQColor(Dr::GetColor(Window_Colors::Button_Dark)) );
    painter.drawArc(br, 240 * 16, 180 * 16);

    // Find ratio between current value and maximum to calculate angle
    double ratio = static_cast<double>(QDial::value()) / static_cast<double>(QDial::maximum());

    // The maximum amount of degrees is 270, offset by 225
    double angle = ratio * degree270 - degree225;

    // Radius of background circle
    double radius = QDial::height() / 2.0;

    // Add r to have (0,0) in center of dial
    double y = sin(angle) * (radius - c_knob_radius - c_knob_margin) + radius;
    double x = cos(angle) * (radius - c_knob_radius - c_knob_margin) + radius;

    // Draw the Value Circle
    painter.setBrush( Dr::ToQColor(Dr::GetColor(Window_Colors::Icon_Light)) );
    painter.drawEllipse(QPointF(x,y), c_knob_radius, c_knob_radius);
}










