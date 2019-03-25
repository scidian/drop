//
//      Created by Stephens Nunnally on 3/24/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef FORM_ATLAS_H
#define FORM_ATLAS_H

#include <QWidget>

class DrProject;

class FormAtlas : public QWidget
{
private:
    DrProject   *m_project;                         // Pointer to the open project
    bool         m_shown_yet = false;               // True after shown for the first time

public:
    FormAtlas(DrProject *project, QWidget *parent = nullptr);

    // Event Overrides
    virtual void showEvent(QShowEvent *event) override;


};


#endif // FORM_ATLAS_H




