//
//      Created by Stephens Nunnally on 1/3/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      A sub classed QGraphicsView so we can override events for our View Area
//
//
#ifndef EDITOR_VIEW_H
#define EDITOR_VIEW_H

#include <QGraphicsView>
#include <QRubberBand>
#include <QTime>

#include "constants.h"
#include "enums.h"
#include "enums_form_main.h"

// Forward declarations
class DrProject;
class DrStage;
class DrThing;
class DrItem;
class SelectionGroup;
class IEditorRelay;
class DrScene;
class DrSettings;
class DrViewRubberBand;
class DrViewToolTip;


//####################################################################################
//##    Local Enumerations
//############################
enum class Handle_Shapes {
    Circles,
    Squares,
};

struct Transform_Data {
    double  rotation;
    QPointF scale;
    QPointF skew;
};

enum class X_Axis {         Left,   Right,    None      };
enum class Y_Axis {         Top,    Bottom,   None      };
enum class Grid_Style {     Lines,  Dots,               };


// Class constants
constexpr double c_angle_tolerance = 2.5;                           // Angle distance to fuzzy compare to desired angle
constexpr int    c_angle_step = 15;                                 // Angle intervals rotate function should snap to


//####################################################################################
//##    DrView
//##        A sub classed QGraphicsView to show our QGraphicsScene
//############################
class DrView : public QGraphicsView
{
    Q_OBJECT

private:
    // External Borrowed Pointers
    DrProject              *m_project;                              // Pointer to currently loaded project
    IEditorRelay           *m_editor_relay;                         // Pointer to IEditorRelay class of parent form

    DrScene                *my_scene;                               // Holds the scene() this view is set to as a DrScene Class

    // Local Variables
    View_Mode               m_view_mode = View_Mode::None;          // Tracks current view interaction mode

    const QPixmap p_circle = QPixmap(":/assets/gui_misc/handle_circle.png");
    const QPixmap p_square = QPixmap(":/assets/gui_misc/handle_square.png");
    const QPixmap p_rotate = QPixmap(":/assets/gui_misc/handle_rotate.png");

    // Display Variables
    int          m_zoom = 200;                                      // Zoom level of current view, 200 is 50% - 250 is 100%
    double       m_zoom_scale = 1;                                  // Updated in zoomInOut for use during painting grid, DO NOT SET MANUALLY
    QTime        m_zoom_timer;                                      // Used to auto hide zoom tool tip after time has passed
    int          m_rotate = 0;                              // NOT IMPLEMENTED: Rotation of current view

    // Stage Drawing Variables
    QRectF       m_game_frame;                                      // Used to overlay screen size / draw Stage Boundary

    // Grid Drawing Variables
    QVector<QPointF> m_grid_points;                                 // Holds latest calculated grid points
    QVector<QLineF>  m_grid_lines;                                  // Holds latest calculated grid lines
    QPixmap          m_grid_buffer;                                 // Back buffer for painting, grid lines are drawn onto this when view changes and then
                                                                    //      this gets painted instead of drawLine calls every time paintEvent is called
    QRectF           m_grid_view_rect;                              // Holds the desired area we wish to draw lines or dots
    bool             m_grid_needs_redraw = true;                    // Flag used to mark grid for redrawing during next paintEvent

    // Grid Style Variables (currently re-populated from DrView::updateGrid by way of updateEditorWidgetsAfterItemChange -> DrScene::updateChangesInScene)
    Grid_Style   m_grid_style { Grid_Style::Lines };                // Grid type to display
    QPointF      m_grid_origin { 0, 0 };                            // Origin point of grid in scene
    QPointF      m_grid_size { 50, 50 };                            // Grid size
    QPointF      m_grid_scale { 1, 1 };                             // X and Y scaling for after grid has been rotated
    double       m_grid_rotate = 0;                                 // Rotation of grid lines
    QColor       m_grid_color;                                      // Grid color
    bool         m_back_color_use = false;                          // Should we use background color?
    QColor       m_back_color;                                      // World background color

    bool         m_grid_should_snap = true;                         // Should snap to grid?
    bool         m_grid_resize_snap = true;                         // Should snap while resizing?
    bool         m_grid_show_on_top = false;                        // Paint grid on top?

    // Misc Flags
    bool         m_flag_has_shown_a_scene_yet = false;              // False until a scene has been loaded into the view

    // Keyboard Flags
    bool         m_flag_dont_check_keys =   false;                  // True when we don't want mouseMoveEvent to update key flags
    bool         m_flag_key_down_spacebar = false;                  // True when View has focus and spacebar      is down
    bool         m_flag_key_down_control =  false;                  // True when View has focus and control (cmd) is down
    bool         m_flag_key_down_alt =      false;                  // True when View has focus and alt (option)  is down
    bool         m_flag_key_down_shift =    false;                  // True when View has focus and shift         is down

    // Drag and Drop Variables
    bool         m_drop_might_happen = false;                       // True when its possible we may get an asset drop and need to draw crosshairs
    QPointF      m_drop_location;                                   // Potential drop location of drag and drop operation

