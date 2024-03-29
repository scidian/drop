//
//      Created by Stephens Nunnally on 11/15/2019, (c) 2019 Scidian Studios, All Rights Reserved
//
//  File:
//
//
//
#include <numeric>

#include "core/dr_math.h"
#include "engine/engine.h"
#include "engine/engine_camera.h"
#include "engine/engine_spawner.h"
#include "engine/form_engine.h"
#include "engine/opengl/opengl.h"
#include "engine/thing/components/thing_comp_physics.h"
#include "engine/thing/components/thing_comp_player.h"
#include "engine/thing/components/thing_comp_soft_body.h"
#include "engine/thing/engine_thing.h"
#include "engine/world/engine_world.h"
#include "project/dr_project.h"
#include "project/entities/dr_world.h"
#include "project/entities/dr_stage.h"


//####################################################################################
//##    Updates Physics Object Velocity
//####################################################################################
extern void ObjectUpdateVelocity(cpBody *body, cpVect gravity, cpFloat damping, cpFloat dt) {
    // ***** Grab object from User Data
    DrEngineThing *thing = static_cast<DrEngineThing*>(cpBodyGetUserData(body));
    if (thing == nullptr) return;
    if (thing->compPhysics() == nullptr) return;

    // Adjust object gravity
    gravity.x *= thing->compPhysics()->getGravityScale().x;
    gravity.y *= thing->compPhysics()->getGravityScale().y;

    // ***** Update Velocity - #NOTE: MUST CALL actual Update Velocity function some time during this callback!
    cpBodyUpdateVelocity(body, gravity, damping, dt);
}


//####################################################################################
//##    Updates Kinematic Angular Velocity
//####################################################################################
extern void KinematicUpdateVelocity(cpBody *body, cpVect gravity, cpFloat damping, cpFloat dt) {
    // Grab object from User Data
    DrEngineThing *thing = static_cast<DrEngineThing*>(cpBodyGetUserData(body));
    if (thing == nullptr) return;
    ThingCompPhysics *physics = thing->compPhysics();
    if (physics == nullptr) return;

    // Try and turn object to rotate toward player
    if (physics->getRotateToPlayer()) {
        // Angle to Player
        cpVect my_pos =     cpBodyGetPosition(body);
        double my_angle =   cpBodyGetAngle(body);
        double angle1 =     Dr::CalcRotationAngleInDegrees(DrPointF(my_pos.x, my_pos.y), g_player_position);
               angle1 =     Dr::EqualizeAngle0to360(angle1);

        // Angle of Object
        DrPointF up =       Dr::RotatePointAroundOrigin(DrPointF(my_pos.x, my_pos.y + 1.0), DrPointF(my_pos.x, my_pos.y), my_angle, true);
        double angle2 =     Dr::CalcRotationAngleInDegrees(DrPointF(my_pos.x, my_pos.y), up);
               angle2 =     Dr::EqualizeAngle0to360(angle2);

        // Set Rotate Direction Towards Player
        double angle3 =     Dr::FindClosestAngle180(angle2, angle1);
        double angle_vel =  abs(physics->getOriginalSpinVelocity());
        double new_spin =   (angle3 > angle2) ? angle_vel : -angle_vel;

        double angle_diff = abs(angle3 - angle2);
        if (angle_diff < 10.0) new_spin *= (angle_diff / 10.0);
        cpBodySetAngularVelocity(body, new_spin);

        ///g_info = "Angle 1: " +      std::to_string(angle1,     'f', 2) +
        ///       ", Angle 3: " +      std::to_string(angle2,     'f', 2) +
        ///       ", Angle Diff: " +   std::to_string(angle_diff, 'f', 2);
    }

    // Figure out new velocity based on current object angle
    if (physics->getUseAngleVelocity()) {
        double  angle =     Dr::RadiansToDegrees( cpBodyGetAngle(body) );
        double  x_scale =   (thing->getScaleX() < 0.f) ? -1.0 : 1.0;
        double  y_scale =   (thing->getScaleY() < 0.f) ? -1.0 : 1.0;

        DrPointF original = DrPointF(physics->getOriginalVelocityX(), physics->getOriginalVelocityY());
        DrPointF rotated =  Dr::RotatePointAroundOrigin( DrPointF(original.x * x_scale, original.y * y_scale), DrPointF(0, 0), angle);
        cpBodySetVelocity( body, cpv(rotated.x, rotated.y) );
    }

    // Call real update function
    cpBodyUpdateVelocity(body, gravity, damping, dt);
}













