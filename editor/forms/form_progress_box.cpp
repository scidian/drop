//
//      Created by Stephens Nunnally on 2/5/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QApplication>
#include <QDebug>
#include <QProgressBar>
#include <QPushButton>
#include <QVBoxLayout>

#include "core/colors/colors.h"
#include "core/dr_random.h"
#include "editor/event_filters.h"
#include "editor/forms/form_blank.h"
#include "editor/forms/form_progress_box.h"
#include "editor/helper_library.h"
#include "editor/style/style.h"
#include "project/dr_project.h"


//####################################################################################
//##    Constructor
//####################################################################################
FormProgressBox::FormProgressBox(QString info_text, QString cancel_button_text, int items, QWidget *parent)
    : QWidget(parent), IProgressBar(items) {

    // ***** Initialize member variables
    m_start_value = 0;
    m_end_value =   100 * items;

    // ***** Set up initial window
    setAttribute(Qt::WidgetAttribute::WA_DeleteOnClose, true);
    setWindowFlags(Qt::WindowType::FramelessWindowHint | Qt::WindowType::Tool | Qt::WindowStaysOnTopHint);
    setWindowModality(Qt::WindowModality::ApplicationModal);
    setMinimumSize(QSize(300, 150));
    setObjectName(QStringLiteral("childForm"));
    this->setStyleSheet( Dr::CustomStyleSheetFormatting() );

    // ***** Create a contianer widget, this will allow Create a layout for the form and add a button
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(1, 1, 1, 1);

    m_inner_widget = new QWidget();
    m_inner_widget->setObjectName(QStringLiteral("innerWidget"));
    QVBoxLayout *inner_layout = new QVBoxLayout(m_inner_widget);

        m_info_text = new QLabel(info_text);
            std::string label_style = " QLabel { color: " + Dr::GetColor(Window_Colors::Text_Light).name() + "; } ";
            m_info_text->setStyleSheet(QString::fromStdString(label_style));
            m_info_text->setFixedHeight(50);
            m_info_text->setAlignment(Qt::AlignmentFlag::AlignCenter);
            QFont label_font = Dr::CustomFont(0);
                  label_font.setBold(true);
            m_info_text->setFont(label_font);
        inner_layout->addWidget(m_info_text);

        m_progress_bar = new QProgressBar();
            m_progress_bar->setRange(m_start_value, m_end_value);
            m_progress_bar->setTextVisible(false);
            std::string style;
            style +=    " QProgressBar          { "
                        "       border: " + Dr::BorderWidth() + " solid; background-color:transparent; height: 13px; border-radius: 4px; "
                        "       border-color: " + Dr::GetColor(Window_Colors::Background_Dark).darker(250).name() + "; } ";
                        ///"    background-image:url(:/assets/textures/fire_noise.png);  } ";
            style +=    " QProgressBar::chunk { border-radius: 4px; background: transparent; } ";
                        ///"    background:url(:/assets/textures/fire_noise.jpg); } ";
            m_progress_bar->setStyleSheet(QString::fromStdString(style));
        inner_layout->addStretch();
        inner_layout->addWidget(m_progress_bar);

        QWidget *button_holder = new QWidget();
            QHBoxLayout *button_spacer = new QHBoxLayout(button_holder);
            button_spacer->addStretch();

            QPushButton *cancel = new QPushButton("  " + cancel_button_text + "  ");
            ///cancel->setFixedHeight(22);
            ///cancel->setFont(label_font);
            Dr::ApplyDropShadowByType(cancel, Shadow_Types::Button_Shadow);
            cancel->setObjectName(QStringLiteral("button"));

            // Connect a lambda function to the "exit" button to cancel the Progress Box
            connect(cancel, &QPushButton::clicked, [this] () {
                setCanceled();
            });
            button_spacer->addWidget(cancel);
            button_spacer->addStretch();
        inner_layout->addWidget(button_holder);

    layout->addWidget(m_inner_widget);

    // ***** Start time count, zero average time buffer
    m_start_time.restart();
    m_estimated_times.clear();
    for (int i = 0; i < 20; ++i) m_estimated_times.push_back(0.0);

    // Color Timer Variables
    m_last_color_update.restart();
    m_color_seconds = 0.0;

    // ***** Center window on screen and install dragging event filter
    Dr::CenterFormOnScreen(parent, this);
    this->installEventFilter(new DrFilterClickAndDragWindow(this));

    // ***** Timer to update progress bar colors
    m_color_timer = new QTimer();
    connect(m_color_timer, SIGNAL(timeout()), this, SLOT(updateColors()));
    m_color_timer->setInterval(30);
    m_color_timer->setTimerType(Qt::PreciseTimer);
    m_color_timer->start();

}

FormProgressBox::~FormProgressBox() { }


//####################################################################################
//##    QWidget Events
//####################################################################################
void FormProgressBox::paintEvent(QPaintEvent *event) {
    QWidget::paintEvent(event);
}

