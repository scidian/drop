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

    m_start_resize_rect = totalSelectedItemsSceneRect();                // Store starting scene rect of initial selection bounding box
    m_last_angle_diff = 0;                                              // Reset initial rotate event angle at 0
}


//
//
//      !!!!!!!! Need to have angles snap to good angles, ie 0, 15, 30, 45, 60, 75, 90 etc
//
//
// Now that we decided the item isnt rotated, round angle to nearest 90 and store back in item
//double angle = scene()->selectedItems().first()->data(User_Roles::Rotation).toDouble();
//angle = round(angle / 90) * 90;
//scene()->selectedItems().first()->setData(User_Roles::Rotation, angle);

// Returns true is 'check_angle' in within 'tolerance' to 0, 90, 180, or 270, i.e. "square" angle
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
// Returns true if 'number_desired' is within +-'tolerance' of 'number_to_check'
bool SceneGraphicsView::isCloseTo(double number_desired, double number_to_check, double tolerance)
{   return ( (number_to_check <= (number_desired + tolerance)) && (number_to_check >= (number_desired - tolerance)) );  }



// Calculates rotation
void SceneGraphicsView::rotateSelection(QPointF mouse_in_view)
{
    // Try and lock function, so we ony run this once at a time
    if (rotate_mutex.tryLock() == false) return;

    double angle1 = calcRotationAngleInDegrees( mapFromScene(m_start_resize_rect.center()), m_origin);
    double angle2 = calcRotationAngleInDegrees( mapFromScene(m_start_resize_rect.center()), mouse_in_view);
    double angle = angle2 - angle1;
    double angle_diff = angle - m_last_angle_diff;

    if (angle_diff < -180) angle_diff += 360;
    if (angle_diff > 180)  angle_diff -= 360;

    // Group selected items and apply rotation to collection
    QGraphicsItemGroup *group = scene()->createItemGroup(scene()->selectedItems());
    QPointF offset = group->sceneBoundingRect().center();
    QTransform transform;

    // Offset difference of original center bounding box to possible slightly different center of new bounding box
    offset.setX(offset.x() - (offset.x() - m_start_resize_rect.center().x()) );
    offset.setY(offset.y() - (offset.y() - m_start_resize_rect.center().y()) );

    // Save new item rotations for use later
    for (auto item : group->childItems()) {
        double my_angle = item->data(User_Roles::Rotation).toDouble();
        double new_angle =  my_angle + angle_diff;

        if (new_angle >=  360) { do { new_angle -= 360; } while (new_angle >=  360); }
        if (new_angle <= -360) { do { new_angle += 360; } while (new_angle <= -360); }

        item->setData(User_Roles::Rotation, new_angle);
    }

    // Apply new rotations and destroy group
    transform.translate( offset.x(),  offset.y());
    transform.rotate(angle_diff);
    transform.translate(-offset.x(), -offset.y());

    group->setTransform(transform);
    scene()->destroyItemGroup(group);

    m_last_angle_diff = angle;

    // !!!!! TEMP:
    m_interface->setLabelText(Label_Names::Label_Object_3, "Angle 1: " + QString::number(angle1) +
                                                         ", Angle 2: " + QString::number(angle2));
    m_interface->setLabelText(Label_Names::Label_Object_4, "Angle: " + QString::number(angle) +
                                                         ", Diff: " + QString::number(angle_diff) );
    // !!!!! END

    rotate_mutex.unlock();
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












