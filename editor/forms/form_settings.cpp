//
//      Created by Stephens Nunnally on 3/21/2019, (c) 2019 Scidian Studios, All Rights Reserved
//
//  File:
//
//
//
#include <QDebug>
#include <QMouseEvent>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>

#include "editor/event_filters/event_filters.h"
#include "editor/forms/form_color_magnifier.h"
#include "editor/forms/form_settings.h"
#include "editor/helper_library.h"
#include "editor/interface_editor_relay.h"
#include "editor/preferences.h"
#include "editor/style/style.h"
#include "project/dr_project.h"


//####################################################################################
//##    Constructor
//####################################################################################
FormSettings::FormSettings(DrProject *project, QWidget *parent) : QWidget(parent) {

    // ***** Initialize member variables
    m_project = project;

    // ***** Set up initial window
    setAttribute(Qt::WidgetAttribute::WA_DeleteOnClose, true);
    setWindowFlags(Qt::WindowType::FramelessWindowHint | Qt::WindowType::Tool);

    setMinimumSize(QSize(200, 200));
    setObjectName(QStringLiteral("childForm"));
    this->setStyleSheet( Dr::CustomStyleSheetFormatting() );

    // ***** Create a contianer widget, this will allow us to create a layout for the form and add buttons and stuff
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(1, 1, 1, 1);

    m_inner_widget = new QWidget();
    m_inner_widget->setObjectName(QStringLiteral("innerWidget"));
    QVBoxLayout *inner_layout = new QVBoxLayout(m_inner_widget);

        QPushButton *exit = new QPushButton("  Exit  ");
        Dr::ApplyDropShadowByType(exit, Shadow_Types::Button_Shadow);
        exit->setObjectName(QStringLiteral("buttonDefault"));
        inner_layout->addWidget(exit);

        // Connect a lambda function to the "exit" button to close the form
        connect(exit, &QPushButton::clicked, [this] () {
            this->close();
        });


    layout->addWidget(m_inner_widget);

    // ***** Center window on Parent Form and install dragging event filter
    if (parent) Dr::CenterFormOnParent(parent->window(), this);
    this->installEventFilter(new DrFilterClickAndDragWindow(this));
}

FormSettings::~FormSettings() { }


//####################################################################################
//##    Keeps container widget same size as form
//####################################################################################
void FormSettings::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);

    Dr::ApplyRoundedCornerMask(this, 8, 8);
}















