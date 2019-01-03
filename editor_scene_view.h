//
//      Created by Stephens Nunnally on 1/3/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      A sub classed QGraphicsView so we can override events for our View Area
//
//
#ifndef EDITOR_VIEW_SCENE_H
#define EDITOR_VIEW_SCENE_H

#include <QtWidgets>

class DrProject;
class InterfaceRelay;
class SceneViewRubberBand;

class SceneGraphicsView : public QGraphicsView
{
    Q_OBJECT

private:
    // Local member variables
    DrProject              *m_project;                  // Pointer to currently loaded project
    InterfaceRelay         *m_interface;                // Pointer to interface class of parent form

    int          m_zoom = 250;
    int          m_rotate = 0;
    bool         m_flag_key_down_spacebar = false;
    bool         m_flag_key_down_control = false;
    bool         m_flag_key_down_alt = false;

    QMutex                  selection_mutex { QMutex::NonRecursive };             // Used to keep mouse move from backing up
    QPoint                  m_origin;
    QPointF                 m_origin_in_scene;
    QGraphicsItem          *m_origin_item;
    SceneViewRubberBand    *m_rubber_band;
    QList<QGraphicsItem *>  m_items_start;
    bool                    m_is_selecting = false;
    bool                    m_is_resizing = false;
    QRectF                  m_selection_rect;


public:
    // Constructor
    explicit SceneGraphicsView(QWidget *parent, DrProject *project, InterfaceRelay *interface);

    // Event Overrides, start at Qt Docs for QGraphicsView Class to find more
    virtual void paintEvent(QPaintEvent *event) override;                               // Inherited from QWidget
    virtual void enterEvent(QEvent *event) override;                                    // Inherited from QWidget
    virtual void keyPressEvent(QKeyEvent *event) override;                              // Inherited from QWidget
    virtual void keyReleaseEvent(QKeyEvent *event) override;                            // Inherited from QWidget
    virtual void mouseMoveEvent(QMouseEvent *event) override;                           // Inherited from QWidget
    virtual void mousePressEvent(QMouseEvent *event) override;                          // Inherited from QWidget
    virtual void mouseReleaseEvent(QMouseEvent *event) override;                        // Inherited from QWidget
#if QT_CONFIG(wheelevent)
    virtual void wheelEvent(QWheelEvent *event) override;                               // Inherited from QWidget
#endif

    // Custom handling functions
    void applyUpdatedMatrix();
    void zoomInOut(int level);

};

class SceneViewRubberBand : public QRubberBand
{
private:
    InterfaceRelay         *m_interface;                // Pointer to interface class of parent form, for getting colors
public:
    SceneViewRubberBand(Shape shape, QWidget *parent, InterfaceRelay *interface) :
                        QRubberBand (shape, parent), m_interface(interface) { }
    virtual void paintEvent(QPaintEvent *) override;
};



#endif // EDITOR_VIEW_SCENE_H

















