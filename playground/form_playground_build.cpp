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
                QLabel *label1 = new QLabel("Settings");
                side_layout->addWidget(label1);
            main_area_layout->addWidget(m_side_bar);

            // ***** QGraphicsView (DrPlaygroundView)
            m_play_scene = new QGraphicsScene();
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


//####################################################################################
//##        Adds QGraphicsItems to Represent Playground Toys
//####################################################################################
QGraphicsLineItem* FormPlayground::addGraphicsLine(DrToy *toy, QColor color) {
    // Create new LineItem
    QGraphicsLineItem *line = new QGraphicsLineItem();
    line->setPen( QPen(color, 2));

    cpVect p1 = cpSegmentShapeGetA(toy->shape);
    cpVect p2 = cpSegmentShapeGetB(toy->shape);
    line->setLine(p1.x, -p1.y, p2.x, -p2.y);

    // Store reference to the associated DrToy in the Space
    line->setData(User_Roles::Key, QVariant::fromValue<DrToy*>(toy));

    // Add to QGraphicsScene
    m_play_scene->addItem(line);
    return line;
}

QGraphicsEllipseItem* FormPlayground::addGraphicsCircle(DrToy *toy, QColor color) {
    // Create new LineItem
    DrPlaygroundCircle *circle = new DrPlaygroundCircle();
    circle->setPen( QPen(color, 2));

    double radius = cpCircleShapeGetRadius(toy->shape);
    circle->setPos(QPointF(toy->m_position.x(), -toy->m_position.y()));
    circle->setRect( QRectF(-radius, -radius, radius*2.0, radius*2.0) );

    // Store reference to the associated DrToy in the Space
    circle->setData(User_Roles::Key, QVariant::fromValue<DrToy*>(toy));

    // Add to QGraphicsScene
    m_play_scene->addItem(circle);
    return circle;
}


QGraphicsRectItem* FormPlayground::addGraphicsBox(DrToy *toy, QColor color) {
    // Create new LineItem
    DrPlaygroundBox *box = new DrPlaygroundBox();
    box->setPen( QPen(color, 2));

    box->setPos(QPointF(toy->m_position.x(), -toy->m_position.y()));
    box->setRect( QRectF(-(toy->m_width/2.0), -(toy->m_height/2.0), toy->m_width, toy->m_height) );

    // Store reference to the associated DrToy in the Space
    box->setData(User_Roles::Key, QVariant::fromValue<DrToy*>(toy));

    // Add to QGraphicsScene
    m_play_scene->addItem(box);
    return box;
}


















