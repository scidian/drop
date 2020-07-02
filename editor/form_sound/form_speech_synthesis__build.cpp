//
//      Created by Stephens Nunnally on 6/4/2020, (c) 2020 Scidian Software, All Rights Reserved
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
#include "core/colors/colors.h"
#include "core/dr_random.h"
#include "editor/event_filters/event_filters.h"
#include "editor/form_sound/form_speech_synthesis.h"
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
void FormSpeechSynthesis::buildSpeechForm() {

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

        // Speech
        QWidget *speech_widget = new QWidget();
        QVBoxLayout *speech_layout = new QVBoxLayout(speech_widget);
        speech_layout->setObjectName(QStringLiteral("verticalLayout"));
        speech_layout->setSpacing(2);
        speech_layout->setContentsMargins(2, 2, 2, 2);

            QTextEdit *speech_text_box = new QTextEdit();
                speech_text_box->setFont(Dr::CustomFont());
                speech_text_box->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding));
                speech_text_box->setSizeAdjustPolicy(QTextEdit::SizeAdjustPolicy::AdjustToContents);
                speech_text_box->setPlaceholderText("Enter text to speak...");
            speech_layout->addWidget(speech_text_box);

            QWidget *speech_sliders = new QWidget();
            QHBoxLayout *speech_slider_layout = new QHBoxLayout(speech_sliders);
            speech_slider_layout->setObjectName(QStringLiteral("horizontalLayout"));
            speech_slider_layout->setSpacing(2);
            speech_slider_layout->setContentsMargins(2, 2, 2, 2);

                QWidget *speech_left_slider = new QWidget();
                QVBoxLayout *speech_left_slider_layout = new QVBoxLayout(speech_left_slider);
                speech_left_slider_layout->setObjectName(QStringLiteral("verticalLayout"));
                speech_left_slider_layout->setSpacing(2);
                speech_left_slider_layout->setContentsMargins(2, 2, 2, 2);

                    // Speech Sliders
                    speech_left_slider_layout->addWidget( sliderPair("Frequency",     m_speech_slider_freq));
                    speech_left_slider_layout->addWidget( sliderPair("Speed",         m_speech_slider_speed));
                    speech_left_slider_layout->addWidget( sliderPair("Declination",   m_speech_slider_decline));
                    m_speech_slider_freq->setRange(0, 5000);    m_speech_slider_freq->setValue(1330);
                    m_speech_slider_speed->setRange(0, 500);    m_speech_slider_speed->setValue(100);
                    m_speech_slider_decline->setRange(0, 100);  m_speech_slider_decline->setValue(50);

                    // Wave Form Drop Down Menu Button
                    QWidget *wave_form = new QWidget();
                    QHBoxLayout *wave_form_layout = new QHBoxLayout(wave_form);
                    wave_form_layout->setObjectName(QStringLiteral("verticalLayout"));
                    wave_form_layout->setSpacing(2);
                    wave_form_layout->setContentsMargins(0, 0, 0, 0);

                    QSizePolicy size_left(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);    size_left.setHorizontalStretch(1);
                    QSizePolicy size_right(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);   size_right.setHorizontalStretch(3);

                        QLabel *wave_label = new QLabel("Wave Form");
                            wave_label->setFont(Dr::CustomFont());
                            wave_label->setSizePolicy(size_left);
                        wave_form_layout->addWidget(wave_label);

                        QPushButton *drop_button = new QPushButton();
                        drop_button->setSizePolicy(size_right);
                        drop_button->setText("Triangle");
                        drop_button->setObjectName(QStringLiteral("spinBoxSelect"));
                        drop_button->setToolTip("Speech Wave Form");
                        drop_button->setFont(Dr::CustomFont());
                        drop_button->setStyleSheet(" QPushButton { border-top-left-radius: 4px; border-bottom-left-radius: 4px; }"
                                                   " QPushButton:pressed { border-radius: 0px; }");
                            QMenu *menu = new QMenu(this);
                                menu->setObjectName(QStringLiteral("menuComboBox"));
                                menu->setMinimumWidth(130);
                            QActionGroup *group;
                                group = new QActionGroup(menu);
                                group->setExclusive(true);
                            QStringList options{ "Saw", "Triangle", "Sin", "Square", "Pulse", "Noise", "Warble" };
                            // Add each option to menu
                            int count = 0;
                            for (auto string : options) {
                                QAction *action = new QAction(" " + tr(string.toLatin1()));
                                action->setCheckable(true);
                                if (count == m_speech_waveform) action->setChecked(true);
                                action->setProperty(User_Property::Integer, QVariant::fromValue(count));
                                group->addAction(action);
                                menu->addAction(action);
                                connect(action, &QAction::triggered, [this, action, drop_button]() {
                                        this->setSpeechWaveForm( action->property(User_Property::Integer).toInt() );
                                        drop_button->setText( action->text() );
                                });
                                count++;
                            }
                        drop_button->setMenu(menu);
                        menu->installEventFilter(new DrFilterPopUpMenuRelocater(menu, 2, 0));//-max_zoom_width));
                        wave_form_layout->addWidget(drop_button);

                    speech_left_slider_layout->addWidget(wave_form);

                speech_slider_layout->addWidget(speech_left_slider);

                QWidget *speech_middle = new QWidget();
                QVBoxLayout *speech_middle_layout = new QVBoxLayout(speech_middle);
                speech_middle_layout->setObjectName(QStringLiteral("verticalLayout"));
                speech_middle_layout->setSpacing(2);
                speech_middle_layout->setContentsMargins(2, 2, 2, 2);

                    QPushButton *play_speech = new QPushButton(tr("Generate Speech"));
                        Dr::ApplyDropShadowByType(play_speech, Shadow_Types::Button_Shadow);
                        play_speech->setObjectName(QStringLiteral("buttonDefault"));
                        connect(play_speech, &QPushButton::clicked, [this, speech_text_box] ()  {   this->playSpeech(speech_text_box->toPlainText().toStdString());   });
                    speech_middle_layout->addWidget(play_speech);

                speech_slider_layout->addWidget(speech_middle);
            speech_layout->addWidget(speech_sliders);
        middle_side_layout->addWidget(speech_widget);

        // Wave Form
        m_sound_wave = new WaveForm();
        m_sound_wave->setObjectName(QStringLiteral("soundWave"));
        m_sound_wave->setFixedHeight(100);
        middle_side_layout->addWidget(m_sound_wave);


        // Fills space
        ///middle_side_layout->addStretch();


        // Buttons
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
                connect(cancel, &QPushButton::clicked, [this] () {
                    delete m_speech;
                    this->close();
                });
            button_layout->addWidget(cancel);

            // Accept Button
            QPushButton *accept = new QPushButton("  Add Speech  ");
                accept->setObjectName(QStringLiteral("buttonDefault"));
                accept->setSizePolicy(button_right);
                Dr::ApplyDropShadowByType(accept, Shadow_Types::Button_Shadow);
                connect(accept, &QPushButton::clicked, [this] () {
                    if (m_speech != nullptr) {
                        // Add Sound
                        std::string sound_name = QString::fromStdString(m_speech_text).left(30).toStdString();
                        DrSound *sound = m_project->addSound(DrSoundType::Speech, m_speech, c_no_key, sound_name);

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

    inner_layout->addWidget(middle_side);

    // Add main widget to form
    layout->addWidget(m_inner_widget);
}



//####################################################################################
//##    Builds a label / slider pair
//####################################################################################
QWidget* FormSpeechSynthesis::sliderPair(QString slider_text, QSlider *&slider) {
    QWidget *pair = new QWidget();
    QHBoxLayout *pair_layout = new QHBoxLayout(pair);
    pair_layout->setSpacing(0);
    pair_layout->setContentsMargins(0, 0, 0, 0);

    QSizePolicy size_left(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);    size_left.setHorizontalStretch(1);
    QSizePolicy size_right(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);   size_right.setHorizontalStretch(3);

    QLabel *label = new QLabel(slider_text);
    label->setFont(Dr::CustomFont());
    label->setSizePolicy(size_left);

    slider = new QSlider(Qt::Orientation::Horizontal);
    slider->setSizePolicy(size_right);
    slider->setMaximumHeight(Dr::Scale(26));
    slider->setTickPosition(QSlider::TickPosition::NoTicks);
    slider->setRange( 0, 100 );
    slider->setValue( 0 );

    pair_layout->addWidget(label);
    pair_layout->addWidget(slider);
    return pair;
}
















