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

#include <enums.h>

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

enum class X_Axis {     Left,   Right,    None  };
enum class Y_Axis {     Top,    Bottom,   None  };
enum class Grid_Style { Lines,  Dots,           };

// Class constants
const double ANGLE_TOLERANCE = .5;                              // Angle distance to fuzzy compare to desired angle


class SceneGraphicsView : public QGraphicsView
{
    Q_OBJECT

private:
    // Local, instance specific member variables
    DrProject              *m_project;                              // Pointer to currently loaded project
    InterfaceRelay         *m_interface;                            // Pointer to interface class of parent form
    QGraphicsScene         *m_scene;                                // Pointer to current scene, updated during paintEvent
    View_Mode               m_view_mode = View_Mode::None;          // Tracks current view interaction mode

    // Display Variables
    int          m_zoom = 250;                                      // Zoom level of current view
    double       m_zoom_scale = 1;                                  // Updated in applyUpdatedMatrix for use during painting grid, DO NOT SET MANUALLY
    int          m_rotate = 0;                              // NOT IMPLEMENTED: Rotation of current view

    // Grid variables
    Grid_Style   m_grid_style = Grid_Style::Lines;                  // Grid type to display
    double       m_grid_x = 10;                                     // Grid size left to right
    double       m_grid_y = 10;                                     // Grid size top to bottom
    double       m_grid_rotate = 0;                         // NOT IMPLEMENTED: Rotation of grid lines

    // Keyboard flags
    bool         m_flag_key_down_spacebar = false;                  // True when View has focus and spacebar      is down
    bool         m_flag_key_down_control =  false;                  // True when View has focus and control (cmd) is down
    bool         m_flag_key_down_alt =      false;                  // True when View has focus and alt (option)  is down

    // Mutexes
    QMutex       mouse_move_mutex { QMutex::NonRecursive };         // Used to keep mouse move from backing up
    QMutex       resize_mutex     { QMutex::NonRecursive };         // Used to keep resizing from backing up
    QMutex       rotate_mutex     { QMutex::NonRecursive };         // Used to keep rotating from backing up


    // Mouse event variables
    QPoint                  m_origin;                                               // Stores mouse down position in view coordinates
    QPointF                 m_origin_in_scene;                                      // Stores mouse down position in scene coordinates
    QGraphicsItem          *m_origin_item;                                          // Stores top item under mouse (if any) on mouse down event


    // Selection Bounding Box Variables
    std::map<Position_Flags, QPolygonF> m_handles;                                  // Stores QRects of current selection box handles
    std::map<Position_Flags, QPointF>   m_handles_centers;                          // Stores QPointF center points of sides polygons
    Position_Flags                      m_over_handle;                              // Tracks if mouse is over a handle
    QPoint                              m_last_mouse_pos;                           // Tracks last known mouse position in view coordinates


    // View_Mode::Selecting Variables
    SceneViewRubberBand    *m_rubber_band;                                          // Holds our view's RubberBand object
    QList<QGraphicsItem *>  m_items_start;                                          // Stores items selected at start of new rubber band box


    // View_Mode::Resizing Variables
    QRectF                  m_selection_rect;                                       // Stores rect of current selection
    QRectF                  m_start_resize_rect;                                    // Stores starting rect of selection before resize starts
    Position_Flags          m_start_resize_grip;                                    // Stores which Size Grip Handle we started resize over
    X_Axis                  m_do_x;                                                 // Processed after click to know which sides to resize from
    Y_Axis                  m_do_y;                                                 // Processed after click to know which sides to resize from


    // View_Mode::Rotating Variables
    double                  m_last_angle_diff;                                      // Stores angle difference last time we checked


    QPolygonF               m_temp_polygon;                         // TEMP
    QPolygonF               m_temp_polygon2;                        // TEMP


public:
    // Constructor
    explicit SceneGraphicsView(QWidget *parent, DrProject *project, InterfaceRelay *interface);
    virtual ~SceneGraphicsView() override;

    // Event Overrides, start at Qt Docs for QGraphicsView Class to find more
    virtual void    paintEvent(QPaintEvent *event) override;                               // Inherited from QWidget
    virtual void    enterEvent(QEvent *event) override;                                    // Inherited from QWidget
    virtual void    keyPressEvent(QKeyEvent *event) override;                              // Inherited from QWidget
    virtual void    keyReleaseEvent(QKeyEvent *event) override;                            // Inherited from QWidget
    virtual void    mouseMoveEvent(QMouseEvent *event) override;                           // Inherited from QWidget
    virtual void    mousePressEvent(QMouseEvent *event) override;                          // Inherited from QWidget
    virtual void    mouseReleaseEvent(QMouseEvent *event) override;                        // Inherited from QWidget
#if QT_CONFIG(wheelevent)
    virtual void    wheelEvent(QWheelEvent *event) override;                               // Inherited from QWidget
#endif

    // Functions
    void            applyUpdatedMatrix();
    double          calcRotationAngleInDegrees(QPointF centerPt, QPointF targetPt);
    void            drawGrid();
    bool            isCloseTo(double number_desired, double number_to_check, double tolerance);
    bool            isSquare(double check_angle, double tolerance);
    QRectF          rectAtCenterPoint(QPoint center, double rect_size);
    QRectF          totalSelectedItemsSceneRect();
    void            updateSelectionRect();
    void            zoomInOut(int level);


    void            startSelect(QMouseEvent *event);
    void            processSelection(QPoint mouse_in_view);

    void            startRotate();
    void            rotateSelection(QPointF mouse_in_view);

    void            startResize();
    void            resizeSelection(QPointF mouse_in_scene);
    void            resizeSelectionOneNoRotate(QPointF mouse_in_scene);
    void            resizeSelectionOneWithRotate(QPointF mouse_in_scene);

    void            resizeMultipleSelection(QPointF mouse_in_scene);

    Position_Flags  findOppositeSide(Position_Flags start_side);


public slots:
    void            sceneChanged(QList<QRectF> region);
    void            selectionChanged();

};



class SceneViewRubberBand : public QRubberBand
{
private:
    InterfaceRelay  *m_interface;                // Pointer to interface class of parent form, for getting colors

public:
    // Constructor
    SceneViewRubberBand(Shape shape, QWidget *parent, InterfaceRelay *interface) :
                        QRubberBand (shape, parent), m_interface(interface) { }

    // Event overrides
    virtual void    paintEvent(QPaintEvent *) override;
};



#endif // EDITOR_VIEW_SCENE_H























