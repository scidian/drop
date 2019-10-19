//
//      Created by Stephens Nunnally on 1/8/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Graphics View Handling - View_Mode::Rotating
//
//
#include <QtMath>

#include "colors/colors.h"
#include "debug.h"
#include "editor_item.h"
#include "editor_scene.h"
#include "editor_view.h"
#include "globals.h"
#include "interface_editor_relay.h"
#include "helper.h"
#include "helper_qt.h"
#include "project/project.h"
#include "project/project_world.h"
#include "project/project_stage.h"
#include "project/project_thing.h"
#include "settings/settings.h"
#include "settings/settings_component.h"
#include "settings/settings_component_property.h"


//####################################################################################
//##    Starts rotating mode
//####################################################################################
void DrView::startRotate(QPoint mouse_in_view, bool use_tool_tip) {
    // Store starting rotation of current selection group
    m_rotate_start_angle = my_scene->getSelectionAngle();

    // Store starting rotation of all selected items
    for (auto child : my_scene->getSelectionItems()) {
        double child_angle = child->data(User_Roles::Rotation).toDouble();
        child->setData(User_Roles::Pre_Rotate_Rotation, child_angle);
    }

    // Set up our tooltip
    if (use_tool_tip)
        m_tool_tip->startToolTip(m_view_mode, mouse_in_view, m_rotate_start_angle);
}


//####################################################################################
//##    Main Rotation Function
//####################################################################################
void DrView::rotateSelection(QPointF mouse_in_view, bool use_exact_angle, double angle_to_use) {

    // Test for scene, convert to our custom class
    if (scene() == nullptr) return;
    QList<QGraphicsItem*>  my_items = my_scene->getSelectionItems();

    // ********** Calculate angle between starting mouse coordinate and latest mouse coordinate
    double angle, angle1, angle2;
    if (!use_exact_angle) {
        QPointF  center_point = mapFromScene( m_selection_points[Position_Flags::Center] );
        angle1 = Dr::CalcRotationAngleInDegrees( DrPointF(center_point.x(), center_point.y()), DrPointF(m_origin.x(), m_origin.y() ));
        angle2 = Dr::CalcRotationAngleInDegrees( DrPointF(center_point.x(), center_point.y()), DrPointF(mouse_in_view.x(), mouse_in_view.y() ));
    } else {
        angle1 = 0;
        angle2 = angle_to_use;
    }
    angle = m_rotate_start_angle + (angle2 - angle1);

    // ********** Snaps angle to nearest c_angle_step (15) degree increment if angle is with +/-, or to grid angle
    //            Tolerance is decreased the further the mouse cursor is away from the center of the selection rectangle
    double mouse_distance_to_center = QLineF(mouse_in_view, mapFromScene( m_selection_points[Position_Flags::Center] )).length();
    double tolerance_multiplier = 1;
    if (mouse_distance_to_center > 200) tolerance_multiplier = (200 / mouse_distance_to_center);
    double tolerance =  c_angle_tolerance * tolerance_multiplier;
    double angle_step = c_angle_step;

    double test_round = abs(angle);
    while (test_round >= angle_step) { test_round -= angle_step; }

    double angle_adjust;
    double angle_diff = angle;

    if (Dr::IsCloseTo(0.0, test_round, tolerance) || Dr::IsCloseTo(angle_step, test_round, tolerance)) {
        angle = round(angle / angle_step) * angle_step;
        angle_adjust = angle - angle_diff;
    } else if (Dr::IsSimilarAngle(m_editor_relay->currentViewGridAngle(), angle, tolerance)) {
        angle = Dr::Closest90DegreeAngle(angle, m_editor_relay->currentViewGridAngle());
        angle_adjust = angle - angle_diff;
    } else {
        angle_adjust = 0;
    }

    // ********** If we didnt snap to nearest ANGLE_STEP, try to snap to nearest .1 decimal place
    if (Dr::FuzzyCompare(angle_adjust, 0.0)) {
        angle = QString::number(angle, 'f', 1).toDouble();
        angle_adjust = angle - angle_diff;
    }

    // ********** Make sure angle is within -360 to 360
    while (angle >=  360) { angle -= 360; }
    while (angle <= -360) { angle += 360; }

    // ********** Add in new angle to all selected items and store in item data
    for (auto child : my_items) {
        double child_angle = child->data(User_Roles::Pre_Rotate_Rotation).toDouble();

        child_angle = child_angle + (angle2 - angle1) + angle_adjust;
        while (child_angle >=  360) { child_angle -= 360; }
        while (child_angle <= -360) { child_angle += 360; }

        child->setData(User_Roles::Rotation, child_angle);
    }

    // ********** Group selected items so we can apply new rotation to all selected items
    QGraphicsItemGroup *group = scene()->createItemGroup( my_items );

    // Offset difference of original center bounding box to possible slightly different center of new bounding box
    QPointF offset = group->sceneBoundingRect().center();
    offset.setX(offset.x() - (offset.x() - m_selection_points[Position_Flags::Center].x()) );
    offset.setY(offset.y() - (offset.y() - m_selection_points[Position_Flags::Center].y()) );

    // Load starting angle pre rotate, and store new angle in item
    double start_angle = my_scene->getSelectionAngle();
    my_scene->setSelectionAngle(angle);
    m_tool_tip->updateToolTipData(angle);

    // ********** Create transform for new angle, apply it, and destroy temporary item group
    QTransform transform = QTransform().translate(offset.x(), offset.y()).rotate(angle - start_angle).translate(-offset.x(), -offset.y());
    group->setTransform(transform);
    scene()->destroyItemGroup(group);

    my_scene->updateSelectionBox();

    // !!!!! #DEBUG:    Rotation data
    if (Dr::CheckDebugFlag(Debug_Flags::Label_Rotation_Data)) {
        Dr::SetLabelText(Label_Names::Label_1, "Angle 1: " + QString::number(angle1) + ", Angle 2: " + QString::number(angle2));
        Dr::SetLabelText(Label_Names::Label_2, "Angle: " +   QString::number(angle) +     ", Diff: " + QString::number(angle - start_angle) );
    }
    // !!!!! END

}