    // Mouse event variables
    QPoint                              m_origin;                   // Stores mouse down position in view coordinates
    QPointF                             m_origin_in_scene;          // Stores mouse down position in scene coordinates
    QGraphicsItem                      *m_origin_item;              // Stores top item under mouse (if any) on mouse down event

    // Tool Tip Variables
    DrViewToolTip                      *m_tool_tip;                 // Holds our view's custom Tool Tip box

    // Selection Bounding Box Variables
    std::map<Position_Flags, QPointF>   m_selection_points;         // Stores all points of the current selection box
    std::map<Position_Flags, QPolygonF> m_handles;                  // Stores QRects of current selection box handles
    std::map<Position_Flags, QPointF>   m_handles_centers;          // Stores QPointF center points of selection box handles
    std::map<Position_Flags, double>    m_handles_angles;           // Stores angle used for mouse cursor of selection box handles
    Handle_Shapes                       m_handles_shape;            // Stores which style handles should we draw
    Position_Flags                      m_over_handle;              // Tracks if mouse is over a handle
    QPoint                              m_last_mouse_pos;           // Tracks last known mouse position in view coordinates

    // View_Mode::Selecting Variables
    DrViewRubberBand               *m_rubber_band;                  // Holds our view's RubberBand object
    QList<QGraphicsItem*>           m_items_start;                  // Stores items selected at start of new rubber band box
    QList<QGraphicsItem*>           m_items_keep;                   // Stores list of items to keep on top of rubber band items (with control key)

    // View_Mode::Translating Variables
    QTime                           m_origin_timer;                 // Tracks time since mouse down to help buffer movement while selecting
    bool                            m_allow_movement = false;       // Used along with m_origin_timer to help buffer movement while selecting
    bool                            m_hide_bounding = false;        // True when moving items to stop bounding box from updating and painting

    // View_Mode::Resizing Variables
    QRectF                          m_start_resize_rect;            // Stores starting rect of selection before resize starts
    Position_Flags                  m_start_resize_grip;            // Stores which Size Grip Handle we started resize over
    X_Axis                          m_do_x;                         // Processed after click to know which sides to resize from
    Y_Axis                          m_do_y;                         // Processed after click to know which sides to resize from
    bool                            m_resize_started = false;       // Turns to true after the first time resize function has been run, resets to false when done
    QPointF                         m_last_mouse_snap { 0, 0 };     // Keeps track of the last point the item was snapped to while resizing
    QPointF                         m_pre_resize_scale;             // Scale of selection group before we start resize
    QMap<Position_Flags, QPointF>   m_pre_resize_corners;           // Stores corner coordinates before resizing starts
    QGraphicsItemGroup             *m_group;                        // Loads a copy of selected items in a new group before resize starts to better calculate resizing

    // View_Mode::Rotating Variables
    double                          m_rotate_start_angle;           // Stores angle of selection group at start of rotate routine


    // !!!!! #DEBUG: Debugging Variables
    long                            m_debug_fps = 0;         // #DEBUG
    long                            m_debug_fps_last;        // #DEBUG
    QTime                           m_debug_timer;           // #DEBUG
    QPolygonF                       m_debug_points;
    QPolygonF                       m_debug_shear, m_debug_shear2, m_debug_shear3;



public:
    // Constructor
    explicit DrView(QWidget *parent, DrProject *project, DrScene *from_scene, IEditorRelay *editor_relay);
    virtual ~DrView() override;

    // Event Overrides - Paint
    virtual void    drawBackground(QPainter *painter, const QRectF &rect) override;
    virtual void    drawForeground(QPainter *painter, const QRectF &rect) override;
    virtual void    paintEvent(QPaintEvent *event) override;                                // Inherited from QWidget

    // Event Overrides - Input
    virtual void    scrollContentsBy(int dx, int dy) override;                              // Inherited from QAbstractScrollArea
    virtual void    keyPressEvent(QKeyEvent *event) override;                               // Inherited from QWidget
    virtual void    keyReleaseEvent(QKeyEvent *event) override;                             // Inherited from QWidget
    virtual void    mouseDoubleClickEvent(QMouseEvent *event) override;                     // Inherited from QWidget
    virtual void    mouseMoveEvent(QMouseEvent *event) override;                            // Inherited from QWidget
    virtual void    mousePressEvent(QMouseEvent *event) override;                           // Inherited from QWidget
    virtual void    mouseReleaseEvent(QMouseEvent *event) override;                         // Inherited from QWidget
#if QT_CONFIG(wheelevent)
    virtual void    wheelEvent(QWheelEvent *event) override;                                // Inherited from QWidget
#endif
    virtual void    resizeEvent(QResizeEvent *event) override;                              // Inherited from QWidget

    // Event Overrides - Drag / Drop
    virtual void    dragEnterEvent(QDragEnterEvent *event) override;
    virtual void    dragLeaveEvent(QDragLeaveEvent *event) override;
    virtual void    dragMoveEvent(QDragMoveEvent *event) override;
    virtual void    dropEvent(QDropEvent *event) override;


