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
    m_pre_resize_scale =  my_scene->getSelectionScale();
    m_start_resize_grip = m_over_handle;                                // Store grip handle we start resize event with
    m_start_resize_rect = my_scene->totalSelectionSceneRect();          // Store starting scene rect of initial selection bounding box

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
    double item_width =  my_scene->getSelectionBox().normalized().width();
    double item_height = my_scene->getSelectionBox().normalized().height();

    Position_Flags use_handle = m_over_handle;

    // ********** Find corners / sides we're working with, calculate new width / height
    QPointF corner_start =    m_handles_centers[use_handle];
    QPointF corner_opposite = m_handles_centers[findOppositeSide(use_handle)];

    // Find center point, load angle of item
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
    qreal   scale_x,   scale_y;


    // !!!!! #DEBUG:    Paints unrotated selection box with distance point used for calculating scale
    if (Dr::CheckDebugFlag(Debug_Flags::Paint_Resize_Calculations)) {
        QPointF zero_rotated_corner = remove_rotation.map( corner_start );
        QPointF zero_rotated_corner_in_scene = mapToScene(zero_rotated_corner.toPoint());
        m_debug_points.clear();
        m_debug_points.append( mapFromScene(point_in_shape) );
        m_debug_points.append( mapFromScene(zero_rotated_opposite_in_scene)) ;
        m_debug_points.append( mapFromScene(zero_rotated_corner_in_scene) );
    }


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


    // ********** Create transform for new group, apply it, and destroy temporary item group
    QPointF old_scale = my_scene->getSelectionScale();

    // Create item group and apply rotation before adding items
    QGraphicsItemGroup *group = new QGraphicsItemGroup();
    scene()->addItem(group);
    QPointF center = group->boundingRect().center();
    QTransform t = QTransform().translate(center.x(), center.y()).rotate(angle).translate(-center.x(), -center.y());
    group->setTransform(t);
    for (auto item : my_scene->getSelectionItems())
        group->addToGroup(item);

    // Remove old scaling and then apply new scale factor
    center = group->boundingRect().center();
    QTransform transform = group->sceneTransform()
            .translate(center.x(), center.y())
            .scale(1 / old_scale.x(), 1 / old_scale.y())
            .scale(scale_x, scale_y)
            .translate(-center.x(), -center.y());
    group->setTransform(transform);



    // ***** Figure out which position from original rect to translate to
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

    QPointF new_pos = m_pre_resize_corners[resize_flag];


    // ***** Adjust destination position if scale is negative in x or y
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

    my_scene->setPositionByOrigin(group, origin_flag, new_pos.x(), new_pos.y());


    // ***** Store new scale and destroy item
    my_scene->setSelectionScale( QPointF(scale_x, scale_y) );
    scene()->destroyItemGroup(group);

    // ***** Remove any shearing that may have been caused to all items in selection group
    QList<QGraphicsItem*> my_items = my_scene->getSelectionItems();
    for (auto child : my_items) removeShearing(child);

    // ***** Updates selection box from new item locations / sizes,
    //       especially important because item locations may have changed during shear remove
    my_scene->updateSelectionBox();


    // ***** Now that shearing has been removed, move items one more time to proper origin point and recalculate box again
    group = new QGraphicsItemGroup();
    scene()->addItem(group);
    center = group->boundingRect().center();
    t = QTransform().translate(center.x(), center.y()).rotate(angle).translate(-center.x(), -center.y());
    group->setTransform(t);
    for (auto item : my_scene->getSelectionItems())
        group->addToGroup(item);
    my_scene->setPositionByOrigin(group, origin_flag, new_pos.x(), new_pos.y());
    scene()->destroyItemGroup(group);
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
void DrView::removeShearing(QGraphicsItem *item)
{
    double  angle = item->data(User_Roles::Rotation).toDouble();
    QPointF scale = extractScaleFromItem(item);

    QTransform no_skew = QTransform().rotate(angle).scale(scale.x(), scale.y());
    item->setTransform(no_skew);

    QPointF center_in_scene = item->sceneTransform().map( item->boundingRect().center() );
    my_scene->setPositionByOrigin(item, Position_Flags::Center, center_in_scene.x(), center_in_scene.y());

    dynamic_cast<DrItem*>(item)->updateProperty(User_Roles::Scale, QPointF(scale.x(), scale.y()) );
}

// Pulls out scale from current item screen transform (takes away angle first from stored angle data)
QPointF DrView::extractScaleFromItem(QGraphicsItem *item)
{
    double  angle = item->data(User_Roles::Rotation).toDouble();
    QPointF origin = item->mapToScene( item->boundingRect().center() );

    QTransform remove_rotation = QTransform().translate(origin.x(), origin.y()).rotate(-angle).translate(-origin.x(), -origin.y());
    QTransform item_no_rotate = item->sceneTransform() * remove_rotation;

    return QPointF(item_no_rotate.m11(), item_no_rotate.m22());
}























