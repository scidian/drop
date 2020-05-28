//
//      Created by Stephens Nunnally on 5/25/2020, (c) 2020 Scidian Software, All Rights Reserved
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
class WaveForm;


//####################################################################################
//##    FormSoundEffect
//##        Form to display Sound Files
//############################
class FormSoundEffect : public QWidget
{
    Q_OBJECT

private:
    // External Borrowed Pointers
    DrProject              *m_project;                                      // Pointer to the open project

    // Local Variables
    long                    m_key_gen = 1;                                  // Key generator for identifying sounds created on this Form

    // Sound Variables
    ///std::list<DrSound*>  m_sounds;                                       // Local list of DrSounds, save Sounds to be used in game to m_project->m_sounds
    SoLoud::Soloud         *m_so_loud;                                      // Soloud instance

    ///SoLoud::Sfxr                 m_effect;
    std::map<long, SoLoud::Sfxr*>   m_effects;                              // Sfxr sound effects
    SoLoud::Speech                  m_speech;
    int                             m_speech_waveform   { KW_TRIANGLE };
    SoLoud::Wav                     m_wave;


    // Form Variables
    QWidget            *m_inner_widget;                                     // Container widget, allows for a double form border

    VisualFrame        *m_visualizer;                                       // Widget that displays visualizer for current sounds playing
    QTimer             *m_visual_timer;                                     // Event timer that calls update for visualizer
    QElapsedTimer       m_last_play_time;                                   // Tracks when sound was last still playing

    QListWidget        *m_list;                                             // Holds recently played sounds
    WaveForm           *m_sound_wave;                                       // Draws currently selected sound wave

    QSlider            *m_slider_0, *m_slider_1, *m_slider_2, *m_slider_3, *m_slider_4;
    QSlider            *m_slider_5, *m_slider_6, *m_slider_7, *m_slider_8, *m_slider_9;

    QSlider            *m_speech_slider_freq, *m_speech_slider_speed, *m_speech_slider_decline;


public:
    // Constructor / Destructor
    explicit FormSoundEffect(DrProject *project, QWidget *parent = nullptr);
    virtual ~FormSoundEffect() override;

    // Event Overrides
    virtual void resizeEvent(QResizeEvent *event) override;

    // Key Gen
    int             getNextKey()                            { return m_key_gen++; }

    // Form Functions
    void            buildSoundEffectForm();
    QWidget*        sliderPair(QString slider_text, QSlider *&slider);

    // Sound Generate Functions
    void            playSpeech(std::string speech_text);
    void            playSfxr(SoLoud::Sfxr::SFXR_PRESETS preset, int seed);
    void            playWav(std::string wav_file);

    // Existing Sounds
    SoLoud::Sfxr*   getEffect(long effect_key);
    QString         stringFromEffectType(SoLoud::Sfxr::SFXR_PRESETS preset);

    // Sound Variable Functions
    int             getSpeechWaveForm()                     { return m_speech_waveform; }
    void            setSpeechWaveForm(int wave_form)        { m_speech_waveform = wave_form; }


public slots:
    void        drawItem();
    void        drawVisuals();
    void        playItem(QListWidgetItem*);

};


#endif // FORM_SOUND_H
