    // View Display Functions
    void            setViewRect(QRectF new_rect);
    void            zoomInOut(int level);

    // Misc Functions
    double          calculateCornerAngle(double angle1, double angle2);
    Transform_Data  decomposeTransform(QTransform &from_transform, bool qr_type = true);
    double          extractAngleFromTransform(QTransform &from_transform);
    QRectF          rectAtCenterPoint(QPoint center, double rect_size);
    void            updateSelectionBoundingBox(int called_from = 0);

    // Grid Functions
    double          currentGridAngle() { return m_grid_rotate; }
    QPointF         currentGridScale() { return m_grid_scale; }
    void            recalculateGrid();
    QPointF         roundToGrid(QPointF point_in_scene);
    void            updateGrid();

    // Paint Functions
    void            paintBoundingBox(QPainter &painter);
    void            paintCrossHairs(QPainter &painter, QPoint center);
    void            paintGameFrame(QPainter &painter);
    void            paintGrid(QPainter &painter);
    void            paintGroupAngle(QPainter &painter, double angle);
    void            paintHandles(QPainter &painter, Handle_Shapes shape_to_draw);
    void            paintItemOutlines(QPainter &painter);
    void            paintItemCenters(QPainter &painter);
    void            paintStageBounds(QPainter &painter, DrStage* stage);
    void            paintToolTip(QPainter &painter);

    // Selection Functions
    void            startSelect(QMouseEvent *event);
    void            processSelection(QPoint mouse_in_view);

    // Rotation Functions
    void            startRotate(QPoint mouse_in_view, bool use_tool_tip = true);
    void            rotateSelection(QPointF mouse_in_view, bool use_exact_angle = false, double angle_to_use = 0.0);
    double          calcRotationAngleInDegrees(QPointF centerPt, QPointF targetPt);

    // Resize functions
    void            startResize(QPoint mouse_in_view, bool use_tool_tip = true);
    void            resizeSelection(QPointF mouse_in_scene, bool use_exact_scale = false, QPointF scale_to_use = QPointF(0, 0));
    void            resizeSelectionWithRotate(QPointF mouse_in_scene, bool use_exact_scale, QPointF scale_to_use);
    Position_Flags  findOppositeSide(Position_Flags start_side);
    void            removeShearing(QGraphicsItem *item, QPointF scale);

    // Getters / Setters
    View_Mode       currentViewMode() { return m_view_mode; }
    QString         currentViewModeAsString();
    double          currentZoomLevel() { return m_zoom_scale; }
    bool            hasShownAScene() const { return m_flag_has_shown_a_scene_yet; }
    void            setHasShownAScene(bool has) { m_flag_has_shown_a_scene_yet = has; }
    void            spaceBarDown();
    void            spaceBarUp();


public slots:
    void    sceneChanged(QList<QRectF>);
    void    sceneRectChanged(QRectF new_rect);
    void    selectionChanged();

    void    checkTranslateToolTipStarted();
    void    stoppedZooming();


signals:
    // Signals used to fire UndoStack Commands
    void    selectionGroupMoved(DrScene *scene, const QPointF &old_position);
    void    selectionGroupNewGroup(DrScene *scene, QList<DrThing*> old_list, QList<DrThing*> new_list);

};


//####################################################################################
//##    DrViewRubberBand
//##        A sub classed QRubberBand so we can override paint event for rubber band
//############################
class DrViewRubberBand : public QRubberBand
{

public:
    // Constructor
    DrViewRubberBand(Shape shape, QWidget *parent) : QRubberBand (shape, parent) { }

    // Event overrides
    virtual void    paintEvent(QPaintEvent *) override;
};



//####################################################################################
//##    DrViewToolTip
//##        A parentless widget to be used as a custom tooltip
//############################
class DrViewToolTip : public QWidget
{
private:
    View_Mode   m_tip_type = View_Mode::None;           // Which type of tool tip to show
    QPoint      m_offset;                               // Stores how much to offset the current tooltip from mouse position
    double      m_angle = 0;                            // Stores angle to show in tooltip
    double      m_x = 0;                                // Stores x value of resizing / moving
    double      m_y = 0;                                // Stores y value of resizing / moving
    int         m_int = 0;                              // Stores zoom scale
    int         m_x_radius = 100;
    int         m_y_radius = 100;

public:
    // Constructor
    DrViewToolTip(QWidget *parent = nullptr);

    // Event overrides
    virtual void    paintEvent(QPaintEvent *) override;

    // Functions
    void            drawText(QPainter &painter, int left_offset = 0, int top_offset = 0);
    void            startToolTip(View_Mode type, QPoint mouse_position, QVariant data);
    void            stopToolTip();
    void            updateToolTipPosition(QPoint mouse_position);
    void            updateToolTipData(QVariant data);

    // Getters and Setters
    QPoint          getOffset()  { return m_offset;   }
    View_Mode       getTipType() { return m_tip_type; }
    int             getXRadius() { return m_x_radius; }
    int             getYRadius() { return m_y_radius; }

};


#endif // EDITOR_VIEW_H























