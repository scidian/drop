//
//      Created by Stephens Nunnally on 1/3/2019, (c) 2019 Scidian Studios, All Rights Reserved
//
//  File:
//      A sub classed QGraphicsView so we can override events for our View Area
//
//
#ifndef EDITOR_VIEW_H
#define EDITOR_VIEW_H

#include <QElapsedTimer>
#include <QGraphicsView>
#include <QMap>

#include "core/types/dr_pointf.h"
#include "editor/constants_advisor_info.h"
#include "editor/enums_editor.h"
#include "project/entities/dr_thing.h"
#include "project/enums_entity_types.h"

// Forward Declarations
class DrProject;
class DrSettings;
class DrStage;
class DrThing;
class EditorItem;
class EditorScene;
class IEditorRelay;
class SelectionGroup;
class ViewRubberBand;
class ViewToolTip;


//####################################################################################
//##    Local Types
//############################
// Local Structs
struct Transform_Data {
    double  rotation;
    QPointF scale;
    QPointF skew;
};

struct Camera_Data {
    DrThing     *thing;
    QRect        view_rect;
    double       z_order;
};

// Class Constants
constexpr double c_angle_tolerance = 2.5;                                   // Angle distance to fuzzy compare to desired angle
constexpr int    c_angle_step = 15;                                         // Angle intervals rotate function should snap to


//####################################################################################
//##    EditorView
//##        A sub classed QGraphicsView to show our QGraphicsScene (EditorScene)
//############################
class EditorView : public QGraphicsView
{
    Q_OBJECT

private:
    // External Borrowed Pointers
    DrProject              *m_project;                                      // Pointer to currently loaded project
    IEditorRelay           *m_editor_relay;                                 // Pointer to IEditorRelay class of parent form

    EditorScene            *my_scene            { nullptr };                // Holds the scene() this view is set to as a EditorScene class

    // Local Variables
    Mouse_Mode              m_mouse_mode        { Mouse_Mode::Pointer };    // Tracks current view mouse mode
    View_Mode               m_view_mode         { View_Mode::None };        // Tracks current view interaction mode

    // Display Variables
    int                     m_zoom              { 200 }; // (50%)           // Zoom level of current view, 200 is 50% - 250 is 100%
    double                  m_zoom_scale        { 0.5 };                    // Updated in zoomInOut for use during painting grid, DO NOT SET MANUALLY
    QElapsedTimer           m_zoom_timer;                                   // Used to auto hide zoom tool tip after time has passed
    int                     m_rotate            { 0 };                      // NOT IMPLEMENTED: Rotation of current view

    // Grid Drawing Variables
    QVector<QPointF>        m_grid_points;                                  // Holds latest calculated grid points
    QVector<QLineF>         m_grid_lines;                                   // Holds latest calculated grid lines
    QPixmap                 m_grid_buffer;                                  // Back buffer for painting, grid lines are drawn onto this when view changes and then
                                                                            //      this gets painted instead of drawLine calls every time paintEvent is called
    QRectF                  m_grid_view_rect;                               // Holds the desired area we wish to draw lines or dots
    bool                    m_grid_needs_redraw = true;                     // Flag used to mark grid for redrawing during next paintEvent

    // Grid Style Variables (currently re-populated from EditorView::updateGrid by way of updateEditorWidgetsAfterItemChange -> EditorScene::updateChangesInScene)
    Grid_Style              m_grid_style        { Grid_Style::Lines };      // Grid type to display
    QPointF                 m_grid_origin       { 0, 0 };                   // Origin point of grid in scene
    QPointF                 m_grid_size         { 50, 50 };                 // Grid size
    QPointF                 m_grid_scale        { 1, 1 };                   // X and Y scaling for after grid has been rotated
    double                  m_grid_rotate       { 0 };                      // Rotation of grid lines
    QColor                  m_grid_color;                                   // Grid color
    bool                    m_back_color_use    { false };                  // Should we use background color?
    QColor                  m_back_color;                                   // World background color

