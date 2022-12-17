//
//      Created by Stephens Nunnally on 6/4/2020, (c) 2020 Scidian Studios, All Rights Reserved
//
//  File:
//
//
//
#ifndef FORM_SPEECH_SYNTHESIS_H
#define FORM_SPEECH_SYNTHESIS_H

#include <QElapsedTimer>
#include <QLabel>
#include <QListWidget>
#include <QSlider>
#include <QWidget>

#include "3rd_party/soloud/soloud.h"
#include "3rd_party/soloud/soloud_sfxr.h"
#include "3rd_party/soloud/soloud_speech.h"
#include "3rd_party/soloud/soloud_wav.h"
#include "project/enums_entity_types.h"

// Forward Declarations
class DrProject;
class DrSound;
class VisualFrame;
class WaveForm;


//####################################################################################
//##    FormSpeechSynthesis
//##        Form to create / add Speech Synthesis
//############################
class FormSpeechSynthesis : public QWidget
{
    Q_OBJECT

private:
    // External Borrowed Pointers
    DrProject              *m_project;                                      // Pointer to the open project

    // Sound Variables
    SoLoud::Speech         *m_speech            { nullptr };
    int                     m_speech_waveform   { KW_TRIANGLE };
    std::string             m_speech_text;

    SoLoud::Wav             m_wave;

    // Form Variables
    QWidget            *m_inner_widget;                                     // Container widget, allows for a double form border

    VisualFrame        *m_visualizer;                                       // Widget that displays visualizer for current sounds playing
    QTimer             *m_visual_timer;                                     // Event timer that calls update for visualizer
    QElapsedTimer       m_last_play_time;                                   // Tracks when sound was last still playing

    WaveForm           *m_sound_wave;                                       // Draws currently selected sound wave

    QSlider            *m_speech_slider_freq, *m_speech_slider_speed, *m_speech_slider_decline;


public:
    // Constructor / Destructor
    explicit FormSpeechSynthesis(DrProject *project, QWidget *parent = nullptr);
    virtual ~FormSpeechSynthesis() override;

    // Event Overrides
    virtual void    resizeEvent(QResizeEvent *event) override;

    // Form Functions
    void            buildSpeechForm();
    QWidget*        sliderPair(QString slider_text, QSlider *&slider);

    // Sound Generate Functions
    void            playSpeech(std::string speech_text);
    void            playWav(std::string wav_file);

    // Sound Variable Functions
    int             getSpeechWaveForm()                     { return m_speech_waveform; }
    void            setSpeechWaveForm(int wave_form)        { m_speech_waveform = wave_form; }


public slots:
    void        drawVisuals();

};


#endif // FORM_SPEECH_SYNTHESIS_H






















