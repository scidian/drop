//
//      Created by Stephens Nunnally on 3/25/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "form_blank.h"

#include <QApplication>
#include <QPushButton>
#include <QGridLayout>
#include <QScreen>
#include <QStyle>

#include "form_atlas.h"
#include "library.h"
#include "project.h"
#include "widgets_event_filters.h"

FormBlank::FormBlank(DrProject *project, QWidget *parent) : QWidget(parent), m_project(project)
{
    // ***** Set up initial window
    setWindowFlags(Qt::WindowType::FramelessWindowHint | Qt::WindowType::Tool);
    setMinimumSize(QSize(200, 200));
    setObjectName(QStringLiteral("childForm"));
    Dr::ApplyCustomStyleSheetFormatting(this);

    // ***** Center window on screen and install dragging event filter
    QRect screenGeometry = QGuiApplication::screens().first()->geometry();
    this->setGeometry(QStyle::alignedRect( Qt::LeftToRight, Qt::AlignCenter, this->size(), screenGeometry ));
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


//####################################################################################
//##        Keeps container widget same size as form
//####################################################################################
void FormBlank::resizeEvent(QResizeEvent *event)
{
    Dr::ApplyRoundedCornerMask(this, 8, 8);
    m_inner_widget->setGeometry( 1, 1, this->geometry().width() - 2, this->geometry().height() - 2);

    QWidget::resizeEvent(event);
}



