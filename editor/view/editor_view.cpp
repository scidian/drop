//
//      Created by Stephens Nunnally on 1/3/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Graphics View Definitions
//
//
#include <QtMath>

#include "core/colors/colors.h"
#include "editor/helper_library.h"
#include "editor/interface_editor_relay.h"
#include "editor/preferences.h"
#include "editor/view/editor_item.h"
#include "editor/view/editor_view.h"
#include "editor/view/editor_scene.h"
#include "engine/debug_flags.h"
#include "project/dr_project.h"
#include "project/entities/dr_world.h"
#include "project/entities/dr_stage.h"
#include "project/entities/dr_thing.h"
#include "project/settings/settings.h"
#include "project/settings/settings_component.h"
#include "project/settings/settings_component_property.h"


//####################################################################################
//##    Constructor / Destructor
//####################################################################################
EditorView::EditorView(QWidget *parent, DrProject *project, EditorScene *from_scene, IEditorRelay *editor_relay)
    : QGraphicsView(parent), m_project(project), m_editor_relay(editor_relay) {

    // Initialize rubber band object used as a selection box
    m_rubber_band = new EditorViewRubberBand(QRubberBand::Shape::Rectangle, this);

    // Initialize tool tip object used for displaying some helpful info
    m_tool_tip = new EditorViewToolTip(this);
    m_tool_tip->hide();

    m_over_handle = Position_Flags::No_Position;

    if (Dr::CheckDebugFlag(Debug_Flags::Label_FPS))
        m_debug_timer.start();

    my_scene = from_scene;
    setScene(my_scene);


    // ********** Connect signals to scene
    connect(my_scene, SIGNAL(selectionChanged()),       this, SLOT(selectionChanged()));
    connect(my_scene, SIGNAL(sceneRectChanged(QRectF)), this, SLOT(sceneRectChanged(QRectF)));
    connect(my_scene, SIGNAL(changed(QList<QRectF>)),   this, SLOT(sceneChanged(QList<QRectF>)));

    connect(my_scene, &EditorScene::updateGrid,  this, [this]() { updateGrid(); });
    connect(my_scene, &EditorScene::updateViews, this, [this]() { update(); });
    connect(my_scene, &EditorScene::setViewRect, this, [this](QRectF new_rect) { setViewRect(new_rect); });

    connect(this,   SIGNAL(selectionGroupMoved(EditorScene*, QPointF)),
            my_scene, SLOT(selectionGroupMoved(EditorScene*, QPointF)));

    connect(this,   SIGNAL(selectionGroupNewGroup(EditorScene*, QList<DrThing*>, QList<DrThing*>)),
            my_scene, SLOT(selectionGroupNewGroup(EditorScene*, QList<DrThing*>, QList<DrThing*>)) );

}


EditorView::~EditorView() {
    delete m_rubber_band;
    delete m_tool_tip;
}

void EditorView::focusInEvent(QFocusEvent *event) {
    m_editor_relay->setActiveWidget(Editor_Widgets::Stage_View);
    QGraphicsView::focusInEvent(event);
}



//####################################################################################
//##    Scene Change SLOTs / Events to update selection box when scene / selection changes
//####################################################################################
// SLOT: Connected from scene().sceneRectChanged
void EditorView::sceneRectChanged(QRectF new_rect) {
    double adjust = 4000;
    QRectF adjusted_rect = new_rect.adjusted(-adjust, -adjust, adjust, adjust);
    this->setSceneRect( adjusted_rect );

    //Dr::SetLabelText(Label_Names::Label_2,
    //                 "VRect X: " + QString::number(round(adjusted_rect.x())) +     ", Y: " + QString::number(round(adjusted_rect.y())) +
    //                     ", W: " + QString::number(round(adjusted_rect.width())) + ", H: " + QString::number(round(adjusted_rect.height())));

    updateSelectionBoundingBox(1);
    updateGrid();
}
void EditorView::setViewRect(QRectF new_rect)   { this->setSceneRect(new_rect); }

