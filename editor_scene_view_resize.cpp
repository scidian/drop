//
//      Created by Stephens Nunnally on 1/8/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Graphics View Handling - View_Mode::Resizing
//
//

#include <math.h>

#include "project.h"
#include "project_world.h"
#include "project_world_scene.h"
#include "project_world_scene_object.h"
#include "editor_scene_item.h"

#include "settings.h"
#include "settings_component.h"
#include "settings_component_property.h"

#include "editor_scene_scene.h"
#include "editor_scene_view.h"
#include "interface_relay.h"


//####################################################################################
//##        Starts resizing mode
//####################################################################################
void SceneGraphicsView::startResize(QPoint mouse_in_view)
{
    m_start_resize_grip = m_over_handle;                        // Store grip handle we start resize event with
    m_start_resize_rect = totalSelectedItemsSceneRect();        // Store starting scene rect of initial selection bounding box

    SceneGraphicsScene *my_scene = dynamic_cast<SceneGraphicsScene *>(scene());
    QGraphicsItem *item = my_scene->getSelectionGroupAsGraphicsItem();
    m_pre_resize_scale = item->data(User_Roles::Scale).toPointF();

    QTransform t = item->sceneTransform();
    QRectF     r = item->boundingRect();
    m_pre_resize_corners[Position_Flags::Top_Left] =     t.map( r.topLeft() );
    m_pre_resize_corners[Position_Flags::Top_Right] =    t.map( r.topRight() );
    m_pre_resize_corners[Position_Flags::Bottom_Left] =  t.map( r.bottomLeft() );
    m_pre_resize_corners[Position_Flags::Bottom_Right] = t.map( r.bottomRight() );
    m_pre_resize_corners[Position_Flags::Center] =       t.map( r.center() );
    m_pre_resize_corners[Position_Flags::Top] =          t.map( QLineF(r.topLeft(), r.topRight()).pointAt(.5) );
    m_pre_resize_corners[Position_Flags::Bottom] =       t.map( QLineF(r.bottomLeft(), r.bottomRight()).pointAt(.5));
    m_pre_resize_corners[Position_Flags::Left] =         t.map( QLineF(r.topLeft(), r.bottomLeft()).pointAt(.5) );
    m_pre_resize_corners[Position_Flags::Right] =        t.map( QLineF(r.topRight(), r.bottomRight()).pointAt(.5) );

    // Set up our custom tool tip
    m_tool_tip->startToolTip(m_view_mode, mouse_in_view, QPointF( m_start_resize_rect.width(), m_start_resize_rect.height() ));
}


//####################################################################################
//##        Call appropriate resize function
//####################################################################################
void SceneGraphicsView::resizeSelection(QPointF mouse_in_scene)
{
    // Figure out what sides to use for x axis and y axis
    switch (m_start_resize_grip) {
    case Position_Flags::Top_Left:      m_do_x = X_Axis::Left;    m_do_y = Y_Axis::Top;     break;
    case Position_Flags::Top:           m_do_x = X_Axis::None;    m_do_y = Y_Axis::Top;     break;
    case Position_Flags::Top_Right:     m_do_x = X_Axis::Right;   m_do_y = Y_Axis::Top;     break;
    case Position_Flags::Right:         m_do_x = X_Axis::Right;   m_do_y = Y_Axis::None;    break;
    case Position_Flags::Bottom_Right:  m_do_x = X_Axis::Right;   m_do_y = Y_Axis::Bottom;  break;
    case Position_Flags::Bottom:        m_do_x = X_Axis::None;    m_do_y = Y_Axis::Bottom;  break;
    case Position_Flags::Bottom_Left:   m_do_x = X_Axis::Left;    m_do_y = Y_Axis::Bottom;  break;
    case Position_Flags::Left:          m_do_x = X_Axis::Left;    m_do_y = Y_Axis::None;    break;
    default:                            m_do_x = X_Axis::None;    m_do_y = Y_Axis::None;
    }

    resizeSelectionWithRotate(mouse_in_scene);
}


