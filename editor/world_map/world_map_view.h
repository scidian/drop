//
//      Created by Stephens Nunnally on 4/30/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef WORLD_MAP_VIEW_H
#define WORLD_MAP_VIEW_H

#include <QElapsedTimer>
#include <QGraphicsView>

#include "editor/enums_editor.h"
#include "project/enums_entity_types.h"

// Forward Declarations
class DrProject;
class IEditorRelay;
class WorldMapScene;


//####################################################################################
//##    WorldMapView
//##        A sub classed QGraphicsView to show our QGraphicsScene (WorldMapScene)
//############################
class WorldMapView : public QGraphicsView
{
    Q_OBJECT

private:
    // External Borrowed Pointers
    DrProject              *m_project;                                      // Pointer to currently loaded Project
    IEditorRelay           *m_editor_relay;                                 // Pointer to IEditorRelay class of parent form

    // Local Variables
    Mouse_Mode              m_mouse_mode        { Mouse_Mode::Pointer };    // Tracks current view mouse mode
    View_Mode               m_view_mode         { View_Mode::None };        // Tracks current view interaction mode

    // Display Variables
    int                     m_zoom              { 200 }; // (50%)           // Zoom level of current view, 200 is 50% - 250 is 100%
    double                  m_zoom_scale        { 0.5 };                    // Updated in zoomInOut for use during painting grid, DO NOT SET MANUALLY
    int                     m_rotate            { 0 };                      // NOT IMPLEMENTED: Rotation of current view

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

    // View_Mode::Translating Variables
    QElapsedTimer           m_origin_timer;                                 // Tracks time since mouse down to help buffer movement while selecting
    bool                    m_allow_movement                { false };      // Used along with m_origin_timer to help buffer movement while selecting
    bool                    m_hide_bounding                 { false };      // True when moving items to stop bounding box from updating and painting
    QPointF                 m_origin_item_start_pos         { 0, 0 };       // Tracks starting position of origin item when View_Mode::Translating started


public:
    // Constructor / Destructor
    explicit WorldMapView(QWidget *parent, DrProject *project, WorldMapScene *scene, IEditorRelay *editor_relay);
    virtual ~WorldMapView() override;

    // Event Overrides - Paint
    virtual void    drawBackground(QPainter *painter, const QRectF &rect) override;
    virtual void    drawForeground(QPainter *painter, const QRectF &rect) override;
    virtual void    paintEvent(QPaintEvent *event) override;                                // Inherited from QWidget

    // Event Overrides - Input
    virtual void    keyPressEvent(QKeyEvent *event) override;                               // Inherited from QWidget
    virtual void    keyReleaseEvent(QKeyEvent *event) override;                             // Inherited from QWidget
    virtual void    mouseDoubleClickEvent(QMouseEvent *event) override;                     // Inherited from QWidget
    virtual void    mouseMoveEvent(QMouseEvent *event) override;                            // Inherited from QWidget
    virtual void    mousePressEvent(QMouseEvent *event) override;                           // Inherited from QWidget
    virtual void    mouseReleaseEvent(QMouseEvent *event) override;                         // Inherited from QWidget
#if QT_CONFIG(wheelevent)
    virtual void    wheelEvent(QWheelEvent *event) override;                                // Inherited from QWidget
#endif

    // View Display Functions
    void            setViewRect(QRectF new_rect);
    void            spaceBarDown();
    void            spaceBarUp();
    void            zoomInOut(int level);
    void            zoomToPower(int level);
    void            zoomToScale(double scale, bool recalculate_level = true);

    // Paint Functions
    void            paintGrid(QPainter &painter);


    // Getters / Setters
    View_Mode       currentViewMode()                   { return m_view_mode; }
    double          currentZoomLevel()                  { return m_zoom_scale; }

};

#endif // WORLD_MAP_VIEW_H











