//
//      Created by Stephens Nunnally on 3/24/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QMouseEvent>
#include <QPushButton>
#include <QSizeGrip>
#include <QStatusBar>
#include <QVBoxLayout>

#include "chipmunk/chipmunk.h"
#include "form_playground.h"
#include "globals.h"
#include "helper.h"
#include "playground.h"
#include "playground_toy.h"
#include "style/style.h"
#include "widgets/widgets_event_filters.h"

//####################################################################################
//##        Constructor
//####################################################################################
FormPlayground::FormPlayground(QWidget *parent) : QWidget(parent) {
    // ***** Set up initial window
    setWindowFlags(Qt::WindowType::FramelessWindowHint | Qt::WindowType::Tool);
    setMinimumSize(QSize(600, 400));
    resize(1200, 900);
    setMouseTracking(true);
    setObjectName(QStringLiteral("childForm"));
    Dr::ApplyCustomStyleSheetFormatting(this);

    // ***** Center window on screen and install dragging event filter
    Dr::CenterFormOnScreen(parent, this);
    this->installEventFilter(new DrFilterClickAndDragWindow(this));

    // ***** Initialize Playground
    m_playground = new DrPlayground(this);

    // ***** Build out form widgets
    buildForm();

    // ***** Create engine timer and connect
    m_update_timer = new QTimer(this);
    connect(m_update_timer, SIGNAL(timeout()), this, SLOT(updateEngine()));

    // ***** Build Space and start update timer
    m_playground->buildSpace();
    startTimers();
}


//####################################################################################
//##        Keeps container widget same size as form
//####################################################################################
void FormPlayground::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);

    // Re-apply nice rounded corner mask
    Dr::ApplyRoundedCornerMask(this, 8, 8);

    // Move corner resize grips when window resizes
    m_grip->setGeometry( this->width() - 10, this->height() - 10, 15, 15);
    m_grip2->setGeometry(this->width() - 10, this->height() - 10, 15, 15);
}


//######################################################################################################
//##    Update Engine
//######################################################################################################
void FormPlayground::startTimers() {
    m_time_update =  Clock::now();
    m_update_timer->start( 1 );                         // Timeout of zero will call this timeout every pass of the event loop
}
void FormPlayground::stopTimers() {
    m_update_timer->stop();
}

// Main Update Routine, connected to m_update_timer
void FormPlayground::updateEngine() {
    if (!m_playground->has_scene) return;
    m_running = true;

    double update_milliseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(Clock::now() - m_time_update).count() /  1000000.0;
    if (update_milliseconds > m_playground->getTimeStepAsMilliseconds()) {
        m_time_update =  Clock::now();
        m_playground->updateSpace(update_milliseconds);                      // Physics Engine
        m_playground->updateSpaceHelper();                                   // Additional Physics Updating
        updateGraphicsView();
    }

    m_running = false;
}













