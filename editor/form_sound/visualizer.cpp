//
//      Created by Stephens Nunnally on 5/28/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QDebug>
#include <QPaintEvent>

#include "3rd_party/soloud/soloud.h"
#include "core/colors/colors.h"
#include "core/dr_math.h"
#include "core/dr_random.h"
#include "core/sound.h"
#include "editor/event_filters/event_filters.h"
#include "editor/form_sound/visualizer.h"
#include "editor/helper_library.h"
#include "editor/interface_editor_relay.h"
#include "editor/preferences.h"
#include "editor/style/style.h"
#include "project/dr_project.h"


//####################################################################################
//##    Visualizer Paint
//####################################################################################
VisualFrame::VisualFrame(QWidget *parent) : QFrame(parent) { }
VisualFrame::~VisualFrame() { }

// Have this frame resize itself due to changes in Dr::Scale()
void VisualFrame::turnOnAutoScale(double width, double height) {
    m_scale_size =      true;
    m_scale_width =     width;
    m_scale_height =    height;
}

// Visualizer Painting
void VisualFrame::paintEvent(QPaintEvent *) {
    // Check frame is proper size
    if (m_scale_size) {
        double target_width =   (m_scale_width  * 0.5   ) + Dr::Scale(m_scale_width  * 0.5);
        double target_height =  (m_scale_height * 0.3333) + Dr::Scale(m_scale_height * 0.6666);
        if (this->width()  != target_width)  setFixedWidth(target_width);
        if (this->height() != target_height) setFixedHeight(target_height);
    }

    // Initialize QPainter
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    // Paint Border
    if (m_show_border) {
        QRect border_rect = this->rect().adjusted(Dr::Scale(1.0), Dr::Scale(1.0), -Dr::Scale(1.0), -Dr::Scale(1.0));

        // Border
        painter.setBrush(QBrush(Dr::ToQColor(Dr::GetColor(Window_Colors::Background_Dark))));
        painter.setPen(QPen(Dr::ToQColor(Dr::GetColor(Window_Colors::Background_Dark).darker(150)), Dr::Scale(1.0)));
        painter.drawRoundedRect(border_rect, Dr::Scale(4.0), Dr::Scale(4.0));

        // Top Dark Inset
        painter.setBrush(QBrush(Dr::ToQColor(Dr::GetColor(Window_Colors::Background_Dark).darker(150))));
        painter.drawRoundedRect(border_rect.x(), border_rect.y(), border_rect.width(), Dr::Scale(4.0), Dr::Scale(4.0), Dr::Scale(4.0));

    }

    // Paint Visuals
    painter.setRenderHint(QPainter::Antialiasing, false);
    double x_padding = 3.0;
    double x_size = 256.0 / (static_cast<double>(this->rect().width()) - (x_padding*2.0));
    painter.setPen( QPen(Dr::ToQColor(Dr::GetColor(Window_Colors::Icon_Light)), x_size) );

    ///float *wave = Dr::GetSoLoud()->getWave();
    float *fft = Dr::GetSoLoud()->calcFFT();

    for (int x = x_padding; x < this->rect().width() - x_padding; x++) {
        int i = Dr::Clamp(static_cast<int>(x_size * x), 0, 255);

        // For Wave Form
        ///painter.setPen( QPen(Dr::ToQColor(Dr::GetColor(Window_Colors::Seperator)), 1) );
        ///float y_size_w = (this->height() / 10.0) * (wave[i] * 32);
        ///painter.drawLine(x, (this->rect().height()/2) - y_size_w, x, (this->rect().height()/2) + y_size_w);

        // For Frequency Visual
        painter.setPen( QPen(Dr::ToQColor(Dr::GetColor(Window_Colors::Icon_Light)), 1) );
        float y_size_f = (this->height() / 400.0) * (fft[i] * 32);
        painter.drawLine(x, (this->rect().height()/2) - y_size_f, x, (this->rect().height()/2) + y_size_f);
    }
}



























