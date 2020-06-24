//
//      Created by Stephens Nunnally on 6/4/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QDebug>
#include <QFileDialog>
#include <QLabel>
#include <QRadioButton>
#include <QStandardPaths>

#include <core/sound.h>
#include "editor/form_main/form_main.h"
#include "editor/form_sound/form_sound_effect.h"
#include "editor/form_sound/form_speech_synthesis.h"
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
            << tr("Add Sound Effect")
            << tr("Add White Noise");
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

        QRadioButton *buttonMix =           new QRadioButton(options[0]);   buttonMix->setObjectName(QStringLiteral("popupRadio"));
        QRadioButton *buttonAudio =         new QRadioButton(options[1]);   buttonAudio->setObjectName(QStringLiteral("popupRadio"));
        QRadioButton *buttonInstrument =    new QRadioButton(options[2]);   buttonInstrument->setObjectName(QStringLiteral("popupRadio"));
        QRadioButton *buttonSpeech =        new QRadioButton(options[3]);   buttonSpeech->setObjectName(QStringLiteral("popupRadio"));
        QRadioButton *buttonEffect =        new QRadioButton(options[4]);   buttonEffect->setObjectName(QStringLiteral("popupRadio"));
        QRadioButton *buttonNoise =         new QRadioButton(options[5]);   buttonNoise->setObjectName(QStringLiteral("popupRadio"));

        buttonMix->setFont(font);           buttonMix->setFixedHeight(          rects[0].height() + 4 );
        buttonAudio->setFont(font);         buttonAudio->setFixedHeight(        rects[1].height() + 4 );
        buttonInstrument->setFont(font);    buttonInstrument->setFixedHeight(   rects[2].height() + 4 );
        buttonSpeech->setFont(font);        buttonSpeech->setFixedHeight(       rects[3].height() + 4 );
        buttonEffect->setFont(font);        buttonEffect->setFixedHeight(       rects[4].height() + 4 );
        buttonNoise->setFont(font);         buttonNoise->setFixedHeight(        rects[5].height() + 4 );

        buttonMix->setCheckable(false);
        buttonAudio->setCheckable(false);
        buttonInstrument->setCheckable(false);
        buttonSpeech->setCheckable(false);
        buttonEffect->setCheckable(false);
        buttonNoise->setCheckable(false);

        // ****************************************

        // ********** Adds new Mix to Project
        connect(buttonMix, &QRadioButton::released, [this]() {            
            IEditorRelay *editor = Dr::GetActiveEditorRelay();
            if (editor) editor->stopAllSound();                                 // Cannot load files while SoLoud is playing sound

            // Add Mix
            DrMix *mix = m_project->addMix();

            // Update EditorRelay widgets
            if (editor && mix) {
                editor->buildAssetTree();
                editor->buildInspector( { mix->getKey() } );
                editor->getAssetTree()->setSelectedKey(mix->getKey());
                editor->getAssetTree()->setFocus(Qt::FocusReason::PopupFocusReason);
            }
            // Close this popup
            this->close();
            // Make sure we leave with Asset Tree highlighted and active
            if (editor && mix != nullptr) {
                editor->getAssetTree()->setFocus(Qt::FocusReason::PopupFocusReason);
            }
        });

        // ********** Adds Audio File
        connect(buttonAudio, &QRadioButton::released, [this]() {
            // Grab directory from current save file, if no save file yet, use Desktop location
            QString directory = QString::fromStdString(getProject()->getOption(Project_Options::File_Name_Path).toString());
            if (directory == "") {
                directory = QStandardPaths::writableLocation(QStandardPaths::StandardLocation::DesktopLocation);
            }

            // Show dialog for opening a file
            QString caption =   tr("Open Project");
            QString filter =    tr("All Files (*);;Audio Files (*.mp3 *.wav *.flac *.ogg);;Mp3 Files (*.mp3)");
            QString selected =  tr("Mp3 Files (*.mp3)");
            QFileDialog::Options dialog_options;
            QString open_file = QFileDialog::getOpenFileName(this, caption, directory, filter, &selected, dialog_options);

            if (!open_file.isEmpty()) {
                // Figure out Sound name
                QFileInfo file_info(open_file);
                    QString full_path =     file_info.path();
                    QString file_name =     file_info.fileName();
                    QString simple_name =   file_info.baseName();
                            simple_name =   simple_name.replace("_", " ");      // Replace underscores with spaces
                            simple_name =   simple_name.replace("-", " ");      // Replace hyphens with spaces

                // Load Audio File
                SoLoud::Wav *wav = new SoLoud::Wav();
                int result = wav->load(open_file.toStdString().data());

                if (result == SoLoud::SO_NO_ERROR) {
                    IEditorRelay *editor = Dr::GetActiveEditorRelay();
                    if (editor) editor->stopAllSound();                         // Cannot load files while SoLoud is playing sound

                    // Add Audio File Sound
                    DrSound *sound = m_project->addSound(DrSoundType::Audio_File, wav, c_no_key, simple_name.toStdString());

                    // Update EditorRelay widgets
                    if (editor && sound) {
                        editor->buildAssetTree();
                        editor->buildInspector( { sound->getKey() } );
                        editor->getAssetTree()->setSelectedKey(sound->getKey());
                        editor->getAssetTree()->setFocus(Qt::FocusReason::PopupFocusReason);
                    }
                    // Close this popup
                    this->close();
                    // Make sure we leave with Asset Tree highlighted and active
                    if (editor && sound) {
                        editor->getAssetTree()->setFocus(Qt::FocusReason::PopupFocusReason);
                    }
                }
            }
        });

        // ********** Adds Instrument Sound
        connect(buttonInstrument, &QRadioButton::released, []() {

        });

        // ********** Adds Speech Synthesis
        connect(buttonSpeech, &QRadioButton::released, [this]() {
            FormMain *form_main = Dr::GetActiveFormMain();
            if (form_main != nullptr) {
                form_main->stopAllSound();                                      // Cannot load files while SoLoud is playing sound
                FormSpeechSynthesis *speech = new FormSpeechSynthesis(m_project, form_main);
                speech->setFocus(Qt::FocusReason::PopupFocusReason);
                speech->show();
                this->close();
                speech->setFocus(Qt::FocusReason::PopupFocusReason);
            } else {
                this->close();
            }
        });

        // ********** Adds Sound Effect
        connect(buttonEffect, &QRadioButton::released, [this]() {
            FormMain *form_main = Dr::GetActiveFormMain();
            if (form_main != nullptr) {
                form_main->stopAllSound();                                      // Cannot load files while SoLoud is playing sound
                FormSoundEffect *sound_effects = new FormSoundEffect(m_project, form_main);
                sound_effects->setFocus(Qt::FocusReason::PopupFocusReason);
                sound_effects->show();
                this->close();
                sound_effects->setFocus(Qt::FocusReason::PopupFocusReason);
            } else {
                this->close();
            }
        });

        // ********** Adds White Noise
        connect(buttonEffect, &QRadioButton::released, []() {
        });

        // ****************************************

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
        layout->addWidget(buttonNoise);
        layout->addSpacing(4);
}











