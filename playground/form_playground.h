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
#include <QLabel>
#include <QPushButton>
#include <QSizeGrip>
#include <QTimer>
#include <QWidget>
#include <chrono>

#include "playground/playground_toy.h"

// Forward declarations
class DrPlayground;
class DrPlaygroundView;
class DrProject;

// QVariant Declaration for storing DrToy pointers with QWidget setData functions
Q_DECLARE_METATYPE(DrToy*)

// Type Definitions
typedef std::chrono::high_resolution_clock Clock;


//####################################################################################
//##    FormPlayground
//##        Form for playing with physics
//############################
class FormPlayground : public QWidget
{
    Q_OBJECT

private:
    // Widgets
    QWidget             *m_inner_widget;                    // Container widget, allows for a double form border
    QSizeGrip           *m_grip, *m_grip2;

    QWidget             *m_side_bar;
    QPushButton         *m_start_timers;
    QPushButton         *m_stop_timers;
    QPushButton         *m_reset_world;
    QLabel              *m_world_info;
    QLabel              *m_object_info;

    QWidget             *m_main_area;
    DrPlaygroundView    *m_play_view;
    QGraphicsScene      *m_play_scene;

    // Local Variables
    DrPlayground        *m_playground;                      // Class that holds the Playground Physics Space

    // Timer Variables
    QTimer              *m_update_timer;
    Clock::time_point    m_time_update;                     // Checks time between SpaceStep calls
    bool                 m_updating = false;

public:
    FormPlayground(QWidget *parent = nullptr);

    // Event Overrides
    virtual void resizeEvent(QResizeEvent *event) override;

    // Function Calls
    QGraphicsLineItem*      addGraphicsLine(DrToy *toy, QColor color);
    QGraphicsEllipseItem*   addGraphicsCircle(DrToy *toy, QColor color);
    QGraphicsRectItem*      addGraphicsBox(DrToy *toy, QColor color);
    void                    buildForm();
    void                    updateGraphicsView();

    // Getters / Setters
    QGraphicsScene*         getScene() { return m_play_scene; }
    bool                    isUpdating() { return m_updating; }

    void                    setObjectInfo(QString new_info) { m_object_info->setText(new_info); }
    void                    setWorldInfo(QString new_info) { m_world_info->setText(new_info); }


public slots:
    void        updateEngine();

    void        selectionChanged();
    void        startTimers();
    void        stopTimers();
    void        resetWorld();
};


//####################################################################################
//##    DrPlaygroundView
//##        A sub classed QGraphicsView to show our Playground
//############################
class DrPlaygroundView : public QGraphicsView
{
    Q_OBJECT

private:
    // External Borrowed Objects
     DrPlayground        *m_playground;                     // Class that holds the Playground Physics Space

    // Display Variables
    int             m_zoom = 200;                           // Zoom level of current view, 200 is 50% - 250 is 100%
    double          m_zoom_scale = 1;                       // Updated in zoomInOut for use during painting grid, DO NOT SET MANUALLY

    cpConstraint   *mouse_joint = nullptr;                  // Joint to use to attach to mouse

public:
    // Constructor / Destructor
    DrPlaygroundView(QGraphicsScene *scene, DrPlayground *playground) : QGraphicsView(scene), m_playground(playground) { }

    // Event Overrides
    void            mouseMoveEvent(QMouseEvent *event) override;
    void            mousePressEvent(QMouseEvent *event) override;
    void            mouseReleaseEvent(QMouseEvent *event) override;
#if QT_CONFIG(wheelevent)
    virtual void    wheelEvent(QWheelEvent *event) override;                                // Inherited from QWidget
#endif

    // Functions
    void            zoomInOut(int level);

    // Getters / Setters
    void            setZoomLevel(int level) { m_zoom = level; }
};


//####################################################################################
//##    DrPlaygroundLine
//##        A sub classed QGraphicsLineItem for custom drawing
//############################
class DrPlaygroundLine : public QGraphicsLineItem
{
public:
    // Constructor / Destructor
    DrPlaygroundLine() {}

    // Event Overrides
    virtual QVariant        itemChange(GraphicsItemChange change, const QVariant &value) override;
    virtual void            paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
    virtual QPainterPath    shape() const override;
};


//####################################################################################
//##    DrPlaygroundCircle
//##        A sub classed QGraphicsEllipseItem for custom drawing
//############################
class DrPlaygroundCircle : public QGraphicsEllipseItem
{
public:
    // Constructor / Destructor
    DrPlaygroundCircle() {}

    // Event Overrides
    virtual QVariant    itemChange(GraphicsItemChange change, const QVariant &value) override;
    virtual void        paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
};


//####################################################################################
//##    DrPlaygroundBox
//##        A sub classed QGraphicsRectItem for custom drawing
//############################
class DrPlaygroundBox : public QGraphicsRectItem
{
public:
    // Constructor / Destructor
    DrPlaygroundBox() {}

    // Event Overrides
    virtual QVariant    itemChange(GraphicsItemChange change, const QVariant &value) override;
    virtual void        paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
};


#endif // FORM_PLAYGROUND_H

