// Finds the ooposite side name of side passed in, no error catch if corner passed in
Position_Flags SceneGraphicsView::findOppositeSide(Position_Flags start_side)
{
    switch (start_side)
    {
    case Position_Flags::Top:          return Position_Flags::Bottom;
    case Position_Flags::Bottom:       return Position_Flags::Top;
    case Position_Flags::Left:         return Position_Flags::Right;
    case Position_Flags::Right:        return Position_Flags::Left;
    case Position_Flags::Top_Left:     return Position_Flags::Bottom_Right;
    case Position_Flags::Top_Right:    return Position_Flags::Bottom_Left;
    case Position_Flags::Bottom_Left:  return Position_Flags::Top_Right;
    case Position_Flags::Bottom_Right: return Position_Flags::Top_Left;
    default:    return Position_Flags::Top;
    }
}



//####################################################################################
//##        Main resize function
//####################################################################################
void SceneGraphicsView::resizeSelectionWithRotate(QPointF mouse_in_scene)
{
    // Test for scene, convert to our custom class
    if (scene() == nullptr) return;
    SceneGraphicsScene    *my_scene = dynamic_cast<SceneGraphicsScene *>(scene());

    // Load item starting width and height
    QGraphicsItem *item = my_scene->getSelectionGroupAsGraphicsItem();
    qreal item_width =  item->boundingRect().width();
    qreal item_height = item->boundingRect().height();

    // ********** Find corners / sides we're working with, calculate new width / height
    QPointF corner_start =    m_handles_centers[static_cast<Position_Flags>(m_over_handle)];
    QPointF corner_opposite = m_handles_centers[findOppositeSide(m_over_handle)];

    // Find center point, load angle of item, load original scale
    QPointF center_point = QLineF(corner_start, corner_opposite).pointAt(.5);
    double  angle = item->data(User_Roles::Rotation).toDouble();

    // Create transform to rotate center line back to zero
    QTransform remove_rotation;
    remove_rotation.translate(center_point.x(), center_point.y());
    remove_rotation.rotate(-angle);
    remove_rotation.translate(-center_point.x(), -center_point.y());

    // Rotate back to zero degree shape angle, starts in view coordinates - ends up in scene coords
    QPointF zero_rotated_opposite = remove_rotation.map( corner_opposite );
    QPointF zero_rotated_opposite_in_scene = mapToScene(zero_rotated_opposite.toPoint());


    QPointF point_in_shape = mapToScene(remove_rotation.map(mapFromScene(mouse_in_scene)));
    qreal   scale_x,   scale_y;

    // ***** Calculate X scale
    if (m_do_x == X_Axis::Right)
        scale_x = QLineF(zero_rotated_opposite_in_scene, point_in_shape).dx() / item_width;
    else if (m_do_x == X_Axis::Left)
        scale_x = QLineF(point_in_shape, zero_rotated_opposite_in_scene).dx() / item_width;
    else        
        scale_x = m_pre_resize_scale.x();

    // ***** Calculate Y scale
    if (m_do_y == Y_Axis::Bottom)
        scale_y = QLineF(zero_rotated_opposite_in_scene, point_in_shape).dy() / item_height;
    else if (m_do_y == Y_Axis::Top)
        scale_y = QLineF(point_in_shape, zero_rotated_opposite_in_scene).dy() / item_height;
    else
        scale_y = m_pre_resize_scale.y();


    // Make sure it doesnt disappear off the screen, Qt doesnt like when items scale go to zero
    if (scale_x <  .0001 && scale_x >= 0) scale_x =  .0001;
    if (scale_x > -.0001 && scale_x <= 0) scale_x = -.0001;
    if (scale_y <  .0001 && scale_y >= 0) scale_y =  .0001;
    if (scale_y > -.0001 && scale_y <= 0) scale_y = -.0001;

    // If shift or control keys are down, maintain starting aspect ratio
    if (m_flag_key_down_shift) {
        double pre_resize_ratio;
        if (m_do_y == Y_Axis::None) {
            pre_resize_ratio = m_pre_resize_scale.y() / m_pre_resize_scale.x();
            scale_y = scale_x * pre_resize_ratio;
        } else {
            pre_resize_ratio = m_pre_resize_scale.x() / m_pre_resize_scale.y();
            scale_x = scale_y * pre_resize_ratio;
        }
    }


    // ***** Apply new scale
    QTransform t = QTransform().rotate(angle).scale(scale_x, scale_y);
    item->setTransform(t);

    // ***** Translate if needed
    QPointF new_pos = item->pos();
    Position_Flags resize_flag = Position_Flags::Top_Left;

    if (m_flag_key_down_control)
        resize_flag = Position_Flags::Center;

    else if (m_do_x == X_Axis::Left && m_do_y == Y_Axis::None)
        resize_flag = Position_Flags::Right;
    else if (m_do_x == X_Axis::Left && m_do_y == Y_Axis::Top)
        resize_flag = Position_Flags::Bottom_Right;
    else if (m_do_x == X_Axis::Left && m_do_y == Y_Axis::Bottom)
        resize_flag = Position_Flags::Top_Right;

    else if (m_do_x == X_Axis::Right && m_do_y == Y_Axis::None)
        resize_flag = Position_Flags::Left;
    else if (m_do_x == X_Axis::Right && m_do_y == Y_Axis::Top)
        resize_flag = Position_Flags::Bottom_Left;
    else if (m_do_x == X_Axis::Right && m_do_y == Y_Axis::Bottom)
        resize_flag = Position_Flags::Top_Left;

    else if (m_do_x == X_Axis::None && m_do_y == Y_Axis::Top)
        resize_flag = Position_Flags::Bottom;
    else if (m_do_x == X_Axis::None && m_do_y == Y_Axis::Bottom)
        resize_flag = Position_Flags::Top;

    new_pos = m_pre_resize_corners[resize_flag] ;
    my_scene->setPositionByOrigin(item, resize_flag, new_pos.x(), new_pos.y());



    // Remove any shearing that may have been caused to all items in selection group
    QList<QGraphicsItem*> my_items = my_scene->getSelectionGroupItems();
    for (auto child : my_items) my_scene->getSelectionGroup()->removeFromGroup(child);
    for (auto child : my_items) removeShearing(child);
    for (auto child : my_items) my_scene->getSelectionGroup()->addToGroup(child);

    // Update stored scale User_Role property
    QPointF group_scale = extractScaleFromItem(item);
    item->setData(User_Roles::Scale, QPointF(group_scale.x(), group_scale.y()) );

    // Update tool tip
    double group_width =  QLineF( mapToScene(m_handles_centers[Position_Flags::Left].toPoint()),
                                  mapToScene(m_handles_centers[Position_Flags::Right].toPoint()) ).length();
    double group_height = QLineF( mapToScene(m_handles_centers[Position_Flags::Top].toPoint()),
                                  mapToScene(m_handles_centers[Position_Flags::Bottom].toPoint()) ).length();
    m_tool_tip->updateToolTipData( QPointF( group_width, group_height ));
}


//####################################################################################
//##        Remove shearing from item
//####################################################################################
void SceneGraphicsView::removeShearing(QGraphicsItem *item)
{
    double  angle = item->data(User_Roles::Rotation).toDouble();
    QPointF scale = extractScaleFromItem(item);

    QTransform no_skew = QTransform().rotate(angle).scale(scale.x(), scale.y());
    item->setTransform(no_skew);

    item->setData(User_Roles::Scale, QPointF(scale.x(), scale.y()) );
}

// Pulls out scale from current item screen transform (takes away angle first from stored angle data)
QPointF SceneGraphicsView::extractScaleFromItem(QGraphicsItem *item)
{
    double  angle = item->data(User_Roles::Rotation).toDouble();
    QPointF origin = item->mapToScene( item->boundingRect().center() );

    QTransform remove_rotation = QTransform().translate(origin.x(), origin.y()).rotate(-angle).translate(-origin.x(), -origin.y());
    QTransform item_no_rotate = item->sceneTransform() * remove_rotation;

    return QPointF(item_no_rotate.m11(), item_no_rotate.m22());
}















