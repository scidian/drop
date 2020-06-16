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
#include <QPushButton>
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
class FormMain;
class VisualFrame;
class WaveForm;

// Type Definitions
typedef SoLoud::Sfxr::SFXR_PRESETS SoundEffectType;

// Local Enums
enum SoundEffectWaveType {
    Square      = 0,
    Sawtooth    = 1,
    Sine        = 2,
    Noise       = 3,
};


//####################################################################################
//##    FormSoundEffect
//##        Form to create / add Sfxr Sound Effect
//############################
class FormSoundEffect : public QWidget
{
    Q_OBJECT

private:
    // External Borrowed Pointers
    FormMain               *m_sound_creator;                                // Pointer to Parent FormMain
    DrProject              *m_project;                                      // Pointer to the open project

    // Local Variables
    long                            m_key_gen           { 1 };              // Key generator for identifying sounds created on this Form
    std::map<SoundEffectType, long> m_type_counts;                          // Keeps track of counts of different sound effect types for naming
    long                            m_current_effect   { c_no_key };        // Local key of the Effect currently selected (to be passed back to Project)

    // Sound Variables
    std::map<long, SoLoud::Sfxr*>   m_effects;                              // Sfxr sound effects

    // Form Variables
    QWidget            *m_inner_widget;                                     // Container widget, allows for a double form border

    VisualFrame        *m_visualizer;                                       // Widget that displays visualizer for current sounds playing
    QTimer             *m_visual_timer;                                     // Event timer that calls update for visualizer
    QElapsedTimer       m_last_play_time;                                   // Tracks when sound was last still playing

    QListWidget        *m_list;                                             // Holds recently played sounds
    WaveForm           *m_sound_wave;                                       // Draws currently selected sound wave

    // Wave Type Drop Down
    QPushButton        *m_wave_button;
    QAction            *m_action_square;
    QAction            *m_action_saw;
    QAction            *m_action_sine;
    QAction            *m_action_noise;

    // Effect Sliders
    QSlider            *m_slider_0;         // Frequency
    QSlider            *m_slider_1;         // Frequency Ramp
    QSlider            *m_slider_2;         // Frequency Slide
    QSlider            *m_slider_3;         // Delta Slide
    QSlider            *m_slider_4;         // Vibrato Depth
    QSlider            *m_slider_5;         // Vibrato Speed

    QSlider            *m_slider_6;
    QSlider            *m_slider_7;
    QSlider            *m_slider_8;
    QSlider            *m_slider_9;
    QSlider            *m_slider_10;
    QSlider            *m_slider_11;


public:
    // Constructor / Destructor
    explicit FormSoundEffect(DrProject *project, FormMain *form_main, QWidget *parent = nullptr);
    virtual ~FormSoundEffect() override;

    // Event Overrides
    virtual void    resizeEvent(QResizeEvent *event) override;

    // Key Gen
    int             getNextKey()                            { return m_key_gen++; }

    // Form Functions
    void            buildSoundEffectForm();
    QWidget*        sliderPair(QString slider_text, QSlider *&slider, double range_min = 0.0, double range_max = 1.0);

    // Sound Generate Functions
    void            generateSfxr(SoundEffectType preset, int seed);
    void            updateSliders(SoLoud::Sfxr *effect);
    void            updateSelectedEffect(QSlider *from_slider);
    void            updateWaveType(SoundEffectWaveType wave_type);

    // Existing Sounds
    SoLoud::Sfxr*   getEffect(long effect_key);
    void            playSelected();
    long            currentEffectKey()                      { return m_current_effect; }

    // Enums
    QString         stringFromEffectType(SoundEffectType preset);
    QString         stringFromWaveType(SoundEffectWaveType wave);


public slots:
    void        drawVisuals();
    void        currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);
    void        itemClicked(QListWidgetItem *clicked);
    void        itemEntered(QListWidgetItem *entered);

};


#endif // FORM_SOUND_H
























