//
//      Created by Stephens Nunnally on 1/8/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Graphics View Handling - View_Mode::Rotating
//
//

#include "library.h"

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
void SceneGraphicsView::startRotate(QPoint mouse_in_view)
{
    if (scene() == nullptr) return;

    m_view_mode = View_Mode::Rotating;

    // Set up our tooltip
    QPoint tip_pos = mouse_in_view + m_tool_tip->getOffset(); // + QWidget::mapToGlobal(this->rect().topLeft());
    m_tool_tip->move(tip_pos);
    m_tool_tip->show();

    // Grab starting angle of selection group before rotating starts
    SceneGraphicsScene *my_scene = dynamic_cast<SceneGraphicsScene *>(scene());

    // Store starting rotation of current selection group
    m_rotate_start_angle = my_scene->getSelectionGroupAsGraphicsItem()->data(User_Roles::Rotation).toDouble();

    // Store starting rotation of all selected items
    for (auto child : my_scene->getSelectionGroupItems()) {
        double child_angle = child->data(User_Roles::Rotation).toDouble();
        child->setData(User_Roles::Pre_Rotate_Rotation, child_angle);
    }

    // Store starting scene rect of initial selection bounding box
    m_rotate_start_rect = totalSelectedItemsSceneRect();
}


//####################################################################################
//##        Angle Comparision Functions
//####################################################################################
// Returns true is 'check_angle' in equal to 0, 90, 180, or 270, i.e. "square" angle
bool SceneGraphicsView::isSquare(double check_angle)
{
    check_angle = abs(check_angle);
    while (check_angle >= 360) check_angle -= 360;
    if (qFuzzyCompare(check_angle, 0))   return true;
    if (qFuzzyCompare(check_angle, 90))  return true;
    if (qFuzzyCompare(check_angle, 180)) return true;
    if (qFuzzyCompare(check_angle, 270)) return true;
    return false;
}



//####################################################################################
//##        Main Rotation Function
//####################################################################################
void SceneGraphicsView::rotateSelection(QPointF mouse_in_view)
{
    // Test for scene, convert to our custom class
    if (scene() == nullptr) return;
    SceneGraphicsScene    *my_scene = dynamic_cast<SceneGraphicsScene *>(scene());

    QGraphicsItem         *item =     my_scene->getSelectionGroupAsGraphicsItem();
    QList<QGraphicsItem*>  my_items = my_scene->getSelectionGroupItems();

    // ********** Calculate angle between starting mouse coordinate and latest mouse coordinate
    double angle1 = calcRotationAngleInDegrees( mapFromScene(m_rotate_start_rect.center()), m_origin);
    double angle2 = calcRotationAngleInDegrees( mapFromScene(m_rotate_start_rect.center()), mouse_in_view);

    double angle = m_rotate_start_angle + (angle2 - angle1);
    while (angle >=  360) { angle -= 360; }
    while (angle <= -360) { angle += 360; }


    // ********** Snaps angle to nearest 15 degree increment if angle is with +/-
    double tolerance =  ANGLE_TOLERANCE;
    double angle_step = ANGLE_STEP;

    double test_round = abs(angle);
    while (test_round >= angle_step) { test_round -= angle_step; }

    double angle_diff = angle;
    if (Dr::IsCloseTo(0, test_round, tolerance) || Dr::IsCloseTo(angle_step, test_round, tolerance)) {
        angle = round(angle / angle_step) * angle_step;
        angle_diff = angle - angle_diff;
    }
    else {
        angle_diff = 0;
    }

    // ********** Group selected items so we can apply new rotation to all selected items
    QGraphicsItemGroup *group = scene()->createItemGroup( { item } );

    // Offset difference of original center bounding box to possible slightly different center of new bounding box
    QPointF offset = group->sceneBoundingRect().center();
    offset.setX(offset.x() - (offset.x() - m_rotate_start_rect.center().x()) );
    offset.setY(offset.y() - (offset.y() - m_rotate_start_rect.center().y()) );

    // Load starting angle pre rotate, and store new angle in item
    double start_angle = item->data(User_Roles::Rotation).toDouble();
    item->setData(User_Roles::Rotation, angle);

    // ********** Create transform for new angle, apply it, and destroy temporary item group
    QTransform transform = QTransform().translate(offset.x(), offset.y()).rotate(angle - start_angle).translate(-offset.x(), -offset.y());
    group->setTransform(transform);
    scene()->destroyItemGroup(group);

    // ********** Break apart selection group so we can grab and store new rotation of all selected items, then put back together
    //for (auto item: my_items) my_scene->removeFromGroupNoUpdate(item);
    for (auto child : my_items) {

        double child_angle = child->data(User_Roles::Pre_Rotate_Rotation).toDouble();

        //m_relay->setLabelText(Label_Names::Label_1, "Child Angle: " + QString::number(child_angle));

        child_angle = child_angle + (angle2 - angle1) + angle_diff;
        while (child_angle >=  360) { child_angle -= 360; }
        while (child_angle <= -360) { child_angle += 360; }

        //m_relay->setLabelText(Label_Names::Label_2, "New Angle: " + QString::number(child_angle));

        child->setData(User_Roles::Rotation, child_angle);
    }
    //for (auto item: my_items) my_scene->addToGroupNoUpdate(item);



    // !!!!! #DEBUG:    Rotation data
    if (Dr::CheckDebugFlag(Debug_Flags::Rotation_Data)) {
        m_relay->setLabelText(Label_Names::Label_1, "Angle 1: " + QString::number(angle1) + ", Angle 2: " + QString::number(angle2));
        m_relay->setLabelText(Label_Names::Label_2, "Angle: " + QString::number(angle) +       ", Diff: " + QString::number(angle - start_angle) );
    }
    // !!!!! END
}



