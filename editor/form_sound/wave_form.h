//
//      Created by Stephens Nunnally on 5/28/2020, (c) 2020 Scidian Studios, All Rights Reserved
//
//  File:
//
//
//
#ifndef WAVE_FORM_H
#define WAVE_FORM_H

#include <QFrame>

#include "3rd_party/soloud/soloud.h"
#include "project/enums_entity_types.h"

// Local Structs
struct sample_pixel {
    float min       { 0 };
    float max       { 0 };
    float average   { 0 };
    sample_pixel(float _min, float _max, float _average) : min(_min), max(_max), average(_average) { }
};


//####################################################################################
//##    VisualWave
//##        Allows us to draw sound wave of a SoLoud AudioSource base class
//############################
class WaveForm : public QFrame
{
    Q_OBJECT

private:
    // External Borrowed Pointers
    SoLoud::AudioSource        *m_audio             { nullptr };

    // Local Variables
    bool                        m_show_border       { false };              // Should draw a border around frame?
    std::vector<sample_pixel>   m_pixels;                                   // Holds pixel data for painting
    float                       m_audio_length      { 0 };                  // Length of AudioSource, in seconds
    float                       m_largest_sample    { 0 };                  // Stores largest sample in data (for relative painting)


public:
    WaveForm(QWidget *parent = nullptr, SoLoud::AudioSource *audio_source = nullptr);
    virtual ~WaveForm() override;

    // Event Overrides
    virtual void    paintEvent(QPaintEvent *event) override;
    virtual void    resizeEvent(QResizeEvent *event) override;

    // Getters / Setters
    float           getAudioLength()                            { return m_audio_length; }
    void            setAudioLength(double length)               { m_audio_length = length; }
    void            setSound(SoLoud::AudioSource* audio);
    void            showBorder(bool border_on) { m_show_border = border_on; }

};


#endif // WAVE_FORM_H












