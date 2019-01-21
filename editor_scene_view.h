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
    Dragging,
};

enum class Handle_Shapes {
    Circles,
    Squares,
};

struct Transform_Data {
    double  rotation;
    QPointF scale;
    QPointF skew;
};

enum class X_Axis {     Left,   Right,    None  };
enum class Y_Axis {     Top,    Bottom,   None  };
enum class Grid_Style { Lines,  Dots,           };


// Class constants
const double ANGLE_TOLERANCE = 2.5;                                 // Angle distance to fuzzy compare to desired angle
const int    ANGLE_STEP = 15;                                       // Angle intervals rotate function should snap to


class SceneGraphicsView : public QGraphicsView
{
    Q_OBJECT

private:
    // Local, instance specific member variables
    DrProject              *m_project;                              // Pointer to currently loaded project
    InterfaceRelay         *m_relay;                                // Pointer to InterfaceRelay class of parent form
    QGraphicsScene         *m_scene;                                // Pointer to current scene, updated during paintEvent
    View_Mode               m_view_mode = View_Mode::None;          // Tracks current view interaction mode

    QPixmap p_circle = QPixmap(":/gui_misc/handle_circle.png");
    QPixmap p_square = QPixmap(":/gui_misc/handle_square.png");
    QPixmap p_rotate = QPixmap(":/gui_misc/handle_rotate.png");

    QCursor c_size_vertical =   QCursor(QPixmap(":/cursors/size_vertical.png"));
    QCursor c_size_022 =        QCursor(QPixmap(":/cursors/size_022.png"));
    QCursor c_size_045 =        QCursor(QPixmap(":/cursors/size_045.png"));
    QCursor c_size_067 =        QCursor(QPixmap(":/cursors/size_067.png"));
    QCursor c_size_horizontal = QCursor(QPixmap(":/cursors/size_horizontal.png"));
    QCursor c_size_112 =        QCursor(QPixmap(":/cursors/size_112.png"));
    QCursor c_size_135 =        QCursor(QPixmap(":/cursors/size_135.png"));
    QCursor c_size_157 =        QCursor(QPixmap(":/cursors/size_157.png"));
    QCursor c_rotate_all =      QCursor(QPixmap(":/cursors/rotate_all.png"));

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


    // Mouse event variables
    QPoint                              m_origin;                   // Stores mouse down position in view coordinates
    QPointF                             m_origin_in_scene;          // Stores mouse down position in scene coordinates
    QGraphicsItem                      *m_origin_item;              // Stores top item under mouse (if any) on mouse down event


    // Selection Bounding Box Variables
    std::map<Position_Flags, QPolygonF> m_handles;                  // Stores QRects of current selection box handles
    std::map<Position_Flags, QPointF>   m_handles_centers;          // Stores QPointF center points of selection box handles
    std::map<Position_Flags, double>    m_handles_angles;           // Stores angle used for mouse cursor of selection box handles
    Handle_Shapes                       m_handles_shape;            // Stores which style handles should we draw
    Position_Flags                      m_over_handle;              // Tracks if mouse is over a handle
    QPoint                              m_last_mouse_pos;           // Tracks last known mouse position in view coordinates


    // View_Mode::Selecting Variables
    SceneViewRubberBand            *m_rubber_band;                  // Holds our view's RubberBand object
    QList<QGraphicsItem *>          m_items_start;                  // Stores items selected at start of new rubber band box


    // View_Mode::Resizing Variables
    QRectF                          m_start_resize_rect;            // Stores starting rect of selection before resize starts
    Position_Flags                  m_start_resize_grip;            // Stores which Size Grip Handle we started resize over
    X_Axis                          m_do_x;                         // Processed after click to know which sides to resize from
    Y_Axis                          m_do_y;                         // Processed after click to know which sides to resize from
    QPointF                         m_pre_resize_scale;             // Scale of selection group before we start resize
    QMap<Position_Flags, QPointF>   m_pre_resize_corners;           // Stores corner coordinates before resizing starts

    // View_Mode::Rotating Variables
    QRectF                          m_rotate_start_rect;            // Stores starting rect of selection before resize starts
    double                          m_rotate_start_angle;           // Stores angle of selection group at start of rotate routine

