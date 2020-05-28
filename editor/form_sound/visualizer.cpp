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
VisualFrame::VisualFrame(SoLoud::Soloud *so_loud, QWidget *parent) : QFrame(parent), m_so_loud(so_loud) { }
VisualFrame::~VisualFrame() { }

void VisualFrame::paintEvent(QPaintEvent *event) {
    QPainter painter(this);

    double x_size = 256.0 / static_cast<double>(this->rect().width());
    painter.setPen( QPen(Dr::ToQColor(Dr::GetColor(Window_Colors::Icon_Light)), x_size) );

    if (m_so_loud) {
        ///float *wave = m_so_loud->getWave();
        float *fft = m_so_loud->calcFFT();

        for (int x = 0; x < this->rect().width(); x++) {
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
    } else {
        QFrame::paintEvent(event);
    }
}



























