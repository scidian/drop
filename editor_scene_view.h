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

// Interactive mouse modes
enum class View_Mode {
    None,
    Selecting,
    Resizing,
    Rotating,
    Translating,
};

// Possible handle rects, order is vector / array index critical
enum class Position_Flags {
    Top          = 0,
    Bottom       = 1,
    Left         = 2,
    Top_Left     = 3,
    Bottom_Left  = 4,
    Right        = 5,
    Top_Right    = 6,
    Bottom_Right = 7,
    Total,
    No_Position,
};

enum class X_Axis {  Left,   Right,    None  };
enum class Y_Axis {  Top,    Bottom,   None  };

enum class Grid_Style {  Lines, Dots,   };


class SceneGraphicsView : public QGraphicsView
{
    Q_OBJECT

private:
    // Local member variables
    DrProject              *m_project;                              // Pointer to currently loaded project
    InterfaceRelay         *m_interface;                            // Pointer to interface class of parent form
    QGraphicsScene         *m_scene;                                // Pointer to current scene, updated during paintEvent
    View_Mode               m_view_mode = View_Mode::None;          // Tracks current view interaction mode

    int          m_zoom = 250;
    int          m_rotate = 0;

    Grid_Style   m_grid_style = Grid_Style::Dots;
    double       m_grid_x = 10;
    double       m_grid_y = 10;

    bool         m_flag_key_down_spacebar = false;
    bool         m_flag_key_down_control = false;
    bool         m_flag_key_down_alt = false;

    QMutex                  mouse_move_mutex { QMutex::NonRecursive };              // Used to keep mouse move from backing up
    QPoint                  m_origin;                                               // Stores mouse down position in view coordinates
    QPointF                 m_origin_in_scene;                                      // Stores mouse down position in scene coordinates
    QGraphicsItem          *m_origin_item;                                          // Stores top item under mouse (if any) on mouse down event

    // Selection Bounding Box Variables
    QVector<QRectF>         m_handles;                                              // Stores QRects of current selection box handles
    Position_Flags          m_over_handle;                                          // Tracks if mouse is over a handle
    QPoint                  m_last_mouse_pos;                                       // Tracks last known mouse position in view coordinates

    // View_Mode::Selecting Variables
    SceneViewRubberBand    *m_rubber_band;                                          // Holds our view's RubberBand object
    QList<QGraphicsItem *>  m_items_start;                                          // Stores items selected at start of new rubber band box

    // View_Mode::Resizing Variables
    QRectF                  m_selection_rect;                                       // Stores rect of current selection
    QRectF                  m_start_resize_rect;                                    // Stores starting rect of selection before resize starts
    Position_Flags          m_start_resize_grip;                                    // Stores which Size Grip Handle we started resize over

    // View_Mode::Rotating Variables
    QMutex                  rotate_mutex { QMutex::NonRecursive };                  // Used to keep rotating from backing up
    QPointF                 m_selection_center;                                     // Stores center point of selection
    double                  m_last_angle_diff;                                      // Stores angle difference last time we checked

public:
    // Constructor
    explicit SceneGraphicsView(QWidget *parent, DrProject *project, InterfaceRelay *interface);
    virtual ~SceneGraphicsView() override;

    // Event Overrides, start at Qt Docs for QGraphicsView Class to find more
    virtual void paintEvent(QPaintEvent *event) override;                               // Inherited from QWidget
    virtual void drawBackground(QPainter *painter, const QRectF &rect) override;        // Inherited from QGraphicsView

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
    void updateSelectionRect();
    void zoomInOut(int level);

public slots:
    double  calcRotationAngleInDegrees(QPointF centerPt, QPointF targetPt);
    void    sceneChanged(QList<QRectF> region);
    void    selectionChanged();

    void    startSelect(QMouseEvent *event);
    void    startResize();
    void    startRotate();

    void    processSelection(QPoint mouse_in_view);
    void    resizeSelection(QPointF mouse_in_scene);
    void    rotateSelection(QPointF mouse_in_view);
};



class SceneViewRubberBand : public QRubberBand
{
private:
    InterfaceRelay         *m_interface;                // Pointer to interface class of parent form, for getting colors

public:
    // Constructor
    SceneViewRubberBand(Shape shape, QWidget *parent, InterfaceRelay *interface) :
                        QRubberBand (shape, parent), m_interface(interface) { }

    // Event overrides
    virtual void paintEvent(QPaintEvent *) override;
};



#endif // EDITOR_VIEW_SCENE_H























