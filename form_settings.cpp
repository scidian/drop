//
//      Created by Stephens Nunnally on 3/21/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QMouseEvent>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>

#include "form_color_magnifier.h"
#include "form_settings.h"
#include "globals.h"
#include "interface_editor_relay.h"
#include "library.h"
#include "project.h"
#include "widgets_event_filters.h"


FormSettings::FormSettings(DrProject *project, QWidget *parent) : QWidget(parent), m_project(project)
{
    // ***** Set up initial window
    setWindowFlags(Qt::WindowType::FramelessWindowHint | Qt::WindowType::Tool);

    setMinimumSize(QSize(200, 200));
    setObjectName(QStringLiteral("childForm"));
    Dr::ApplyCustomStyleSheetFormatting(this);

    // ***** Center window on screen and install dragging event filter
    Dr::CenterFormOnScreen(parent, this);
    this->installEventFilter(new ClickAndDragWindow(this));

    // Create a contianer widget, this will allow Create a layout for the form and add a button
    m_inner_widget = new QWidget(this);
    m_inner_widget->setObjectName(QStringLiteral("innerWidget"));
    QVBoxLayout *layout = new QVBoxLayout(m_inner_widget);
    layout->setAlignment(Qt::AlignCenter);

        m_label = new MouseLabel(this);
        m_label->setFixedSize(100, 50);
        m_label->setAlignment(Qt::AlignCenter);
        layout->addWidget(m_label);
        m_label->setColor(this, Qt::red);

        QPushButton *exit = new QPushButton("  Exit  ");
        Dr::ApplyDropShadowByType(exit, Shadow_Types::Button_Shadow);
        exit->setObjectName(QStringLiteral("button"));
        layout->addWidget(exit);

        // Connect a lambda function to the "exit" button to close the form
        connect(exit, &QPushButton::clicked, [this] () {
            this->close();
        });

}

FormSettings::~FormSettings() { }


//####################################################################################
//##        Keeps container widget same size as form
//####################################################################################
void FormSettings::resizeEvent(QResizeEvent *event)
{
    Dr::ApplyRoundedCornerMask(this, 8, 8);
    m_inner_widget->setGeometry( 1, 1, this->geometry().width() - 2, this->geometry().height() - 2);

    QWidget::resizeEvent(event);
}





//####################################################################################
//##        Initializes a Color Magnifier instance
//####################################################################################
MouseLabel::MouseLabel(QWidget *parent) : QLabel(parent) { }
MouseLabel::~MouseLabel() { }

void MouseLabel::mouseReleaseEvent(QMouseEvent *event) { QLabel::mouseReleaseEvent(event); }
void MouseLabel::mousePressEvent(QMouseEvent *event)
{
    QLabel::mousePressEvent(event);

    FormColorMagnifier *picker = new FormColorMagnifier(this, event->globalPos(), 115, 115, 8);
    connect(picker, SIGNAL(colorGrabbed(QWidget*, QColor)), this, SLOT(setColor(QWidget*, QColor)) );
    picker->show();
}

void MouseLabel::setColor(QWidget *widget, QColor color)
{
    Q_UNUSED (widget)
    m_color = color;
    this->setText( m_color.name() );

    QColor text_color = Qt::black;
    if (text_color.red() < 128 && text_color.green() < 128 && text_color.blue() < 128)
        text_color = Qt::white;
    this->setStyleSheet("color: " + text_color.name() + "; background: " + m_color.name() + ";" );
}



















