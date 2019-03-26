//
//      Created by Stephens Nunnally on 3/25/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef FORM_POPUP_H
#define FORM_POPUP_H

#include <QMenu>
#include <QWidget>

class DrProject;

class FormPopup : public QWidget
{
private:
    DrProject   *m_project;                         // Pointer to the open project
    QWidget     *inner_widget;                      // Container widget, allows for a double form border


public:
    FormPopup(DrProject *project, QWidget *parent = nullptr);

    // Event Overrides
    virtual void paintEvent(QPaintEvent *event) override;
    virtual void resizeEvent(QResizeEvent *event) override;
    virtual void showEvent(QShowEvent *event) override;


};

#endif // FORM_POPUP_H