// SLOTs: Connected from scene().changed and scene().selectionChanged
void EditorView::sceneChanged(QList<QRectF>)    { updateSelectionBoundingBox(7); }
void EditorView::selectionChanged()             { updateSelectionBoundingBox(2); }

// EVENT: Called when viewport is dragged or scrollbars are used
void EditorView::scrollContentsBy(int dx, int dy) {
    QGraphicsView::scrollContentsBy(dx, dy);
    updateSelectionBoundingBox(3);
    updateGrid();                               // Updates grid when view is dragged or zoomed, also called from zoom
                                                // function (zoomInOut()) in case there are no scrollbars due to zoomed too far out
}


//####################################################################################
//##    Recenters view when window is resized (after FormMain has loaded and view
//##    has shown a scene at least once)
//####################################################################################
void EditorView::resizeEvent(QResizeEvent *event) {
    QGraphicsView::resizeEvent(event);

    if (Dr::CheckDoneLoading() == false) return;
    if (my_scene == nullptr) return;
    if (my_scene->getCurrentStageShown() == nullptr) return;
    if (hasShownAScene() == false) return;

    m_editor_relay->viewCenterOnPoint( Dr::ToQPointF(my_scene->getCurrentStageShown()->getViewCenterPoint()) );
}


//####################################################################################
//##    Returns current view mode as QString
//####################################################################################
QString EditorView::currentViewModeAsString() {
    switch (m_view_mode) {
        case View_Mode::None:               return "None";
        case View_Mode::Disable_Update:     return "Disable Updates";
        case View_Mode::Selecting:          return "Selecting";
        case View_Mode::Resizing:           return "Resizing";
        case View_Mode::Rotating:           return "Rotating";
        case View_Mode::Translating:        return "Translating";
        case View_Mode::Dragging:           return "Dragging";
        case View_Mode::Zooming:            return "Zooming";
        case View_Mode::Holding_Keys:       return "Holding Keys";
        case View_Mode::Moving_Camera:      return "Moving Camera";
        case View_Mode::Resizing_Stage:     return "Resizing Stage";
    }
    return "Mode not handled...";
}


