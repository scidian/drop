//
//      Created by Stephens Nunnally on 5/20/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "chipmunk/chipmunk.h"
#include "engine.h"


#define MOVE_SPEED_X    500.0                   // Movement speed x
#define MOVE_SPEED_Y      0.0                   // Movement speed y

#define MAX_SPEED_X    1000.0                   // Max fall speed x
#define MAX_SPEED_Y    1000.0                   // Max fall speed y



#define JUMP_FORCE_X      0.0                   // Jump force x
#define JUMP_FORCE_Y     90.0                   // Jump force y
#define JUMP_TIME_OUT   800.0                   // Milliseconds to allow for jump to continue to receive a boost when jump button is held down


#define PLATFORM_FRICTION 0.5                   // 0 to 1+, unknwon limit

#define AIR_DRAG_X        0.2                   // Air drag x, 0 to 1



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
    if (object->remaining_boost > 0) {
        object->remaining_boost -= dt;
    }
    if (key_jump && object->remaining_boost > 0.0) {
        cpVect  player_v = cpBodyGetVelocity( object->body );
        cpFloat jump_v = cpfsqrt(2.0 * JUMP_FORCE_Y * -gravity.y) * dt;
        player_v = cpvadd(player_v, cpv(0.0, jump_v));
        cpBodySetVelocity( object->body, player_v );
    }

    // ***** Process Jump - If the jump key was just pressed this frame and it wasn't pressed last frame, and we're on the ground, jump!
    if (    (g_jump_button == true) && (object->jump_state == Jump_State::Need_To_Jump) && (object->lost_control == false) &&
            (object->grounded || (object->jump_count == -1) || (object->remaining_jumps > 0))   ) {
        object->jump_state = Jump_State::Jumped;
        cpFloat jump_vx = cpfsqrt(2.0 * JUMP_FORCE_X * -gravity.x);
        cpFloat jump_vy = cpfsqrt(2.0 * JUMP_FORCE_Y * -gravity.y);

        cpVect  player_v = cpBodyGetVelocity( object->body );
        player_v.y = 0;                                                 // Starting a new jump so cancel out last jump
        player_v = cpvadd(player_v, cpv(jump_vx, jump_vy));
        cpBodySetVelocity( object->body, player_v );
        object->remaining_boost = JUMP_TIME_OUT / 1000;
        object->remaining_jumps--;
    }
    if (!g_jump_button) object->jump_state = Jump_State::Need_To_Jump;


    // ***** Target horizontal speed for air / ground control
    cpFloat target_vx = MOVE_SPEED_X * key_x;

    // Platform Friction
    for (auto shape : object->shapes) {
        cpShapeSetFriction( shape, PLATFORM_FRICTION );
    }


    // Velocity
    cpVect body_v = cpBodyGetVelocity( object->body );

    double air_accel =    MOVE_SPEED_X / 0.35;
    double ground_accel = MOVE_SPEED_X / 0.15;

    if (!object->grounded) {

        body_v.x = cpflerpconst( body_v.x, target_vx, air_accel * dt);

    } else {

        if (qFuzzyCompare(target_vx, 0) == false)
            body_v.x = cpflerpconst( body_v.x, target_vx, ground_accel * dt);
        else
            body_v.x = cpflerpconst( body_v.x, 0, ground_accel * dt);

    }


    // Max Speed - Limit Velocity
    body_v.x = cpfclamp(body_v.x, -MAX_SPEED_X, MAX_SPEED_X);
    body_v.y = cpfclamp(body_v.y, -MAX_SPEED_Y, MAX_SPEED_Y);
    cpBodySetVelocity( object->body, body_v );

    // ***** Update Velocity - NOTE: MUST CALL actual Update Velocity function some time during this callback!
    cpBodyUpdateVelocity(body, gravity, damping, dt);
}












