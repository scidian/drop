//
//      Created by Stephens Nunnally on 3/21/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef FORM_SETTINGS_H
#define FORM_SETTINGS_H

#include <QWidget>

class DrProject;

class FormSettings : public QWidget
{
private:
    DrProject   *m_project;                         // Pointer to the open project
    QWidget     *m_inner_widget;                    // Container widget, allows for a double form border

public:
    FormSettings(DrProject *project, QWidget *parent = nullptr);

    // Event Overrides
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void resizeEvent(QResizeEvent *event) override;

};


#endif // FORM_SETTINGS_H
