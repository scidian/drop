//
//      Created by Stephens Nunnally on 5/20/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "chipmunk/chipmunk.h"
#include "engine.h"


//######################################################################################################
//##    Definition of Global Variables to be used to pass info to callbacks
//######################################################################################################
int      g_keyboard_x = 0;
int      g_keyboard_y = 0;
bool     g_jump_button = false;
bool     g_rotate_cw = false;
bool     g_rotate_ccw = false;

cpVect   g_gravity_normal = cpv(0, 0);

QString  g_info = "";


//######################################################################################################
//##    Calculate angles of player collisions to find if we're on ground
//######################################################################################################
static void selectPlayerGroundNormal(cpBody *, cpArbiter *arb, double *smallest_dot_product) {
    // Get normal vector of collision
    cpVect n = cpvneg( cpArbiterGetNormal(arb) );

    // Compare angle of gravity to angle of normal
    double dot = cpvdot( n, g_gravity_normal );

    // Store the lowest dot product we find, 1 == gravity, -1 == opposite direction of gravity, 0 == perpendicular to gravity
    if (dot < (*smallest_dot_product)) (*smallest_dot_product) = dot;
}


//######################################################################################################
//##    Updates Jump Player Velocity
//######################################################################################################
// NOTE: The order of the ground check, jump operations is important
extern void playerUpdateVelocity(cpBody *body, cpVect gravity, cpFloat damping, cpFloat dt) {
    // Grab object from User Data
    SceneObject *object = static_cast<SceneObject*>(cpBodyGetUserData(body));

    // ***** Get Keys - If player is still active get keyboard status
    int key_y = 0,      key_x = 0,      key_jump = 0;
    int key_cw = 0,     key_ccw = 0;
    if (!object->lost_control) {
        key_x =     g_keyboard_x;
        key_y =     g_keyboard_y;
        key_jump =  g_jump_button;
        key_cw =    g_rotate_cw;
        key_ccw =   g_rotate_ccw;
    }

    // ***** Ground Check - Grab the grounding normal from last frame, if we hit the ground, turn off remaining_boost time
    double smallest_dot_product = 1.0;
    cpBodyEachArbiter(object->body, cpBodyArbiterIteratorFunc(selectPlayerGroundNormal), &smallest_dot_product);

    object->on_wall =  smallest_dot_product <  0.15;
    if (object->on_wall) object->remaining_wall_time = .25;                 // Give player some time to do a wall jump
    else                 object->remaining_wall_time -= dt;
    if (object->remaining_wall_time < 0) object->remaining_wall_time = 0;

    object->grounded = smallest_dot_product < -0.30;
    if (object->grounded || object->on_wall) {
        ///object->on_wall = false;
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
            (object->remaining_wall_time > 0.0 ) ||                                                 // Jump from wall
            ///(object->on_wall && object->remaining_jumps > 0) ||                                  // Jump from wall if jumps remaining
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
            if (object->remaining_jumps < -1) object->remaining_jumps = -1;
        }
    }
    // If jump button is let go, reset this objects ability to receive a jump command
    if (!g_jump_button) object->jump_state = Jump_State::Need_To_Jump;



    // ***** Velocity - Target horizontal speed for air / ground control
    cpVect  body_v = cpBodyGetVelocity( object->body );

    cpFloat target_vx = object->move_speed_x * key_x;
    cpFloat target_vy = object->move_speed_y * key_y;

    double air_accel_x =    object->move_speed_x / (object->air_drag + .001);
    double air_accel_y =    object->move_speed_y / (object->air_drag + .001);
    double ground_accel_x = object->move_speed_x / (object->ground_drag + .001);
    double ground_accel_y = object->move_speed_y / (object->ground_drag + .001);

    if (!object->grounded && (!object->on_wall)) {

        if ((qFuzzyCompare(body_v.x, 0) == false && qFuzzyCompare(target_vx, 0) == false) ||
            (body_v.x <= 0 && target_vx > 0) || (body_v.x >= 0 && target_vx < 0)) {
            body_v.x = cpflerpconst( body_v.x, target_vx, air_accel_x * dt);
        }

        if ((qFuzzyCompare(body_v.y, 0) == false && qFuzzyCompare(target_vy, 0) == false) ||
            (body_v.y <= 0 && target_vy > 0) || (body_v.y >= 0 && target_vy < 0)) {
            body_v.y = cpflerpconst( body_v.y, target_vy, air_accel_y * dt);
        }

    } else {

        if (qFuzzyCompare(target_vx, 0) == false)
            body_v.x = cpflerpconst( body_v.x, target_vx, ground_accel_x * dt);

        if (qFuzzyCompare(target_vy, 0) == false)
            body_v.y = cpflerpconst( body_v.y, target_vy, ground_accel_y * dt);
    }



    // ***** Max Speed - Limit Velocity
    body_v.x = cpfclamp(body_v.x, -object->max_speed_x, object->max_speed_x);
    body_v.y = cpfclamp(body_v.y, -object->max_speed_y, object->max_speed_y);
    cpBodySetVelocity( object->body, body_v );

    // ***** Update Velocity - NOTE: MUST CALL actual Update Velocity function some time during this callback!
    cpBodyUpdateVelocity(body, gravity, damping, dt);
}












