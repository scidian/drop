//
//      Created by Stephens Nunnally on 3/21/2019, (c) 2019 Scidian Software, All Rights Reserved
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

#include "form_font_editor.h"
#include "library.h"
#include "project.h"

FormFontEditor::FormFontEditor(DrProject *project, QWidget *parent) : QWidget(parent)
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
//##        Mouse Overrides allow for moving form by clicking anywhere in the background
//####################################################################################
// Save the press position (this is relative to the current widget)
void FormFontEditor::mousePressEvent(QMouseEvent* event) {
    press_pos = event->pos();
    is_moving = true;

    QWidget::mousePressEvent(event);
}

// Calculate difference between the press position and the new Mouse position, relative to the current widget
void FormFontEditor::mouseMoveEvent(QMouseEvent* event) {
    if (is_moving) {
        QPoint diff = event->pos() - press_pos;
        window()->move( window()->pos() + diff );
    }
    QWidget::mouseMoveEvent(event);
}
void FormFontEditor::mouseReleaseEvent(QMouseEvent* event) {
    is_moving = false;
    QWidget::mouseReleaseEvent(event);
}



//####################################################################################
//##        Upon first showing creates some rounded corners
//####################################################################################
void FormFontEditor::showEvent(QShowEvent *event)
{
    Q_UNUSED(event)

    if (!m_shown_yet) {
        Dr::ApplyRoundedCornerMask(this, 8, 8);
        m_shown_yet = true;
    }
    QWidget::showEvent(event);
}





