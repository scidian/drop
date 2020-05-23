//
//      Created by Stephens Nunnally on 3/21/2019, (c) 2019 Scidian Software, All Rights Reserved
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
#include "editor/forms/form_settings.h"
#include "editor/helper_library.h"
#include "editor/interface_editor_relay.h"
#include "editor/preferences.h"
#include "editor/style/style.h"
#include "project/dr_project.h"


//####################################################################################
//##    Constructor
//####################################################################################
FormSettings::FormSettings(DrProject *project, QWidget *parent) : QWidget(parent) {

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

FormSettings::~FormSettings() { }


//####################################################################################
//##    Keeps container widget same size as form
//####################################################################################
void FormSettings::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);

    Dr::ApplyRoundedCornerMask(this, 8, 8);
}





void FormSettings::playSound() {

    SoLoud::Soloud  gSoloud;    // SoLoud engine
    gSoloud.init();             // Initialize SoLoud

    Dr::Sleep(100);

    //SoLoud::Sfxr coin;
    //coin.loadPreset(SoLoud::Sfxr::EXPLOSION, 2000);
    //coin.setVolume(100);
    //gSoloud.play(coin);


    SoLoud::Speech sp;
    sp.setText("I love Julie so much. She is my wifey. She is such a babe!");
    gSoloud.play(sp);

//    SoLoud::Wav     gWave;        // One wave file
//    gWave.load("pew_pew.wav");    // Load a wave
//    gSoloud.play(gWave);          // Play the wave


    Dr::Sleep(10000);

    gSoloud.deinit();           // Clean up!

}
















