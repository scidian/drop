//
//      Created by Stephens Nunnally on 5/20/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "chipmunk/chipmunk.h"
#include "engine.h"


#define MOVE_SPEED_X    400.0                   // Movement speed x
#define MOVE_SPEED_Y      0.0                   // Movement speed y

#define PLATFORM_FRICTION 0.45                  // 0 to 1+, unknown limit




// Definition of global variables to be used to pass info to callbacks
int      g_keyboard_x = 0;
int      g_keyboard_y = 0;
bool     g_jump_button = false;
QString  g_info = "";


//######################################################################################################
//##    Updates Jump Player Velocity
//######################################################################################################
static void selectPlayerGroundNormal(cpBody *, cpArbiter *arb, cpVect *ground_normal) {
    cpVect n = cpvneg( cpArbiterGetNormal(arb) );
    if (n.y > ground_normal->y)
        (*ground_normal) = n;
}

// NOTE: The order of the ground check, jump operations is important
extern void playerUpdateVelocity(cpBody *body, cpVect gravity, cpFloat damping, cpFloat dt) {
    // Grab object from User Data
    SceneObject *object = static_cast<SceneObject*>(cpBodyGetUserData(body));

    // ***** Get Keys - If player is still active get keyboard status
    int key_y = 0, key_x = 0, key_jump = 0;
    if (!object->lost_control) {
        key_x =     g_keyboard_x;
        key_y =     g_keyboard_y;
        key_jump =  g_jump_button;
    }

    // ***** Ground Check - Grab the grounding normal from last frame, if we hit the ground, turn off remaining_boost time
    cpVect ground_normal = cpvzero;
    cpBodyEachArbiter(object->body, cpBodyArbiterIteratorFunc(selectPlayerGroundNormal), &ground_normal);
    object->grounded = (ground_normal.y > 0.0);
    if (object->grounded || ground_normal.y < 0.0) {
        object->remaining_jumps = object->jump_count;
        object->remaining_boost = 0.0;
    }

    // ***** Process Boost - Continues to provide jump velocity, although slowly fades
    if (object->remaining_boost > 0) object->remaining_boost -= dt;
    if (key_jump && object->remaining_boost > 0.0) {
        cpVect  player_v = cpBodyGetVelocity( object->body );
        cpFloat jump_vx = object->jump_force_x * 2.0 * dt;
        cpFloat jump_vy = object->jump_force_y * 2.0 * dt;
        player_v = cpvadd(player_v, cpv(jump_vx, jump_vy));
        cpBodySetVelocity( object->body, player_v );
    }

    // ***** Process Jump - If the jump key was just pressed this frame and it wasn't pressed last frame, jump!
    if ((key_jump == true) && (object->jump_state == Jump_State::Need_To_Jump)) {
        if ((object->grounded) ||                                                                   // Jump from ground
            (object->air_jump && object->remaining_jumps > 0) ||                                    // Jump from air if jumps remaining
            (object->remaining_jumps > 0 && object->jump_count != object->remaining_jumps) ||       // Already jumped once from ground and jumps remaining
            (object->jump_count == -1) ) {                                                          // Unlimited jumping

            // Calculate jumps forces
            object->jump_state = Jump_State::Jumped;                                                // Mark current jump button press as processed
            cpFloat jump_vx = object->jump_force_x * 2.0; //cpfsqrt(2.0 * object->jump_force_x * -gravity.x);
            cpFloat jump_vy = object->jump_force_y * 2.0; //cpfsqrt(2.0 * object->jump_force_y * -gravity.y);

            // Starting a new jump so cancel out last jump forces
            cpVect  player_v = cpBodyGetVelocity( object->body );
            if (qFuzzyCompare(jump_vx, 0) == false) player_v.x = 0;
            if (qFuzzyCompare(jump_vy, 0) == false) player_v.y = 0;
            player_v = cpvadd(player_v, cpv(jump_vx, jump_vy));
            cpBodySetVelocity( object->body, player_v );

            // Reset timeout boost and subtract remaining jumps until ground
            object->remaining_boost = object->jump_timeout / 1000;
            object->remaining_jumps--;
        }
    }
    // If jump button is let go, reset this objects ability to receive a jump command
    if (!g_jump_button) object->jump_state = Jump_State::Need_To_Jump;




    // ***** Rotation - If object can rotate it doesn't receive movement from movement keys, process velocity and get out
    if (object->can_rotate) {
        cpBodyUpdateVelocity(body, gravity, damping, dt);
        return;
    }


    // ***** Target horizontal speed for air / ground control
    cpFloat target_vx = MOVE_SPEED_X * key_x;

    // Platform Friction
    for (auto shape : object->shapes) {
        cpShapeSetFriction( shape, PLATFORM_FRICTION );
    }


    // Velocity
    cpVect body_v = cpBodyGetVelocity( object->body );

    double air_accel =    MOVE_SPEED_X / (object->air_drag + .001);
    double ground_accel = MOVE_SPEED_X / (object->ground_drag + .001);

    if (!object->grounded) {

        if ((qFuzzyCompare(body_v.x, 0) == false && qFuzzyCompare(target_vx, 0) == false) ||
            (body_v.x <= 0 && target_vx > 0) || (body_v.x >= 0 && target_vx < 0)) {
            body_v.x = cpflerpconst( body_v.x, target_vx, air_accel * dt);
        }




    } else {

        if (qFuzzyCompare(target_vx, 0) == false)
            body_v.x = cpflerpconst( body_v.x, target_vx, ground_accel * dt);
        else
            body_v.x = cpflerpconst( body_v.x, 0, 1000 * PLATFORM_FRICTION * dt);

    }


    // Max Speed - Limit Velocity
    body_v.x = cpfclamp(body_v.x, -object->max_speed_x, object->max_speed_x);
    body_v.y = cpfclamp(body_v.y, -object->max_speed_y, object->max_speed_y);
    cpBodySetVelocity( object->body, body_v );

    // ***** Update Velocity - NOTE: MUST CALL actual Update Velocity function some time during this callback!
    cpBodyUpdateVelocity(body, gravity, damping, dt);
}