//####################################################################################
//##    Recalculates corner and sides handles,
//##    Usually called after View or Item changes
//####################################################################################
void EditorView::updateSelectionBoundingBox(int called_from) {
    // Test for scene, convert to our custom class and lock the scene
    if (scene() == nullptr) return;
    if (m_hide_bounding == true) return;

    QRectF           rect =      my_scene->getSelectionBox().normalized();
    QTransform       transform = my_scene->getSelectionTransform();

    if (my_scene->getSelectionCount() < 1) return;
    if (my_scene->scene_mutex.tryLock(0) == false) return;

    // !!!!! #DEBUG: Shows where the call to the UpdateSelectionBoxData function originated from
    if (Dr::CheckDebugFlag(Debug_Flags::Label_Where_Update_Box_From)) {
        Dr::SetLabelText(Label_Names::Label_2, "SB From: " + QString::number(called_from) + ", " + Dr::CurrentTimeAsString() );
    }
    // !!!!! END

    // Size of side handle boxes (left, top, right, bottom)
    double side_size = 10;
    double s_half = side_size / 2;

    // Size of corner handle boxes (top left, top right, bottom left, bottom right
    double corner_size = 14;

    // Check if bounding box handles should be squares or circles
    double angle = my_scene->getSelectionAngle();
    if ( my_scene->shouldEnableResizeToGrid() &&
         m_grid_resize_snap )   m_handles_shape = Handle_Shapes::Squares;
    else                        m_handles_shape = Handle_Shapes::Circles;


    // ***** Store selection box points in scene coordinates
    m_selection_points[Position_Flags::Top_Left] =     transform.map(rect.topLeft());
    m_selection_points[Position_Flags::Top_Right] =    transform.map(rect.topRight());
    m_selection_points[Position_Flags::Bottom_Left] =  transform.map(rect.bottomLeft());
    m_selection_points[Position_Flags::Bottom_Right] = transform.map(rect.bottomRight());
    m_selection_points[Position_Flags::Center] =       transform.map(rect.center());
    m_selection_points[Position_Flags::Top] =          transform.map(QPointF( rect.center().x(), rect.top()    ));
    m_selection_points[Position_Flags::Bottom] =       transform.map(QPointF( rect.center().x(), rect.bottom() ));
    m_selection_points[Position_Flags::Left] =         transform.map(QPointF( rect.left(),  rect.center().y()  ));
    m_selection_points[Position_Flags::Right] =        transform.map(QPointF( rect.right(), rect.center().y()  ));

    // Starting positions
    QPointF top_left =  m_selection_points[Position_Flags::Top_Left];
    QPointF top_right = m_selection_points[Position_Flags::Top_Right];
    QPointF bot_left =  m_selection_points[Position_Flags::Bottom_Left];
    QPointF bot_right = m_selection_points[Position_Flags::Bottom_Right];
    QPointF center =    m_selection_points[Position_Flags::Center];


    // ***** Store view coodinate rectangles of corners for size grip handles
    m_handles[Position_Flags::Top_Left] =     rectAtCenterPoint( mapFromScene( top_left  ), corner_size);
    m_handles[Position_Flags::Top_Right] =    rectAtCenterPoint( mapFromScene( top_right ), corner_size);
    m_handles[Position_Flags::Bottom_Left] =  rectAtCenterPoint( mapFromScene( bot_left  ), corner_size);
    m_handles[Position_Flags::Bottom_Right] = rectAtCenterPoint( mapFromScene( bot_right ), corner_size);
    m_handles[Position_Flags::Center] =       rectAtCenterPoint( mapFromScene( center    ), corner_size);

    // ***** Remove rotation from bounding box in scene, map bounding box scene coordinates to view coordinates
    QTransform remove_rotation = Dr::CreateRotatedQTransform(center, -angle);
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
    QTransform add_rotation = Dr::CreateRotatedQTransform(center, angle);
    m_handles[Position_Flags::Top] =    add_rotation.map(temp_top);
    m_handles[Position_Flags::Bottom] = add_rotation.map(temp_bottom);
    m_handles[Position_Flags::Left] =   add_rotation.map(temp_left);
    m_handles[Position_Flags::Right] =  add_rotation.map(temp_right);

    // *****  Store polygon centers for use later in paintHandles
    for (auto h : m_handles) {
        m_handles_centers[h.first] = h.second.boundingRect().center();
    }

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

    QTransform rotate = Dr::CreateRotatedQTransform(top, m_handles_angles[Position_Flags::Top]);
    zero = rotate.map(zero);
    m_handles[Position_Flags::Rotate] = rectAtCenterPoint( zero, corner_size);
    m_handles_centers[Position_Flags::Rotate] = m_handles[Position_Flags::Rotate].boundingRect().center();


    // ***** If we're resizing we now know the new size, update the tooltip with it
    if (m_view_mode == View_Mode::Resizing) {
        if (m_tool_tip->getTipType() == View_Mode::Resizing) {
            double group_width =  QLineF( m_selection_points[Position_Flags::Left], m_selection_points[Position_Flags::Right] ).length();
            double group_height = QLineF( m_selection_points[Position_Flags::Top],  m_selection_points[Position_Flags::Bottom]).length();
            m_tool_tip->updateToolTipData( QPointF( group_width, group_height ));
        }
    }

    my_scene->scene_mutex.unlock();
}

// Helper function, returns a RectF around center point with sides length of rect_size
QRectF EditorView::rectAtCenterPoint(QPoint center, double rect_size) {
    return QRectF(center.x() - rect_size / 2, center.y() - rect_size / 2, rect_size, rect_size);
}

// Calculates the angle facing away from the corner of two angles, for calculating mouse angle of corners
double EditorView::calculateCornerAngle(double angle1, double angle2) {
    double bigger_angle = angle1;
    if (angle1 < angle2) bigger_angle += 360;

    double new_angle = angle1 - ((bigger_angle - angle2) / 2);
    if (new_angle < 0)   new_angle += 360;
    if (new_angle > 360) new_angle -= 360;
    return new_angle;
}



















