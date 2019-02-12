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
#include "project_world_stage.h"
#include "project_world_stage_object.h"
#include "editor_item.h"

#include "settings.h"
#include "settings_component.h"
#include "settings_component_property.h"

#include "editor_scene.h"
#include "editor_view.h"
#include "interface_relay.h"


//####################################################################################
//##        Starts resizing mode
//####################################################################################

void DrView::startResize(QPoint mouse_in_view)
{
    // Figure out starting orientation
    m_pre_resize_scale =  QPointF(1, 1);
    if (my_scene->getSelectionScale().x() < 0) m_pre_resize_scale.setX(-1);
    if (my_scene->getSelectionScale().y() < 0) m_pre_resize_scale.setY(-1);

    // Create a graphics item group with a copy of selected items to use for resizing
    double angle = my_scene->getSelectionAngle();
    m_group = my_scene->createEmptyItemGroup(angle, m_pre_resize_scale);
    for (auto child : my_scene->getSelectionItems()) {
        child->setData(User_Roles::Pre_Resize_Scale, (child->data(User_Roles::Scale).toPointF()) );
        DrItem *child_as_item = dynamic_cast<DrItem*>(child);
        DrItem *dritem = new DrItem(m_project, child_as_item->getObject(), true);
        dritem->setVisible(false);
        dritem->setEnabled(false);
        dritem->setPos(child_as_item->pos());
        dritem->setTransform(child_as_item->transform());
        m_group->addToGroup(dritem);
    }
    m_group->setEnabled(false);
    m_group->setVisible(false);

    // Store which grip handle we started over and the initial slection bounding box
    m_start_resize_grip = m_over_handle;
    m_start_resize_rect = my_scene->totalSelectionSceneRect();

    // Load selection box corners in scene coordinates
    QTransform t = my_scene->getSelectionTransform();
    QRectF     r = my_scene->getSelectionBox().normalized();
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
    double group_width =  QLineF( mapToScene(m_handles_centers[Position_Flags::Left].toPoint()),
                                  mapToScene(m_handles_centers[Position_Flags::Right].toPoint()) ).length();
    double group_height = QLineF( mapToScene(m_handles_centers[Position_Flags::Top].toPoint()),
                                  mapToScene(m_handles_centers[Position_Flags::Bottom].toPoint()) ).length();
    m_tool_tip->startToolTip(m_view_mode, mouse_in_view, QPointF( group_width, group_height ) );
}


//####################################################################################
//##        Call appropriate resize function
//####################################################################################
void DrView::resizeSelection(QPointF mouse_in_scene)
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
Position_Flags DrView::findOppositeSide(Position_Flags start_side)
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
void DrView::resizeSelectionWithRotate(QPointF mouse_in_scene)
{
    // Test for scene, convert to our custom class
    if (scene() == nullptr) return;

    // Load item starting width and height
    qreal item_width =  m_group->boundingRect().width();
    qreal item_height = m_group->boundingRect().height();

    // ********** Find corners / sides we're working with, calculate new width / height
    QPointF corner_start =    m_handles_centers[static_cast<Position_Flags>(m_over_handle)];
    QPointF corner_opposite = m_handles_centers[findOppositeSide(m_over_handle)];

    // Find center point, load angle of item, load original scale
    QPointF center_point = QLineF(corner_start, corner_opposite).pointAt(.5);
    double  angle = my_scene->getSelectionAngle();

    // Create transform to rotate center line back to zero
    QTransform remove_rotation;
    remove_rotation.translate(center_point.x(), center_point.y());
    remove_rotation.rotate(-angle);
    remove_rotation.translate(-center_point.x(), -center_point.y());

    // Rotate back to zero degree shape angle, starts in view coordinates - ends up in scene coords
    QPointF zero_rotated_opposite = remove_rotation.map( corner_opposite );
    QPointF zero_rotated_opposite_in_scene = mapToScene(zero_rotated_opposite.toPoint());


    QPointF point_in_shape = mapToScene(remove_rotation.map(mapFromScene(mouse_in_scene)));
    double  scale_x;
    double  scale_y;

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
    m_group->setTransform(t);

    // ***** Translate if needed
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

    QPointF new_pos = m_pre_resize_corners[resize_flag] ;
    my_scene->setPositionByOrigin(m_group, resize_flag, new_pos.x(), new_pos.y());


    // ***** Remove any shearing
    QPointF new_scale(scale_x, scale_y);
    for (auto child : my_scene->getSelectionItems())
        removeShearing(child, new_scale);


    // ***** Aligns new selected items origin location with proper resize starting point
    Position_Flags origin_flag = resize_flag;
    if (scale_y < 0 && scale_x > 0) {
         switch (origin_flag)
         {
         case Position_Flags::Bottom:        origin_flag = Position_Flags::Top;              break;
         case Position_Flags::Bottom_Right:  origin_flag = Position_Flags::Top_Right;        break;
         case Position_Flags::Bottom_Left:   origin_flag = Position_Flags::Top_Left;         break;
         case Position_Flags::Top:           origin_flag = Position_Flags::Bottom;           break;
         case Position_Flags::Top_Right:     origin_flag = Position_Flags::Bottom_Right;     break;
         case Position_Flags::Top_Left:      origin_flag = Position_Flags::Bottom_Left;      break;
         default: ;

        }
    } else if (scale_y > 0 && scale_x < 0) {
        switch (origin_flag)
        {
        case Position_Flags::Right:         origin_flag = Position_Flags::Left;             break;
        case Position_Flags::Top_Right:     origin_flag = Position_Flags::Top_Left;         break;
        case Position_Flags::Bottom_Right:  origin_flag = Position_Flags::Bottom_Left;      break;
        case Position_Flags::Left:          origin_flag = Position_Flags::Right;            break;
        case Position_Flags::Top_Left:      origin_flag = Position_Flags::Top_Right;        break;
        case Position_Flags::Bottom_Left:   origin_flag = Position_Flags::Bottom_Right;     break;
        default: ;
        }
    } else if (scale_y < 0 && scale_x < 0) {
        switch (origin_flag)
        {
        case Position_Flags::Right:         origin_flag = Position_Flags::Left;             break;
        case Position_Flags::Left:          origin_flag = Position_Flags::Right;            break;
        case Position_Flags::Top:           origin_flag = Position_Flags::Bottom;           break;
        case Position_Flags::Bottom:        origin_flag = Position_Flags::Top;              break;
        case Position_Flags::Top_Right:     origin_flag = Position_Flags::Bottom_Left;      break;
        case Position_Flags::Bottom_Right:  origin_flag = Position_Flags::Top_Left;         break;
        case Position_Flags::Top_Left:      origin_flag = Position_Flags::Bottom_Right;     break;
        case Position_Flags::Bottom_Left:   origin_flag = Position_Flags::Top_Right;        break;
        default: ;
        }
    }
    QGraphicsItemGroup *temp = my_scene->createEmptyItemGroup(angle);
    for (auto child : my_scene->getSelectionItems())
        temp->addToGroup(child);
    my_scene->setPositionByOrigin(temp, origin_flag, new_pos.x(), new_pos.y());
    my_scene->destroyItemGroup(temp);

    // ***** Update stored scale and selection box
    my_scene->setSelectionScale( new_scale );
    my_scene->updateSelectionBox();

    // ***** Update tool tip
    double group_width =  QLineF( mapToScene(m_handles_centers[Position_Flags::Left].toPoint()),
                                  mapToScene(m_handles_centers[Position_Flags::Right].toPoint()) ).length();
    double group_height = QLineF( mapToScene(m_handles_centers[Position_Flags::Top].toPoint()),
                                  mapToScene(m_handles_centers[Position_Flags::Bottom].toPoint()) ).length();
    m_tool_tip->updateToolTipData( QPointF( group_width, group_height ));
}