    bool                    m_grid_should_snap  { true };                   // Should snap to grid?
    bool                    m_grid_resize_snap  { true };                   // Should snap while resizing?
    bool                    m_grid_show_on_top  { false };                  // Paint grid on top?

    // Stage Bounds Variables
    QPolygonF               m_stage_grab_handle;                            // Stores QRect of box that can be dragged to change Stage size

    // Misc Flags
    bool                    m_flag_has_shown_a_scene_yet    { false };      // False until a scene has been loaded into the view

    // Keyboard Flags
    bool                    m_flag_dont_check_keys          { false };      // True when we don't want mouseMoveEvent to update key flags
    bool                    m_flag_key_down_spacebar        { false };      // True when View has focus and spacebar      is down
    bool                    m_flag_key_down_control         { false };      // True when View has focus and control (cmd) is down
    bool                    m_flag_key_down_alt             { false };      // True when View has focus and alt (option)  is down
    bool                    m_flag_key_down_shift           { false };      // True when View has focus and shift         is down

    // Mouse Variables
    QPoint                  m_last_mouse_pos;                               // Tracks last known mouse position in view coordinates
    QPoint                  m_origin;                                       // Stores mouse down position in view coordinates
    QPointF                 m_origin_in_scene;                              // Stores mouse down position in scene coordinates
    QGraphicsItem          *m_origin_item;                                  // Stores top item under mouse (if any) on mouse down event
    Position_Flags          m_over_handle;                                  // Tracks if mouse is over a handle

    // Drag and Drop Variables
    bool                    m_drop_might_happen             { false };      // True when its possible we may get an asset drop and need to draw crosshairs
    QPointF                 m_drop_location;                                // Potential drop location of drag and drop operation

    // Tool Tip Variables
    ViewToolTip                        *m_tool_tip;                         // Holds our view's custom Tool Tip box

    // Selection Bounding Box Variables
    std::map<Position_Flags, QPointF>   m_selection_points;                 // Stores all points of the current selection box
    std::map<Position_Flags, QPolygonF> m_handles;                          // Stores QRects of current selection box handles
    std::map<Position_Flags, QPointF>   m_handles_centers;                  // Stores QPointF center points of selection box handles
    std::map<Position_Flags, double>    m_handles_angles;                   // Stores angle used for mouse cursor of selection box handles
    Handle_Shapes                       m_handles_shape;                    // Stores which style handles should we draw

    // View_Mode::Selecting Variables
    ViewRubberBand                     *m_rubber_band;                      // Holds our view's RubberBand object
    QList<QGraphicsItem*>               m_items_start;                      // Stores items selected at start of new rubber band box
    QList<QGraphicsItem*>               m_items_keep;                       // Stores list of items to keep on top of rubber band items (with control key)

    // View_Mode::Translating Variables
    QElapsedTimer                       m_origin_timer;                     // Tracks time since mouse down to help buffer movement while selecting
    bool                                m_allow_movement = false;           // Used along with m_origin_timer to help buffer movement while selecting
    bool                                m_hide_bounding = false;            // True when moving items to stop bounding box from updating and painting
    QPointF                             m_origin_item_start_pos { 0, 0 };   // Tracks starting position of origin item when View_Mode::Translating started

    // View_Mode::Resizing Variables
    QRectF                              m_start_resize_rect;                // Stores starting rect of selection before resize starts
    Position_Flags                      m_start_resize_grip;                // Stores which Size Grip Handle we started resize over
    X_Axis                              m_do_x;                             // Processed after click to know which sides to resize from
    Y_Axis                              m_do_y;                             // Processed after click to know which sides to resize from
    bool                                m_resize_started = false;           // Turns to true after the first time resize function has been run, resets to false when done
    QPointF                             m_last_mouse_snap { 0, 0 };         // Keeps track of the last point the item was snapped to while resizing
    QPointF                             m_pre_resize_scale;                 // Scale of selection group before we start resize
    QMap<Position_Flags, QPointF>       m_pre_resize_corners;               // Stores corner coordinates before resizing starts
    QGraphicsItemGroup                 *m_group;                            // Loads a copy of selected items in a new group before resize starts to better calculate resizing

