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


// Starts resizing mode
void SceneGraphicsView::startResize()
{
    m_view_mode = View_Mode::Resizing;
    m_start_resize_grip = m_over_handle;                        // Store grip handle we start resize event with
    m_start_resize_rect = totalSelectedItemsSceneRect();        // Store starting scene rect of initial selection bounding box

    // Store some starting info about the selected items before we start resizing
    for (auto item: scene()->selectedItems()) {
        // Store starting scale of each selected item
        QPointF my_scale = item->data(User_Roles::Scale).toPointF();
        item->setData(User_Roles::Pre_Resize_Scale, my_scale);

        // Store starting position of each selected item
        QPointF my_pos(item->pos().x(), item->pos().y());
        item->setData(User_Roles::Position, my_pos);

        QTransform t = item->sceneTransform();
        QPointF top_left = t.map( item->boundingRect().topLeft() );
        QPointF top_right = t.map( item->boundingRect().topRight() );
        QPointF bottom_left = t.map( item->boundingRect().bottomLeft() );
        QPointF bottom_right = t.map( item->boundingRect().bottomRight() );
        item->setData(User_Roles::Pre_Resize_Top_Left, top_left);
        item->setData(User_Roles::Pre_Resize_Top_Right, top_right);
        item->setData(User_Roles::Pre_Resize_Bottom_Left, bottom_left);
        item->setData(User_Roles::Pre_Resize_Bottom_Right, bottom_right);
    }
}


// Call appropriate resize function
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

    // Try and lock function, so we ony run this once at a time
    if (resize_mutex.tryLock() == false) return;
    resizeSelectionWithRotate(mouse_in_scene);
    resize_mutex.unlock();
}


// Returns the ooposite side name of side passed in, no error catch if corner passed in
Position_Flags SceneGraphicsView::findOppositeSide(Position_Flags start_side)
{
    switch (start_side)
    {
    case Position_Flags::Top:       return Position_Flags::Bottom;
    case Position_Flags::Bottom:    return Position_Flags::Top;
    case Position_Flags::Left:      return Position_Flags::Right;
    case Position_Flags::Right:     return Position_Flags::Left;
    case Position_Flags::Top_Left:     return Position_Flags::Bottom_Right;
    case Position_Flags::Top_Right:    return Position_Flags::Bottom_Left;
    case Position_Flags::Bottom_Left:  return Position_Flags::Top_Right;
    case Position_Flags::Bottom_Right: return Position_Flags::Top_Left;
    default:    return Position_Flags::Top;
    }
}


// Calculates selection resizing
void SceneGraphicsView::resizeSelectionWithRotate(QPointF mouse_in_scene)
{
    // Load item starting width and height
    QGraphicsItem *item = scene()->selectedItems().first();
    qreal item_width =  item->boundingRect().width();
    qreal item_height = item->boundingRect().height();

    // ********** Find corners / sides we're working with, calculate new width / height
    QPointF corner_start =    m_handles_centers[static_cast<Position_Flags>(m_over_handle)];
    QPointF corner_opposite = m_handles_centers[findOppositeSide(m_over_handle)];

    // Find center point, load angle of item, load original scale
    QPointF center_point = QLineF(corner_start, corner_opposite).pointAt(.5);
    double  angle = scene()->selectedItems().first()->data(User_Roles::Rotation).toDouble();
    QPointF old_scale = item->data(User_Roles::Pre_Resize_Scale).toPointF();

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
        scale_x = old_scale.x();

    // ***** Calculate Y scale
    if (m_do_y == Y_Axis::Bottom)
        scale_y = QLineF(zero_rotated_opposite_in_scene, point_in_shape).dy() / item_height;
    else if (m_do_y == Y_Axis::Top)
        scale_y = QLineF(point_in_shape, zero_rotated_opposite_in_scene).dy() / item_height;
    else
        scale_y = old_scale.y();



    // ***** Apply new scale
    QTransform t = QTransform().rotate(angle).scale(scale_x, scale_y);
    item->setTransform(t);



    // ***** Translate if needed
    SceneGraphicsScene *my_scene = dynamic_cast<SceneGraphicsScene *>(scene());
    QPointF new_pos = item->pos();

    if (m_do_x == X_Axis::Left && m_do_y != Y_Axis::Top) {
        new_pos = item->data(User_Roles::Pre_Resize_Top_Right).toPointF();
        my_scene->setPositionByOrigin(scene()->selectedItems().first(), Origin::Top_Right, new_pos.x(), new_pos.y());
    }
    else if (m_do_x == X_Axis::Left && m_do_y == Y_Axis::Top) {
        new_pos = item->data(User_Roles::Pre_Resize_Bottom_Right).toPointF();
        my_scene->setPositionByOrigin(scene()->selectedItems().first(), Origin::Bottom_Right, new_pos.x(), new_pos.y());
    }
    else if ((m_do_x == X_Axis::Right && m_do_y == Y_Axis::Top) || (m_do_y == Y_Axis::Top)) {
        new_pos = item->data(User_Roles::Pre_Resize_Bottom_Left).toPointF();
        my_scene->setPositionByOrigin(scene()->selectedItems().first(), Origin::Bottom_Left, new_pos.x(), new_pos.y());
    }
    else {
        new_pos = item->data(User_Roles::Pre_Resize_Top_Left).toPointF();
        my_scene->setPositionByOrigin(scene()->selectedItems().first(), Origin::Top_Left, new_pos.x(), new_pos.y());
    }

    item->setData(User_Roles::Scale, QPointF(scale_x, scale_y) );
}









