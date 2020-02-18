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

    // Qt Progress Dialog Example
    /**
    QProgressDialog progress("Copying files...", "Abort Copy", 0, 10000000, this);
    progress.setWindowModality(Qt::WindowModal);
    for (int i = 0; i < 10000000; i++) {
        progress.setValue(i);
        if (progress.wasCanceled()) break;
        for (int j = 0; j < 1000000; j++) { double d = sqrt(j); }
    }
    progress.setValue(1000000);
    */

    // ***** Initialize member variables
    m_start_value = 0;
    m_end_value =   100 * items;

    // ***** Set up initial window
    setAttribute(Qt::WidgetAttribute::WA_DeleteOnClose, true);
    setWindowFlags(Qt::WindowType::FramelessWindowHint | Qt::WindowType::Tool);
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
            QFont label_font = Dr::CustomFont(1);
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
                m_canceled = true;
            });
            button_spacer->addWidget(cancel);
            button_spacer->addStretch();
        inner_layout->addWidget(button_holder);

    layout->addWidget(m_inner_widget);

    // ***** Start time count
    if (m_time_set == false) {
        m_start_time.restart();
        m_estimated_times.resize(20, 0.0);
        m_time_set = true;
    }

    // ***** Timer to update progress bar colors
    m_color_timer = new QTimer();
    connect(m_color_timer, SIGNAL(timeout()), this, SLOT(updateColors()));
    m_color_timer->start(30);

    // ***** Center window on screen and install dragging event filter
    Dr::CenterFormOnScreen(parent, this);
    this->installEventFilter(new DrFilterClickAndDragWindow(this));
}

FormProgressBox::~FormProgressBox() {
    m_color_timer->deleteLater();
}


//####################################################################################
//##    Keeps container widget same size as form
//####################################################################################
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
    m_info_text->setText(QString::fromStdString(info_text));
}

void FormProgressBox::stopProgress() {
    m_color_timer->stop();
    qApp->processEvents();
    this->close();
}

// Updates value
bool FormProgressBox::updateValue(int i) {
    int new_value = ((this->getCurrentItem() - 1) * 100) + i;
    if (new_value < m_start_value) new_value = m_start_value;
    if (new_value > m_end_value)   new_value = m_end_value;
    return this->setValue(new_value);
}


//####################################################################################
//##
//##    Progress Functions
//##
//####################################################################################
// Updates progress bar, shows form if its going to take longer than 2 seconds
//      RETURNS: True if "cancel" button has been pressed
bool FormProgressBox::setValue(int new_value) {
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
        }
    }

    // Update Progress Bar
    if (m_progress_bar->value() != new_value) {
        m_progress_bar->setValue(new_value);
    }

    // Close If Complete
//    if (m_progress_bar->value() == m_progress_bar->maximum()) {
//        this->stopProgress();
//    }

    update();
    qApp->processEvents();
    return m_canceled;
}

// SLOT: Updates Progress Bar Colors
void FormProgressBox::updateColors() {
    double percent = static_cast<double>(m_progress_bar->value() - m_start_value) / static_cast<double>(m_end_value - m_start_value);
    double seconds_ellapsed = m_start_time.elapsed() / 1000.0;                                          // Get time passed since progress bar start
    double hw_ratio = m_progress_bar->geometry().width() / m_progress_bar->geometry().height();         // Get widget width to height ratio
    double pixel_w =  15.00 / m_progress_bar->geometry().width()  / (percent * 0.5);                    // Set inital pixel width of gradient
    double pixel_h =  pixel_w/hw_ratio/percent;                                                         // Apply gradient to y axis
    double offset = -(seconds_ellapsed/8.0) / percent;                                                  // Adjust gradient start by time to animate

    // If longer than desired minimum time, show this progress box
    if (this->isVisible() == false && seconds_ellapsed > getShowIfWaitIsLongetThan()) {
        this->show();
    }

    // Update Style Sheet to Animate Progress Bar
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




























