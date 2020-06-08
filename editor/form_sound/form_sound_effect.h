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

// Type Definitions
typedef SoLoud::Sfxr::SFXR_PRESETS SoundEffectType;

//####################################################################################
//##    FormSoundEffect
//##        Form to create / add Sfxr Sound Effect
//############################
class FormSoundEffect : public QWidget
{
    Q_OBJECT

private:
    // External Borrowed Pointers
    DrProject              *m_project;                                      // Pointer to the open project

    // Local Variables
    std::map<SoundEffectType, long> m_key_gen;                              // Key generator for identifying sounds created on this Form
    long                            m_selected_effect   { c_no_key };       // Local key of the Effect currently selected (to be passed back to Project)

    // Sound Variables
    std::map<long, SoLoud::Sfxr*>   m_effects;                              // Sfxr sound effects

    // Form Variables
    QWidget            *m_inner_widget;                                     // Container widget, allows for a double form border

    VisualFrame        *m_visualizer;                                       // Widget that displays visualizer for current sounds playing
    QTimer             *m_visual_timer;                                     // Event timer that calls update for visualizer
    QElapsedTimer       m_last_play_time;                                   // Tracks when sound was last still playing

    QListWidget        *m_list;                                             // Holds recently played sounds
    WaveForm           *m_sound_wave;                                       // Draws currently selected sound wave

    QSlider            *m_slider_0, *m_slider_1, *m_slider_2, *m_slider_3, *m_slider_4;
    QSlider            *m_slider_5, *m_slider_6, *m_slider_7, *m_slider_8, *m_slider_9;


public:
    // Constructor / Destructor
    explicit FormSoundEffect(DrProject *project, QWidget *parent = nullptr);
    virtual ~FormSoundEffect() override;

    // Event Overrides
    virtual void resizeEvent(QResizeEvent *event) override;

    // Key Gen
    int             getNextKey(SoundEffectType type)        { return m_key_gen[type]++; }

    // Form Functions
    void            buildSoundEffectForm();
    QWidget*        sliderPair(QString slider_text, QSlider *&slider);

    // Sound Generate Functions
    void            playSfxr(SoundEffectType preset, int seed);

    // Existing Sounds
    SoLoud::Sfxr*   getEffect(long effect_key);
    long            selectedEffectKey()                     { return m_selected_effect; }
    QString         stringFromEffectType(SoundEffectType preset);


public slots:
    void        drawItem();
    void        drawVisuals();
    void        playItem(QListWidgetItem*);

};


#endif // FORM_SOUND_H
























