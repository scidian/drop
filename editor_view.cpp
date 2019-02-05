//
//      Created by Stephens Nunnally on 1/3/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Graphics View Definitions
//
//

#include "project.h"
#include "project_world.h"
#include "project_world_stage.h"
#include "project_world_stage_object.h"
#include "editor_item.h"

#include "settings.h"
#include "settings_component.h"
#include "settings_component_property.h"

#include "editor_view.h"
#include "editor_scene.h"
#include "interface_relay.h"
#include "library.h"


//####################################################################################
//##        Constructor & destructor
//####################################################################################
DrView::DrView(QWidget *parent, DrProject *project, DrScene *my_scene, InterfaceRelay *relay) :
               QGraphicsView(parent = nullptr), m_project(project), m_relay(relay)
{
    // Initialize rubber band object used as a selection box
    m_rubber_band = new DrViewRubberBand(QRubberBand::Shape::Rectangle, this);

    // Initialize tool tip object used for displaying some helpful info
    m_tool_tip = new DrViewToolTip(this);
    m_tool_tip->hide();

    m_over_handle = Position_Flags::No_Position;

    if (Dr::CheckDebugFlag(Debug_Flags::Label_FPS))
        m_debug_timer.start();



    // ********** Connect signals to scene
    connect(my_scene, SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));
    connect(my_scene, SIGNAL(changed(QList<QRectF>)), this, SLOT(sceneChanged(QList<QRectF>)));

    connect(my_scene, &DrScene::updateViews, this, [this]() { update(); });

    connect(this,   SIGNAL(selectionGroupMoved(DrScene*, QPointF)),
            my_scene, SLOT(selectionGroupMoved(DrScene*, QPointF)));

    connect(this,   SIGNAL(selectionGroupNewGroup(DrScene*, QList<DrObject*>, QList<DrObject*>)),
            my_scene, SLOT(selectionGroupNewGroup(DrScene*, QList<DrObject*>, QList<DrObject*>)) );

}


DrView::~DrView()
{
    // ********** Disconnect signals from scene
    DrScene *my_scene = dynamic_cast<DrScene*>(scene());

    disconnect(my_scene, SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));
    disconnect(my_scene, SIGNAL(changed(QList<QRectF>)), this, SLOT(sceneChanged(QList<QRectF>)));

    disconnect(my_scene, &DrScene::updateViews, this, nullptr);

    disconnect(this,   SIGNAL(selectionGroupMoved(DrScene*, QPointF)),
               my_scene, SLOT(selectionGroupMoved(DrScene*, QPointF)));

    disconnect(this,   SIGNAL(selectionGroupNewGroup(DrScene*, QList<DrObject*>, QList<DrObject*>)),
               my_scene, SLOT(selectionGroupNewGroup(DrScene*, QList<DrObject*>, QList<DrObject*>)) );

    delete m_tool_tip;
}




//####################################################################################
//##        Key Events
//####################################################################################
// Key press event
void DrView::keyPressEvent(QKeyEvent *event)
{
    // When space bar is down, enabled mouse press and move to translate viewable area
    if (event->key() == Qt::Key::Key_Space) {   m_flag_key_down_spacebar = true;
        setDragMode(QGraphicsView::DragMode::ScrollHandDrag);
        setInteractive(false);
    }

    // Store when control / alt are down
    if (event->key() == Qt::Key::Key_Control) { m_flag_key_down_control = true; }
    if (event->key() == Qt::Key::Key_Alt) {     m_flag_key_down_alt = true; }
    if (event->key() == Qt::Key::Key_Shift) {   m_flag_key_down_shift = true; }

    QGraphicsView::keyPressEvent(event);

    // Fire mouse move event to update mouse cursor on key presses
    QMouseEvent *fire_mouse_move = new QMouseEvent(QEvent::MouseMove, m_last_mouse_pos, Qt::NoButton,
                                                   Qt::MouseButtons(), Qt::KeyboardModifiers());
    mouseMoveEvent(fire_mouse_move);
}

