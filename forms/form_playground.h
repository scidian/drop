//
//      Created by Stephens Nunnally on 3/24/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef FORM_PLAYGROUND_H
#define FORM_PLAYGROUND_H

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QSizeGrip>
#include <QWidget>

// Forward declarations
class DrProject;

//####################################################################################
//##    FormPlayground
//##        Form for playing with physics
//############################
class FormPlayground : public QWidget
{
private:
    // External Borrowed Objects
    DrProject   *m_project;                             // Pointer to the open project

    // Local Variables
    QWidget         *m_inner_widget;                    // Container widget, allows for a double form border
    QSizeGrip       *m_grip, *m_grip2;

    QWidget         *m_playground;
    QWidget         *m_side_bar;

    QGraphicsView   *m_play_view;
    QGraphicsScene  *m_play_scene;



public:
    FormPlayground(DrProject *project, QWidget *parent = nullptr);

    // Event Overrides
    virtual void resizeEvent(QResizeEvent *event) override;


};


#endif // FORM_PLAYGROUND_H




