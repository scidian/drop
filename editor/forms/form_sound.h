//
//      Created by Stephens Nunnally on 5/23/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef FORM_SOUND_H
#define FORM_SOUND_H

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


//####################################################################################
//##    FormSound
//##        Form to display Sound Files
//############################
class FormSound : public QWidget
{
    Q_OBJECT

private:
    // External Borrowed Pointers
    DrProject          *m_project;                              // Pointer to the open project

    // Sound Variables
    SoLoud::Soloud     *m_so_loud;                              // Soloud instance
    SoLoud::Sfxr        m_effect;
    SoLoud::Speech      m_speech;
    int                 m_speech_waveform   { KW_TRIANGLE };
    SoLoud::Wav         m_wave;

    std::list<DrSound*> m_sounds;                               // Local list of DrSounds, save Sounds to be used in game to m_project->m_sounds

    // Form Variables
    QWidget            *m_inner_widget;                         // Container widget, allows for a double form border

    VisualFrame        *m_visualizer;
    QListWidget        *m_list;

    QSlider            *m_slider_0, *m_slider_1, *m_slider_2, *m_slider_3, *m_slider_4;
    QSlider            *m_slider_5, *m_slider_6, *m_slider_7, *m_slider_8, *m_slider_9;

    QSlider            *m_speech_slider_freq, *m_speech_slider_speed, *m_speech_slider_decline;


public:
    // Constructor / Destructor
    explicit FormSound(DrProject *project, QWidget *parent = nullptr);
    virtual ~FormSound() override;

    // Event Overrides
    virtual void resizeEvent(QResizeEvent *event) override;

    // Form Functions
    void        buildSoundForm();
    QWidget*    sliderPair(QString slider_text, QSlider *&slider);

    // Sound Functions
    void        playSpeech(std::string speech_text);
    void        playSfxr(SoLoud::Sfxr::SFXR_PRESETS preset, int seed);
    void        playWav(std::string wav_file);

    // Sound Variable Functions
    int         getSpeechWaveForm()                     { return m_speech_waveform; }
    void        setSpeechWaveForm(int wave_form)        { m_speech_waveform = wave_form; }


public slots:
    void        drawVisuals();

};



//####################################################################################
//##    VisualFrame
//##        Allows us to use Style Sheets with QSlider and still have Tick Marks painted
//############################
class VisualFrame : public QFrame
{
    Q_OBJECT

private:
    // External Borrowed Pointers
    SoLoud::Soloud     *m_so_loud;

public:
    VisualFrame(SoLoud::Soloud *so_loud, QWidget *parent = nullptr);
    virtual ~VisualFrame() override;

    // Event Overrides
    virtual void    paintEvent(QPaintEvent *event) override;
};



#endif // FORM_SOUND_H






















