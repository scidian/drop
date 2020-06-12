//
//      Created by Stephens Nunnally on 5/25/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
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
            m_list->connect(m_list, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(playItem(QListWidgetItem*)));
            m_list->connect(m_list, SIGNAL(itemSelectionChanged()), this, SLOT(drawItem()));
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
            m_visualizer->setFixedHeight(100);
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
                    connect(play_effect_coin, &QPushButton::clicked, [this] ()      { this->playSfxr(SoundEffectType::COIN, Dr::RandomInt(0, 500000)); });
                sound_effect_layout_1->addWidget(play_effect_coin);

                QPushButton *play_effect_laser = new QPushButton("Laser / Shoot");
                    Dr::ApplyDropShadowByType(play_effect_laser, Shadow_Types::Button_Shadow);
                    play_effect_laser->setObjectName(QStringLiteral("buttonDefault"));
                    connect(play_effect_laser, &QPushButton::clicked, [this] ()     { this->playSfxr(SoundEffectType::LASER, Dr::RandomInt(0, 500000)); });
                sound_effect_layout_1->addWidget(play_effect_laser);

                QPushButton *play_effect_explode = new QPushButton("Explode");
                    Dr::ApplyDropShadowByType(play_effect_explode, Shadow_Types::Button_Shadow);
                    play_effect_explode->setObjectName(QStringLiteral("buttonDefault"));
                    connect(play_effect_explode, &QPushButton::clicked, [this] ()   { this->playSfxr(SoundEffectType::EXPLOSION, Dr::RandomInt(0, 500000)); });
                sound_effect_layout_1->addWidget(play_effect_explode);

                QPushButton *play_effect_power = new QPushButton("Powerup");
                    Dr::ApplyDropShadowByType(play_effect_power, Shadow_Types::Button_Shadow);
                    play_effect_power->setObjectName(QStringLiteral("buttonDefault"));
                    connect(play_effect_power, &QPushButton::clicked, [this] ()     { this->playSfxr(SoundEffectType::POWERUP, Dr::RandomInt(0, 500000)); });
                sound_effect_layout_1->addWidget(play_effect_power);

            middle_side_layout->addWidget(sound_effects_1);

            // Sound Effects Second Row
            QWidget *sound_effects_2 = new QWidget();
            sound_effects_2->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));
            QHBoxLayout *sound_effect_layout_2 = new QHBoxLayout(sound_effects_2);
            sound_effect_layout_2->setObjectName(QStringLiteral("horizontalLayout"));
            sound_effect_layout_2->setSpacing(4);
            sound_effect_layout_2->setContentsMargins(4, 2, 4, 2);

                QPushButton *play_effect_hurt = new QPushButton("Hurt / Hit");
                    Dr::ApplyDropShadowByType(play_effect_hurt, Shadow_Types::Button_Shadow);
                    play_effect_hurt->setObjectName(QStringLiteral("buttonDefault"));
                    connect(play_effect_hurt, &QPushButton::clicked, [this] ()      { this->playSfxr(SoundEffectType::HURT, Dr::RandomInt(0, 500000)); });
                sound_effect_layout_2->addWidget(play_effect_hurt);

                QPushButton *play_effect_jump = new QPushButton("Jump");
                    Dr::ApplyDropShadowByType(play_effect_jump, Shadow_Types::Button_Shadow);
                    play_effect_jump->setObjectName(QStringLiteral("buttonDefault"));
                    connect(play_effect_jump, &QPushButton::clicked, [this] ()      { this->playSfxr(SoundEffectType::JUMP, Dr::RandomInt(0, 500000)); });
                sound_effect_layout_2->addWidget(play_effect_jump);

                QPushButton *play_effect_blip = new QPushButton("Blip / Select");
                    Dr::ApplyDropShadowByType(play_effect_blip, Shadow_Types::Button_Shadow);
                    play_effect_blip->setObjectName(QStringLiteral("buttonDefault"));
                    connect(play_effect_blip, &QPushButton::clicked, [this] ()      { this->playSfxr(SoundEffectType::BLIP, Dr::RandomInt(0, 500000)); });
                sound_effect_layout_2->addWidget(play_effect_blip);

            middle_side_layout->addWidget(sound_effects_2);

            // Sliders
            QWidget *sound_sliders = new QWidget();
            ///sound_sliders->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));
            QHBoxLayout *sound_slider_layout = new QHBoxLayout(sound_sliders);
            sound_slider_layout->setObjectName(QStringLiteral("horizontalLayout"));
            sound_slider_layout->setSpacing(4);
            sound_slider_layout->setContentsMargins(2, 2, 2, 2);

                QWidget *left_sliders = new QWidget();
                QVBoxLayout *left_slider_layout = new QVBoxLayout(left_sliders);
                left_slider_layout->setObjectName(QStringLiteral("verticalLayout"));
                left_slider_layout->setSpacing(2);
                left_slider_layout->setContentsMargins(2, 2, 2, 2);
                    left_slider_layout->addWidget( sliderPair("Slide 0", m_slider_0));
                    left_slider_layout->addWidget( sliderPair("Slide 1", m_slider_1));
                    left_slider_layout->addWidget( sliderPair("Slide 2", m_slider_2));
                    left_slider_layout->addWidget( sliderPair("Slide 3", m_slider_3));
                    left_slider_layout->addWidget( sliderPair("Slide 4", m_slider_4));
                sound_slider_layout->addWidget(left_sliders);

                QWidget *right_sliders = new QWidget();
                QVBoxLayout *right_slider_layout = new QVBoxLayout(right_sliders);
                right_slider_layout->setObjectName(QStringLiteral("verticalLayout"));
                right_slider_layout->setSpacing(2);
                right_slider_layout->setContentsMargins(2, 2, 2, 2);
                    right_slider_layout->addWidget( sliderPair("Slide 5", m_slider_5));
                    right_slider_layout->addWidget( sliderPair("Slide 6", m_slider_6));
                    right_slider_layout->addWidget( sliderPair("Slide 7", m_slider_7));
                    right_slider_layout->addWidget( sliderPair("Slide 8", m_slider_8));
                    right_slider_layout->addWidget( sliderPair("Slide 9", m_slider_9));
                sound_slider_layout->addWidget(right_sliders);

            middle_side_layout->addWidget(sound_sliders);
            middle_side_layout->addStretch();


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
                        SoLoud::Sfxr *effect = getEffect(selectedEffectKey());

                        if (effect != nullptr) {
                            // Add Sound
                            std::string sound_name = m_list->selectedItems().first()->text().toStdString();
                            DrSound *sound = m_project->addSound(DrSoundType::Sound_Effect, effect, c_no_key, sound_name);

                            // Update EditorRelay widgets
                            IEditorRelay *editor = Dr::GetActiveEditorRelay();
                            if (editor && sound) {
                                editor->buildAssetTree();
                                editor->buildInspector( { sound->getKey() } );
                                editor->updateItemSelection(Editor_Widgets::Asset_Tree);
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
QWidget* FormSoundEffect::sliderPair(QString slider_text, QSlider *&slider) {
    QWidget *pair = new QWidget();
    QHBoxLayout *pair_layout = new QHBoxLayout(pair);
    pair_layout->setSpacing(0);
    pair_layout->setContentsMargins(0, 0, 0, 0);

    QSizePolicy size_left(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);    size_left.setHorizontalStretch(1);
    QSizePolicy size_right(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);   size_right.setHorizontalStretch(3);

    QLabel *label = new QLabel(slider_text);
    label->setSizePolicy(size_left);

    slider = new QSlider(Qt::Orientation::Horizontal);
    slider->setSizePolicy(size_right);
    slider->setMaximumHeight(26);
    slider->setTickPosition(QSlider::TickPosition::NoTicks);
    slider->setRange( 0, 100 );
    slider->setValue( 0 );

    pair_layout->addWidget(label);
    pair_layout->addWidget(slider);
    return pair;
}





















