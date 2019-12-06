//
//      Created by Stephens Nunnally on 10/22/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QApplication>
#include <QCloseEvent>
#include <QDateTime>
#include <QPushButton>
#include <QVBoxLayout>

#include "editor/event_filters.h"
#include "editor/forms/form_expire.h"
#include "editor/style/style.h"


//####################################################################################
//##    Constructor
//####################################################################################
FormExpire::FormExpire(QWidget *parent) : QMainWindow(parent) {

    // ***** Set up initial window
    this->setStyleSheet( Dr::CustomStyleSheetFormatting() );
    this->setWindowFlags(Qt::WindowType::FramelessWindowHint | Qt::WindowType::Tool);
    this->setWindowModality(Qt::NonModal);
    this->setMinimumSize(QSize(400, 200));
    this->setMaximumWidth(500);
    this->setObjectName(QStringLiteral("childForm"));
    this->setMouseTracking(true);
    this->setAcceptDrops(true);
    this->setWindowIcon(QIcon(":/assets/icon/icon256.png"));

    // ***** Create a contianer widget, this will allow Create a layout for the form and add a button
    QWidget *widget_central = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(widget_central);
    layout->setContentsMargins(1, 1, 1, 1);

    m_inner_widget = new QWidget();
    m_inner_widget->setObjectName(QStringLiteral("innerWidget"));
    QVBoxLayout *inner_layout = new QVBoxLayout(m_inner_widget);

        QLabel *drop = new QLabel();
        drop->setFixedHeight(160);
        drop->setAlignment(Qt::AlignmentFlag::AlignCenter);
        drop->setPixmap(QPixmap(":/assets/icon/expired.png"));///.scaled(256, 256, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        drop->setStyleSheet( " padding-left: 14px; ");
        drop->setAttribute(Qt::WidgetAttribute::WA_TransparentForMouseEvents, true);
        inner_layout->addWidget(drop);

        QLabel *about_1 = new QLabel("<center>"
                                     "<font size=+1>A Drag and Drop Game Creator</font><br><br>"
                                     "by<br>"
                                     "<b><font size=+1>Stephens Nunnally &nbsp; · &nbsp; Julie Machamer</b></font><br>"
                                     "</center>");
        about_1->setAttribute(Qt::WidgetAttribute::WA_TransparentForMouseEvents, true);
        inner_layout->addWidget(about_1);


        QLabel *date_time = new QLabel();
        date_time->setAlignment(Qt::AlignCenter);
        date_time->setAttribute(Qt::WidgetAttribute::WA_TransparentForMouseEvents, true);
        date_time->setText("<font size=+1>DEMO VERSION EXPIRED</font><br><br>"
                           "Thanks for your continued interest, <br>"
                           "For updates on development and future release please visit");
        inner_layout->addWidget(date_time);

        QLabel *website = new QLabel();
        website->setAlignment(Qt::AlignmentFlag::AlignCenter);
        website->setText("<a href=\"http://scidian.com/\" style=\"color: #00B4AF\">www.scidian.com</a>");
        website->setTextFormat(Qt::RichText);
        website->setTextInteractionFlags(Qt::TextBrowserInteraction);
        website->setOpenExternalLinks(true);
        website->setFocusPolicy(Qt::NoFocus);
        inner_layout->addWidget(website);


        QLabel *scidian = new QLabel();
        scidian->setAlignment(Qt::AlignmentFlag::AlignCenter);
        ///scidian->setPixmap(QPixmap(":/assets/icon/scidian_logo.png").scaled(60, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        scidian->setText("<a href=\"http://scidian.com/\"><img src=\":/assets/icon/scidian_logo_small.png\"></a>");
        scidian->setTextFormat(Qt::RichText);
        scidian->setTextInteractionFlags(Qt::TextBrowserInteraction);
        scidian->setOpenExternalLinks(true);
        scidian->setFocusPolicy(Qt::NoFocus);
        inner_layout->addWidget(scidian);

        QLabel *about_2 = new QLabel("<center><font size=-1>"
                                     "Copyright © 2018-2019 Scidian™ Software<br>"
                                     "Drop™, Scidian™ and the Scidian Blob™ are trademarks of Scidian™ Software<br>"
                                     "All Rights Reserved"
                                     "</font></center>");
        about_2->setAttribute(Qt::WidgetAttribute::WA_TransparentForMouseEvents, true);
        inner_layout->addWidget(about_2);

        QPushButton *exit = new QPushButton("  Exit  ");
        Dr::ApplyDropShadowByType(exit, Shadow_Types::Button_Shadow);
        exit->setObjectName(QStringLiteral("button"));
        inner_layout->addWidget(exit);

        // Connect a lambda function to the "exit" button to close the form
        connect(exit, &QPushButton::clicked, [this] () {
            this->close();
        });

    layout->addWidget(m_inner_widget);
    this->setCentralWidget(widget_central);

    // ***** Center Form and Install dragging event filter
    this->installEventFilter(new DrFilterClickAndDragWindow(this));
}


//####################################################################################
//##    Keeps container widget same size as form
//####################################################################################
void FormExpire::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);

    if (m_just_loaded == true) {
        Dr::CenterFormOnScreen(this, this);
        m_just_loaded = false;
    }
    Dr::ApplyRoundedCornerMask(this, 8, 8);
}


//####################################################################################
//##    Overrides close event to make sure all rogue windows are closed
//####################################################################################
void FormExpire::closeEvent(QCloseEvent *event) {
    // Accept close event
    event->accept();

    // Close everything and terminate app
    qApp->closeAllWindows();
    qApp->quit();
}













