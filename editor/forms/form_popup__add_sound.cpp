//
//      Created by Stephens Nunnally on 6/4/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QDebug>
#include <QLabel>
#include <QRadioButton>

#include "editor/form_main/form_main.h"
#include "editor/form_sound/form_sound_effect.h"
#include "editor/forms/form_popup.h"
#include "editor/helper_library.h"
#include "editor/preferences.h"
#include "editor/project/project.h"
#include "editor/trees/tree_assets.h"
#include "editor/trees/tree_inspector.h"
#include "editor/trees/tree_project.h"
#include "project/dr_project.h"
#include "project/entities_sound/dr_mix.h"
#include "project/entities_sound/dr_sound.h"
#include "project/settings/settings.h"


//####################################################################################
//##    Builds out FormPop's inner widget to show options for Adding a DrSound to Project
//####################################################################################
void FormPopup::buildPopupAddSoundEntity() {
    QFont font = Dr::CustomFont();
    QFontMetrics font_metrics { font };

    // Figure out best size of popup
    QStringList options;
    options << tr("Add New Mix")
            << tr("Add Audio File")
            << tr("Add Instrument Sound")
            << tr("Add Speech Synthesis")
            << tr("Add Sound Effect");
    QList<QRect> rects;
    int width = 0;
    for (auto option : options) {
        rects.append( font_metrics.boundingRect(option));
        width = qMax(width, font_metrics.boundingRect(option).width() + 50);
    }
    this->setMinimumWidth(width);
    this->setMinimumHeight( (rects.first().height() * options.count()) + 50);

    // Layout popup
    QVBoxLayout *layout = new QVBoxLayout(m_inner_widget);
    layout->setContentsMargins(8, 0, 5, 10);
    layout->setAlignment(Qt::AlignVCenter);
    layout->setSpacing(6);

        QRadioButton *buttonMix =           new QRadioButton(options[0]);   buttonMix->setObjectName( QStringLiteral("popupRadio"));
        QRadioButton *buttonAudio =         new QRadioButton(options[1]);   buttonAudio->setObjectName( QStringLiteral("popupRadio"));
        QRadioButton *buttonInstrument =    new QRadioButton(options[2]);   buttonInstrument->setObjectName(  QStringLiteral("popupRadio"));
        QRadioButton *buttonSpeech =        new QRadioButton(options[3]);   buttonSpeech->setObjectName(QStringLiteral("popupRadio"));
        QRadioButton *buttonEffect =        new QRadioButton(options[4]);   buttonEffect->setObjectName(QStringLiteral("popupRadio"));

        buttonMix->setFont(font);           buttonMix->setFixedHeight(          rects[0].height() + 4 );
        buttonAudio->setFont(font);         buttonAudio->setFixedHeight(        rects[1].height() + 4 );
        buttonInstrument->setFont(font);    buttonInstrument->setFixedHeight(   rects[2].height() + 4 );
        buttonSpeech->setFont(font);        buttonSpeech->setFixedHeight(       rects[3].height() + 4 );
        buttonEffect->setFont(font);        buttonEffect->setFixedHeight(       rects[4].height() + 4 );

        buttonMix->setCheckable(false);
        buttonAudio->setCheckable(false);
        buttonInstrument->setCheckable(false);
        buttonSpeech->setCheckable(false);
        buttonEffect->setCheckable(false);

        // Adds new Mix to Project
        connect(buttonMix, &QRadioButton::released, [this]() {
            DrMix *mix = nullptr;
            IEditorRelay *editor = Dr::GetActiveEditorRelay();
            if (editor) {
                editor->buildInspector( { } );      // Clear inspector to stop Inspector signals
                mix = m_project->addMix();
                editor->buildProjectTree();
                editor->buildInspector( { mix->getKey() } );
                editor->updateItemSelection(Editor_Widgets::View, { mix->getKey() } );
                editor->getProjectTree()->setFocus(Qt::FocusReason::PopupFocusReason);
                editor->buildScene( mix->getKey() );
            }
            this->close();
            if (editor && mix != nullptr) {
                editor->updateItemSelection(Editor_Widgets::View, { mix->getKey() } );
                editor->getProjectTree()->setFocus(Qt::FocusReason::PopupFocusReason);
            }
        });

        // Adds Audio File
        connect(buttonAudio, &QRadioButton::released, []() {
        });

        // Adds Instrument Sound
        connect(buttonInstrument,   &QRadioButton::released, []() {
        });

        // Adds Speech Synthesis
        connect(buttonSpeech, &QRadioButton::released, []() {
        });

        // Adds Sound Effect
        connect(buttonEffect, &QRadioButton::released, [this]() {
            FormSoundEffect *sound_effects = new FormSoundEffect(m_project, nullptr);
            sound_effects->show();
            sound_effects->setFocus(Qt::FocusReason::PopupFocusReason);
            this->close();                                                                              // Close this popup
            sound_effects->setFocus(Qt::FocusReason::PopupFocusReason);
        });

        // Add Mix option
        layout->addSpacing(4);
        layout->addWidget(buttonMix);
        layout->addSpacing(3);

        // Divider line looks nice
        QLabel *line_1 = new QLabel();
        line_1->setMaximumHeight(1);
        line_1->setStyleSheet(QString::fromStdString(
                              "color: " +      Dr::GetColor(Window_Colors::Midlight).name() + "; "
                              "background: " + Dr::GetColor(Window_Colors::Midlight).name() + "; "
                              "border: none; margin-left: 5px; margin-right: 5px;"));
        layout->addWidget(line_1);

        // Add Sound options
        layout->addWidget(buttonAudio);
        layout->addWidget(buttonInstrument);
        layout->addWidget(buttonSpeech);
        layout->addWidget(buttonEffect);
        layout->addSpacing(4);
}











