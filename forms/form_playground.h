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

#include "playground/playground_toy.h"

// Forward declarations
class DrPlayground;
class DrPlaygroundView;
class DrProject;

// QVariant Declaration for storing DrToy pointers with QWidget setData functions
Q_DECLARE_METATYPE(DrToy*)

//####################################################################################
//##    FormPlayground
//##        Form for playing with physics
//############################
class FormPlayground : public QWidget
{
private:
    // External Borrowed Objects
    DrProject   *m_project;                             // Pointer to the open project

    // Widgets
    QWidget             *m_inner_widget;                // Container widget, allows for a double form border
    QSizeGrip           *m_grip, *m_grip2;
    QWidget             *m_side_bar;
    QWidget             *m_main_area;
    DrPlaygroundView    *m_play_view;
    QGraphicsScene      *m_play_scene;

    // Local Variables
    DrPlayground        *m_playground;                  // Class that holds the Playground Physics Space

public:
    FormPlayground(QWidget *parent = nullptr);

    // Event Overrides
    virtual void resizeEvent(QResizeEvent *event) override;

    // Function Calls
    QGraphicsLineItem*  addGraphicsLine(DrToy *toy, QColor color);
    void                buildForm();

    // Getters / Setters
    QGraphicsScene*     getScene() { return m_play_scene; }

};


//####################################################################################
//##    DrPlaygroundView
//##        A sub classed QGraphicsView to show our Playground
//############################
class DrPlaygroundView : public QGraphicsView
{
    Q_OBJECT

private:
    // Display Variables
    int          m_zoom = 200;                                      // Zoom level of current view, 200 is 50% - 250 is 100%
    double       m_zoom_scale = 1;                                  // Updated in zoomInOut for use during painting grid, DO NOT SET MANUALLY

public:
    // Constructor / Destructor
    DrPlaygroundView(QGraphicsScene *scene) : QGraphicsView(scene) { }

    // Event Overrides
#if QT_CONFIG(wheelevent)
    virtual void    wheelEvent(QWheelEvent *event) override;                                // Inherited from QWidget
#endif

    // Functions
    void            zoomInOut(int level);

    // Getters / Setters
    void            setZoomLevel(int level) { m_zoom = level; }
};


#endif // FORM_PLAYGROUND_H

















