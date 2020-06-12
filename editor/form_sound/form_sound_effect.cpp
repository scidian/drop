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

#include "3rd_party/soloud/soloud_sfxr.h"
#include "core/colors/colors.h"
#include "core/dr_math.h"
#include "core/dr_random.h"
#include "core/sound.h"
#include "editor/event_filters/event_filters.h"
#include "editor/form_main/form_main.h"
#include "editor/form_sound/form_sound_effect.h"
#include "editor/form_sound/visualizer.h"
#include "editor/helper_library.h"
#include "editor/interface_editor_relay.h"
#include "editor/preferences.h"
#include "editor/style/style.h"
#include "project/dr_project.h"


//####################################################################################
//##    Constructor
//####################################################################################
FormSoundEffect::FormSoundEffect(DrProject *project, FormMain *form_main, QWidget *parent)
    : QWidget(parent), m_sound_creator(form_main), m_project(project) {

    // ***** Initialize type counts
    m_type_counts[SoundEffectType::BLIP] = 1;
    m_type_counts[SoundEffectType::COIN] = 1;
    m_type_counts[SoundEffectType::HURT] = 1;
    m_type_counts[SoundEffectType::JUMP] = 1;
    m_type_counts[SoundEffectType::LASER] = 1;
    m_type_counts[SoundEffectType::POWERUP] = 1;
    m_type_counts[SoundEffectType::EXPLOSION] = 1;

    // ***** Set up initial window
    setWindowFlags(Qt::WindowType::FramelessWindowHint | Qt::WindowType::Tool);
    setMouseTracking(true);
    setAttribute(Qt::WA_DeleteOnClose, true);
    setAttribute(Qt::WA_Hover, true);
    setAttribute(Qt::WA_NoMousePropagation, false);

    setMinimumSize(QSize(740, 560));
    setObjectName(QStringLiteral("childForm"));
    setStyleSheet( Dr::CustomStyleSheetFormatting() );

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
    m_visual_timer->setTimerType(Qt::PreciseTimer);
    connect(m_visual_timer, SIGNAL(timeout()), this, SLOT(drawVisuals()));
}


//####################################################################################
//##    Destructor
//####################################################################################
FormSoundEffect::~FormSoundEffect() {
    // ***** Delete sounds
    for (auto effect : m_effects) {
        // Don't delete sound to be passed on back to Project
        if (effect.first != m_selected_effect) {
            delete effect.second;
        }
    }
}


//####################################################################################
//##    Events
//####################################################################################
// Keeps container widget same size as form
void FormSoundEffect::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    Dr::ApplyRoundedCornerMask(this, 8, 8);
}

// Visualizer
void FormSoundEffect::drawVisuals() {
    if (Dr::GetSoLoud()->getActiveVoiceCount() > 0) {
        m_sound_creator->drawVisuals();
        m_visual_timer->start();
        m_last_play_time.restart();
    }

    if (m_last_play_time.elapsed() < 2000) {
        m_visualizer->repaint();
    } else {
        m_visual_timer->stop();
    }
}











