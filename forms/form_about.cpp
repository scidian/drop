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
    QString about_txt_1 =   "<center>"
                            "<font size=+1>A Drag and Drop Game Creator</font><br><br>"
                            "by<br><br>"
                            "<b><font size=+1>Stephens Nunnally &nbsp; · &nbsp; Julie Machamer</b></font><br>"
                            "</center>";
    QString about_txt_2 =   "<center><font size=-1>"
                            "Copyright © 2018-2019 Scidian™ Software<br>"
                            "Drop™, Scidian™ and the Scidian Blob™ are trademarks of Scidian™ Software<br>"
                            "All Rights Reserved"
                            "</font></center>";
    // ***** Build Form
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(1, 1, 1, 1);

    m_inner_widget = new QWidget();
    m_inner_widget->setObjectName(QStringLiteral("innerWidget"));
    m_inner_widget->setMinimumWidth(400);
    QVBoxLayout *inner_layout = new QVBoxLayout(m_inner_widget);
    inner_layout->setAlignment(Qt::AlignmentFlag::AlignCenter);

        QLabel *drop = new QLabel();
        drop->setAlignment(Qt::AlignmentFlag::AlignCenter);
        drop->setPixmap(QPixmap(":/assets/icon/drop_logo.png"));///.scaled(256, 256, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        drop->setStyleSheet( " padding-left: 14px; ");
        drop->setAttribute(Qt::WidgetAttribute::WA_TransparentForMouseEvents, true);
        inner_layout->addWidget(drop);

        QLabel *about_1 = new QLabel(about_txt_1);
        about_1->setAttribute(Qt::WidgetAttribute::WA_TransparentForMouseEvents, true);
        inner_layout->addWidget(about_1);

        QLabel *scidian = new QLabel();
        scidian->setAlignment(Qt::AlignmentFlag::AlignCenter);
        ///scidian->setPixmap(QPixmap(":/assets/icon/scidian_logo.png").scaled(60, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        scidian->setText("<a href=\"http://scidian.com/\"><img src=\":/assets/icon/scidian_logo_small.png\"></a>");
        scidian->setTextFormat(Qt::RichText);
        scidian->setTextInteractionFlags(Qt::TextBrowserInteraction);
        scidian->setOpenExternalLinks(true);
        scidian->setFocusPolicy(Qt::NoFocus);
        inner_layout->addWidget(scidian);

        QLabel *about_2 = new QLabel(about_txt_2);
        about_2->setAttribute(Qt::WidgetAttribute::WA_TransparentForMouseEvents, true);
        inner_layout->addWidget(about_2);

        QPushButton *exit = new QPushButton("  Okay  ");
        Dr::ApplyDropShadowByType(exit, Shadow_Types::Button_Shadow);
        exit->setObjectName(QStringLiteral("button"));
        //exit->setMaximumWidth(250);
        connect(exit, &QPushButton::clicked, [this] () { this->close(); });
        inner_layout->addWidget(exit);

    layout->addWidget(m_inner_widget);
}