    // View_Mode::Rotating Variables
    double                              m_rotate_start_angle;               // Stores angle of selection group at start of rotate routine

    // View_Mode::Moving_Camera Variables
    QPoint                              m_cam_mouse_start;                  // Starting mouse position when Camera Move starts
    double                              m_cam_angle_x_start = 0;            // Camera angle when camera move starts
    double                              m_cam_angle_y_start = 0;            // Camera angle when camera move starts
    bool                                m_cam_at_back = false;              // Whether or not camera is behind target when camera move starts
    bool                                m_cam_reversed = false;             // Stores is x rotation is reversed when camera move starts

    // View_Mode::Resizing_Stage Variables
    QPoint                              m_resize_stage_mouse_start;         // Starting mouse position when Stage Resize starts
    double                              m_resize_stage_angle;               // Stage angle (DrWorld Game Direction)
    double                              m_resize_stage_start_size;          // Stage size when Stage Resize starts

    // Debug Camera Mode Variables
    DrThing                            *m_cam_mouse_over = nullptr;         // Stores which camera mouse is over, if none == nullptr
    DrThing                            *m_cam_selected = nullptr;           // Stores which camera is selected, if none == nullptr
    std::map<DrThing*, Camera_Data>     m_cam_data;                         // Stores view bounding coordinates of all cameras within view


    // !!!!! #DEBUG: Debugging Variables
    long                                m_debug_fps = 0;         // #DEBUG
    long                                m_debug_fps_last;        // #DEBUG
    QElapsedTimer                       m_debug_timer;           // #DEBUG



public:
    // Constructor
    explicit EditorView(QWidget *parent, DrProject *project, IEditorRelay *editor_relay, EditorScene *from_scene);
    virtual ~EditorView() override;

    // Event Overrides - Paint
    virtual void    drawBackground(QPainter *painter, const QRectF &rect) override;
    virtual void    drawForeground(QPainter *painter, const QRectF &rect) override;
    virtual void    paintEvent(QPaintEvent *event) override;                                // Inherited from QWidget

    // Event Overrides - Input
    virtual void    scrollContentsBy(int dx, int dy) override;                              // Inherited from QAbstractScrollArea
    virtual void    keyPressEvent(QKeyEvent *event) override;
    virtual void    keyReleaseEvent(QKeyEvent *event) override;
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

    // Event Overrides - Misc
    virtual void    focusInEvent(QFocusEvent *event) override;
    virtual void    leaveEvent(QEvent *event) override;

    // View Display Functions
    void                setViewRect(QRectF new_rect);
    void                spaceBarDown();
    void                spaceBarUp();
    void                zoomInOut(int level);
    void                zoomToContents();
    void                zoomToPower(int level);
    void                zoomToScale(double scale, bool recalculate_level = true);

    // Misc Functions
    double              calculateCornerAngle(double angle1, double angle2);
    Transform_Data      decomposeTransform(QTransform &from_transform, bool qr_type = true);
    double              extractAngleFromTransform(QTransform &from_transform);
    void                updateSelectionBoundingBox(int called_from = 0);

    // Grid Functions
    double              currentGridAngle()                  { return m_grid_rotate; }
    QPointF             currentGridScale()                  { return m_grid_scale; }
    static QRectF       gameFrame(DrProject *project);
    void                recalculateGrid();
    QPointF             roundToGrid(QPointF point_in_scene);
    static QRectF       stageBoundingRect(DrProject *project, DrStage *stage, double &half_height);
    void                updateGrid();

