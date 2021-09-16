//
//      Created by Stephens Nunnally on 5/25/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QActionGroup>
#include <QDebug>
#include <QSizeGrip>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMenu>
#include <QMouseEvent>
#include <QPushButton>
#include <QSplitter>
#include <QTextEdit>
#include <QTimer>
#include <QToolButton>
#include <QVBoxLayout>

#include "3rd_party/soloud/soloud.h"
#include "core/colors/colors.h"
#include "core/dr_random.h"
#include "core/sound.h"
#include "editor/event_filters/event_filters.h"
#include "editor/form_sound/form_sound_effect.h"
#include "editor/form_sound/visualizer.h"
#include "editor/form_sound/wave_form.h"
#include "editor/helper_library.h"
#include "editor/interface_editor_relay.h"
#include "editor/preferences.h"
#include "editor/style/style.h"
#include "editor/trees/tree_assets.h"
#include "editor/widgets/widgets_inspector.h"
#include "project/dr_project.h"
#include "project/entities_sound/dr_sound.h"


//####################################################################################
//##    Builds out Sound Form
//####################################################################################
void FormSoundEffect::buildSoundEffectForm() {

    // Create a contianer widget, this will allow us to create a layout for the form and add buttons and stuff
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(1, 1, 1, 1);

    // Main Widget & Layout
    m_inner_widget = new QWidget();
    m_inner_widget->setObjectName(QStringLiteral("innerWidget"));
    QHBoxLayout *inner_layout = new QHBoxLayout(m_inner_widget);
    inner_layout->setObjectName(QStringLiteral("horizontalLayout"));
    inner_layout->setSpacing(0);
    inner_layout->setContentsMargins(6, 6, 6, 6);

    // Splitter widget and layout
    QWidget *splitter_holder = new QWidget();
    QHBoxLayout *splitter_layout = new QHBoxLayout(splitter_holder);
    splitter_layout->setObjectName(QStringLiteral("horizontalLayout"));
    splitter_layout->setSpacing(0);
    splitter_layout->setContentsMargins(0, 0, 0, 0);

    // Add splitter to splitter layout
    QSplitter *splitter_horizontal = new QSplitter();
    splitter_horizontal->setObjectName(QStringLiteral("splitterHorizontalSound"));
    splitter_horizontal->setLineWidth(0);
    splitter_horizontal->setOrientation(Qt::Horizontal);
    splitter_horizontal->setHandleWidth(4);

        // List view on the left
        QWidget *left_side = new QWidget();
        left_side->setObjectName(QStringLiteral("widgetHolder"));
        QVBoxLayout *left_side_layout = new QVBoxLayout(left_side);
        left_side_layout->setObjectName(QStringLiteral("verticalLayout"));
        left_side_layout->setSpacing(0);
        left_side_layout->setContentsMargins(0, 0, 0, 0);
            m_list = new QListWidget();
                m_list->setObjectName(QStringLiteral("listView"));
                m_list->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
                m_list->setMouseTracking(true);
                connect(m_list, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),  this, SLOT(currentItemChanged(QListWidgetItem*,QListWidgetItem*)));
                connect(m_list, SIGNAL(itemClicked(QListWidgetItem*)),  this, SLOT(itemClicked(QListWidgetItem*)));
                connect(m_list, SIGNAL(itemEntered(QListWidgetItem*)),  this, SLOT(itemEntered(QListWidgetItem*)));
            left_side_layout->addWidget(m_list);

            m_sound_wave = new WaveForm();
                m_sound_wave->setObjectName(QStringLiteral("soundWave"));
                m_sound_wave->setFixedHeight(100);
            left_side_layout->addWidget(m_sound_wave);
        splitter_horizontal->addWidget(left_side);

        // Buttons and stuff on the right
        QWidget *middle_side = new QWidget();
        middle_side->setObjectName(QStringLiteral("widgetHolder"));
        QVBoxLayout *middle_side_layout = new QVBoxLayout(middle_side);
        middle_side_layout->setObjectName(QStringLiteral("verticalLayout"));
        middle_side_layout->setSpacing(4);
        middle_side_layout->setContentsMargins(2, 2, 2, 2);

            // Visualizer
            m_visualizer = new VisualFrame();
            m_visualizer->setFixedHeight(70);
            middle_side_layout->addWidget(m_visualizer);


            // Sound Effects Top Row
            QWidget *sound_effects_1 = new QWidget();
            sound_effects_1->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));
            QHBoxLayout *sound_effect_layout_1 = new QHBoxLayout(sound_effects_1);
            sound_effect_layout_1->setObjectName(QStringLiteral("horizontalLayout"));
            sound_effect_layout_1->setSpacing(4);
            sound_effect_layout_1->setContentsMargins(4, 0, 0, 2);

                QPushButton *play_effect_coin = new QPushButton("Coin / Success");
                    Dr::ApplyDropShadowByType(play_effect_coin, Shadow_Types::Button_Shadow);
                    play_effect_coin->setObjectName(QStringLiteral("buttonDefault"));
                    connect(play_effect_coin, &QPushButton::clicked, [this] ()      { this->generateSfxr(SoundEffectType::COIN, Dr::RandomInt(0, 500000)); });
                sound_effect_layout_1->addWidget(play_effect_coin);

                QPushButton *play_effect_laser = new QPushButton("Laser / Shoot");
                    Dr::ApplyDropShadowByType(play_effect_laser, Shadow_Types::Button_Shadow);
                    play_effect_laser->setObjectName(QStringLiteral("buttonDefault"));
                    connect(play_effect_laser, &QPushButton::clicked, [this] ()     { this->generateSfxr(SoundEffectType::LASER, Dr::RandomInt(0, 500000)); });
                sound_effect_layout_1->addWidget(play_effect_laser);

                QPushButton *play_effect_explode = new QPushButton("Explode");
                    Dr::ApplyDropShadowByType(play_effect_explode, Shadow_Types::Button_Shadow);
                    play_effect_explode->setObjectName(QStringLiteral("buttonDefault"));
                    connect(play_effect_explode, &QPushButton::clicked, [this] ()   { this->generateSfxr(SoundEffectType::EXPLOSION, Dr::RandomInt(0, 500000)); });
                sound_effect_layout_1->addWidget(play_effect_explode);

                QPushButton *play_effect_power = new QPushButton("Powerup");
                    Dr::ApplyDropShadowByType(play_effect_power, Shadow_Types::Button_Shadow);
                    play_effect_power->setObjectName(QStringLiteral("buttonDefault"));
                    connect(play_effect_power, &QPushButton::clicked, [this] ()     { this->generateSfxr(SoundEffectType::POWERUP, Dr::RandomInt(0, 500000)); });
                sound_effect_layout_1->addWidget(play_effect_power);

            middle_side_layout->addWidget(sound_effects_1);

            // Sound Effects Second Row
            QWidget *sound_effects_2 = new QWidget();
            sound_effects_2->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));
            QHBoxLayout *sound_effect_layout_2 = new QHBoxLayout(sound_effects_2);
            sound_effect_layout_2->setObjectName(QStringLiteral("horizontalLayout"));
            sound_effect_layout_2->setSpacing(4);
            sound_effect_layout_2->setContentsMargins(4, 2, 0, 2);

                QPushButton *play_effect_hurt = new QPushButton("Hurt / Hit");
                    Dr::ApplyDropShadowByType(play_effect_hurt, Shadow_Types::Button_Shadow);
                    play_effect_hurt->setObjectName(QStringLiteral("buttonDefault"));
                    connect(play_effect_hurt, &QPushButton::clicked, [this] ()      { this->generateSfxr(SoundEffectType::HURT, Dr::RandomInt(0, 500000)); });
                sound_effect_layout_2->addWidget(play_effect_hurt);

                QPushButton *play_effect_jump = new QPushButton("Jump");
                    Dr::ApplyDropShadowByType(play_effect_jump, Shadow_Types::Button_Shadow);
                    play_effect_jump->setObjectName(QStringLiteral("buttonDefault"));
                    connect(play_effect_jump, &QPushButton::clicked, [this] ()      { this->generateSfxr(SoundEffectType::JUMP, Dr::RandomInt(0, 500000)); });
                sound_effect_layout_2->addWidget(play_effect_jump);

                QPushButton *play_effect_blip = new QPushButton("Blip / Select");
                    Dr::ApplyDropShadowByType(play_effect_blip, Shadow_Types::Button_Shadow);
                    play_effect_blip->setObjectName(QStringLiteral("buttonDefault"));
                    connect(play_effect_blip, &QPushButton::clicked, [this] ()      { this->generateSfxr(SoundEffectType::BLIP, Dr::RandomInt(0, 500000)); });
                sound_effect_layout_2->addWidget(play_effect_blip);

            middle_side_layout->addWidget(sound_effects_2);


            // Wave Form Drop Down Menu Button
            QWidget *wave_form = new QWidget();
            QHBoxLayout *wave_form_layout = new QHBoxLayout(wave_form);
            wave_form_layout->setObjectName(QStringLiteral("verticalLayout"));
            wave_form_layout->setSpacing(4);
            wave_form_layout->setContentsMargins(8, 2, 4, 2);

                QSizePolicy size_left( QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);   size_left.setHorizontalStretch(1);
                QSizePolicy size_right(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);   size_right.setHorizontalStretch(3);

                QLabel *wave_label = new QLabel("Wave Type");
                        wave_label->setFont(Dr::CustomFont());
                        wave_label->setSizePolicy(size_left);
                wave_form_layout->addWidget(wave_label);

                m_wave_button = new QPushButton();
                m_wave_button->setSizePolicy(size_right);
                m_wave_button->setText(stringFromWaveType(Square));
                m_wave_button->setObjectName(QStringLiteral("spinBoxSelect"));
                m_wave_button->setToolTip("Sound Effect Wave Type");
                m_wave_button->setFont(Dr::CustomFont());
                m_wave_button->setStyleSheet(" QPushButton { border-top-left-radius: 4px; border-bottom-left-radius: 4px; }"
                                             " QPushButton:pressed { border-radius: 0px; }");
                    QMenu *menu = new QMenu(this);
                        menu->setObjectName(QStringLiteral("menuComboBox"));
                        menu->setMinimumWidth(130);
                    QActionGroup *group;
                        group = new QActionGroup(menu);
                        group->setExclusive(true);

                    // Add options to menu
                    auto createAction = [this, group, menu](QAction *&action, SoundEffectWaveType wave_type) {
                        action = new QAction(stringFromWaveType(wave_type));
                        action->setCheckable(true);
                        group->addAction(action);
                        menu->addAction(action);
                        connect(action, &QAction::triggered, [this, wave_type]() { updateWaveType(wave_type); this->playSelected(); });
                    };
                    createAction(m_action_square,   SoundEffectWaveType::Square);
                    createAction(m_action_saw,      SoundEffectWaveType::Sawtooth);
                    createAction(m_action_sine,     SoundEffectWaveType::Sine);
                    createAction(m_action_noise,    SoundEffectWaveType::Noise);
                    m_action_square->setChecked(true);

                m_wave_button->setMenu(menu);
                menu->installEventFilter(new DrFilterPopUpMenuRelocater(menu, 2, 0));//-max_zoom_width));
                wave_form_layout->addWidget(m_wave_button);
            middle_side_layout->addWidget(wave_form);


            // Sliders
            QWidget *sound_sliders = new QWidget();
            ///sound_sliders->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));
            QHBoxLayout *sound_slider_layout = new QHBoxLayout(sound_sliders);
            sound_slider_layout->setObjectName(QStringLiteral("horizontalLayout"));
            sound_slider_layout->setSpacing(4);
            sound_slider_layout->setContentsMargins(4, 2, 2, 2);

                QWidget *left_sliders = new QWidget();
                QVBoxLayout *left_slider_layout = new QVBoxLayout(left_sliders);
                left_slider_layout->setObjectName(QStringLiteral("verticalLayout"));
                left_slider_layout->setSpacing(2);
                left_slider_layout->setContentsMargins(2, 2, 2, 2);
                    left_slider_layout->addWidget( sliderPair("Frequency",          m_slider_0));
                    left_slider_layout->addWidget( sliderPair("Freq Limit",         m_slider_1));
                    left_slider_layout->addWidget( sliderPair("Freq Slide",         m_slider_2, -1.0, 1.0));
                    left_slider_layout->addWidget( sliderPair("Delta Slide",        m_slider_3, -1.0, 1.0));
                    left_slider_layout->addWidget( sliderPair("Vibrato Depth",      m_slider_4));
                    left_slider_layout->addWidget( sliderPair("Vibrato Speed",      m_slider_5));
                    left_slider_layout->addWidget( sliderPair("Attack Time",        m_slider_6));
                    left_slider_layout->addWidget( sliderPair("Sustain Time",       m_slider_7));
                    left_slider_layout->addWidget( sliderPair("Decay Time",         m_slider_8));
                    left_slider_layout->addWidget( sliderPair("Sustain Punch",      m_slider_9));
                sound_slider_layout->addWidget(left_sliders);

                // Frequency
                //  0   float p_base_freq;          //  0, 1    Frequency                       Base note of sound
                //  1   float p_freq_limit;         //  0, 1    Frequency Cutoff                If sliding, frequency will stop here to prevent low notes
                //  2   float p_freq_ramp;          // -1, 1    Frequency Slide                 Slides the frequency up or down
                //  3   float p_freq_dramp;         // -1, 1    Delta Slide                     Accelerates frequency slide, can be used to change slide directions
                // Vibrato
                //  4   float p_vib_strength;       //  0, 1    Vibrato Depth                   Strength of the vibrato effect
                //  5   float p_vib_speed;          //  0, 1    Vibrato Speed                   Speed of the bibrato effect (i.e. frequency)
                // Volume Envelope
                //  6   float p_env_attack;         //  0, 1    Attack Time                     Length of the volume envelope attack
                //  7   float p_env_sustain;        //  0, 1    Sustain Time                    Length of the volume envelope sustain
                //  8   float p_env_decay;          //  0, 1    Decay Time                      Length of the volume envelope decay (aka release)
                //  9   float p_env_punch;          //  0, 1    Sustain Punch                   Tilts the sustain envelope for more 'pop'

                // Repeat
                // 10   float p_repeat_speed;       //  0, 1    Repeat Speed                    Speed of the note repeating
                // Phaser
                // 11   float p_pha_offset;         // -1, 1    Phaser (Flanger) Offset         Offsets a second copy of the wave by a small phase, changing the timbre
                // 12   float p_pha_ramp;           // -1, 1    Phaser (Flanger) Sweep          Sweeps the phase up or down
                // Pitch Jump
                // 13   float p_arp_speed;          //  0, 1    Pitch Jump (Change) Speed       Larger values means more pitch jumps, useful for arpeggiation
                // 14   float p_arp_mod;            // -1, 1    Pitch Jump (Change) Amount      Jump in pitch, up or down
                // Low-pass / High-pass Filters
                // 15   float p_lpf_resonance;      //  0, 1    Low-pass Filter Resonance       Changes the attenuation rate for the low-pass filter, changing the timbre
                // 16   float p_lpf_freq;           //  0, 1    Low-pass Filter Cutoff          Frequency at which the low-pass filter starts attenuating higher frequencies
                // 17   float p_lpf_ramp;           // -1, 1    Low-pass Filter Sweep           Sweeps the low-pass cutoff up or down
                // 18   float p_hpf_freq;           //  0, 1    High-pass Filter Cutoff         Frequency at which the high-pass filter starts attenuating lower frequencies
                // 19   float p_hpf_ramp;           // -1, 1    High-pass Filter Sweep          Sweeps the high-pass filter up or down

                QWidget *right_sliders = new QWidget();
                QVBoxLayout *right_slider_layout = new QVBoxLayout(right_sliders);
                right_slider_layout->setObjectName(QStringLiteral("verticalLayout"));
                right_slider_layout->setSpacing(2);
                right_slider_layout->setContentsMargins(2, 2, 2, 2);
                    right_slider_layout->addWidget( sliderPair("Repeat Speed",          m_slider_10));
                    right_slider_layout->addWidget( sliderPair("Phaser Offset",         m_slider_11, -1.0, 1.0));
                    right_slider_layout->addWidget( sliderPair("Phaser Sweep",          m_slider_12, -1.0, 1.0));
                    right_slider_layout->addWidget( sliderPair("Pitch Jump Speed",      m_slider_13));
                    right_slider_layout->addWidget( sliderPair("Pitch Jump Amount",     m_slider_14, -1.0, 1.0));
                    right_slider_layout->addWidget( sliderPair("Low-pass Resonance",    m_slider_15));
                    right_slider_layout->addWidget( sliderPair("Low-pass Cutoff",       m_slider_16));
                    right_slider_layout->addWidget( sliderPair("Low-pass Sweep",        m_slider_17, -1.0, 1.0));
                    right_slider_layout->addWidget( sliderPair("High-pass Cutoff",      m_slider_18));
                    right_slider_layout->addWidget( sliderPair("High-pass Sweep",       m_slider_19, -1.0, 1.0));
                sound_slider_layout->addWidget(right_sliders);

            middle_side_layout->addWidget(sound_sliders);
            middle_side_layout->addStretch();


            // Buttons on the bottom of Form
            QWidget *buttons = new QWidget();
            QHBoxLayout *button_layout = new QHBoxLayout(buttons);
            button_layout->setObjectName(QStringLiteral("horizontalLayout"));
            button_layout->setSpacing(4);
            button_layout->setContentsMargins(4, 0, 0, 0);

                QSizePolicy button_left(QSizePolicy::Expanding,  QSizePolicy::Maximum); button_left.setHorizontalStretch(1);
                QSizePolicy button_right(QSizePolicy::Expanding, QSizePolicy::Maximum); button_right.setHorizontalStretch(2);

                // Cancel Button
                QPushButton *cancel = new QPushButton("  Cancel  ");
                    cancel->setObjectName(QStringLiteral("buttonDefault"));
                    cancel->setSizePolicy(button_left);
                    Dr::ApplyDropShadowByType(cancel, Shadow_Types::Button_Shadow);
                    connect(cancel, &QPushButton::clicked, [this] () { this->close(); });
                button_layout->addWidget(cancel);

                // Accept Button
                QPushButton *accept = new QPushButton("  Add Sound Effect  ");
                    accept->setObjectName(QStringLiteral("buttonDefault"));
                    accept->setSizePolicy(button_right);
                    Dr::ApplyDropShadowByType(accept, Shadow_Types::Button_Shadow);
                    connect(accept, &QPushButton::clicked, [this] () {
                        SoLoud::Sfxr *effect = getEffect(currentEffectKey());

                        if (effect != nullptr) {
                            // Add Sound
                            std::string sound_name = m_list->selectedItems().first()->text().toStdString();
                            DrSound *sound = m_project->addSound(DrSoundType::Sound_Effect, effect, c_no_key, sound_name);

                            // Update EditorRelay widgets
                            IEditorRelay *editor = Dr::GetActiveEditorRelay();
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
                        } else {
                            this->close();
                        }
                    });
                button_layout->addWidget(accept);
            middle_side_layout->addWidget(buttons);

        splitter_horizontal->addWidget(middle_side);


    // Sets splitter starting sizes, gives a size ratio for the layout to work with
    splitter_horizontal->setSizes( { 200, 540 } );

    // Add splitter to splitter layout, add splitter to main layout
    splitter_layout->addWidget(splitter_horizontal);
    inner_layout->addWidget(splitter_holder);

    // Add main widget to form
    layout->addWidget(m_inner_widget);
}



