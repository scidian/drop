//
//      Created by Stephens Nunnally on 10/13/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QDebug>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>

#include "editor/event_filters.h"
#include "editor/forms/form_about.h"
#include "editor/globals_editor.h"
#include "editor/style/style.h"
#include "library/colors/colors.h"
#include "model/project/project.h"


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
    std::string version = Dr::GetPreference(Preferences::Version_Major).toString() + "." +
                          Dr::GetPreference(Preferences::Version_Minor).toString() + "." +
                          Dr::GetPreference(Preferences::Version_Build).toString();

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

        QLabel *about_1 = new QLabel("<center>"
                                     "<font size=+1>A Drag and Drop Game Creator</font><br><br>"
                                     "by<br>"
                                     "<b><font size=+1>Stephens Nunnally &nbsp; · &nbsp; Julie Machamer</b></font><br>"
                                     "</center>");
        about_1->setAttribute(Qt::WidgetAttribute::WA_TransparentForMouseEvents, true);
        inner_layout->addWidget(about_1);

        ///QLabel *version_label = new QLabel("v " + version);
        QLabel *version_label = new QLabel("---------- Educational Version Only ----------<br>"
                                           "<b>NOT FOR REDISTRIBUTION</b><br>");
        version_label->setAlignment(Qt::AlignmentFlag::AlignCenter);
        version_label->setContentsMargins(0,0,0,0);
        version_label->setAttribute(Qt::WidgetAttribute::WA_TransparentForMouseEvents, true);
        inner_layout->addWidget(version_label);

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

        QTextEdit *license = new QTextEdit(licenseText());
        license->setReadOnly(true);
        license->setFixedHeight(100);
        license->setStyleSheet( QString::fromStdString(
                                " QTextEdit { background: " + Dr::GetColor(Window_Colors::Background_Dark).name() + "; "
                                "             color: " + Dr::GetColor(Window_Colors::Button_Light).name() + "; "
                                "             border-color: " + Dr::GetColor(Window_Colors::Midlight).name() + "; } "
                                " QScrollBar:vertical { width: 10px; margin: 0px; border-radius: 0px; "
                                "       background: " + Dr::GetColor(Window_Colors::Background_Dark).name() + "; } "
                                " QScrollBar::handle:vertical { margin: 1px; border-radius: 2px; "
                                "       background: " + Dr::GetColor(Window_Colors::Button_Light).name() + "; } "
                                " QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0px; } "
                                " QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical { height: 0px; } "
                                ));
        inner_layout->addWidget(license);

        QPushButton *exit = new QPushButton("  Okay  ");
        Dr::ApplyDropShadowByType(exit, Shadow_Types::Button_Shadow);
        exit->setObjectName(QStringLiteral("button"));
        //exit->setMaximumWidth(250);
        connect(exit, &QPushButton::clicked, [this] () { this->close(); });
        inner_layout->addWidget(exit);

    layout->addWidget(m_inner_widget);
}


//####################################################################################
//##    Builds interior widget of FormAbout
//####################################################################################
QString FormAbout::licenseText() {

    return  "This software dynamically links to unmodified code from the Qt framework licensed under the LPGLv3. Qt libraries and sources can be "
            "downloaded from https://www.qt.io/download/"
            "<br><br>"
            "The Qt framework is Copyright (C) 2019 The Qt Company. For more info visit https://www.qt.io/contact-us/"
            "<br><br>"

            "This software uses Boost Libraries distributed under the Boost Software License. "
            "Copyright (c) Beman Dawes, David Abrahams, 1998-2005. "
            "Copyright (c) Rene Rivera 2004-2007. "
            "<br><br>"

            "This software uses Chipmunk Physics Library written by Scott Lembcke and distributed under the MIT License. "
            "Copyright (c) 2007-2015 Scott Lembcke and Howling Moon Software."
            "<br><br>"

            "This software uses Delaunay Triangulation writted by Volodymyr Bilonenko and distributed under the MIT License. "
            "Copyright (c) 2018 Volodymyr Bilonenko."
            "<br><br>"

            "This software uses OpenGL Mathematics (glm) written by G-Truc Creation and distributed under the MIT License. "
            "Copyright (c) 2005-2014 by G-Truc Creation."
            "<br><br>"

            "This software uses Poly Partition written by Ivan Fratric and distributed under the MIT License. "
            "Copyright (c) 2011 by Ivan Fratric."
            "<br><br>"

            "This software uses Simplex Noise (3D) written by Iam McEwan and distributed under the MIT License. "
            "Copyright (c) 2011 by Ashima Arts."
            "<br><br>"

            "This software uses STB Single File Libraries written by Sean Barrett and distributed under the MIT License. "
            "Copyright (c) 2017 Sean Barrett."
            "<br><br>"

            "This software features the font, 'Aileron', designed by Sora Sagano and released under CC0 1.0. "
            "<br><br>"
            ;
}













