//
//      Created by Stephens Nunnally on 5/28/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QDebug>
#include <QPaintEvent>

#include "3rd_party/soloud/soloud.h"
#include "core/colors/colors.h"
#include "core/dr_math.h"
#include "core/dr_random.h"
#include "core/dr_string.h"
#include "editor/event_filters/event_filters.h"
#include "editor/form_sound/wave_form.h"
#include "editor/helper_library.h"
#include "editor/interface_editor_relay.h"
#include "editor/preferences.h"
#include "editor/style/style.h"
#include "project/dr_project.h"


//####################################################################################
//##    Constructor / Destructor
//####################################################################################
WaveForm::WaveForm(QWidget *parent) : QFrame(parent) { }
WaveForm::~WaveForm() { }


//####################################################################################
//##    Assign SoLoud::AudioSource, Calculate Sample Data
//####################################################################################
void WaveForm::setSound(SoLoud::AudioSource* audio) {
    m_audio = audio;
    m_pixels.clear();
    m_largest_sample = 0.1;

    // Create an instance of the selected SoLoud::AudioSource so we can find wave form
    SoLoud::AudioSourceInstance *voice = nullptr;
    if (m_audio != nullptr) voice = m_audio->createInstance();

    // If we successfully createds AudioInstance
    if (voice != nullptr) {
        // ***** Get AudioSource sample data
        std::vector<float> voice_data;                                                          // Vector to hold voice data
        do {
            float buffer[SAMPLE_GRANULARITY];                                                   // Create an empty array
            memset(buffer, 0, sizeof(float) * SAMPLE_GRANULARITY);                              // Zero out the array
            int readcount = voice->getAudio(buffer, SAMPLE_GRANULARITY, SAMPLE_GRANULARITY);    // Read some samples from the voice (advances internal play time)
            voice_data.insert(voice_data.end(), &buffer[0], &buffer[readcount]);                // Copy the samples into a vector
        } while (!voice->hasEnded());

        // ***** Calculate AudioSource length in seconds
        /// Example data:
        ///     m_audio->mBaseSamplerate == 44100
        ///     m_audio->mChannels == 1
        setAudioLength((static_cast<float>(voice_data.size()) / voice->mBaseSamplerate) / static_cast<float>(voice->mChannels));

        // ***** Get sample min / max / averages for each pixel
        float sample_step = static_cast<float>(voice_data.size()) / static_cast<float>(this->width());
        for (float x = 0; x < this->width(); x++) {

            // Process sample_step samples per pixel
            float min = 0, max = 0, average = 0;
            for (int i = 0; i < sample_step; i++) {
                // Get sample
                float sample = 0;
                try { sample = voice_data.at(i + (x * sample_step)); }
                catch (std::out_of_range const& error) { sample = 0; }

                // Check min, max
                min = Dr::Min(min, sample);
                max = Dr::Max(max, sample);
                average += abs(sample);
            }
            average /= static_cast<float>(int(sample_step));
            m_pixels.push_back(sample_pixel(min, max, average));

            m_largest_sample = Dr::Max(m_largest_sample, abs(min));
            m_largest_sample = Dr::Max(m_largest_sample, abs(max));
        }

    // Error creating AudioInstance
    } else {
        m_audio = nullptr;
        setAudioLength(0.0);
    }
}


//####################################################################################
//##    Paint Wave Form
//##        - For all samples with the same x, determine the min and the max separately for positive and negative values, draw dark line
//##        - For all samples with the same x, average all samples, draw light line
//####################################################################################
void WaveForm::paintEvent(QPaintEvent *) {

    // ***** Paint Wave Form
    QPainter painter(this);
    float mid = static_cast<float>(this->height()) / 2.f;

    for (int x = 0; x < this->width(); x++) {
        sample_pixel pixel(0, 0, 0);
        try { pixel = m_pixels.at(x); }
        catch (std::out_of_range const& error) { pixel = sample_pixel(0, 0, 0); }

        // Draw min / max samples
        float height_up =   abs(pixel.max / m_largest_sample);
        float height_down = abs(pixel.min / m_largest_sample);
        painter.setPen( QPen(Dr::ToQColor(Dr::GetColor(Window_Colors::Icon_Light)), 1) );
        painter.drawLine(x, mid - (mid * height_up), x, mid + (mid * height_down));

        // Draw average samples
        height_up =   pixel.average / m_largest_sample;
        height_down = pixel.average / m_largest_sample;
        painter.setPen( QPen(Dr::ToQColor(Dr::GetColor(Window_Colors::Icon_Dark)), 1) );
        painter.drawLine(x, mid - (mid * height_up), x, mid + (mid * height_down));
    }

    // Draw playback length
    painter.setPen( QPen(Dr::ToQColor(Dr::GetColor(Window_Colors::Text)), 1) );
    painter.setFont(Dr::CustomFont());
    QString play_time = Dr::RemoveTrailingDecimals(m_audio_length, 2) + "sec";
    painter.drawText(this->rect(), Qt::AlignRight | Qt::AlignBottom, play_time);

}




