    // Paint Functions
    void                paintBoundingBox(QPainter &painter);
    void                paintCameras(QPainter &painter, DrStage *stage);
    void                paintCrossHairs(QPainter &painter, QPoint center);
    void                paintCollisionShapes(QPainter &painter, DrStage *stage);
    void                paintDebugHealth(QPainter &painter, DrStage *stage);
    void                paintGameFrame(QPainter &painter);
    void                paintGrid(QPainter &painter);
    void                paintGroupAngle(QPainter &painter, double angle);
    void                paintHandles(QPainter &painter, Handle_Shapes shape_to_draw);
    void                paintItemOutlines(QPainter &painter);
    void                paintItemCenters(QPainter &painter);
    void                paintStageBounds(QPainter &painter, DrStage* stage);
    void                paintToolTip(QPainter &painter);

    // Selection Functions
    QList<DrSettings*>  convertItemListToSettings(QList<QGraphicsItem*> list);
    QGraphicsItem*      setInspectorClearSelection(DrThing *thing);
    void                startSelect(QMouseEvent *event);
    void                processSelection(QPoint mouse_in_view);

    // Rotation Functions
    void                startRotateSelection(QPoint mouse_in_view, bool use_tool_tip = true);
    void                rotateSelection(QPointF mouse_in_view, bool use_exact_angle = false, double angle_to_use = 0.0);

    // Resize functions
    bool                containsSquareItem(QGraphicsItem *item);
    void                startResizeSelection(QPoint mouse_in_view, bool use_tool_tip = true);
    void                resizeSelection(QPointF mouse_in_scene, bool use_exact_scale = false, QPointF scale_to_use = QPointF(0, 0));
    void                resizeSelectionWithRotate(QPointF mouse_in_scene, bool use_exact_scale, QPointF scale_to_use);
    Position_Flags      findOppositeSide(Position_Flags start_side);
    void                removeShearing(QGraphicsItem *item, QPointF scale);

    // Camera Move Functions
    void                startRotateCamera(QPoint mouse_in_view);
    void                rotateCamera(QPointF mouse_in_view);

    // Resize Stage Functions
    void                startResizeStage(QPoint mouse_in_view);
    void                resizeStage(QPointF mouse_in_view);

    // Getters / Setters
    IEditorRelay*       getEditorRelay()                    { return m_editor_relay; }
    DrProject*          getProject()                        { return m_project; }

    View_Mode           currentViewMode()                   { return m_view_mode; }
    double              currentZoomLevel()                  { return m_zoom_scale; }
    bool                hasShownAScene() const              { return m_flag_has_shown_a_scene_yet; }
    EditorScene*        getEditorScene()                    { return my_scene; }
    Mouse_Mode          getMouseMode()                      { return m_mouse_mode; }
    void                setHasShownAScene(bool has)         { m_flag_has_shown_a_scene_yet = has; }
    void                setMouseCursorFromAngle(double angle_in_degrees);
    void                setMouseMode(Mouse_Mode mode)       { m_mouse_mode = mode; }
    void                setViewMode(View_Mode mode)         { m_view_mode = mode; }


public slots:
    void    sceneChanged(QList<QRectF>);
    void    sceneRectChanged(QRectF new_rect);
    void    selectionChanged();

    void    checkTranslateToolTipStarted();
    void    stoppedZooming();


signals:
    // Signals used to fire UndoStack Commands
    void    selectionGroupMoved(EditorScene *scene, const QPointF &old_position);
    void    selectionGroupNewGroup(EditorScene *scene, QList<DrThing*> old_list, QList<DrThing*> new_list);

    // Mouse Mode Hand Signals
    void    updateCenterPointX(double new_value);                       // Emitted to update View Tool Bar Hand Mode Center Point X Coordinate
    void    updateCenterPointY(double new_value);                       // Emitted to update View Tool Bar Hand Mode Center Point Y Coordinate

    // Mouse Mode Zoom Signals
    void    updateZoomSlider(int new_value);                            // Emitted to update View Tool Bar Zoom Mode Slider
    void    updateZoomSpin(int new_value);                              // Emitted to update View Tool Bar Zoom Mode Spin Box

};



#endif // EDITOR_VIEW_H























