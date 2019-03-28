//
//      Created by Stephens Nunnally on 3/24/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QPushButton>
#include <QVBoxLayout>

#include "form_atlas.h"
#include "library.h"
#include "project.h"
#include "widgets_event_filters.h"

FormAtlas::FormAtlas(DrProject *project, QWidget *parent) : QWidget(parent), m_project(project)
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
void FormAtlas::resizeEvent(QResizeEvent *event)
{
    Dr::ApplyRoundedCornerMask(this, 8, 8);
    m_inner_widget->setGeometry( 1, 1, this->geometry().width() - 2, this->geometry().height() - 2);

    QWidget::resizeEvent(event);
}




