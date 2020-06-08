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
FormSoundEffect::FormSoundEffect(DrProject *project, QWidget *parent) : QWidget(parent), m_project(project) {

    // ***** Initialize Key Generators
    m_key_gen[SoundEffectType::BLIP] = 1;
    m_key_gen[SoundEffectType::COIN] = 1;
    m_key_gen[SoundEffectType::HURT] = 1;
    m_key_gen[SoundEffectType::JUMP] = 1;
    m_key_gen[SoundEffectType::LASER] = 1;
    m_key_gen[SoundEffectType::POWERUP] = 1;
    m_key_gen[SoundEffectType::EXPLOSION] = 1;

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
    connect(m_visual_timer, SIGNAL(timeout()), this, SLOT(drawVisuals()));
    m_visual_timer->setTimerType(Qt::PreciseTimer);
}


//####################################################################################
//##    Destructor
//####################################################################################
FormSoundEffect::~FormSoundEffect() {
    // ***** Delete sounds
    for (auto effect : m_effects) {
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
        m_visual_timer->setInterval(20);
    }
    m_visualizer->update();

    if (m_last_play_time.elapsed() > 1000) {
        m_visualizer->repaint();
        m_visual_timer->setInterval(500);
    }
}











