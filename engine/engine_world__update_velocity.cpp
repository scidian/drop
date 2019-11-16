//
//      Created by Stephens Nunnally on 11/15/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QtMath>
#include <QRandomGenerator>
#include <numeric>

#include "engine/engine.h"
#include "engine/engine_camera.h"
#include "engine/engine_spawner.h"
#include "engine_things/engine_thing_light.h"
#include "engine_things/engine_thing_object.h"
#include "engine/engine_world.h"
#include "form_engine.h"
#include "helper.h"
#include "helper_qt.h"
#include "opengl/opengl.h"
#include "project/project.h"
#include "project/project_world.h"
#include "project/project_stage.h"


//####################################################################################
//##    Updates Physics Object Velocity
//####################################################################################
extern void ObjectUpdateVelocity(cpBody *body, cpVect gravity, cpFloat damping, cpFloat dt) {
    // ***** Grab object from User Data
    DrEngineObject *object = static_cast<DrEngineObject*>(cpBodyGetUserData(body));
    if (object == nullptr) return;

    // ***** Update Velocity - #NOTE: MUST CALL actual Update Velocity function some time during this callback!
    if (object->ignoreGravity()) {
        cpBodyUpdateVelocityNoGravity(body, gravity, damping, dt);
    } else {
        cpBodyUpdateVelocity(body, gravity, damping, dt);
    }
}

//####################################################################################
//##    Updates Kinematic Angular Velocity
//####################################################################################
extern void KinematicUpdateVelocity(cpBody *body, cpVect gravity, cpFloat damping, cpFloat dt) {
    // Grab object from User Data
    DrEngineObject *object = static_cast<DrEngineObject*>(cpBodyGetUserData(body));
    if (object == nullptr) return;

    // Try and turn object to rotate toward player
    if (object->getRotateToPlayer()) {
        // Angle to Player
        cpVect my_pos =     cpBodyGetPosition(body);
        double my_angle =   cpBodyGetAngle(body);
        double angle1 =     Dr::CalcRotationAngleInDegrees(DrPointF(my_pos.x, my_pos.y), g_player_position);
               angle1 =     Dr::EqualizeAngle0to360(angle1);

        // Angle of Object
        DrPointF up =       Dr::RotatePointAroundOrigin(DrPointF(my_pos.x, my_pos.y - 1.0), DrPointF(my_pos.x, my_pos.y), my_angle, true);
        double angle2 =     Dr::CalcRotationAngleInDegrees(DrPointF(my_pos.x, my_pos.y), up);
               angle2 =     Dr::EqualizeAngle0to360(angle2);

        // Set Rotate Direction Towards Player
        double angle3 =     Dr::FindClosestAngle180(angle2, angle1);
        double angle_vel =  abs(object->getOriginalSpinVelocity());
        double new_spin =   (angle3 > angle2) ? angle_vel : -angle_vel;

        double angle_diff = abs(angle3 - angle2);
        if (angle_diff < 10.0) new_spin *= (angle_diff / 10.0);
        cpBodySetAngularVelocity(body, new_spin);

        ///g_info = "Angle 1: " +      QString::number(angle1,     'f', 2) +
        ///       ", Angle 3: " +      QString::number(angle2,     'f', 2) +
        ///       ", Angle Diff: " +   QString::number(angle_diff, 'f', 2);
    }

    // Figure out new velocity based on current object angle
    if (object->getUseAngleVelocity()) {
        double  angle = qRadiansToDegrees( cpBodyGetAngle(body) );
        QPointF original = QPointF( object->getOriginalVelocityX(), object->getOriginalVelocityY() );
        QPointF rotated = QTransform().rotate(angle).map(original);
        cpBodySetVelocity( body, cpv(rotated.x(), rotated.y()) );
    }

    // Call real update function
    cpBodyUpdateVelocity(body, gravity, damping, dt);
}













