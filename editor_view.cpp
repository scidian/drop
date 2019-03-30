//
//      Created by Stephens Nunnally on 1/3/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Graphics View Definitions
//
//
#include <QtMath>

#include "colors.h"
#include "debug.h"
#include "editor_item.h"
#include "editor_view.h"
#include "editor_scene.h"
#include "globals.h"
#include "interface_editor_relay.h"
#include "library.h"
#include "project.h"
#include "project_world.h"
#include "project_world_stage.h"
#include "project_world_stage_object.h"
#include "settings.h"
#include "settings_component.h"
#include "settings_component_property.h"


//####################################################################################
//##        Constructor & destructor
//####################################################################################
DrView::DrView(QWidget *parent, DrProject *project, DrScene *from_scene, IEditorRelay *editor_relay) :
               QGraphicsView(parent), m_project(project), m_editor_relay(editor_relay)
{
    // Initialize rubber band object used as a selection box
    m_rubber_band = new DrViewRubberBand(QRubberBand::Shape::Rectangle, this);

    // Initialize tool tip object used for displaying some helpful info
    m_tool_tip = new DrViewToolTip(this);
    m_tool_tip->hide();

    m_over_handle = Position_Flags::No_Position;

    if (Dr::CheckDebugFlag(Debug_Flags::Label_FPS))
        m_debug_timer.start();

    my_scene = from_scene;
    setScene(my_scene);


    // ********** Connect signals to scene
    connect(my_scene, SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));
    connect(my_scene, SIGNAL(sceneRectChanged(QRectF)), this, SLOT(sceneRectChanged(QRectF)));
    connect(my_scene, SIGNAL(changed(QList<QRectF>)), this, SLOT(sceneChanged(QList<QRectF>)));

    connect(my_scene, &DrScene::updateGrid,  this, [this]() { updateGrid(); });
    connect(my_scene, &DrScene::updateViews, this, [this]() { update(); });
    connect(my_scene, &DrScene::setViewRect, this, [this](QRectF new_rect) { setViewRect(new_rect); });

    connect(this,   SIGNAL(selectionGroupMoved(DrScene*, QPointF)),
            my_scene, SLOT(selectionGroupMoved(DrScene*, QPointF)));

    connect(this,   SIGNAL(selectionGroupNewGroup(DrScene*, QList<DrObject*>, QList<DrObject*>)),
            my_scene, SLOT(selectionGroupNewGroup(DrScene*, QList<DrObject*>, QList<DrObject*>)) );

}


DrView::~DrView()
{
    delete m_rubber_band;
    delete m_tool_tip;
}





//####################################################################################
//##        Scene Change SLOTs / Events to update selection box when scene / selection changes
//####################################################################################
// SLOT: Connected from scene().sceneRectChanged
void DrView::sceneRectChanged(QRectF new_rect)
{
    double adjust = 4000;
    QRectF adjusted_rect = new_rect.adjusted(-adjust, -adjust, adjust, adjust);
    this->setSceneRect( adjusted_rect );

    //Dr::SetLabelText(Label_Names::Label_2,
    //                 "VRect X: " + QString::number(round(adjusted_rect.x())) +     ", Y: " + QString::number(round(adjusted_rect.y())) +
    //                     ", W: " + QString::number(round(adjusted_rect.width())) + ", H: " + QString::number(round(adjusted_rect.height())));

    updateSelectionBoundingBox(1);
    updateGrid();
}
void DrView::setViewRect(QRectF new_rect) { this->setSceneRect(new_rect); }

// SLOTs: Connected from scene().changed and scene().selectionChanged
void DrView::sceneChanged(QList<QRectF>) { updateSelectionBoundingBox(7); }
void DrView::selectionChanged()          { updateSelectionBoundingBox(2);}

// EVENT: Called when viewport is dragged or scrollbars are used
void DrView::scrollContentsBy(int dx, int dy)
{
    QGraphicsView::scrollContentsBy(dx, dy);
    updateSelectionBoundingBox(3);
    updateGrid();                       // Updates grid when view is dragged or zoomed, also called from zoom
                                        // function (zoomInOut()) in case there are no scrollbars due to zoomed too far out
}


