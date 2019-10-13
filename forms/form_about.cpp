//
//      Created by Stephens Nunnally on 10/13/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QPushButton>
#include <QVBoxLayout>

#include "form_about.h"
#include "helper.h"
#include "project/project.h"
#include "style/style.h"
#include "widgets/widgets_event_filters.h"

//####################################################################################
//##    Constructor
//####################################################################################
FormAbout::FormAbout(DrProject *project, QWidget *parent) : QWidget(parent) {

    // ***** Initialize member variables
    m_project = project;

    // ***** Set up initial window
    this->setAttribute(Qt::WidgetAttribute::WA_DeleteOnClose, true);
    this->setWindowFlags(Qt::WindowType::FramelessWindowHint | Qt::WindowType::Tool);
    this->setObjectName(QStringLiteral("childForm"));
    this->setWindowModality(Qt::WindowModality::ApplicationModal);
    this->setStyleSheet( Dr::CustomStyleSheetFormatting() );

    // ***** Builds FormAbout
    buildForm();

    // ***** Install dragging event filter
    this->installEventFilter(new DrFilterClickAndDragWindow(this));
}


//####################################################################################
//##    Keeps container widget same size as form
//####################################################################################
void FormAbout::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);

    Dr::ApplyRoundedCornerMask(this, 8, 8);

    // ***** Center window on screen
    if (m_finished_loading == false) {
        Dr::CenterFormOnScreen(parentWidget(), this);
        m_finished_loading = true;
    }
}


//####################################################################################
//##    Builds interior widget of FormAbout
//####################################################################################
void FormAbout::buildForm() {
    // ***** About Text
    QString about_txt = "<center>"
                        "<font size=+3>Drop Creator</font><br>"
                        "A Drag and Drop Game Maker<br><br>"
                        "by<br>"
                        "Stephens Nunnally<br>"
                        "Julie Machamer"
                        "</center>";

    // ***** Build Form
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(1, 1, 1, 1);

    m_inner_widget = new QWidget();
    m_inner_widget->setObjectName(QStringLiteral("innerWidget"));
    QHBoxLayout *inner_layout = new QHBoxLayout(m_inner_widget);

        QLabel *drop = new QLabel();
        drop->setPixmap(QPixmap(":/assets/icon/icon256.png").scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        inner_layout->addWidget(drop);

        QWidget *right_side = new QWidget();
        QVBoxLayout *right_layout = new QVBoxLayout(right_side);
        right_layout->setAlignment(Qt::AlignmentFlag::AlignCenter);

            QLabel *about = new QLabel(about_txt);
            about->setMinimumWidth(250);
            about->setMinimumHeight(150);
            about->setAttribute(Qt::WidgetAttribute::WA_TransparentForMouseEvents, true);
            right_layout->addWidget(about);

            QPushButton *exit = new QPushButton("  Okay  ");
            Dr::ApplyDropShadowByType(exit, Shadow_Types::Button_Shadow);
            exit->setObjectName(QStringLiteral("button"));
            right_layout->addWidget(exit);

            // Connect a lambda function to the "exit" button to close the form
            connect(exit, &QPushButton::clicked, [this] () {
                this->close();
            });
        inner_layout->addWidget(right_side);

    layout->addWidget(m_inner_widget);
}










