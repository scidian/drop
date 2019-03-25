//
//      Created by Stephens Nunnally on 3/21/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Font Builder
//
//
#include <QApplication>
#include <QPushButton>
#include <QGridLayout>
#include <QScreen>
#include <QStyle>

#include "form_fonts.h"
#include "library.h"
#include "project.h"
#include "widgets_event_filters.h"

FormFonts::FormFonts(DrProject *project, QWidget *parent) : QWidget(parent)
{
    m_project = project;

    // ***** Set up initial window
    setWindowFlag(Qt::WindowType::FramelessWindowHint);
    setWindowFlag(Qt::WindowType::Tool);
    setMinimumSize(QSize(200, 200));
    setObjectName(QStringLiteral("childForm"));
    Dr::ApplyCustomStyleSheetFormatting(this);

    // ***** Center window on screen
    QRect screenGeometry = QGuiApplication::screens().first()->geometry();
    this->setGeometry(QStyle::alignedRect( Qt::LeftToRight, Qt::AlignCenter, this->size(), screenGeometry ));
    this->installEventFilter(new ClickAndDragWindow(this));

    // Create a layout for the form and add a button
    QGridLayout *grid_layout = new QGridLayout(this);
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
//##        Upon first showing creates some rounded corners
//####################################################################################
void FormFonts::showEvent(QShowEvent *event)
{
    Q_UNUSED(event)

    if (!m_shown_yet) {
        Dr::ApplyRoundedCornerMask(this, 8, 8);
        m_shown_yet = true;
    }
    QWidget::showEvent(event);
}




