//
//      Created by Stephens Nunnally on 3/21/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef FORM_FONT_EDITOR_H
#define FORM_FONT_EDITOR_H

#include <QWidget>

class DrProject;

class FormFontEditor : public QWidget
{
private:
    DrProject   *m_project;                         // Pointer to the open project
    bool         m_shown_yet = false;               // True after shown for the first time

    QPoint       press_pos;
    bool         is_moving = false;                 // Flag makes sure that the drag and drop event originated from within the form

public:
    FormFontEditor(DrProject *project, QWidget *parent = nullptr);

    // Event Overrides
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void showEvent(QShowEvent *event) override;


};



#endif // FORM_FONT_EDITOR_H



