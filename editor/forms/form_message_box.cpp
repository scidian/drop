//
//      Created by Stephens Nunnally on 2/20/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QApplication>
#include <QDebug>
#include <QPushButton>
#include <QVBoxLayout>

#include "core/colors/colors.h"
#include "core/dr_random.h"
#include "editor/event_filters/event_filters.h"
#include "editor/forms/form_message_box.h"
#include "editor/helper_library.h"
#include "editor/style/style.h"
#include "project/dr_project.h"


//####################################################################################
//##    Constructor
//####################################################################################
FormMessageBox::FormMessageBox(std::string message, QPixmap pixmap, QWidget *parent, QMessageBox::StandardButtons buttons)
    : QWidget(parent), m_parent(parent) {

    // ***** Set up initial window
    setAttribute(Qt::WidgetAttribute::WA_DeleteOnClose, true);
    setWindowFlags(Qt::WindowType::FramelessWindowHint | Qt::WindowType::Dialog | Qt::WindowStaysOnTopHint);
    setWindowModality(Qt::WindowModality::ApplicationModal);
    setMinimumSize(QSize(300, 130));
    setObjectName(QStringLiteral("childForm"));
    this->setStyleSheet( Dr::CustomStyleSheetFormatting() );


    // ***** Create a contianer widget, this will allow us to create a layout for the form and add buttons and stuff
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(1, 1, 1, 1);

    m_inner_widget = new QWidget();
    m_inner_widget->setObjectName(QStringLiteral("innerWidget"));
    QHBoxLayout *inner_layout = new QHBoxLayout(m_inner_widget);
    inner_layout->setSpacing(10);
    inner_layout->setContentsMargins(15, 10, 20, 15);

        QSizePolicy sp_left( QSizePolicy::Preferred, QSizePolicy::Preferred);
        QSizePolicy sp_right(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sp_left.setHorizontalStretch( 1);   sp_left.setVerticalStretch( 0);
        sp_right.setHorizontalStretch(3);   sp_right.setVerticalStretch(0);

        // Message Box Picture
        m_pixmap = new QLabel();
        m_pixmap->setSizePolicy(sp_left);
        m_pixmap->setAlignment(Qt::AlignCenter);
        m_pixmap->setPixmap(pixmap);
        inner_layout->addWidget(m_pixmap);

        // Right Side of Message Box
        QWidget *right_side = new QWidget();
        right_side->setSizePolicy(sp_right);
        QVBoxLayout *right_layout = new QVBoxLayout(right_side);
        right_layout->setSpacing(10);
        right_layout->setContentsMargins(0, 0, 0, 0);

            // Main Message Box Text Message
            QLabel *box_text = new QLabel(QString::fromStdString(message));
            box_text->setFont( Dr::CustomFont() );
            box_text->setMaximumWidth(400);
            box_text->setMinimumWidth(250);
            box_text->setWordWrap(true);
            right_layout->addWidget(box_text);

            // Buttons of Message Box
            QWidget *button_holder = new QWidget();
            QHBoxLayout *button_layout = new QHBoxLayout(button_holder);
            button_layout->setSpacing(4);
            button_layout->setContentsMargins(2, 0, 2, 0);
            button_layout->addStretch(0);
                if (buttons & QMessageBox::StandardButton::Cancel) {    button_layout->addWidget(addButton(tr("Cancel"),    QMessageBox::Cancel)); }
                if (buttons & QMessageBox::StandardButton::No) {        button_layout->addWidget(addButton(tr("No"),        QMessageBox::No) ); }
                if (buttons & QMessageBox::StandardButton::Ok) {        button_layout->addWidget(addButton(tr("Okay"),      QMessageBox::Ok)); }
                if (buttons & QMessageBox::StandardButton::Apply) {     button_layout->addWidget(addButton(tr("Apply"),     QMessageBox::Apply)); }
                if (buttons & QMessageBox::StandardButton::Yes) {       button_layout->addWidget(addButton(tr("Yes"),       QMessageBox::Yes)); }
                if (buttons & QMessageBox::StandardButton::YesAll) {    button_layout->addWidget(addButton(tr("Yes All"),   QMessageBox::YesAll)); }

                // Center a singular button
                if (m_button_list.size() == 1) {
                    button_layout->addStretch(0);
                }

            right_layout->addWidget(button_holder);
        inner_layout->addWidget(right_side);
    layout->addWidget(m_inner_widget);

    // Set Button tab focus in reverse order
    for (int i = static_cast<int>(m_button_list.size()) - 1; i >= 0; --i) {
        m_button_list[static_cast<size_t>(i)]->setAutoDefault(true);
    }

}

FormMessageBox::~FormMessageBox() { }


//####################################################################################
//##    QWidget Events
//####################################################################################
// Keeps container widget same size as form
void FormMessageBox::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    Dr::ApplyRoundedCornerMask(this, 8, 8);
}

// Center window on Parent Form and install dragging event filter
void FormMessageBox::showEvent(QShowEvent *event) {
    QWidget::showEvent(event);
    if (m_parent == nullptr) {
        Dr::CenterFormOnScreen(m_parent->window(), this);
    } else {
        Dr::CenterFormOnParent(m_parent->window(), this);
    }
    this->installEventFilter(new DrFilterClickAndDragWindow(this));
}


//####################################################################################
//##    Form Building
//####################################################################################
QPushButton* FormMessageBox::addButton(QString text, QMessageBox::StandardButton button) {
    QPushButton *push = new QPushButton(" " + text + " ");
    push->setObjectName(QStringLiteral("buttonDefault"));
    push->setFont( Dr::CustomFont() );
    push->setFocusPolicy(Qt::StrongFocus);
    Dr::ApplyDropShadowByType(push, Shadow_Types::Button_Shadow);

    connect(push, &QPushButton::clicked, [this, button] () {
        m_button = button;
        this->close();
    });

    m_button_list.push_back(push);
    return push;
}


//####################################################################################
//##    Form Actions
//####################################################################################
QMessageBox::StandardButton FormMessageBox::execute() {
    // Show Form, Make sure we have Focus
    this->show();
    this->raise();
    this->activateWindow();
    this->setWindowState(Qt::WindowActive);
    this->setFocus(Qt::ActiveWindowFocusReason);

    // Set Focus on first PushButton
    if (m_button_list.size() > 0) {
        m_button_list[0]->setFocus(Qt::FocusReason::ActiveWindowFocusReason);
    }

    // Loop until button pressed
    while (m_button == QMessageBox::NoButton) {
        qApp->processEvents();
    }
    return m_button;
}


















