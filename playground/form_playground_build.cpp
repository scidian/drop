//
//      Created by Stephens Nunnally on 6/15/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QLabel>
#include <QMouseEvent>
#include <QPushButton>
#include <QSizeGrip>
#include <QStatusBar>
#include <QVBoxLayout>

#include "form_playground.h"
#include "globals.h"
#include "helper.h"
#include "style/style.h"
#include "widgets/widgets_event_filters.h"

//####################################################################################
//##        Builds Form
//####################################################################################
void FormPlayground::buildForm() {

    // Main Form Layout - Holds m_inner_widget, allowing a border
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(1, 1, 1, 1);

    // Inner widget, used for an addditional border
    m_inner_widget = new QWidget();
    m_inner_widget->setObjectName(QStringLiteral("innerWidget"));
    QVBoxLayout *inner_layout = new QVBoxLayout(m_inner_widget);

        m_main_area = new QWidget();
        QHBoxLayout *main_area_layout = new QHBoxLayout(m_main_area);

            // ***** Side Bar
            m_side_bar = new QWidget();
            m_side_bar->setFixedWidth(200);
            QVBoxLayout *side_layout = new QVBoxLayout(m_side_bar);

                m_start_timers = new QPushButton("Start Physics");
                m_start_timers->setObjectName("button");
                Dr::ApplyDropShadowByType(m_start_timers, Shadow_Types::Button_Shadow);
                connect(m_start_timers, SIGNAL(pressed()), SLOT(startTimers()) );
                side_layout->addWidget(m_start_timers);

                m_stop_timers = new QPushButton("Stop Physics");
                m_stop_timers->setObjectName("button");
                Dr::ApplyDropShadowByType(m_stop_timers, Shadow_Types::Button_Shadow);
                connect(m_stop_timers, SIGNAL(pressed()), SLOT(stopTimers()) );
                side_layout->addWidget(m_stop_timers);

                m_reset_world = new QPushButton("Reset World");
                m_reset_world->setObjectName("button");
                Dr::ApplyDropShadowByType(m_reset_world, Shadow_Types::Button_Shadow);
                connect(m_reset_world, SIGNAL(pressed()), SLOT(resetWorld()) );
                side_layout->addWidget(m_reset_world);

                m_world_info = new QLabel("World Info");
                side_layout->addWidget(m_world_info);

                m_object_info = new QLabel("<center>No Item Selected</center>");
                side_layout->addWidget(m_object_info);

                m_debug = new QLabel("Timer");
                m_debug->setAlignment(Qt::AlignCenter);
                side_layout->addWidget(m_debug);

            main_area_layout->addWidget(m_side_bar);

            // ***** Initialize QGraphicsScene for use in DrPlaygroundView
            m_play_scene = new QGraphicsScene();

            // ***** QGraphicsView (DrPlaygroundView)
            m_play_view = new DrPlaygroundView(m_play_scene, m_playground);
            m_play_view->setObjectName(QStringLiteral("playView"));
            m_play_view->setAcceptDrops(false);
            m_play_view->setFrameShape(QFrame::NoFrame);
            m_play_view->setDragMode(QGraphicsView::DragMode::NoDrag);
            m_play_view->setTransformationAnchor(QGraphicsView::ViewportAnchor::AnchorUnderMouse);
            m_play_view->setOptimizationFlags(QGraphicsView::OptimizationFlag::DontSavePainterState);
            m_play_view->setViewportUpdateMode(QGraphicsView::ViewportUpdateMode::SmartViewportUpdate);
            m_play_view->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::HighQualityAntialiasing);
            m_play_view->setSceneRect(-1000, -1000, 2000, 2000);

            // Set initial zoom level
            m_play_view->setZoomLevel(180);
            m_play_view->zoomInOut(0);

            main_area_layout->addWidget(m_play_view);

        inner_layout->addWidget(m_main_area);

        QPushButton *exit = new QPushButton("  Exit  ");
        Dr::ApplyDropShadowByType(exit, Shadow_Types::Button_Shadow);
        exit->setObjectName(QStringLiteral("button"));
        inner_layout->addWidget(exit);

        // Connect a lambda function to the "exit" button to close the form
        connect(exit, &QPushButton::clicked, [this] () {
            this->close();
        });

    layout->addWidget(m_inner_widget);

    // Add size grip resizers to bottom right part of Form
    m_grip =  new QSizeGrip(this);
    m_grip2 = new QSizeGrip(m_inner_widget);
}











