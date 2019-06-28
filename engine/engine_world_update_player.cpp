//
//      Created by Stephens Nunnally on 5/20/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QtMath>
#include <QDebug>

#include "engine.h"
#include "engine_thing_object.h"
#include "engine_world.h"


//####################################################################################
//##    Definition of Global Variables to be used to pass info to callbacks
//####################################################################################
int      g_keyboard_x = 0;                              // Set to -1 for left, 1 for right
int      g_keyboard_y = 0;                              // Set to -1 for down, 1 for up
bool     g_jump_button = false;                         // Set to False for Not-Pressed, True for Pressed
Pedal    g_pedal = Pedal::None;                         // Pedal enumeration for rotate key state

cpVect   g_gravity_normal = cpv(0, 0);

QString  g_info = "";


//####################################################################################
//##    Calculate angles of player collisions to find if we're on ground
//####################################################################################
struct Ground_Data {
    cpVect normal;
    double dot_product;
};

static void SelectPlayerGroundNormal(cpBody *, cpArbiter *arb, Ground_Data *ground) {
    // Get normal vector of collision
    cpVect n = cpvneg( cpArbiterGetNormal(arb) );

    // Compare angle of gravity to angle of normal
    double dot = cpvdot( n, g_gravity_normal );

    // Store the lowest dot product we find, 1 == gravity, -1 == opposite direction of gravity, 0 == perpendicular to gravity
    if (dot < (*ground).dot_product) {
        (*ground).dot_product = dot;
        (*ground).normal = n;
    }
}


