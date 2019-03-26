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
    QWidget     *inner_widget;                      // Container widget, allows for a double form border


public:
    FormAtlas(DrProject *project, QWidget *parent = nullptr);

    // Event Overrides
    virtual void resizeEvent(QResizeEvent *event) override;


};


#endif // FORM_ATLAS_H




