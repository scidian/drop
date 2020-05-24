//
//      Created by Stephens Nunnally on 5/23/2020, (c) 2020 Scidian Software, All Rights Reserved
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
#include "core/dr_random.h"
#include "editor/event_filters.h"
#include "editor/forms/form_color_magnifier.h"
#include "editor/forms/form_sound.h"
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
    setAttribute(Qt::WA_DeleteOnClose, true);
    setAttribute(Qt::WA_Hover, true);
    setMouseTracking(true);
    setWindowFlags(Qt::WindowType::FramelessWindowHint | Qt::WindowType::Tool);

    setMinimumSize(QSize(740, 560));
    setObjectName(QStringLiteral("childForm"));
    this->setStyleSheet( Dr::CustomStyleSheetFormatting() );

    // ***** Build Form
    buildSoundForm();

    // ***** Center window on Parent Form and install dragging event filter
    Dr::CenterFormOnParent(parent->window(), this);
    this->installEventFilter(new DrFilterClickAndDragWindow(this));

    // ***** Visual Timer
    QTimer *visual_timer = new QTimer(this);
    visual_timer->setInterval(20);
    connect(visual_timer, SIGNAL(timeout()), this, SLOT(drawVisuals()));
    visual_timer->setInterval(30);
    visual_timer->setTimerType(Qt::PreciseTimer);
    visual_timer->start();
}

FormSound::~FormSound() {
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
        m_play_time.restart();
    }

    if (m_play_time.elapsed() < 100) {
        m_visualizer->update();
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

    double x_size = static_cast<double>(this->rect().width() / 256.0);
    painter.setPen( QPen(Dr::ToQColor(Dr::GetColor(Window_Colors::Icon_Light)), x_size) );

    if (m_so_loud) {
        float *fft = m_so_loud->calcFFT();
        float x = 0;
        for (int i = 0; i < 256; i++) {
            x += x_size;
            float y_size = (fft[i] * 32) / 2;
            painter.drawLine(x, (this->rect().height()/2) - y_size, x + 1.0, (this->rect().height()/2) + y_size);
        }
    } else {
        QFrame::paintEvent(event);
    }
}















