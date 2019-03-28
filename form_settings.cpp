//
//      Created by Stephens Nunnally on 3/21/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QMouseEvent>
#include <QPushButton>
#include <QVBoxLayout>

#include "form_settings.h"
#include "library.h"
#include "project.h"
#include "widgets_event_filters.h"



#include <QApplication>
#include <QDesktopWidget>
#include <QGuiApplication>
#include <QScreen>

#include "form_main.h"
#include "globals.h"

FormSettings::FormSettings(DrProject *project, QWidget *parent) : QWidget(parent), m_project(project)
{
    // ***** Set up initial window
    setWindowFlags(Qt::WindowType::FramelessWindowHint | Qt::WindowType::Tool);

    setMinimumSize(QSize(200, 200));
    setObjectName(QStringLiteral("childForm"));
    Dr::ApplyCustomStyleSheetFormatting(this);

    ///// Transparent / translucent background
    ///this->setAttribute(Qt::WA_TranslucentBackground, true);
    ///this->setStyleSheet("background-color: rgba(255, 0, 0, 128)");

    // ***** Center window on screen and install dragging event filter
    Dr::CenterFormOnScreen(parent, this);
    this->installEventFilter(new ClickAndDragWindow(this));

    // Create a contianer widget, this will allow Create a layout for the form and add a button
    m_inner_widget = new QWidget(this);
    m_inner_widget->setObjectName(QStringLiteral("innerWidget"));
    QVBoxLayout *layout = new QVBoxLayout(m_inner_widget);
    layout->setAlignment(Qt::AlignCenter);

        m_label = new QLabel();
        m_label->setFixedSize(100, 50);
        m_label->setAlignment(Qt::AlignCenter);
        layout->addWidget(m_label);
        setColor(Qt::red);

        QPushButton *exit = new QPushButton("  Exit  ");
        Dr::ApplyDropShadowByType(exit, Shadow_Types::Button_Shadow);
        exit->setObjectName(QStringLiteral("button"));
        layout->addWidget(exit);


        // Connect a lambda function to the "exit" button to close the form
        connect(exit, &QPushButton::clicked, [this] () {
            this->close();
        });

}


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
//##        Following is for color grabber
//####################################################################################
void FormSettings::setColor(QColor color)
{
    m_color = color;
    m_label->setText( m_color.name() );

    QColor text_color = Qt::black;
    if (text_color.red() < 128 && text_color.green() < 128 && text_color.blue() < 128)
        text_color = Qt::white;
    m_label->setStyleSheet("color: " + text_color.name() + "; background: " + m_color.name() + ";" );
}



void FormSettings::mousePressEvent(QMouseEvent *event)
{
    QPoint   mouse_pos = event->globalPos();
    m_screen = QGuiApplication::screenAt(mouse_pos);
    ///QPoint screen_pos = QCursor::pos(screen);            // Also works, but also still grabs in virtualGeometry coordinates

    // Take a screenshot of the current screen
    QRect sg  = m_screen->geometry();
    m_capture = m_screen->grabWindow( QApplication::desktop()->winId(), sg.x(), sg.y(), sg.width(), sg.height());
    m_image   = m_capture.toImage();



    QWidget::mousePressEvent(event);
}


// This function is all screen grabber stuff, ultimately we need to grab a copy of the screen, draw it onto a form.
// Update the screen grab every now and then
void FormSettings::mouseMoveEvent(QMouseEvent *event)
{
    // Get global mouse position, figure out screen position
    QPoint mouse_pos = event->globalPos();
    QPoint screen_pos;
    QRect  sg =  m_screen->geometry();
    screen_pos.setX( static_cast<int>( (mouse_pos.x() - sg.x()) * m_screen->devicePixelRatio()) );
    screen_pos.setY( static_cast<int>( (mouse_pos.y() - sg.y()) * m_screen->devicePixelRatio()) );

    // Position out of bounds, exit out
    if (screen_pos.x() < 0 || screen_pos.x() > (m_image.width() - 1) || screen_pos.y() < 0 || screen_pos.y() > (m_image.height() - 1)) {
        QWidget::mouseMoveEvent(event);
        return;
    }

    // Get pixel color, set new color on color label
    QColor pixel = m_image.pixelColor(screen_pos.x(), screen_pos.y());
    setColor(pixel);

    QPoint   size(110, 110);
    double   zoom = 9;

    // Set round clipping path for pixmap
    QPixmap  small(size.x(), size.y());
    small.fill( Qt::transparent );
    QPainter paint(&small);
    QPainterPath circle_path, square_path;
    square_path.addRect(   0, 0, small.width(), small.height());
    circle_path.addEllipse(2, 2, small.width() - 4, small.height() - 4);
    paint.setClipPath(circle_path);

    // Draw zoomed in portion and square around selected pixel
    paint.drawPixmap(QRectF(0, 0, size.x(), size.y()), m_capture,
                     QRectF(screen_pos.x() - (size.x() / 2 / zoom), screen_pos.y() - (size.y() / 2 / zoom), size.x() / zoom, size.y() / zoom) );
    paint.setClipPath(square_path);

    paint.setBrush( Qt::NoBrush );
    paint.setPen( Qt::white );
    paint.drawRect( static_cast<int>(size.x() / 2), static_cast<int>(size.y() / 2), static_cast<int>(zoom), static_cast<int>(zoom) );
    paint.setPen( Qt::black );
    paint.drawRect( static_cast<int>(size.x() / 2) - 1, static_cast<int>(size.y() / 2) - 1, static_cast<int>(zoom) + 2, static_cast<int>(zoom) + 2);

    // Draw rings around zoom
    paint.setRenderHint(QPainter::Antialiasing, true);
    paint.setPen( QPen( Dr::GetColor(Window_Colors::Background_Dark), 4 ) );
    paint.drawEllipse(2, 2, small.width() - 4, small.height() - 4);

    paint.setPen( QPen( Dr::GetColor(Window_Colors::Midlight), 2 ) );
    paint.drawEllipse(2, 2, small.width() - 4, small.height() - 4);

    paint.setPen( QPen( m_color, 4 ) );
    paint.drawEllipse(5, 5, small.width() - 10, small.height() - 10);
    paint.end();







    // Remove any set curosr, apply new pixmap cursor
    while (QApplication::overrideCursor()) QApplication::restoreOverrideCursor();
    QCursor cursor( small, static_cast<int>(size.x() / 2), static_cast<int>(size.y() / 2));
    QApplication::setOverrideCursor( cursor );

    Dr::SetLabelText(Label_Names::Label_1, "Mouse Screen X: " + QString::number(mouse_pos.x()) +   ", Y: " + QString::number(mouse_pos.y()) );
    ///Dr::SetLabelText(Label_Names::Label_1, "Capture Size: X: " + QString::number(m_capture.width()) + ", Y: " + QString::number(m_capture.height()) );
    ///Dr::SetLabelText(Label_Names::Label_2, "Sg X: " + QString::number(sg.x()) +     ", Y: " + QString::number(sg.y()) +
    ///                                        ", W: " + QString::number(sg.width()) + ", H: " + QString::number(sg.height()) );

    QWidget::mouseMoveEvent(event);
}

void FormSettings::mouseReleaseEvent(QMouseEvent *event)
{
    while (QApplication::overrideCursor()) QApplication::restoreOverrideCursor();
    QWidget::mouseReleaseEvent(event);
}










