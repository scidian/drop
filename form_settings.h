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
    bool         m_shown_yet = false;               // True after shown for the first time

    QPoint       press_pos;
    bool         is_moving = false;                 // Flag makes sure that the drag and drop event originated from within the form

public:
    FormSettings(DrProject *project, QWidget *parent = nullptr);

    // Event Overrides
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void showEvent(QShowEvent *event) override;


};


#endif // FORM_SETTINGS_H