//####################################################################################
//##    Updates Jump Player Velocity
//####################################################################################
// #NOTE: The order of the ground check, jump operations is important
extern void PlayerUpdateVelocity(cpBody *body, cpVect gravity, cpFloat damping, cpFloat dt) {
    // Grab object from User Data
    DrEngineObject *object = static_cast<DrEngineObject*>(cpBodyGetUserData(body));

    // ***** Get Keys - If player is still active get keyboard status
    int key_y = 0,      key_x = 0,      key_jump = 0;
    if (!object->hasLostControl()) {
        key_x =     g_keyboard_x;
        key_y =     g_keyboard_y;
        key_jump =  g_jump_button;
    }

    // ***** Ground Check - Grab the grounding normal from last frame, if we hit the ground, turn off m_remaining_boost time
    Ground_Data ground;
    ground.dot_product = 2.0;                                                   // 2.0 is an impossible test number, dot products will always be between -1.0 to 1.0
    cpBodyEachArbiter(object->body, cpBodyArbiterIteratorFunc(SelectPlayerGroundNormal), &ground);

    // Figure out if any collision points count as ground or as wall
    object->setOnWall( ground.dot_product <= 0.50 && object->canWallJump() );   //  0.50 == approx up to ~30 degrees roof (slightly overhanging)
    object->setOnGround( ground.dot_product < -0.30 );                          // -0.30 == approx up to ~20 degrees slab (slightly less than vertical)
    if (object->isOnGround() || object->isOnWall()) {
        object->setLastTouchedGroundNormal( ground.normal );
        object->setLastTouchedGroundDot( ground.dot_product );
        object->setRemainingJumps( object->getJumpCount() );
        object->setRemainingBoost( 0.0 );
    }

    // Update wall jump time (gives player some time to do a wall jump)
    object->setRemainingWallTime( object->getRemainingWallTime() - dt );
    if (object->isOnWall())
        object->setRemainingWallTime( 0.25 );
    if (object->isOnGround() || object->getRemainingWallTime() < 0.0)
        object->setRemainingWallTime( 0.00 );


    // ***** Process Boost - Continues to provide jump velocity, although slowly fades
    if (object->getRemainingBoost() > 0) object->setRemainingBoost( object->getRemainingBoost() - dt );
    if (key_jump && object->getRemainingBoost() > 0.0) {
        cpVect  player_v = cpBodyGetVelocity( object->body );
        cpFloat jump_vx = object->getJumpForceX() * 2.0 * dt;
        cpFloat jump_vy = object->getJumpForceY() * 2.0 * dt;
        player_v = cpvadd(player_v, cpv(jump_vx, jump_vy));
        cpBodySetVelocity( object->body, player_v );
    }

    // ***** Process Jump - If the jump key was just pressed this frame and it wasn't pressed last frame, jump!
    if ((key_jump == true) && (object->getJumpState() == Jump_State::Need_To_Jump)) {
        if ( object->isOnGround() ||                                                                        // Jump from ground
            (object->getRemainingWallTime() > 0.0 ) ||                                                      // Jump from wall
            ///(object->isOnWall() && object->getRemainingJumps() > 0) ||                                   // Jump from wall if jumps remaining
            (object->canAirJump() && object->getRemainingJumps() > 0) ||                                    // Jump from air if jumps remaining
            (object->getRemainingJumps() > 0 && object->getJumpCount() != object->getRemainingJumps()) ||   // Already jumped once from ground and jumps remaining
            (object->getJumpCount() == -1) ) {                                                              // Unlimited jumping

            // Mark current jump button press as processed
            object->setJumpState( Jump_State::Jumped );

            // Calculate wall jump forces
            cpFloat jump_vx, jump_vy;
            if (!object->isOnGround() && object->getRemainingWallTime() > 0.0) {
                double angle = atan2(object->getLastTouchedGroundNormal().y, object->getLastTouchedGroundNormal().x) - atan2(g_gravity_normal.y, g_gravity_normal.x);
                angle = qRadiansToDegrees( angle ) - 180;
                ///qDebug() << "Wall jump - Angle: " << angle << ", Dot: " << object->getLastTouchedGroundDot();
                if (angle < -180) angle += 360;
                if (angle >  180) angle -= 360;
                angle /= 3;

                QPointF wall_jump_force = QTransform().rotate(angle).map( QPointF(object->getJumpForceX(), object->getJumpForceY()) );
                jump_vx = wall_jump_force.x() * 2.0;
                jump_vy = wall_jump_force.y() * 2.0;

            // Calculate ground jump forces
            } else {
                jump_vx = object->getJumpForceX() * 2.0;   ///cpfsqrt(2.0 * object->getJumpForceX() * -gravity.x);
                jump_vy = object->getJumpForceY() * 2.0;   ///cpfsqrt(2.0 * object->getJumpForceY() * -gravity.y);
            }

            // Starting a new jump so partially cancel any previous jump forces
            cpVect  player_v = cpBodyGetVelocity( object->body );
            if (player_v.x < 0) {
                if (jump_vx < 0) {
                    if (player_v.x - (jump_vx*.5) < 0) player_v.x -= jump_vx*.5;
                    else                               player_v.x = 0;
                } else if (jump_vx > 0) {
                    player_v.x = 0;
                }
            } else if (player_v.x > 0) {
                if (jump_vx > 0) {
                    if (player_v.x - (jump_vx*.5) > 0) player_v.x -= jump_vx*.5;
                    else                               player_v.x = 0;
                } else if (jump_vx < 0) {
                    player_v.x = 0;
                }
            }
            if (player_v.y < 0) {
                if (jump_vy < 0) {
                    if (player_v.y - (jump_vy*.5) < 0) player_v.y -= jump_vy*.5;
                    else                               player_v.y = 0;
                } else if (jump_vy > 0) {
                    player_v.y = 0;
                }
            } else if (player_v.y > 0) {
                if (jump_vy > 0) {
                    if (player_v.y - (jump_vy*.5) > 0) player_v.y -= jump_vy*.5;
                    else                               player_v.y = 0;
                } else if (jump_vy < 0) {
                    player_v.y = 0;
                }
            }
            ///if (qFuzzyCompare(jump_vx, 0) == false) player_v.x = 0;
            ///if (qFuzzyCompare(jump_vy, 0) == false) player_v.y = 0;

            player_v = cpvadd(player_v, cpv(jump_vx, jump_vy));
            cpBodySetVelocity( object->body, player_v );

            // Reset wall_timeout, jump timeout boost and subtract remaining jumps until ground
            object->setRemainingWallTime( 0.0 );
            object->setRemainingBoost( static_cast<double>(object->getJumpTimeout()) / 1000.0 );
            object->setRemainingJumps( object->getRemainingJumps() - 1 );
            if (object->getRemainingJumps() < c_unlimited_jump) object->setRemainingJumps( c_unlimited_jump );
        }
    }

    // If jump button is let go, reset this objects ability to receive a jump command
    if (!g_jump_button) object->setJumpState( Jump_State::Need_To_Jump );



    // ***** Velocity - Target horizontal speed for air / ground control
    cpVect  body_v = cpBodyGetVelocity( object->body );

    // Calculate target velocity, includes any Forced Movement
    cpFloat target_vx = (object->getMoveSpeedX() * key_x) + object->getForcedSpeedX();
    cpFloat target_vy = (object->getMoveSpeedY() * key_y) + object->getForcedSpeedY();

    // This code subtracts gravity from target speed, not sure if we want to leave this in
    //      (useful for allowing movement force against gravity for m_cancel_gravity property, i.e. climbing up ladders)
    if (target_vy < 0) {
        target_vy -= gravity.y * object->getTempGravityMultiplier();
        if (target_vy > 0) target_vy = 0;
    } else if (target_vy > 0) {
        target_vy += gravity.y * object->getTempGravityMultiplier();
        if (target_vy < 0) target_vy = 0;
    }
    if (target_vx < 0) {
        target_vx -= gravity.x * object->getTempGravityMultiplier();
        if (target_vx > 0) target_vx = 0;
    } else if (target_vx > 0) {
        target_vx += gravity.x * object->getTempGravityMultiplier();
        if (target_vx < 0) target_vx = 0;
    }

    // Some drag multipliers
    double c_buffer =      0.001;
    double c_drag_ground = 0.005;
    double c_drag_air =    0.005;
    double c_drag_rotate = 0.025;

    // This increases slowdown speed while in contact with a ladder (cancel gravity object)
    if (qFuzzyCompare(object->getTempGravityMultiplier(), 1.0) == false) {
        c_drag_air    = (c_drag_air * object->getTempGravityMultiplier()) + 0.0001;
        c_drag_ground = (c_drag_ground * object->getTempGravityMultiplier()) + 0.0001;
    }

    // Calculate air / ground acceleration
    double air_drag =       object->getAirDrag();
    double ground_drag =    object->getGroundDrag();
    double air_accel_x =    object->getMoveSpeedX() / (air_drag + c_buffer);
    double air_accel_y =    object->getMoveSpeedY() / (air_drag + c_buffer);
    double ground_accel_x = object->getMoveSpeedX() / (ground_drag + c_buffer);
    double ground_accel_y = object->getMoveSpeedY() / (ground_drag + c_buffer);

    // Interpolate towards desired velocity if in air
    if (!object->isOnGround() && !object->isOnWall()) {
        if ((qFuzzyCompare(body_v.x, 0) == false && qFuzzyCompare(target_vx, 0) == false) ||
            (body_v.x <= 0 && target_vx > 0) || (body_v.x >= 0 && target_vx < 0))
                body_v.x = cpflerpconst( body_v.x, target_vx, air_accel_x * dt);
        else    body_v.x = cpflerpconst( body_v.x, 0, air_drag / c_drag_air * dt);

        if ((qFuzzyCompare(body_v.y, 0) == false && qFuzzyCompare(target_vy, 0) == false) ||
            (body_v.y <= 0 && target_vy > 0) || (body_v.y >= 0 && target_vy < 0))
                body_v.y = cpflerpconst( body_v.y, target_vy, air_accel_y * dt);
        else    body_v.y = cpflerpconst( body_v.y, 0, air_drag / c_drag_air * dt);

    // Interpolate towards desired velocity if on ground / wall
    } else {
        if (qFuzzyCompare(target_vx, 0) == false)
                body_v.x = cpflerpconst( body_v.x, target_vx, ground_accel_x * dt);
        else    body_v.x = cpflerpconst( body_v.x, target_vx, ground_drag / c_drag_ground * dt);

        if (qFuzzyCompare(target_vy, 0) == false)
                body_v.y = cpflerpconst( body_v.y, target_vy, ground_accel_y * dt);
        else    body_v.y = cpflerpconst( body_v.y, target_vy, ground_drag / c_drag_ground * dt);
    }


    // ***** Rotation drag
    double body_r = cpBodyGetAngularVelocity( body );
    body_r = cpflerpconst( body_r, 0, object->getRotateDrag() / c_drag_rotate * dt);
    cpBodySetAngularVelocity( body, body_r );


    // ***** Max Speed - Limit Velocity
    body_v.x = cpfclamp(body_v.x, -object->getMaxSpeedX(), object->getMaxSpeedX());
    body_v.y = cpfclamp(body_v.y, -object->getMaxSpeedY(), object->getMaxSpeedY());
    cpBodySetVelocity( object->body, body_v );


    // ***** Update Velocity - #NOTE: MUST CALL actual Update Velocity function some time during this callback!
    if (object->ignoreGravity()) {
        cpBodyUpdateVelocityNoGravity(body, gravity, damping, dt);
    } else {
        cpBodyUpdateVelocity(body, cpv(gravity.x * object->getTempGravityMultiplier(), gravity.y * object->getTempGravityMultiplier()), damping, dt);
    }
}


























