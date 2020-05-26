//
//      Created by Stephens Nunnally on 5/25/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QDebug>
#include <QMouseEvent>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>

#include "3rd_party/soloud/soloud.h"
#include "3rd_party/soloud/soloud_sfxr.h"
#include "3rd_party/soloud/soloud_speech.h"
#include "3rd_party/soloud/soloud_wav.h"
#include "core/colors/colors.h"
#include "core/dr_math.h"
#include "core/dr_random.h"
#include "editor/event_filters/event_filters.h"
#include "editor/form_sound/form_sound.h"
#include "editor/forms/form_color_magnifier.h"
#include "editor/helper_library.h"
#include "editor/interface_editor_relay.h"
#include "editor/preferences.h"
#include "editor/style/style.h"
#include "project/dr_project.h"


//####################################################################################
//##    Constructor
//####################################################################################
FormSound::FormSound(DrProject *project, QWidget *parent) : QWidget(parent), m_project(project) {

    // ***** Initialize SoLoud
    m_so_loud = new SoLoud::Soloud();
    m_so_loud->init();
    m_so_loud->setVisualizationEnable(true);

    // ***** Set up initial window
    setWindowFlags(Qt::WindowType::FramelessWindowHint | Qt::WindowType::Tool);
    setMouseTracking(true);
    setAttribute(Qt::WA_DeleteOnClose, true);
    setAttribute(Qt::WA_Hover, true);
    setAttribute(Qt::WA_NoMousePropagation, false);

    setMinimumSize(QSize(740, 560));
    setObjectName(QStringLiteral("childForm"));
    this->setStyleSheet( Dr::CustomStyleSheetFormatting() );

    // ***** Build Form
    buildSoundForm();

    // ***** Center window on Parent Form and install dragging event filter
    if (parent) {
        Dr::CenterFormOnParent(parent->window(), this);
    } else {
        Dr::CenterFormOnScreen(parent, this);
    }
    this->installEventFilter(new DrFilterClickAndDragWindow(this, true));

    // ***** Visual Timer
    m_visual_timer = new QTimer(this);
    m_visual_timer->setInterval(20);
    connect(m_visual_timer, SIGNAL(timeout()), this, SLOT(drawVisuals()));
    m_visual_timer->setInterval(25);
    m_visual_timer->setTimerType(Qt::PreciseTimer);
}

FormSound::~FormSound() {
    // Delete sounds
    for (auto effect : m_effects)   { delete effect.second; }

    // Clean up sound object
    m_so_loud->deinit();
    delete m_so_loud;
}


//####################################################################################
//##    Events
//####################################################################################
// Keeps container widget same size as form
void FormSound::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    Dr::ApplyRoundedCornerMask(this, 8, 8);
}

// Visualizer
void FormSound::drawVisuals() {
    if (m_so_loud->getActiveVoiceCount() > 0) {
        m_last_play_time.restart();
    }
    m_visualizer->update();

    if (m_last_play_time.elapsed() > 350) {
        m_visualizer->repaint();
        m_visual_timer->stop();
    }
}



//####################################################################################
//##    Visual Paint
//####################################################################################
VisualFrame::VisualFrame(SoLoud::Soloud *so_loud, QWidget *parent)
    : QFrame(parent), m_so_loud(so_loud) { }

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