//####################################################################################
//##        Remove shearing from item
//####################################################################################
void DrView::removeShearing(QGraphicsItem *item, QPointF scale)
{
    // Figure out which item in resize group to base transform off of
    DrItem *original = dynamic_cast<DrItem*>(item);
    DrItem *clone = nullptr;
    for (auto child : m_group->childItems()) {
        DrItem *child_as_item = dynamic_cast<DrItem*>(child);
        if (child_as_item->getObjectKey() == original->getObjectKey()) {
            clone = child_as_item;
            break;
        }
    }
    if (clone == nullptr) return;

    // Calculate a transform that includes rotation and new scale
    double  angle = original->data(User_Roles::Rotation).toDouble();
    QPointF start_scale = original->data(User_Roles::Pre_Resize_Scale).toPointF();
    double new_scale_x = abs(start_scale.x()) * abs(scale.x());
    double new_scale_y = abs(start_scale.y()) * abs(scale.y());

    // Check to make sure we respect flipping
    if ((m_pre_resize_scale.x() < 0 && start_scale.x() < 0) || (m_pre_resize_scale.x() > 0 && start_scale.x() > 0)) {
        if (scale.x() < 0) new_scale_x *= -1;
    } else {
        if (scale.x() > 0) new_scale_x *= -1;
    }
    if ((m_pre_resize_scale.y() < 0 && start_scale.y() < 0) || (m_pre_resize_scale.y() > 0 && start_scale.y() > 0)) {
        if (scale.y() < 0) new_scale_y *= -1;
    } else {
        if (scale.y() > 0) new_scale_y *= -1;
    }


//    QTransform start = QTransform().rotate(angle).scale(start_scale.x(), start_scale.y());
//    QPointF bot_left =  start.map( original->boundingRect().bottomLeft() );
//    QPointF top_right = start.map( original->boundingRect().topRight() );

//    double initial_w = QLineF(bot_left, top_right).dx() * scale.x();
//    double initial_h = QLineF(bot_left, top_right).dy() * scale.y();


    // Update item property
    original->setData(User_Roles::Scale, QPointF(new_scale_x, new_scale_y) );

    QTransform no_shear = QTransform()
            .rotate(angle)
            .scale(new_scale_x, new_scale_y);
    original->setTransform(no_shear);

    // Set item to center point of clone
    QPointF center_point = clone->mapToScene( clone->boundingRect().center() );
    my_scene->setPositionByOrigin(original, Position_Flags::Center, center_point.x(), center_point.y());

}






















