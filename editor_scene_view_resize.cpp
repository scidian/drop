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
        QPointF my_scale(item->transform().m11(), item->transform().m22());
        item->setData(User_Roles::Scale_Pre_Resize, my_scale);

        // Store starting position of each selected item
        QPointF my_pos(item->pos().x(), item->pos().y());
        item->setData(User_Roles::Position, my_pos);
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
        double angle = scene()->selectedItems().first()->data(User_Roles::Rotation).toDouble();
        if (isSquare(angle, ANGLE_TOLERANCE) == true)
            resizeSelectionOneNoRotate(mouse_in_scene);
        else
            resizeSelectionOneWithRotate(mouse_in_scene);

    // Otherwise, multiple items are selected, start alternate routine
    } else {

    }
}

bool SceneGraphicsView::isSquare(double check_angle, double tolerance)
{
    check_angle = abs(check_angle);
    while (check_angle >= 360) check_angle -= 360;
    if (isCloseTo(check_angle, 0, tolerance))   return true;
    if (isCloseTo(check_angle, 90, tolerance))  return true;
    if (isCloseTo(check_angle, 180, tolerance)) return true;
    if (isCloseTo(check_angle, 270, tolerance)) return true;
    return false;
}

bool SceneGraphicsView::isCloseTo(double number_desired, double number_to_check, double tolerance)
{
    if ( (number_to_check <= (number_desired + tolerance)) && (number_to_check >= (number_desired - tolerance)) )
        return true;
    else
        return false;
}




// Calculates selection resizing
void SceneGraphicsView::resizeSelectionOneNoRotate(QPointF mouse_in_scene)
{
    // Now that we decided the item isnt rotated, round angle to nearest 90 and store
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
}




// Calculates selection resizing
void SceneGraphicsView::resizeSelectionOneWithRotate(QPointF mouse_in_scene)
{
    //m_handles[static_cast<int>(Position_Flags::Top_Left)] =     rectAtCenterPoint(to_view.point(0), r_size);
    //m_handles[static_cast<int>(Position_Flags::Top_Right)] =    rectAtCenterPoint(to_view.point(1), r_size);
    //m_handles[static_cast<int>(Position_Flags::Bottom_Left)] =  rectAtCenterPoint(to_view.point(3), r_size);
    //m_handles[static_cast<int>(Position_Flags::Bottom_Right)] = rectAtCenterPoint(to_view.point(2), r_size);

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
}










