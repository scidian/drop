//
//      Created by Stephens Nunnally on 3/24/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QApplication>
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
    stopTimers();                                           // Call this to enable movable items while update timer is not running
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
//##    Timers
//######################################################################################################
void FormPlayground::startTimers() {
    m_start_timers->setEnabled(false);
    m_stop_timers->setEnabled(true);
    for (auto item : m_play_scene->items()) {
        item->setFlag(QGraphicsItem::ItemIsMovable, false);
        item->setFlag(QGraphicsItem::ItemSendsScenePositionChanges, false);
    }
    m_time_update =  Clock::now();
    m_update_timer->start( 1 );                             // Timeout of zero will call this timeout every pass of the event loop
}
void FormPlayground::stopTimers() {
    m_start_timers->setEnabled(true);
    m_stop_timers->setEnabled(false);
    m_update_timer->stop();
    do { qApp->processEvents(); } while (m_running);        // Make sure we're not running updateEngine before we turn on ItemSendsScenePositionChanges
    for (auto item : m_play_scene->items()) {
        item->setFlag(QGraphicsItem::ItemIsMovable, true);
        item->setFlag(QGraphicsItem::ItemSendsScenePositionChanges, true);
    }
}
void FormPlayground::resetWorld() {
    m_update_timer->stop();
    m_playground->clearSpace();
    m_playground->buildSpace();
    stopTimers();                                           // Call this to enable movable items while update timer is not running
}


//######################################################################################################
//##    Main Update Routine, connected to m_update_timer
//######################################################################################################
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


//######################################################################################################
//##    QGraphicsScene selection changed
//######################################################################################################
void FormPlayground::selectionChanged() {
    if (m_play_scene->selectedItems().count() < 1) {
        setObjectInfo("<center>No Item Selected</center>");
        return;
    }

    QGraphicsItem *item = m_play_scene->selectedItems().first();
    DrToy *toy = item->data(User_Roles::Toy).value<DrToy*>();

    QString info;
    info += "<center><b>Object Info</b></center><br>";
    info += "<b>Body Type:</b> " + Dr::StringFromBodyType(toy->body_type) + "<br>";
    info += "<b>Shape Type:</b> " + Dr::StringFromShapeType(toy->shape_type) + "<br>";
    info += "<b>Friction:</b> " + QString::number( cpShapeGetFriction(toy->shape) ) + "<br>";
    info += "<b>Bounce:</b> " + QString::number( cpShapeGetElasticity(toy->shape)) + "<br>";

    setObjectInfo(info);
}











