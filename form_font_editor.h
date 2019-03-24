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

public:
    FormFontEditor(DrProject *project, QWidget *parent = nullptr);

    // Event Overrides
    virtual void showEvent(QShowEvent *event) override;


};



#endif // FORM_FONT_EDITOR_H