    // !!!!! DEBUG: Debugging variables
    QPolygonF                       m_debug_polygon;                // Holds a polygon used for resize routine
    QPolygonF                       m_debug_polygon2;        // TEMP
    long                            m_debug_fps = 0;         // TEMP
    long                            m_debug_fps_last;        // TEMP
    QTime                           m_debug_timer;           // TEMP



public:
    // Constructor
    explicit SceneGraphicsView(QWidget *parent, DrProject *project, InterfaceRelay *relay);
    virtual ~SceneGraphicsView() override;

    // Event Overrides, start at Qt Docs for QGraphicsView Class to find more
    virtual void    paintEvent(QPaintEvent *event) override;                                // Inherited from QWidget

    virtual bool    eventFilter(QObject *obj, QEvent *event) override;                      // Inherited from QObject
    virtual void    scrollContentsBy(int dx, int dy) override;                              // Inherited from QAbstractScrollArea

    virtual void    enterEvent(QEvent *event) override;                                     // Inherited from QWidget
    virtual void    keyPressEvent(QKeyEvent *event) override;                               // Inherited from QWidget
    virtual void    keyReleaseEvent(QKeyEvent *event) override;                             // Inherited from QWidget
    virtual void    mouseMoveEvent(QMouseEvent *event) override;                            // Inherited from QWidget
    virtual void    mousePressEvent(QMouseEvent *event) override;                           // Inherited from QWidget
    virtual void    mouseReleaseEvent(QMouseEvent *event) override;                         // Inherited from QWidget
#if QT_CONFIG(wheelevent)
    virtual void    wheelEvent(QWheelEvent *event) override;                                // Inherited from QWidget
#endif

    // View Display Functions
    void            applyUpdatedMatrix();
    void            zoomInOut(int level);

    // Misc Functions
    double          calculateCornerAngle(double angle1, double angle2);
    Transform_Data  decomposeTransform(QTransform &from_transform, bool qr_type = true);
    double          extractAngleFromTransform(QTransform &from_transform);
    QPointF         extractScaleFromItem(QGraphicsItem *item);
    QGraphicsItem*  itemOnTopAtPosition(QPoint check_point, bool take_item_on_top_out = false);
    QRectF          rectAtCenterPoint(QPoint center, double rect_size);
    QRectF          totalSelectedItemsSceneRect();
    void            updateSelectionBoundingBox();

    // Paint Functions
    void            paintBoundingBox(QPainter &painter);
    void            paintGrid();
    void            paintHandles(QPainter &painter, Handle_Shapes shape_to_draw);
    void            paintItemOutlines(QPainter &painter);

    // Selection Functions
    void            startSelect(QMouseEvent *event);
    void            processSelection(QPoint mouse_in_view);

    // Rotation Functions
    void            startRotate();
    void            rotateSelection(QPointF mouse_in_view);
    double          calcRotationAngleInDegrees(QPointF centerPt, QPointF targetPt);
    bool            isCloseTo(double number_desired, double number_to_check, double tolerance);
    bool            isSquare(double check_angle);

    // Resize functions
    void            startResize();
    void            resizeSelection(QPointF mouse_in_scene);
    void            resizeSelectionWithRotate(QPointF mouse_in_scene);
    Position_Flags  findOppositeSide(Position_Flags start_side);
    void            removeShearing(QGraphicsItem *item);


public slots:
    void    sceneChanged(QList<QRectF> region);
    void    selectionChanged();
};


//############################
//##    SceneViewRubberBand
//##        A sub classed QRubberBand so we can override paint event for rubber band
//############################
class SceneViewRubberBand : public QRubberBand
{
private:
    InterfaceRelay  *m_relay;                // Pointer to InterfaceRelay class of parent form, for getting colors

public:
    // Constructor
    SceneViewRubberBand(Shape shape, QWidget *parent, InterfaceRelay *relay) : QRubberBand (shape, parent), m_relay(relay) { }

    // Event overrides
    virtual void    paintEvent(QPaintEvent *) override;
};



#endif // EDITOR_VIEW_SCENE_H























