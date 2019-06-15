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

#include "form_playground.h"
#include "helper.h"
#include "project/project.h"
#include "style/style.h"
#include "widgets/widgets_event_filters.h"

//####################################################################################
//##        Constructor
//####################################################################################
FormPlayground::FormPlayground(DrProject *project, QWidget *parent) : QWidget(parent) {

    // ***** Initialize member variables
    m_project = project;

    // ***** Set up initial window
    setWindowFlags(Qt::WindowType::FramelessWindowHint | Qt::WindowType::Tool);
    setMinimumSize(QSize(800, 600));
    setMouseTracking(true);
    setObjectName(QStringLiteral("childForm"));
    Dr::ApplyCustomStyleSheetFormatting(this);

    // ***** Center window on screen and install dragging event filter
    Dr::CenterFormOnScreen(parent, this);
    this->installEventFilter(new DrFilterClickAndDragWindow(this));

    // Create a contianer widget, this will allow Create a layout for the form and add a button
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(1, 1, 1, 1);

    m_inner_widget = new QWidget();
    m_inner_widget->setObjectName(QStringLiteral("innerWidget"));
    QVBoxLayout *inner_layout = new QVBoxLayout(m_inner_widget);

        m_playground = new QWidget();
        QHBoxLayout *playground_layout = new QHBoxLayout(m_playground);

            m_side_bar = new QWidget();
            m_side_bar->setFixedWidth(200);
            QVBoxLayout *side_layout = new QVBoxLayout(m_side_bar);
                QLabel *label1 = new QLabel("Settings");
                side_layout->addWidget(label1);
            playground_layout->addWidget(m_side_bar);

            m_play_view = new QGraphicsView();

            m_play_view->setObjectName(QStringLiteral("playView"));
            m_play_view->setAcceptDrops(false);
            m_play_view->setFrameShape(QFrame::NoFrame);
            m_play_view->setDragMode(QGraphicsView::DragMode::NoDrag);
            m_play_view->setTransformationAnchor(QGraphicsView::ViewportAnchor::AnchorUnderMouse);
            m_play_view->setOptimizationFlags(QGraphicsView::OptimizationFlag::DontSavePainterState);
            m_play_view->setViewportUpdateMode(QGraphicsView::ViewportUpdateMode::SmartViewportUpdate);
            m_play_view->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::HighQualityAntialiasing);

            playground_layout->addWidget(m_play_view);



        inner_layout->addWidget(m_playground);

        QPushButton *exit = new QPushButton("  Exit  ");
        Dr::ApplyDropShadowByType(exit, Shadow_Types::Button_Shadow);
        exit->setObjectName(QStringLiteral("button"));
        inner_layout->addWidget(exit);

        // Connect a lambda function to the "exit" button to close the form
        connect(exit, &QPushButton::clicked, [this] () {
            this->close();
        });

    layout->addWidget(m_inner_widget);

    m_grip =  new QSizeGrip(this);
    m_grip2 = new QSizeGrip(m_inner_widget);
}


//####################################################################################
//##        Keeps container widget same size as form
//####################################################################################
void FormPlayground::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);

    Dr::ApplyRoundedCornerMask(this, 8, 8);

    m_grip->setGeometry( this->width() - 10, this->height() - 10, 15, 15);
    m_grip2->setGeometry(this->width() - 10, this->height() - 10, 15, 15);
}
















