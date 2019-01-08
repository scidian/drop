//
//      Created by Stephens Nunnally on 1/8/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Graphics View Handling - View_Mode::Resizing
//
//

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
    m_start_resize_grip = m_over_handle;

    // Store some starting info about the selected items before we start resizing
    m_start_resize_rect = scene()->selectedItems().first()->sceneBoundingRect();
    for (auto item: scene()->selectedItems()) {
        // Add on each items Rect to store total size of selection rectangle
        m_start_resize_rect = m_start_resize_rect.united(item->sceneBoundingRect());

        // Store starting scale of each selected item
        QPointF my_scale(item->transform().m11(), item->transform().m22());
        item->setData(User_Roles::Scale, my_scale);

        // Store starting position of each selected item
        QPointF my_pos(item->pos().x(), item->pos().y());
        item->setData(User_Roles::Position, my_pos);
    }

}





// Calculates selection resizing
void SceneGraphicsView::resizeSelection(QPointF mouse_in_scene)
{
    // Figure out what sides to use for x axis and y axis
    X_Axis do_x;
    Y_Axis do_y;
    switch (m_start_resize_grip) {
    case Position_Flags::Top_Left:      do_x = X_Axis::Left;    do_y = Y_Axis::Top;     break;
    case Position_Flags::Top:           do_x = X_Axis::None;    do_y = Y_Axis::Top;     break;
    case Position_Flags::Top_Right:     do_x = X_Axis::Right;   do_y = Y_Axis::Top;     break;
    case Position_Flags::Right:         do_x = X_Axis::Right;   do_y = Y_Axis::None;    break;
    case Position_Flags::Bottom_Right:  do_x = X_Axis::Right;   do_y = Y_Axis::Bottom;  break;
    case Position_Flags::Bottom:        do_x = X_Axis::None;    do_y = Y_Axis::Bottom;  break;
    case Position_Flags::Bottom_Left:   do_x = X_Axis::Left;    do_y = Y_Axis::Bottom;  break;
    case Position_Flags::Left:          do_x = X_Axis::Left;    do_y = Y_Axis::None;    break;
    default:                            do_x = X_Axis::None;    do_y = Y_Axis::None;
    }

    ///// To be used to calculate size while doing uniform resizing
    ///qreal dist = sqrt(pow(m_click_pos.x() - pos.x(), 2) + pow(m_click_pos.y() - pos.y(), 2));
    QPointF top_left_select =  m_start_resize_rect.topLeft();
    QPointF bot_right_select = m_start_resize_rect.bottomRight();
    qreal original_x, original_y;
    qreal new_x,      new_y;
    qreal scale_x,    scale_y;

    // Find original distance of mouse to opposite selection sides
    switch (do_x) {
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

    switch (do_y) {
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
        get_data = item->data(User_Roles::Scale);       my_scale = get_data.toPointF();
        get_data = item->data(User_Roles::Position);    my_pos =   get_data.toPointF();

        // Apply new scale to each item
        qreal sx = my_scale.x() * scale_x;
        qreal sy = my_scale.y() * scale_y;
        if (sx <  .001 && sx >= 0) sx =  .001;
        if (sx > -.001 && sx < 0)  sx = -.001;
        if (sy <  .001 && sy >= 0) sy =  .001;
        if (sy > -.001 && sy < 0)  sy = -.001;


        QTransform scale_transform;
        scale_transform = QTransform::fromScale(sx, sy);
        item->setTransform(scale_transform);


//        QPointF center = m_selection_rect.center();
//        qreal angle = item->rotation();
//        QTransform t;
//        t.translate(center.x(), center.y());
//        t.rotate(-angle);
//        t.scale(sx, sy);
//        t.rotate(angle);
//        t.translate(-center.x(), -center.y());

//        item->setTransform(t, false);

        //item->setPos(t.map(item->pos()));
        //item->setRotation(angle);



        // Move each item as neccessary to keep them aligned within selection box
        qreal original_x_dist, original_y_dist;
        qreal new_left, new_top;

        switch (do_x) {
        case X_Axis::Right:
            original_x_dist = (my_pos.x() - top_left_select.x());
            new_left = top_left_select.x() + (original_x_dist * scale_x);
            break;
        case X_Axis::Left:
            original_x_dist = (bot_right_select.x() - my_pos.x());
            new_left = bot_right_select.x() - (original_x_dist * scale_x);
            break;
        case X_Axis::None:      new_left = my_pos.x();
        }

        switch (do_y) {
        case Y_Axis::Bottom:
            original_y_dist = (my_pos.y() - top_left_select.y());
            new_top =  top_left_select.y() + (original_y_dist * scale_y);
            break;
        case Y_Axis::Top:
            original_y_dist = (bot_right_select.y() - my_pos.y());
            new_top =  bot_right_select.y() - (original_y_dist * scale_y);
            break;
        case Y_Axis::None:      new_top = my_pos.y();
        }

        item->setPos(new_left, new_top);
    }
}










