//
//      Created by Stephens Nunnally on 3/25/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QApplication>
#include <QPushButton>
#include <QGridLayout>
#include <QMouseEvent>
#include <QScreen>
#include <QStyle>

#include "form_popup.h"
#include "library.h"
#include "project.h"
#include "widgets_event_filters.h"

FormPopup::FormPopup(DrProject *project, QWidget *parent) : QWidget(parent)
{
    m_project = project;

    // ***** Set up initial window
    setWindowFlag(Qt::WindowType::FramelessWindowHint);
    setWindowFlag(Qt::WindowType::Popup);
    setMinimumSize(QSize(200, 200));
    setObjectName(QStringLiteral("childForm"));
    Dr::ApplyCustomStyleSheetFormatting(this);

    // Create a layout for the form and add a button
    inner_widget = new QWidget(this);
    inner_widget->setGeometry( 1, 1, this->geometry().width() - 2, this->geometry().height() - 2);
    inner_widget->setObjectName(QStringLiteral("innerWidget"));
    QGridLayout *grid_layout = new QGridLayout(inner_widget);
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
void FormPopup::resizeEvent(QResizeEvent *event)
{
    inner_widget->setGeometry( 1, 1, this->geometry().width() - 2, this->geometry().height() - 2);
    QWidget::resizeEvent(event);
}



//####################################################################################
//##        Upon first showing, shape window
//####################################################################################
void FormPopup::showEvent(QShowEvent *event)
{
    Q_UNUSED(event)

    if (!m_shown_yet) {
        Dr::ApplyRoundedCornerMask(this, 8, 8);
        m_shown_yet = true;
    }

    // Find new desired popup location relative to parent button
    QRect parent_rect = parentWidget()->geometry();
    int x = parentWidget()->mapToGlobal(parent_rect.center()).x() - this->geometry().width() / 2;
    int y = parentWidget()->mapToGlobal(parent_rect.bottomLeft()).y() + 2;

    // Make sure it is within the screen
    QRect screenGeometry = QGuiApplication::screens().first()->geometry();
    if (x < 5) x = 5;
    if (x + this->geometry().width() + 5 > screenGeometry.width()) x = screenGeometry.width() - this->geometry().width() - 5;
    this->setGeometry(x, y, this->width(), this->height());

    QWidget::showEvent(event);
}























