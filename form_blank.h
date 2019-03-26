//
//      Created by Stephens Nunnally on 3/25/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef FORM_BLANK_H
#define FORM_BLANK_H

#include <QWidget>

class DrProject;

class FormBlank : public QWidget
{
private:
    DrProject   *m_project;                         // Pointer to the open project

    QWidget     *inner_widget;                      // Container widget, allows for a double form border
    bool         m_shown_yet = false;               // True after shown for the first time

public:
    FormBlank(DrProject *project, QWidget *parent = nullptr);

    // Event Overrides
    virtual void resizeEvent(QResizeEvent *event) override;
    virtual void showEvent(QShowEvent *event) override;


};


#endif // FORM_BLANK_H