//####################################################################################
//##    Extract Angle, Scale and Skew from Transforms
//####################################################################################
double DrView::extractAngleFromTransform(QTransform &from_transform) {
    QTransform t = from_transform;
    return qRadiansToDegrees(qAtan2(t.m12(), t.m11()));
}

Transform_Data DrView::decomposeTransform(QTransform &from_transform, bool qr_type) {
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
        if (Dr::FuzzyCompare(a, 0.0) == false || Dr::FuzzyCompare(b, 0.0) == false) {
            double r = sqrt(a * a + b * b);
            transform.rotation = b > 0 ? qAcos(a / r) : -qAcos(a / r);
            transform.scale = QPointF( r, delta / r );
            transform.skew =  QPointF( qAtan((a * c + b * d) / (r * r)),  0 );

        } else if (Dr::FuzzyCompare(c, 0.0) == false || Dr::FuzzyCompare(d, 0.0) == false) {
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

    if (Dr::IsCloseTo(0.0, transform.scale.x(), 0.000001)) transform.scale.setX(0);
    if (Dr::IsCloseTo(0.0, transform.scale.y(), 0.000001)) transform.scale.setY(0);
    if (Dr::IsCloseTo(0.0, transform.skew.x(),  0.000001)) transform.skew.setX(0);
    if (Dr::IsCloseTo(0.0, transform.skew.y(),  0.000001)) transform.skew.setY(0);

    transform.rotation *= (180.0 / 3.141592653589793238463);

    while (transform.rotation >=  360) { transform.rotation -= 360; }
    while (transform.rotation <= -360) { transform.rotation += 360; }

    return transform;
}