//####################################################################################
//##        Extract Angle, Scale and Skew from Transforms
//####################################################################################
double SceneGraphicsView::extractAngleFromTransform(QTransform &from_transform)
{
    QTransform t = from_transform;
    return qRadiansToDegrees(qAtan2(t.m12(), t.m11()));
}

Transform_Data SceneGraphicsView::decomposeTransform(QTransform &from_transform, bool qr_type)
{
    double a = from_transform.m11();    double c = from_transform.m12();
    double b = from_transform.m21();    double d = from_transform.m22();

    //double delta = a * d - b * c;

    double delta = a * d - b * c;

    Transform_Data transform;
    transform.rotation = 0;
    transform.scale = QPointF(0, 0);
    transform.skew =  QPointF(0, 0);

    // Apply the QR-like decomposition.
    if (qr_type) {
        if (qFuzzyCompare(a, 0) == false || qFuzzyCompare(b, 0) == false) {
            double r = sqrt(a * a + b * b);
            transform.rotation = b > 0 ? qAcos(a / r) : -qAcos(a / r);
            transform.scale = QPointF( r, delta / r );
            transform.skew =  QPointF( qAtan((a * c + b * d) / (r * r)),  0 );

        } else if (qFuzzyCompare(c, 0) == false || qFuzzyCompare(d, 0) == false) {
            double s = sqrt(c * c + d * d);
            transform.rotation = (3.141592653589793238463 / 2.0) - (d > 0 ? qAcos(-c / s) : -qAcos(c / s));
            transform.scale = QPointF( delta / s, s );
            transform.skew =  QPointF( 0, qAtan((a * c + b * d) / (s * s)) );
        }
    }

    // scaleX = √(a^2+c^2)
    // scaleY = √(b^2+d^2)
    // rotation = tan^-1(c/d) = tan^-1(-b/a) it will not work sometimes
    // rotation = a / scaleX  = d / scaleY

    if (Dr::IsCloseTo(0, transform.scale.x(), .000001)) transform.scale.setX(0);
    if (Dr::IsCloseTo(0, transform.scale.y(), .000001)) transform.scale.setY(0);
    if (Dr::IsCloseTo(0, transform.skew.x(),  .000001)) transform.skew.setX(0);
    if (Dr::IsCloseTo(0, transform.skew.y(),  .000001)) transform.skew.setY(0);

    transform.rotation *= (180.0 / 3.141592653589793238463);

    while (transform.rotation >=  360) { transform.rotation -= 360; }
    while (transform.rotation <= -360) { transform.rotation += 360; }

    return transform;
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












