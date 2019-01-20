//
//      Created by Stephens Nunnally on 1/8/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Graphics View Handling - View_Mode::Rotating
//
//

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
//##        Starts rotating mode
//####################################################################################
void SceneGraphicsView::startRotate()
{


    m_view_mode = View_Mode::Rotating;

    // Grab starting angle of selection group before rotating starts
    SceneGraphicsScene *my_scene = dynamic_cast<SceneGraphicsScene *>(scene());
    m_rotate_start_angle = my_scene->getSelectionGroupAsGraphicsItem()->data(User_Roles::Rotation).toDouble();

    m_rotate_start_rect = totalSelectedItemsSceneRect();                // Store starting scene rect of initial selection bounding box
}


//      !!!!!!!! Need to have angles snap to good angles, ie 0, 15, 30, 45, 60, 75, 90 etc
//
// Now that we decided the item isnt rotated, round angle to nearest 90 and store back in item
//double angle = scene()->selectedItems().first()->data(User_Roles::Rotation).toDouble();
//angle = round(angle / 90) * 90;
//scene()->selectedItems().first()->setData(User_Roles::Rotation, angle);
//


//####################################################################################
//##        Angle Comparision Functions
//####################################################################################
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




//####################################################################################
//##        Main Rotation Function
//####################################################################################
void SceneGraphicsView::rotateSelection(QPointF mouse_in_view)
{
    // Try and lock function, so we ony run this once at a time
    if (rotate_mutex.tryLock() == false) return;

    SceneGraphicsScene    *my_scene = dynamic_cast<SceneGraphicsScene *>(scene());
    QGraphicsItem         *item =     my_scene->getSelectionGroupAsGraphicsItem();
    QList<QGraphicsItem*>  my_items = my_scene->getSelectionGroupItems();

    double angle1 = calcRotationAngleInDegrees( mapFromScene(m_rotate_start_rect.center()), m_origin);
    double angle2 = calcRotationAngleInDegrees( mapFromScene(m_rotate_start_rect.center()), mouse_in_view);

    double angle = m_rotate_start_angle + (angle2 - angle1);
    while (angle >=  360) { angle -= 360; }
    while (angle <= -360) { angle += 360; }

    // Group selected items so we can apply new rotation to all selected items
    QGraphicsItemGroup *group = scene()->createItemGroup( { item } );

    // Offset difference of original center bounding box to possible slightly different center of new bounding box
    QPointF offset = group->sceneBoundingRect().center();
    offset.setX(offset.x() - (offset.x() - m_rotate_start_rect.center().x()) );
    offset.setY(offset.y() - (offset.y() - m_rotate_start_rect.center().y()) );

    // Load starting angle pre rotate, and store new angle in item
    double start_angle = item->data(User_Roles::Rotation).toDouble();
    item->setData(User_Roles::Rotation, angle);

    // Create transform for new angle, apply it, and destroy temporary item group
    QTransform transform = QTransform().translate(offset.x(), offset.y()).rotate(angle - start_angle).translate(-offset.x(), -offset.y());
    group->setTransform(transform);
    scene()->destroyItemGroup(group);

    // Break apart selection group so we can grab and store new rotation of all selected items, then put back together
    for (auto item: my_items) my_scene->removeFromGroupNoUpdate(item);
    for (auto child : my_items) {
        QTransform t = child->transform();
        child->setData(User_Roles::Rotation, extractAngleFromTransform(t));
    }
    for (auto item: my_items) my_scene->addToGroupNoUpdate(item);



    // !!!!! #DEBUG:    Rotation data
    if (m_relay->debugFlag(Debug_Flags::Rotation_Data)) {
        m_relay->setLabelText(Label_Names::Label_1, "Angle 1: " + QString::number(angle1) + ", Angle 2: " + QString::number(angle2));
        m_relay->setLabelText(Label_Names::Label_2, "Angle: " + QString::number(angle) +       ", Diff: " + QString::number(angle - start_angle) );
    }
    // !!!!! END

    rotate_mutex.unlock();
}


double SceneGraphicsView::extractAngleFromTransform(QTransform &from_transform)
{
    QTransform t = from_transform;
    return qRadiansToDegrees(qAtan2(t.m12(), t.m11()));
}


//####################################################################################
//##        Calculates angle from a center point to any target point, 0 = Up
//####################################################################################
double SceneGraphicsView::calcRotationAngleInDegrees(QPointF centerPt, QPointF targetPt)
{
    // Calculate the angle theta from the deltaY and deltaX values (atan2 returns radians values from [-PI, PI])
    // 0 currently points EAST
    // NOTE: By preserving Y and X param order to atan2,  we are expecting a CLOCKWISE angle direction
    double theta = qAtan2(targetPt.y() - centerPt.y(), targetPt.x() - centerPt.x());

    // Rotate the theta angle clockwise by 90 degrees (this makes 0 point NORTH)
    // NOTE: adding to an angle rotates it clockwise, subtracting would rotate it counter-clockwise
    theta += 3.141592653589793238463 / 2.0;

    // Convert from radians to degrees this will give you an angle from [0->270], [-180,0]
    double angle = theta * (180.0 / 3.141592653589793238463);

    // Convert to positive range (0-360) since we want to prevent negative angles, adjust them now
    // We can assume that atan2 will not return a negative value greater than one partial rotation
    if (angle < 0) { angle += 360; }

    return angle;
}












