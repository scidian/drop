//
//      Created by Stephens Nunnally on 3/25/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef FORM_POPUP_H
#define FORM_POPUP_H

#include <QVBoxLayout>
#include <QWidget>

class DrProject;

class FormPopup : public QWidget
{
private:
    DrProject   *m_project;                         // Pointer to the open project

    QWidget     *m_mapper;                          // Widget to use for mapping to global coordinates
    QWidget     *m_inner_widget;                    // Container widget, allows for a double form border

    QPoint       m_offset { 0, 5};                  // Adjustment to popup location from parent

public:
    FormPopup(DrProject *project, QWidget *mapper, QWidget *parent);

    // Event Overrides
    virtual void paintEvent(QPaintEvent *event) override;
    virtual void resizeEvent(QResizeEvent *event) override;
    virtual void showEvent(QShowEvent *event) override;

    // Getters and Setters
    QWidget*     getWidget() { return m_inner_widget; }

    QPoint       getOffset() { return m_offset; }
    void         setOffset(QPoint offset) { m_offset = offset; }

};

#endif // FORM_POPUP_H
