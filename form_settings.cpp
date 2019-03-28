//
//      Created by Stephens Nunnally on 3/21/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QPushButton>
#include <QGridLayout>
#include <QMouseEvent>

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

    // ***** Center window on screen and install dragging event filter
    Dr::CenterFormOnScreen(parent, this);
    this->installEventFilter(new ClickAndDragWindow(this));

    // Create a contianer widget, this will allow Create a layout for the form and add a button
    m_inner_widget = new QWidget(this);
    m_inner_widget->setObjectName(QStringLiteral("innerWidget"));
    QGridLayout *grid_layout = new QGridLayout(m_inner_widget);

        QPushButton *exit = new QPushButton("  Exit  ");
        Dr::ApplyDropShadowByType(exit, Shadow_Types::Button_Shadow);
        exit->setObjectName(QStringLiteral("button"));
        grid_layout->addWidget(exit);

        // Connect a lambda function to the "exit" button to close the form
        connect(exit, &QPushButton::clicked, [this] () {
            this->close();
        });

}


// This function is all screen grabber stuff, ultimately we need to grab a copy of the screen, draw it onto a form.
// Update the screen grab every now and then
void FormSettings::mouseMoveEvent(QMouseEvent *event)
{
    static long count = 1;

    QPoint   mouse_pos = event->globalPos();
    //QPoint   mouse_pos = this->mapToGlobal( event->pos() );    // Same as below
    QScreen *screen = QGuiApplication::screenAt(mouse_pos);
    //QPoint screen_pos = QCursor::pos(screen);

    /// Creates and returns a pixmap constructed by grabbing the contents of the given window restricted by QRect(x, y, width, height)
    QPixmap  capture;
    if (count % 50 == 0) {
        FormMain* fm = dynamic_cast<FormMain*>(parent());

        QRect sg = screen->geometry();
        capture = screen->grabWindow( QApplication::desktop()->winId(), sg.x(), sg.y(), sg.width(), sg.height());


        QPoint screen_pos;
        screen_pos.setX( static_cast<int>( (mouse_pos.x() - sg.x()) * screen->devicePixelRatio()) );
        screen_pos.setY( static_cast<int>( (mouse_pos.y() - sg.y()) * screen->devicePixelRatio()) );

        QImage si = capture.toImage();
        QColor pixel = si.pixelColor(screen_pos.x(), screen_pos.y());
        //Dr::SetLabelText(Label_Names::Label_2, "Color: " + pixel.name() );

        //Dr::SetLabelText(Label_Names::Label_1, "Mouse Screen X: " + QString::number(mouse_pos.x()) +   ", Y: " + QString::number(mouse_pos.y()) );
        Dr::SetLabelText(Label_Names::Label_1, "Capture Size: X: " + QString::number(capture.width()) + ", Y: " + QString::number(capture.height()) );
        Dr::SetLabelText(Label_Names::Label_2, "Sg X: " + QString::number(sg.x()) +     ", Y: " + QString::number(sg.y()) +
                                                ", W: " + QString::number(sg.width()) + ", H: " + QString::number(sg.height()) );

        if (count % 50 == 0) {
            capture = capture.scaledToWidth( capture.width() / 3 );
            Dr::ShowMessageBox("Test Image", capture);
        }

        fm->label2->setStyleSheet("background: " + pixel.name() + "; " );
    }
    //count++;


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