// Calculates selection resizing
void SceneGraphicsView::resizeSelection2(QPointF mouse_in_scene)
{
    // Class Variables used:
    //      m_start_resize_rect         QRect               Starting size of bounding box in scene coordinates
    //      m_start_resize_grip         Position_Flags      Represents which directions we're scaling from
    //      m_origin                    QPoint              Starting mouse position in view coordinates
    //      m_origin_in_scene           QPointF             Starting mouse position in scene coordinates

    ///// To be used to calculate size while doing uniform resizing
    ///qreal dist = sqrt(pow(m_click_pos.x() - pos.x(), 2) + pow(m_click_pos.y() - pos.y(), 2));

    QPointF top_left_select =  m_start_resize_rect.topLeft();
    QPointF bot_right_select = m_start_resize_rect.bottomRight();
    ///qreal start_width = m_start_resize_rect.width();
    ///qreal start_height = m_start_resize_rect.height();

    qreal start_adjust_x;//, start_adjust_y;
    qreal new_width;//, new_height;
    qreal scale_x,    scale_y;

    // Create groupd from currently selected items
    QGraphicsItemGroup *group = scene()->createItemGroup(scene()->selectedItems());
    qreal start_x = group->pos().x(); // is = group->boundingRect().x();
    qreal start_y = group->pos().y(); // is = group->boundingRect().y();



    qreal group_width;


    // Find original distance of mouse to opposite selection sides
    switch (m_do_x) {
    case X_Axis::Right:
        start_adjust_x = bot_right_select.x() - m_origin_in_scene.x();              // takes into account start mouse pos with actual side pos

        new_width = mouse_in_scene.x() - top_left_select.x() + start_adjust_x;

        group_width = group->sceneBoundingRect().width();


        scale_x = new_width / group_width;
        if (scale_x < .001) scale_x = .001;
        if (scale_x > 1000000) scale_x = 1000000;
        break;



    case X_Axis::Left:

        //new_x = bot_right_select.x() - mouse_in_scene.x();
        //scale_x = (new_x) / (original_x + .000001);
        scale_x = 1;
        break;
    case X_Axis::None:

        scale_x = 1;
        ;
    }


    scale_y = 1;


    // Group selected items and apply rotation to collection
    QTransform transform;
    //transform.translate(-group->sceneBoundingRect().x(), -group->sceneBoundingRect().y());
    transform.scale(scale_x, scale_y);
    //transform.translate(group->sceneBoundingRect().x(), group->sceneBoundingRect().y());

    group->setTransform(transform);

    // !!!!! TEMP:
    m_interface->setLabelText(Label_Names::Label_2, "Group Scale  X: " + QString::number(group->transform().m11()) +
                                                               ", Y: " + QString::number(group->transform().m22()) );
    // !!!!! END

    group->setPos(start_x, start_y);


    scene()->destroyItemGroup(group);


//    switch (do_y) {
//    case Y_Axis::Bottom:
//        original_y = m_origin_in_scene.y() - top_left_select.y();
//        new_y = mouse_in_scene.y() - top_left_select.y();
//        scale_y = (new_y) / (original_y + .000001);
//        break;
//    case Y_Axis::Top:
//        original_y = bot_right_select.y() - m_origin_in_scene.y();
//        new_y = bot_right_select.y() - mouse_in_scene.y();
//        scale_y = (new_y) / (original_y + .000001);
//        break;
//    case Y_Axis::None:
//        original_y = 0;     scale_y = 1;
//    }

    // Apply scaling to each selected item
//    for (auto item : scene()->selectedItems()) {
//        // Get stored data from each item
//        QVariant get_data;
//        QPointF my_scale, my_pos;
//        get_data = item->data(User_Roles::Scale_Pre_Resize);    my_scale = get_data.toPointF();
//        get_data = item->data(User_Roles::Position);            my_pos =   get_data.toPointF();

//        // Apply new scale to each item
//        qreal sx = my_scale.x() * scale_x;
//        qreal sy = my_scale.y() * scale_y;
//        if (sx <  .001 && sx >= 0) sx =  .001;
//        if (sx > -.001 && sx < 0)  sx = -.001;
//        if (sy <  .001 && sy >= 0) sy =  .001;
//        if (sy > -.001 && sy < 0)  sy = -.001;


//        QTransform scale_transform;
//        scale_transform = QTransform::fromScale(sx, sy);
//        item->setTransform(scale_transform);



        // Move each item as neccessary to keep them aligned within selection box
//        qreal original_x_dist, original_y_dist;
//        qreal new_left, new_top;
//
//        switch (do_x) {
//        case X_Axis::Right:
//            original_x_dist = (my_pos.x() - top_left_select.x());
//            new_left = top_left_select.x() + (original_x_dist * scale_x);
//            break;
//        case X_Axis::Left:
//            original_x_dist = (bot_right_select.x() - my_pos.x());
//            new_left = bot_right_select.x() - (original_x_dist * scale_x);
//            break;
//        case X_Axis::None:      new_left = my_pos.x();
//        }

//        switch (do_y) {
//        case Y_Axis::Bottom:
//            original_y_dist = (my_pos.y() - top_left_select.y());
//            new_top =  top_left_select.y() + (original_y_dist * scale_y);
//            break;
//        case Y_Axis::Top:
//            original_y_dist = (bot_right_select.y() - my_pos.y());
//            new_top =  bot_right_select.y() - (original_y_dist * scale_y);
//            break;
//        case Y_Axis::None:      new_top = my_pos.y();
//        }

//        item->setPos(new_left, new_top);
//    }


}