//####################################################################################
//##        Recenters view when window is resized (after FormMain has loaded and view
//##            has shown a scene at least once)
//####################################################################################
void DrView::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);

    if (!Dr::CheckDoneLoading()) return;
    if (!my_scene) return;
    if (!my_scene->getCurrentStageShown()) return;
    if (!hasShownAScene()) return;

    m_editor_relay->centerViewOnPoint( my_scene->getCurrentStageShown()->getViewCenterPoint() );
}


//####################################################################################
//##        Returns current view mode as QString
//####################################################################################
QString DrView::currentViewModeAsString()
{
    switch (m_view_mode) {
    case View_Mode::None:               return "None";
    case View_Mode::Disable_Update:     return "Disable Updates";
    case View_Mode::Selecting:          return "Selecting";
    case View_Mode::Resizing:           return "Resizing";
    case View_Mode::Rotating:           return "Rotating";
    case View_Mode::Translating:        return "Translating";
    case View_Mode::Dragging:           return "Dragging";
    case View_Mode::Zooming:            return "Zooming";
    }
}


//####################################################################################
//##        Recalculates corner and sides handles,
//##        Usually called after View or Item changes
//####################################################################################
void DrView::updateSelectionBoundingBox(int called_from)
{
    // Test for scene, convert to our custom class and lock the scene
    if (scene() == nullptr) return;
    if (m_hide_bounding == true) return;

    QRectF           rect =      my_scene->getSelectionBox().normalized();
    QTransform       transform = my_scene->getSelectionTransform();

    if (my_scene->getSelectionCount() < 1) return;
    if (my_scene->scene_mutex.tryLock(0) == false) return;

    // !!!!! DEBUG: Shows where the call to the UpdateSelectionBoxData function originated from
    if (Dr::CheckDebugFlag(Debug_Flags::Label_Where_Update_Box_From)) {
        Dr::SetLabelText(Label_Names::Label_2, "SB From: " + QString::number(called_from) + ", " + Dr::CurrentTimeAsString() );
    }
    // !!!!! END

    // Size of side handle boxes
    double side_size = 10;
    double s_half = side_size / 2;

    // Size of corner handle boxes
    double corner_size = 14;

    // Check if bounding box handles should be squares or circles
    double angle = my_scene->getSelectionAngle();
    if ( my_scene->shouldEnableResizeToGrid() &&
         m_grid_resize_snap )   m_handles_shape = Handle_Shapes::Squares;
    else                        m_handles_shape = Handle_Shapes::Circles;



    // ***** Store corner handle polygons
    QPointF top_left =  transform.map(rect.topLeft());
    QPointF top_right = transform.map(rect.topRight());
    QPointF bot_left =  transform.map(rect.bottomLeft());
    QPointF bot_right = transform.map(rect.bottomRight());
    QPointF center =    transform.map(rect.center());
    QTransform remove_rotation = QTransform().translate(center.x(), center.y()).rotate(-angle).translate(-center.x(), -center.y());

    // ***** Store view coodinate rectangles of corners for size grip handles
    m_handles[Position_Flags::Top_Left] =     rectAtCenterPoint( mapFromScene( top_left  ), corner_size);
    m_handles[Position_Flags::Top_Right] =    rectAtCenterPoint( mapFromScene( top_right ), corner_size);
    m_handles[Position_Flags::Bottom_Left] =  rectAtCenterPoint( mapFromScene( bot_left  ), corner_size);
    m_handles[Position_Flags::Bottom_Right] = rectAtCenterPoint( mapFromScene( bot_right ), corner_size);
    m_handles[Position_Flags::Center] =       rectAtCenterPoint( mapFromScene( center    ), corner_size);

    // ***** Remove rotation from bounding box in scene, map bounding box scene coordinates to view coordinates
    top_left =  mapFromScene( remove_rotation.map(top_left) );
    top_right=  mapFromScene( remove_rotation.map(top_right) );
    bot_left =  mapFromScene( remove_rotation.map(bot_left) );
    bot_right = mapFromScene( remove_rotation.map(bot_right) );

    // ***** Create bounding box rectangles
    QRectF temp_top    ( QPointF(top_left.x() + s_half,  top_left.y() - s_half),  QPointF(top_right.x() - s_half, top_right.y() + s_half));
    QRectF temp_bottom ( QPointF(bot_left.x() + s_half,  bot_left.y() - s_half),  QPointF(bot_right.x() - s_half, bot_right.y() + s_half));
    QRectF temp_left   ( QPointF(top_left.x() - s_half,  top_left.y() + s_half),  QPointF(bot_left.x() + s_half,  bot_left.y() - s_half));
    QRectF temp_right  ( QPointF(top_right.x() - s_half, top_right.y() + s_half), QPointF(bot_right.x() + s_half, bot_right.y() - s_half));

    // ***** Add rotation back onto side boxes
    center = QLineF(top_left, bot_right).pointAt(.5);
    QTransform add_rotation =    QTransform().translate(center.x(), center.y()).rotate( angle).translate(-center.x(), -center.y());
    m_handles[Position_Flags::Top] =    add_rotation.map(temp_top);
    m_handles[Position_Flags::Bottom] = add_rotation.map(temp_bottom);
    m_handles[Position_Flags::Left] =   add_rotation.map(temp_left);
    m_handles[Position_Flags::Right] =  add_rotation.map(temp_right);

    // *****  Store polygon centers for use later in paintHandles
    for (auto h : m_handles)
        m_handles_centers[h.first] = h.second.boundingRect().center();

    // *****  Calculates angles for mouse cursors over sides
    m_handles_angles[Position_Flags::Top] =    QLineF(m_handles_centers[Position_Flags::Top_Left],     m_handles_centers[Position_Flags::Top_Right]).angle();
    m_handles_angles[Position_Flags::Right] =  QLineF(m_handles_centers[Position_Flags::Top_Right],    m_handles_centers[Position_Flags::Bottom_Right]).angle();
    m_handles_angles[Position_Flags::Bottom] = QLineF(m_handles_centers[Position_Flags::Bottom_Right], m_handles_centers[Position_Flags::Bottom_Left]).angle();
    m_handles_angles[Position_Flags::Left] =   QLineF(m_handles_centers[Position_Flags::Bottom_Left],  m_handles_centers[Position_Flags::Top_Left]).angle();

    // Adjust for angle returned by QLineF to match angles we're used in View_Mode::Rotating function
    for (auto &pair : m_handles_angles) pair.second = 360 - pair.second;

    // Calculate angles for mouse cursors over corners
    m_handles_angles[Position_Flags::Top_Right] =    calculateCornerAngle(m_handles_angles[Position_Flags::Right],  m_handles_angles[Position_Flags::Top]);
    m_handles_angles[Position_Flags::Bottom_Right] = calculateCornerAngle(m_handles_angles[Position_Flags::Bottom], m_handles_angles[Position_Flags::Right]);
    m_handles_angles[Position_Flags::Bottom_Left] =  calculateCornerAngle(m_handles_angles[Position_Flags::Left],   m_handles_angles[Position_Flags::Bottom]);
    m_handles_angles[Position_Flags::Top_Left] =     calculateCornerAngle(m_handles_angles[Position_Flags::Top],    m_handles_angles[Position_Flags::Left]);

    // ***** Add handle for rotating
    QPointF scale = my_scene->getSelectionScale();
    QPoint  top = m_handles_centers[Position_Flags::Top].toPoint();
    QPoint  zero = top;

    if ((scale.x() >=0 && scale.y() >= 0) || (scale.x() <= 0 && scale.y() <= 0))
        zero.setY(zero.y() - 22);
    else
        zero.setY(zero.y() + 22);

    QTransform rotate = QTransform().translate(top.x(), top.y()).rotate(m_handles_angles[Position_Flags::Top]).translate(-top.x(), -top.y());
    zero = rotate.map(zero);
    m_handles[Position_Flags::Rotate] = rectAtCenterPoint( zero, corner_size);
    m_handles_centers[Position_Flags::Rotate] = m_handles[Position_Flags::Rotate].boundingRect().center();

    my_scene->scene_mutex.unlock();
}

// Helper function, returns a RectF around center point with sides length of rect_size
QRectF DrView::rectAtCenterPoint(QPoint center, double rect_size)
{
    return QRectF(center.x() - rect_size / 2, center.y() - rect_size / 2, rect_size, rect_size);
}

// Calculates the angle facing away from the corner of two angles, for calculating mouse angle of corners
double DrView::calculateCornerAngle(double angle1, double angle2)
{
    double bigger_angle = angle1;
    if (angle1 < angle2) bigger_angle += 360;

    double new_angle = angle1 - ((bigger_angle - angle2) / 2);
    if (new_angle < 0)   new_angle += 360;
    if (new_angle > 360) new_angle -= 360;
    return new_angle;
}



