// Keeps container widget same size as form
void FormProgressBox::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    Dr::ApplyRoundedCornerMask(this, 8, 8);
}


//####################################################################################
//##
//##    Virtual IProgressBar Functions
//##
//####################################################################################
// Updates working text
void FormProgressBox::setDisplayText(std::string info_text) {
    if (isProgressLocked() == false) {
        if (m_info_text) m_info_text->setText(QString::fromStdString(getPrefix() + info_text + getSuffix()));
        update();
        qApp->processEvents();
    }
}

void FormProgressBox::stopProgress() {
    m_color_timer->stop();
    m_color_timer->deleteLater();
    qApp->processEvents();
    this->close();
}

// Updates value
void FormProgressBox::updateValue(double d) {
    if (isProgressLocked() == false && d >= 0.0) {
        d = getAddIn() + (d * getMultiplier());
        int new_value = ((this->getCurrentItem() - 1) * 100) + static_cast<int>(d);
        if (new_value < m_start_value)      new_value = m_start_value;
        if (new_value > m_end_value)        new_value = m_end_value;
        this->applyValue(new_value);
    }
    update();
    qApp->processEvents();
}


//####################################################################################
//##
//##    Progress Functions
//##
//####################################################################################
// Updates progress bar, shows form if its going to take longer than 2 seconds
//      RETURNS: True if "cancel" button has been pressed
void FormProgressBox::applyValue(int new_value) {
    // Calculate percentage
    double percent = static_cast<double>(new_value - m_start_value) / static_cast<double>(m_end_value - m_start_value);
    double seconds_ellapsed = m_start_time.elapsed() / 1000.0;

    // If we've done 1 percent, see if this is going to take longer than desired minimum time, if so show this progress box
    if (this->isVisible() == false && percent > 0.01) {
        double time_required = seconds_ellapsed * (1.0 / percent);
        m_estimated_times.push_back(time_required);
        if (m_estimated_times.size() > 20) m_estimated_times.pop_front();
        time_required = std::accumulate(m_estimated_times.begin(), m_estimated_times.end(), 0.0) / static_cast<double>(m_estimated_times.size());
        if (time_required > getShowIfWaitIsLongetThan()) {
            this->show();
            this->update();
            qApp->processEvents();
            this->raise();
            this->activateWindow();
        }
    }

    // Update Progress Bar
    if (this->isVisible() && (m_progress_bar->value() != new_value)) {
        m_progress_bar->setValue(new_value);
        updateColors();
    }

    // Close If Complete
    ///if (m_progress_bar->value() == m_progress_bar->maximum()) this->stopProgress();
}


// SLOT: Updates Progress Bar Colors
void FormProgressBox::updateColors() {
    // Get time passed since progress bar start
    double seconds_ellapsed = m_start_time.elapsed() / 1000.0;

    // Update Color Rotation Timer
    m_color_seconds += (m_last_color_update.elapsed() / 1000.0);
    m_last_color_update.restart();

    // If longer than desired minimum time, show this progress box
    if ((this->isVisible() == false) && (seconds_ellapsed > getShowIfWaitIsLongetThan())) {
        this->show();
        this->update();
        qApp->processEvents();
        this->raise();
        this->activateWindow();

    // Update Style Sheet to Animate Progress Bar
    } else if (this->isVisible()) {
        double percent = static_cast<double>(m_progress_bar->value() - m_start_value) / static_cast<double>(m_end_value - m_start_value);
        double hw_ratio = m_progress_bar->geometry().width() / m_progress_bar->geometry().height();         // Get widget width to height ratio
        double pixel_w =  15.00 / m_progress_bar->geometry().width()  / (percent * 0.5);                    // Set inital pixel width of gradient
        double pixel_h =  pixel_w/hw_ratio/percent;                                                         // Apply gradient to y axis
        double offset = -(m_color_seconds/8.0) / percent;                                                   // Adjust gradient start by time to animate
        std::string style;
        style +=    " QProgressBar          { "
                    "       border: " + Dr::BorderWidth() + " solid; background-color:transparent; height: 13px; border-radius: 4px; "
                    "       border-color: " + Dr::GetColor(Window_Colors::Background_Dark).darker(250).name() + "; } ";
        style +=    " QProgressBar::chunk { "
                    "       border-radius: 4px; "
                    "       background: qlineargradient(spread:reflect, "
                                            " x1:" + std::to_string( offset + 0.0 ) + ", "
                                            " y1:" + std::to_string( offset + 0.0 ) +
                                            " x2:" + std::to_string( offset + pixel_w ) +
                                            " y2:" + std::to_string( offset + pixel_h ) + ", "
                    "                   stop:0 " + Dr::GetColor(Window_Colors::Icon_Dark ).darker( 120).name() + ", "
                    "                   stop:1 " + Dr::GetColor(Window_Colors::Icon_Light).lighter(120).name() + "); } ";
        m_progress_bar->setStyleSheet(QString::fromStdString(style));
        update();
        qApp->processEvents();
    }
}
