// Calculates selection resizing
void SceneGraphicsView::resizeSelection3(QPointF mouse_in_scene)
{
    QPointF top_left_select =  m_start_resize_rect.topLeft();
    QPointF bot_right_select = m_start_resize_rect.bottomRight();
    qreal original_x, original_y;
    qreal new_x,      new_y;
    qreal scale_x,    scale_y;

    // Find original distance of mouse to opposite selection sides
    switch (m_do_x) {
    case X_Axis::Right:
        original_x = m_origin_in_scene.x() - top_left_select.x();
        new_x = mouse_in_scene.x() - top_left_select.x();
        scale_x = (new_x) / (original_x + .000001);      // New width of selection / original width
        break;
    case X_Axis::Left:
        original_x = bot_right_select.x() - m_origin_in_scene.x();
        new_x = bot_right_select.x() - mouse_in_scene.x();
        scale_x = (new_x) / (original_x + .000001);
        break;
    case X_Axis::None:
        original_x = 0;     scale_x = 1;
    }

    switch (m_do_y) {
    case Y_Axis::Bottom:
        original_y = m_origin_in_scene.y() - top_left_select.y();
        new_y = mouse_in_scene.y() - top_left_select.y();
        scale_y = (new_y) / (original_y + .000001);
        break;
    case Y_Axis::Top:
        original_y = bot_right_select.y() - m_origin_in_scene.y();
        new_y = bot_right_select.y() - mouse_in_scene.y();
        scale_y = (new_y) / (original_y + .000001);
        break;
    case Y_Axis::None:
        original_y = 0;     scale_y = 1;
    }

    // Apply scaling to each selected item
    for (auto item : scene()->selectedItems()) {
        // Get stored data from each item
        QVariant get_data;
        QPointF my_scale, my_pos;
        double my_angle;
        get_data = item->data(User_Roles::Scale_Pre_Resize);    my_scale = get_data.toPointF();
        get_data = item->data(User_Roles::Position);            my_pos =   get_data.toPointF();
        get_data = item->data(User_Roles::Rotation);            my_angle = get_data.toDouble();

        // Apply new scale to each item
        qreal sx = my_scale.x() * scale_x;
        qreal sy = my_scale.y() * scale_y;
        if (sx <  .001 && sx >= 0) sx =  .001;
        if (sx > -.001 && sx < 0)  sx = -.001;
        if (sy <  .001 && sy >= 0) sy =  .001;
        if (sy > -.001 && sy < 0)  sy = -.001;


        QTransform t;
        t.rotate(my_angle);
        t.scale(sx, sy);
        //t.rotate(-my_angle);

        item->setTransform(t);
        //item->setPos(t.map(item->pos()));


//        t.reset();
//        t.rotate(my_angle);
//        item->setTransform(t, true);


//        QPointF center = m_selection_rect.center();
//        qreal angle = item->rotation();
//        QTransform t;
//        t.translate(center.x(), center.y());
//        t.rotate(-angle);
//        t.scale(sx, sy);
//        t.rotate(angle);
//        t.translate(-center.x(), -center.y());

//        item->setTransform(t, false);
//        item->setPos(t.map(item->pos()));



        // Move each item as neccessary to keep them aligned within selection box
//        qreal original_x_dist, original_y_dist;
//        qreal new_left, new_top;

//        switch (do_x) {
//        case X_Axis::Right:
//            original_x_dist = (my_pos.x() - top_left_select.x());
//            new_left = top_left_select.x() + (original_x_dist * scale_x);
//            break;
//        case X_Axis::Left:
//            original_x_dist = (bot_right_select.x() - my_pos.x());
//            new_left = bot_right_select.x() - (original_x_dist * scale_x);
//            break;
//        case X_Axis::None:      new_left = my_pos.x();
//        }

//        switch (do_y) {
//        case Y_Axis::Bottom:
//            original_y_dist = (my_pos.y() - top_left_select.y());
//            new_top =  top_left_select.y() + (original_y_dist * scale_y);
//            break;
//        case Y_Axis::Top:
//            original_y_dist = (bot_right_select.y() - my_pos.y());
//            new_top =  bot_right_select.y() - (original_y_dist * scale_y);
//            break;
//        case Y_Axis::None:      new_top = my_pos.y();
//        }

//        item->setPos(new_left, new_top);
    }
}














