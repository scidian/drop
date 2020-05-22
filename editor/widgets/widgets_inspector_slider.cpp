//
//      Created by Stephens Nunnally on 1/8/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QtMath>
#include <QStyleOptionSlider>
#include <QStylePainter>

#include "editor/widgets/widgets_inspector.h"


//####################################################################################
//##    Custom Paint Event
//####################################################################################
void InspectorSlider::paintEvent(QPaintEvent *event) {
    (void) event;

    QStylePainter painter(this);
    QStyleOptionSlider opt;
    initStyleOption(&opt);

    QRect handle = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, this);

    // Draw tick marks
    int interval = tickInterval();
    if (interval == 0) {
        interval = pageStep();
    }

    if (tickPosition() != NoTicks) {
        for (int i = minimum(); i <= maximum(); i += interval) {
            double dx = static_cast<double>(i - this->minimum()) / static_cast<double>(this->maximum() - this->minimum());
                   dx *= static_cast<double>(this->width() - handle.width());
                   dx += (static_cast<double>(handle.width()) / 2.0);
            int x = static_cast<int>(round(dx));
            int h = 4;
            painter.setPen(m_tick_color);
            if (tickPosition() == TicksBothSides || tickPosition() == TicksAbove) {
                int y = this->rect().top() + 1;
                painter.drawLine(x, y, x, y + h);
            }
            if (tickPosition() == TicksBothSides || tickPosition() == TicksBelow) {
                int y = this->rect().bottom() - 1;
                painter.drawLine(x, y, x, y - h);
            }
        }
    }

    // Draw the slider (this is basically copy/pasted from QSlider::paintEvent)
    opt.subControls = QStyle::SC_SliderGroove;
    painter.drawComplexControl(QStyle::CC_Slider, opt);

    // Draw the slider handle
    opt.subControls = QStyle::SC_SliderHandle;
    painter.drawComplexControl(QStyle::CC_Slider, opt);
}


//####################################################################################
//##    Non signal activating setValue
//####################################################################################
void InspectorSlider::updateValue(int value) {
    bool before_block = this->signalsBlocked();
    this->blockSignals(true);
    this->setValue( value );
    this->blockSignals(before_block);
}