//####################################################################################
//##    Builds a label / slider pair
//####################################################################################
QWidget* FormSoundEffect::sliderPair(QString slider_text, QSlider *&slider, double range_min, double range_max) {
    QWidget *pair = new QWidget();
    QHBoxLayout *pair_layout = new QHBoxLayout(pair);
    pair_layout->setSpacing(5);
    pair_layout->setContentsMargins(2, 0, 2, 0);

    QSizePolicy size_left( QSizePolicy::Minimum, QSizePolicy::Minimum);                     size_left.setHorizontalStretch(1);
    QSizePolicy size_right(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);   size_right.setHorizontalStretch(2);

    // Create Widgets
    QLabel *label = new QLabel(slider_text);
    InspectorTripleSpinBox *spin = new InspectorTripleSpinBox();
    slider = new QSlider(Qt::Orientation::Horizontal);

    // Label
    label->setFont(Dr::CustomFont());
    label->setSizePolicy(size_left);
    label->setFixedWidth(Dr::CheckFontWidth(Dr::CustomFont(), "Low-pass Resonance"));

    // Spin Box
    spin->setRange(range_min, range_max);
    spin->setSingleStep(0.1);
    spin->setFixedHeight(Dr::Scale(22));
    spin->setFixedWidth(Dr::CheckFontWidth(Dr::CustomFont(), tr(" -0.000 ")));
    spin->setValue(0);
    spin->setFont(Dr::CustomFont());
    spin->setAttribute(Qt::WA_MacShowFocusRect, 0);
    spin->setButtonSymbols(QAbstractSpinBox::ButtonSymbols::NoButtons);

    connect (spin, QOverload<double>::of(&InspectorTripleSpinBox::valueChanged), this, [this, slider] (double d) {
        double new_value = d;
        slider->blockSignals(true);
        slider->setValue(new_value * 1000.0);
        slider->blockSignals(false);
        this->updateSelectedEffect(slider);
    });

    // Slider
    slider->setSizePolicy(size_right);
    slider->setMaximumHeight(Dr::Scale(26));
    slider->setTickPosition(QSlider::TickPosition::NoTicks);
    slider->setRange(range_min * 1000, range_max * 1000);
    slider->setSingleStep(10);
    slider->setPageStep(100);
    slider->setValue(0);

    connect(slider, &QSlider::valueChanged, [this, slider, spin] () {
        double new_value = static_cast<double>(slider->value()) / 1000.0;
        spin->blockSignals(true);
        spin->setValue(new_value);
        spin->blockSignals(false);
        this->updateSelectedEffect(slider);
    });

    connect(slider, &QSlider::sliderReleased, [this] () {
        this->playSelected();
    });

    // Add Widgets to layout
    pair_layout->addWidget(label);
    pair_layout->addWidget(slider);
    pair_layout->addWidget(spin);
    return pair;
}





















