//
//      Created by Stephens Nunnally on 4/30/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef WORLD_MAP_VIEW_H
#define WORLD_MAP_VIEW_H

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


public:
    // Constructor / Destructor
    explicit WorldMapView(QWidget *parent, DrProject *project, WorldMapScene *scene, IEditorRelay *editor_relay);
    virtual ~WorldMapView() override;

    // Event Overrides - Paint
    virtual void    drawBackground(QPainter *painter, const QRectF &rect) override;
    virtual void    drawForeground(QPainter *painter, const QRectF &rect) override;
    virtual void    paintEvent(QPaintEvent *event) override;                                // Inherited from QWidget

    // Event Overrides - Input
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


};

#endif // WORLD_MAP_VIEW_H











