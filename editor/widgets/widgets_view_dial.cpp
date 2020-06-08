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
const double    c_knob_margin       = 10.0;
const double    c_knob_radius       = 10.0;


//####################################################################################
//##    Constructor / Destructor
//####################################################################################
ViewDial::ViewDial(QWidget *parent, int dial_size) : QDial(parent), dialSize_(dial_size) {
    this->setFixedSize(dialSize_, dialSize_);
}

ViewDial::~ViewDial() { }


//####################################################################################
//##    Custom Style Sheet Properties
//####################################################################################
void ViewDial::setDialSize(int size) {
    dialSize_ = size;
}
int ViewDial::getDialSize() const {
    return dialSize_;
}


//####################################################################################
//##    PAINT: Draw our Dial with custom colors
//####################################################################################
void ViewDial::paintEvent(QPaintEvent*) {

    this->setFixedSize(dialSize_, dialSize_);
    double knob_radius = c_knob_radius / (100.0 / dialSize_);
    double knob_margin = c_knob_margin / (100.0 / dialSize_);

    static const double degree270 = 1.50 * DR_PI;
    static const double degree225 = 1.25 * DR_PI;

    QPainter painter(this);
    painter.setBackgroundMode(Qt::OpaqueMode);
    painter.setRenderHint(QPainter::Antialiasing);

    // Draw Background Circle
    painter.setPen(QPen(Qt::NoPen));
    painter.setBrush( Dr::ToQColor(Dr::GetColor(Window_Colors::Background_Light)) );
    painter.drawEllipse(1, 1, QDial::height() - 2, QDial::height() - 2);

    // Draw Background Arcs
    int width =  QDial::width();
    int height = QDial::height();
    double r = Dr::Min(width, height) / 2.0;
    double inset = Dr::Scale(0.0);
    double dx = this->rect().x() + inset + (width - 2  * r) / 2.0 + 1;
    double dy = this->rect().y() + inset + (height - 2 * r) / 2.0 + 1;
    QRect br = QRect(int(dx), int(dy), int(r * 2 - 2 * inset - 2), int(r * 2 - 2 * inset - 2));

    painter.setPen(QPen(Dr::ToQColor(Dr::GetColor(Window_Colors::Background_Dark)), 1.0 + Dr::Scale(1.0)));
    painter.setBrush( Dr::ToQColor(Dr::GetColor(Window_Colors::Background_Dark)) );
    painter.drawArc(br,  60 * 16, 180 * 16);
    painter.setPen(QPen(Dr::ToQColor(Dr::GetColor(Window_Colors::Button_Light)), Dr::Scale(1.0)));
    painter.setBrush( Dr::ToQColor(Dr::GetColor(Window_Colors::Button_Dark)) );
    painter.drawArc(br, 240 * 16, 180 * 16);

    // Find ratio between current value and maximum to calculate angle
    double ratio = static_cast<double>(QDial::value()) / static_cast<double>(QDial::maximum());

    // The maximum amount of degrees is 270, offset by 225
    double angle = ratio * degree270 - degree225;

    // Radius of background circle
    double radius = QDial::height() / 2.0;

    // Add r to have (0,0) in center of dial
    double y = sin(angle) * (radius - knob_radius - knob_margin) + radius;
    double x = cos(angle) * (radius - knob_radius - knob_margin) + radius;

    // Draw the Value Circle
    painter.setBrush( Dr::ToQColor(Dr::GetColor(Window_Colors::Icon_Light)) );
    painter.drawEllipse(QPointF(x,y), knob_radius, knob_radius);


    double mid =   static_cast<double>(QDial::width()) / 2.0;

    double scale = static_cast<double>(QDial::width()) / 6.0;

    DrPointF top(0, scale);
    DrPointF bot(0, scale * 2.0);

    top = Dr::RotatePointAroundOrigin(DrPointF(mid, mid), top, angle);
    bot = Dr::RotatePointAroundOrigin(DrPointF(mid, mid), top, angle);

    painter.setBrush( Qt::white );
    painter.drawLine( Dr::ToQPointF(top), Dr::ToQPointF(bot) );

}































