//
//      Created by Stephens Nunnally on 6/4/2020, (c) 2020 Scidian Software, All Rights Reserved
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
#include "editor/form_sound/form_speech_synthesis.h"
#include "editor/form_sound/visualizer.h"
#include "editor/helper_library.h"
#include "editor/interface_editor_relay.h"
#include "editor/preferences.h"
#include "editor/style/style.h"
#include "project/dr_project.h"


//####################################################################################
//##    Constructor
//####################################################################################
FormSpeechSynthesis::FormSpeechSynthesis(DrProject *project, QWidget *parent) : QWidget(parent), m_project(project) {

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
    buildSoundEffectForm();

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
    m_visual_timer->setTimerType(Qt::PreciseTimer);
}

FormSpeechSynthesis::~FormSpeechSynthesis() {
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
void FormSpeechSynthesis::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    Dr::ApplyRoundedCornerMask(this, 8, 8);
}

// Visualizer
void FormSpeechSynthesis::drawVisuals() {
    if (m_so_loud->getActiveVoiceCount() > 0) {
        m_visual_timer->setInterval(20);
    }
    m_visualizer->update();

    if (m_last_play_time.elapsed() > 1000) {
        m_visualizer->repaint();
        m_visual_timer->setInterval(500);
    }
}







