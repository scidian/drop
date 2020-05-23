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
FormSound::FormSound(DrProject *project, QWidget *parent) : QWidget(parent) {

    // ***** Initialize member variables
    m_project = project;

    // ***** Set up initial window
    setAttribute(Qt::WidgetAttribute::WA_DeleteOnClose, true);
    setWindowFlags(Qt::WindowType::FramelessWindowHint | Qt::WindowType::Tool);

    setMinimumSize(QSize(200, 200));
    setObjectName(QStringLiteral("childForm"));
    this->setStyleSheet( Dr::CustomStyleSheetFormatting() );

    // ***** Create a contianer widget, this will allow Create a layout for the form and add a button
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(1, 1, 1, 1);

    m_inner_widget = new QWidget();
    m_inner_widget->setObjectName(QStringLiteral("innerWidget"));
    QVBoxLayout *inner_layout = new QVBoxLayout(m_inner_widget);


        QPushButton *play = new QPushButton("  Play  ");
        Dr::ApplyDropShadowByType(play, Shadow_Types::Button_Shadow);
        play->setObjectName(QStringLiteral("buttonDefault"));
        inner_layout->addWidget(play);

        // Connect a lambda function to the "exit" button to close the form
        connect(play, &QPushButton::clicked, [this] () {
            this->playSound();
        });


        QPushButton *exit = new QPushButton("  Exit  ");
        Dr::ApplyDropShadowByType(exit, Shadow_Types::Button_Shadow);
        exit->setObjectName(QStringLiteral("buttonDefault"));
        inner_layout->addWidget(exit);

        // Connect a lambda function to the "exit" button to close the form
        connect(exit, &QPushButton::clicked, [this] () {
            this->close();
        });


    layout->addWidget(m_inner_widget);

    // ***** Center window on Parent Form and install dragging event filter
    Dr::CenterFormOnParent(parent->window(), this);
    this->installEventFilter(new DrFilterClickAndDragWindow(this));
}

FormSound::~FormSound() { }


//####################################################################################
//##    Keeps container widget same size as form
//####################################################################################
void FormSound::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);

    Dr::ApplyRoundedCornerMask(this, 8, 8);
}


//####################################################################################
//##    Sample sound playing
//####################################################################################
void FormSound::playSound() {

    // ***** Initialize SoLoud
    SoLoud::Soloud so_loud;         // SoLoud engine
    so_loud.init();                 // Initialize SoLoud

    // ***** Sfxr
    //SoLoud::Sfxr explode;
    //explode.loadPreset(SoLoud::Sfxr::explode, 2000);
    //explode.setVolume(100);
    //so_loud.play(explode);


    // ***** Speech
    SoLoud::Speech speech;
    speech.setText("I love Julie so much. She is such a babe!");
    so_loud.play(speech);


    // ***** Sfxr
    //SoLoud::Wav     wave;         // One wave file
    //wave.load("pew_pew.wav");     // Load a wave
    //so_loud.play(wave);           // Play the wave


    Dr::Sleep(4000);
    so_loud.deinit();               // Clean up!
}











