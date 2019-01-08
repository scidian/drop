//
//      Created by Stephens Nunnally on 1/8/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Graphics View Handling - View_Mode::Rotating
//
//

#include <cmath>            // For std::atan2()

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


// Starts rotating mode
void SceneGraphicsView::startRotate()
{
    m_view_mode = View_Mode::Rotating;

    // Store some starting info about the selected items before we start resizing
    m_start_resize_rect = scene()->selectedItems().first()->sceneBoundingRect();
    for (auto item: scene()->selectedItems()) {
        // Add on each items Rect to store total size of selection rectangle
        m_start_resize_rect = m_start_resize_rect.united(item->sceneBoundingRect());

        // Store starting rotation of each selected item
        item->setData(User_Roles::Rotation, item->rotation());
    }
    QPointF select_center;
    select_center.setX(m_start_resize_rect.x() + (m_start_resize_rect.width() / 2));
    select_center.setY(m_start_resize_rect.y() + (m_start_resize_rect.height() / 2));
    m_selection_center = mapFromScene(select_center);
    m_last_angle_diff = 0;
}


// Calculates rotation
void SceneGraphicsView::rotateSelection(QPointF mouse_in_view)
{
    double angle1 = calcRotationAngleInDegrees(m_selection_center, m_origin);
    double angle2 = calcRotationAngleInDegrees(m_selection_center, mouse_in_view);
    double angle_diff = angle1 - angle2;
    if (angle_diff < -180) angle_diff += 360;
    if (angle_diff > 180)  angle_diff -= 360;

    // Apply rotation to each selected item
    for (auto item : scene()->selectedItems()) {
        // Get stored data from each item
        //QVariant get_data = item->data(User_Roles::Rotation);
        //double my_rotation = get_data.toDouble();

        QPointF center = m_selection_rect.center();
        qreal scale_x = item->transform().m11();
        qreal scale_y = item->transform().m22();

        //qreal angle = (m_last_angle_diff) - angle_diff;
        QTransform t;// = item->transform();
        t.translate(center.x(), center.y());
        t.scale(1 / scale_x, 1 / scale_y);
        t.rotate(angle_diff);
        t.scale(scale_x, scale_y);
        t.translate(-center.x(), -center.y());

        item->setTransform(t, false);

        //item->setPos(t.map(item->pos()));
        //item->setRotation(item->rotation() + angle);



    }
    m_last_angle_diff = angle_diff;

    m_interface->setLabelText(Label_Names::LabelObject1, "Angle 1: " + QString::number(angle1));
    m_interface->setLabelText(Label_Names::LabelObject2, "Angle 2: " + QString::number(angle2));
    m_interface->setLabelText(Label_Names::LabelObject3, "Angle Dif: " + QString::number(angle_diff));
}


double SceneGraphicsView::calcRotationAngleInDegrees(QPointF centerPt, QPointF targetPt)
{
    // calculate the angle theta from the deltaY and deltaX values, (atan2 returns radians values from [-PI,PI])
    // 0 currently points EAST.
    // NOTE: By preserving Y and X param order to atan2,  we are expecting a CLOCKWISE angle direction.
    double theta = std::atan2(targetPt.y() - centerPt.y(), targetPt.x() - centerPt.x());

    // rotate the theta angle clockwise by 90 degrees (this makes 0 point NORTH)
    // NOTE: adding to an angle rotates it clockwise, subtracting would rotate it counter-clockwise
    theta += 3.141592653589793238463/2.0;

    // convert from radians to degrees this will give you an angle from [0->270],[-180,0]
    double angle = theta * (180.0/3.141592653589793238463);

    // convert to positive range (0-360) since we want to prevent negative angles, adjust them now.
    // we can assume that atan2 will not return a negative value greater than one partial rotation
    if (angle < 0) { angle += 360; }

    return angle;
}












