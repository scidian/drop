//
//      Created by Stephens Nunnally on 1/8/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Graphics View Handling - View_Mode::Resizing
//
//

#include <math.h>

#include "01_project.h"
#include "02_world.h"
#include "03_scene.h"
#include "04_object.h"
#include "05_item.h"

#include "30_settings.h"
#include "31_component.h"
#include "32_property.h"

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

    // If only 1 item selected, start appropriate resize routine
    if (scene()->selectedItems().count() == 1){
        resizeSelectionOneWithRotate(mouse_in_scene);

    // Otherwise, multiple items are selected, start alternate routine
    } else {
        resizeMultipleSelection(mouse_in_scene);

    }
}





// Calculates selection resizing
void SceneGraphicsView::resizeSelectionOneNoRotate(QPointF mouse_in_scene)
{
    // Class Variables used:
    //      m_start_resize_rect         QRect               Starting size of bounding box in scene coordinates
    //      m_origin_in_scene           QPointF             Starting mouse position in scene coordinates
    //      m_do_x                      X_Axis              Tells what direction we're resizing from
    //      m_do_y                      Y_Axis              Tells what direction we're resizing from

    // Now that we decided the item isnt rotated, round angle to nearest 90 and store back in item
    double angle = scene()->selectedItems().first()->data(User_Roles::Rotation).toDouble();
    angle = round(angle / 90) * 90;
    scene()->selectedItems().first()->setData(User_Roles::Rotation, angle);

    QPointF top_left_select =  m_start_resize_rect.topLeft();
    QPointF bot_right_select = m_start_resize_rect.bottomRight();

    QGraphicsItem *item = scene()->selectedItems().first();
    qreal item_width =  item->boundingRect().width();
    qreal item_height = item->boundingRect().height();
    qreal start_adjust_x,   start_adjust_y;
    qreal new_width,        new_height;
    qreal scale_x,          scale_y;

    // Calculate X scale
    if (m_do_x == X_Axis::Right) {
        start_adjust_x = bot_right_select.x() - m_origin_in_scene.x();              // takes into account start mouse pos with actual side pos
        new_width = mouse_in_scene.x() - top_left_select.x() + start_adjust_x;
    }
    else if (m_do_x == X_Axis::Left) {
        start_adjust_x = top_left_select.x() - m_origin_in_scene.x();               // takes into account start mouse pos with actual side pos
        new_width = bot_right_select.x() - mouse_in_scene.x() - start_adjust_x;
    } else {
        new_width = bot_right_select.x() - top_left_select.x();
    }
    scale_x = new_width / item_width;

    // Calculate Y scale
    if (m_do_y == Y_Axis::Bottom) {
        start_adjust_y = bot_right_select.y() - m_origin_in_scene.y();              // takes into account start mouse pos with actual side pos
        new_height = mouse_in_scene.y() - top_left_select.y() + start_adjust_y;
    }
    else if (m_do_y == Y_Axis::Top) {
        start_adjust_y = top_left_select.y() - m_origin_in_scene.y();               // takes into account start mouse pos with actual side pos
        new_height = bot_right_select.y() - mouse_in_scene.y() - start_adjust_y;
    } else {
        new_height = bot_right_select.y() - top_left_select.y();
    }
    scale_y = new_height / item_height;

    // Apply new scale
    QTransform t;
    t.scale(scale_x, scale_y);
    item->setTransform(t);

    // Move each item as neccessary to keep them aligned with opposite starting corner
    qreal new_left = top_left_select.x();
    qreal new_top =  top_left_select.y();
    if (m_do_x == X_Axis::Left ) {
        if (scale_x > 0)
            new_left = bot_right_select.x() - item->sceneBoundingRect().width();
        else
            new_left = bot_right_select.x() + item->sceneBoundingRect().width();
    }
    if (m_do_y == Y_Axis::Top  ) {
        if (scale_y > 0)
            new_top = bot_right_select.y() - item->sceneBoundingRect().height();
        else
            new_top = bot_right_select.y() + item->sceneBoundingRect().height();;
    }
    item->setPos(new_left, new_top);

    item->setData(User_Roles::Scale, QPointF(scale_x, scale_y) );
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
void SceneGraphicsView::resizeSelectionOneWithRotate(QPointF mouse_in_scene)
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
    DrItem *item2 = dynamic_cast<DrItem *>(scene()->selectedItems().first());
    QPointF new_pos = item->pos();

    if (m_do_x == X_Axis::Left && m_do_y != Y_Axis::Top) {
        new_pos = item->data(User_Roles::Pre_Resize_Top_Right).toPointF();
        item2->setPositionByOrigin(Origin::Top_Right, new_pos.x(), new_pos.y());
    }
    else if (m_do_x == X_Axis::Left && m_do_y == Y_Axis::Top) {
        new_pos = item->data(User_Roles::Pre_Resize_Bottom_Right).toPointF();
        item2->setPositionByOrigin(Origin::Bottom_Right, new_pos.x(), new_pos.y());
    }
    else if ((m_do_x == X_Axis::Right && m_do_y == Y_Axis::Top) || (m_do_y == Y_Axis::Top)) {
        new_pos = item->data(User_Roles::Pre_Resize_Bottom_Left).toPointF();
        item2->setPositionByOrigin(Origin::Bottom_Left, new_pos.x(), new_pos.y());
    }

    item->setData(User_Roles::Scale, QPointF(scale_x, scale_y) );
}



// Calculates selection resizing
void SceneGraphicsView::resizeMultipleSelection(QPointF mouse_in_scene)
{



}









