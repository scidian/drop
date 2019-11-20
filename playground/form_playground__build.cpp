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
#include <QSpinBox>
#include <QStatusBar>
#include <QVBoxLayout>

#include "editor/tree_inspector.h"
#include "playground/form_playground.h"
#include "playground/playground.h"
#include "playground/playground_toy.h"
#include "style/style.h"
#include "widgets/widgets_event_filters.h"

#include "globals.h"
#include "helper.h"


//####################################################################################
//##    Builds Form
//####################################################################################
void FormPlayground::buildForm() {

    // Main Form Layout - Holds m_inner_widget, allowing a border
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(1, 1, 1, 1);

    // Inner widget, used for an addditional border
    m_inner_widget = new QWidget();
    m_inner_widget->setObjectName(QStringLiteral("innerWidget"));
    QVBoxLayout *inner_layout = new QVBoxLayout(m_inner_widget);
    inner_layout->setContentsMargins(10, 10, 10, 10);

        m_main_area = new QWidget();
        QHBoxLayout *main_area_layout = new QHBoxLayout(m_main_area);
        main_area_layout->setContentsMargins(1, 1, 1, 1);

            // ***** Side Bar
            m_side_bar = new QWidget();
            m_side_bar->setFixedWidth(200);
            QVBoxLayout *side_layout = new QVBoxLayout(m_side_bar);
            side_layout->setContentsMargins(0, 0, 0, 0);
            side_layout->setSpacing(10);

                // ***** Buttons
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

                // ***** World Info
                m_world_widget = new QWidget();
                QVBoxLayout *world_layout = new QVBoxLayout (m_world_widget);
                world_layout->setContentsMargins(0, 0, 0, 0);
                world_layout->setSpacing(0);

                    m_world_info = new QLabel("<center><b>World Info</b></center>");
                    m_world_info->setMaximumHeight(23);
                    world_layout->addWidget(m_world_info);

                    // ***** Time Warp
                    QWidget *pair_warp = new QWidget();
                    pair_warp->setMaximumHeight(23);
                    QHBoxLayout *pair_warp_layout = new QHBoxLayout(pair_warp);
                    pair_warp_layout->setContentsMargins(0, 0, 0, 0);
                    pair_warp_layout->setSpacing(0);
                        m_world_info_time_warp = new QLabel("<b>Time Warp</b>");
                        m_world_info_time_warp->setMaximumHeight(23);
                        pair_warp_layout->addWidget(m_world_info_time_warp);

                        // Example of a spin box, look at editor_tree_inspector_widgets.cpp for more widget examples
                        m_spin_time_warp = new DrQTripleSpinBox();
                        m_spin_time_warp->setAttribute(Qt::WA_MacShowFocusRect, 0);
                        m_spin_time_warp->setMaximumHeight(23);
                        m_spin_time_warp->setDecimals(3);
                        m_spin_time_warp->setRange(0, 100);
                        m_spin_time_warp->setPrefix("x ");
                        m_spin_time_warp->setSingleStep(0.1);
                        m_spin_time_warp->setButtonSymbols(QAbstractSpinBox::ButtonSymbols::NoButtons);
                        connect (m_spin_time_warp,  QOverload<double>::of(&DrQTripleSpinBox::valueChanged),
                             this, [this] (double d) { this->m_playground->setTimeWarp(d); });
                        pair_warp_layout->addWidget(m_spin_time_warp);
                    world_layout->addWidget(pair_warp);

                    // ***** Gravity
                    QWidget *pair_gravity = new QWidget();
                    pair_gravity->setMaximumHeight(23);
                    QHBoxLayout *pair_gravity_layout = new QHBoxLayout(pair_gravity);
                    pair_gravity_layout->setContentsMargins(0, 0, 0, 0);
                    pair_gravity_layout->setSpacing(10);
                        m_world_info_gravity = new QLabel("<b>Gravity</b>");
                        m_world_info_gravity->setMaximumHeight(23);
                        pair_gravity_layout->addWidget(m_world_info_gravity);

                        m_spin_gravity_x = new DrQTripleSpinBox();
                        m_spin_gravity_x->setAttribute(Qt::WA_MacShowFocusRect, 0);
                        m_spin_gravity_x->setMaximumHeight(23);
                        m_spin_gravity_x->setDecimals(3);
                        m_spin_gravity_x->setRange(-1000000, 1000000);
                        m_spin_gravity_x->setPrefix("X: ");
                        m_spin_gravity_x->setSingleStep(100);
                        m_spin_gravity_x->setButtonSymbols(QAbstractSpinBox::ButtonSymbols::NoButtons);
                        connect (m_spin_gravity_x,  QOverload<double>::of(&DrQTripleSpinBox::valueChanged),
                             this, [this] (double d) { this->m_playground->setGravityX(d); });
                        pair_gravity_layout->addWidget(m_spin_gravity_x);

                        m_spin_gravity_y = new DrQTripleSpinBox();
                        m_spin_gravity_y->setAttribute(Qt::WA_MacShowFocusRect, 0);
                        m_spin_gravity_y->setMaximumHeight(23);
                        m_spin_gravity_y->setDecimals(3);
                        m_spin_gravity_y->setRange(-1000000, 1000000);
                        m_spin_gravity_y->setPrefix("Y: ");
                        m_spin_gravity_y->setSingleStep(100);
                        m_spin_gravity_y->setButtonSymbols(QAbstractSpinBox::ButtonSymbols::NoButtons);
                        connect (m_spin_gravity_y,  QOverload<double>::of(&DrQTripleSpinBox::valueChanged),
                             this, [this] (double d) { this->m_playground->setGravityY(d); });
                        pair_gravity_layout->addWidget(m_spin_gravity_y);
                    world_layout->addWidget(pair_gravity);

                    m_world_info_damping =   new QLabel("");
                    m_world_info_damping->setMaximumHeight(23);
                    world_layout->addWidget(m_world_info_damping);

                    m_world_info_friction =  new QLabel("");
                    m_world_info_friction->setMaximumHeight(23);
                    world_layout->addWidget(m_world_info_friction);

                    m_world_info_bounce =    new QLabel("");
                    m_world_info_bounce->setMaximumHeight(23);
                    world_layout->addWidget(m_world_info_bounce);
                side_layout->addWidget(m_world_widget);


                // ***** Object Info
                m_object_info = new QLabel("<center>No Item Selected</center>");
                side_layout->addWidget(m_object_info);


                m_debug = new QLabel("Timer");
                m_debug->setAlignment(Qt::AlignCenter);
                side_layout->addWidget(m_debug);

            main_area_layout->addWidget(m_side_bar);

            // ***** Initialize QGraphicsScene for use in DrPlaygroundView
            m_play_scene = new QGraphicsScene();

            // ***** QGraphicsView (DrPlaygroundView)
            m_play_view = new DrPlaygroundView(m_play_scene, this, m_playground);
            m_play_view->setObjectName(QStringLiteral("playView"));
            m_play_view->setAcceptDrops(false);
            m_play_view->setFrameShape(QFrame::NoFrame);
            m_play_view->setDragMode(QGraphicsView::DragMode::NoDrag);
            m_play_view->setTransformationAnchor(QGraphicsView::ViewportAnchor::AnchorUnderMouse);
            m_play_view->setOptimizationFlags(QGraphicsView::OptimizationFlag::DontSavePainterState);
            m_play_view->setViewportUpdateMode(QGraphicsView::ViewportUpdateMode::SmartViewportUpdate);
            m_play_view->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
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











