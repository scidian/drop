//
//      Created by Stephens Nunnally on 3/24/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QApplication>
#include <QDebug>
#include <QMouseEvent>
#include <QPushButton>
#include <QSizeGrip>
#include <QStatusBar>
#include <QVBoxLayout>

#include "3rd_party_chipmunk/chipmunk.h"
#include "form_playground.h"
#include "globals.h"
#include "helper.h"
#include "helper_qt.h"
#include "playground.h"
#include "playground_toy.h"
#include "style/style.h"
#include "widgets/widgets_event_filters.h"

//####################################################################################
//##    Constructor
//####################################################################################
FormPlayground::FormPlayground(QWidget *parent) : QWidget(parent) {
    // ***** Set up initial window
    setWindowFlags(Qt::WindowType::FramelessWindowHint | Qt::WindowType::Tool);
    setMinimumSize(QSize(600, 400));
    resize(1100, 900);
    setAttribute( Qt::WA_DeleteOnClose, true );                         // Make sure this form is deleted when it closes
    setMouseTracking(true);
    setObjectName(QStringLiteral("childForm"));
    setStyleSheet( Dr::CustomStyleSheetFormatting() );

    // ***** Center window on screen and install dragging event filter
    Dr::CenterFormOnScreen(parent, this);
    this->installEventFilter(new DrFilterClickAndDragWindow(this));

    // ***** Initialize Playground
    m_playground = new DrPlayground(this);

    // ***** Build out form widgets
    buildForm();

    // ***** Create engine timer and connect
    m_update_timer = new QTimer(this);
    connect(m_update_timer, SIGNAL(timeout()), this, SLOT(updateEngine()));                     // Connect to timer

    // ***** Build Space and start update timer
    m_playground->buildSpace();
    m_playground->updateSpace( 0 );
    stopTimers();                                                       // Call this to enable movable items while update timer is not running
    m_play_view->setFocus();
}

FormPlayground::~FormPlayground() {
    delete m_playground;
}

//####################################################################################
//##    Keeps container widget same size as form
//####################################################################################
void FormPlayground::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);

    // Re-apply nice rounded corner mask
    Dr::ApplyRoundedCornerMask(this, 8, 8);

    // Move corner resize grips when window resizes
    m_grip->setGeometry( this->width() - 10, this->height() - 10, 15, 15);
    m_grip2->setGeometry(this->width() - 10, this->height() - 10, 15, 15);
}


//####################################################################################
//##    Timers
//####################################################################################
void FormPlayground::startTimers() {
    m_start_timers->setEnabled(false);
    m_stop_timers->setEnabled(true);
    disableItemFlags();
    m_time_update = Clock::now();
    m_update_timer->start( 1 );                                         // Timeout of zero will call this timeout every pass of the event loop
}
void FormPlayground::stopTimers() {
    m_start_timers->setEnabled(true);
    m_stop_timers->setEnabled(false);
    m_update_timer->stop();
    enableItemFlags();
}
void FormPlayground::resetWorld() {
    m_update_timer->stop();
    disableItemFlags();
    m_playground->clearSpace();
    m_playground->buildSpace();
    m_playground->updateSpace( 0 );
    stopTimers();                                                       // Call this to enable movable items while update timer is not running
}

void FormPlayground::disableItemFlags() {
    for (auto item : m_play_scene->items()) {
        item->setFlag(QGraphicsItem::ItemIsMovable, false);
        item->setFlag(QGraphicsItem::ItemSendsScenePositionChanges, false);

        item->setFlag(QGraphicsItem::ItemIsSelectable, false);          // !!!!! QtBug - Built In Selection was Blocking Event Thread
        item->setFlag(QGraphicsItem::ItemIsFocusable, false);
    }
}
void FormPlayground::enableItemFlags() {
    for (auto item : m_play_scene->items()) {
        item->setFlag(QGraphicsItem::ItemIsMovable, true);
        item->setFlag(QGraphicsItem::ItemSendsScenePositionChanges, true);

        item->setFlag(QGraphicsItem::ItemIsSelectable, false);          // !!!!! QtBug - Built In Selection was Blocking Event Thread
        item->setFlag(QGraphicsItem::ItemIsFocusable, false);
    }
}

//####################################################################################
//##
//##    Main Update Routine, connected to m_update_timer
//##
//####################################################################################
void FormPlayground::updateEngine() {
    if (!m_playground->has_scene) return;
    m_updating = true;

    double update_milliseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(Clock::now() - m_time_update).count() /  1000000.0;
    if (update_milliseconds > m_playground->getTimeStepAsMilliseconds()) {
        m_time_update =  Clock::now();
        m_playground->updateSpace(update_milliseconds);                 // Physics Engine
        m_playground->updateWorld();                                    // Additional World / Toy Updating
        updateGraphicsView();

        m_debug->setText(Dr::CurrentTimeAsString());
    }

    m_updating = false;
}


//####################################################################################
//##    Selected Different Item
//####################################################################################
void FormPlayground::clickedItem(DrToy *toy) {
    if (toy == nullptr) {
        setObjectInfo("<center>No Item Selected</center>");
        return;
    }

    QString info;
    info += "<center><b>Object Info</b></center><br>";
    info += "<b>Body Type:</b> " + QString::fromStdString( Dr::StringFromBodyType(toy->body_type) ) + "<br>";
    info += "<b>Shape Type:</b> " + QString::fromStdString( Dr::StringFromShapeType(toy->shape_type) ) + "<br>";
    info += "<b>Friction:</b> " + QString::number( cpShapeGetFriction(toy->shape) ) + "<br>";
    info += "<b>Bounce:</b> " + QString::number( cpShapeGetElasticity(toy->shape)) + "<br>";

    setObjectInfo(info);
}