// Key release event
void DrView::keyReleaseEvent(QKeyEvent *event)
{
    // When space bar is released, change mode back to select / move items
    if (event->key() == Qt::Key::Key_Space) {   m_flag_key_down_spacebar = false;
        setDragMode(QGraphicsView::DragMode::NoDrag);
        setInteractive(true);
    }

    // Store when control / alt are released
    if (event->key() == Qt::Key::Key_Control) { m_flag_key_down_control = false; }
    if (event->key() == Qt::Key::Key_Alt) {     m_flag_key_down_alt = false; }
    if (event->key() == Qt::Key::Key_Shift) {   m_flag_key_down_shift = false; }

    QGraphicsView::keyReleaseEvent(event);

    // Fire mouse move event to update mouse cursor on key release
    QMouseEvent *fire_mouse_move = new QMouseEvent(QEvent::MouseMove, m_last_mouse_pos, Qt::NoButton,
                                                   Qt::MouseButtons(), Qt::KeyboardModifiers());
    mouseMoveEvent(fire_mouse_move);
}




//####################################################################################
//##        Scene Change SLOTs / Events to update selection box when scene / selection changes
//####################################################################################
// Connected from scene().changed
void DrView::sceneChanged(QList<QRectF>)
{
    double left_adjust =  -4000;
    double right_adjust =  4000;
    double top_adjust =   -4000;
    double bottom_adjust = 4000;
    this->setSceneRect( scene()->sceneRect().adjusted(left_adjust, top_adjust, right_adjust, bottom_adjust) );

    updateSelectionBoundingBox();
    ///update();             // Don't use here!!!!! Calls paint recursively
}

// Connected from scene().selectionChanged
void DrView::selectionChanged()
{
    updateSelectionBoundingBox();
    ///update();             // Don't use here!!!!! Calls paint recursively
}

void DrView::scrollContentsBy(int dx, int dy)
{
    QGraphicsView::scrollContentsBy(dx, dy);
    updateSelectionBoundingBox();
    update();
}

void DrView::updateSelectionBoundingBox()
{
    // Test for scene, convert to our custom class and lock the scene
    if (scene() == nullptr) return;

    DrScene         *my_scene =  dynamic_cast<DrScene*>(scene());
    QRectF           rect =      my_scene->getSelectionBox().normalized();
    QTransform       transform = my_scene->getSelectionTransform();

    if (my_scene->selectedItems().count() < 1) return;
    if (my_scene->scene_mutex.tryLock(0) == false) return;


    // Size of side handle boxes
    double side_size = 10;
    double s_half = side_size / 2;

    // Size of corner handle boxes
    double corner_size = 14;

    // Check if bounding box handles should be squares or circles
    double angle = my_scene->getSelectionAngle();
    if (isSquare(angle) == false)  m_handles_shape = Handle_Shapes::Circles;
    else                           m_handles_shape = Handle_Shapes::Squares;

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
    for (auto h : m_handles) m_handles_centers[h.first] = h.second.boundingRect().center();

    // *****  Calculates angles for mouse cursors over sides
    m_handles_angles[Position_Flags::Top] = QLineF(m_handles_centers[Position_Flags::Top_Left], m_handles_centers[Position_Flags::Top_Right]).angle();
    m_handles_angles[Position_Flags::Right] = QLineF(m_handles_centers[Position_Flags::Top_Right], m_handles_centers[Position_Flags::Bottom_Right]).angle();
    m_handles_angles[Position_Flags::Bottom] = QLineF(m_handles_centers[Position_Flags::Bottom_Right], m_handles_centers[Position_Flags::Bottom_Left]).angle();
    m_handles_angles[Position_Flags::Left] = QLineF(m_handles_centers[Position_Flags::Bottom_Left], m_handles_centers[Position_Flags::Top_Left]).angle();

    // Adjust for angle returned by QLineF to match angles we're used in View_Mode::Rotating function
    for (auto &pair : m_handles_angles) pair.second = 360 - pair.second;

    // Calculate angles for mouse cursors over corners
    m_handles_angles[Position_Flags::Top_Right] =    calculateCornerAngle(m_handles_angles[Position_Flags::Right], m_handles_angles[Position_Flags::Top]);
    m_handles_angles[Position_Flags::Bottom_Right] = calculateCornerAngle(m_handles_angles[Position_Flags::Bottom], m_handles_angles[Position_Flags::Right]);
    m_handles_angles[Position_Flags::Bottom_Left] =  calculateCornerAngle(m_handles_angles[Position_Flags::Left], m_handles_angles[Position_Flags::Bottom]);
    m_handles_angles[Position_Flags::Top_Left] =     calculateCornerAngle(m_handles_angles[Position_Flags::Top], m_handles_angles[Position_Flags::Left]);

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



















