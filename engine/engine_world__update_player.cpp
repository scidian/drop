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
#include "engine_things/engine_thing_object.h"
#include "engine_world.h"
#include "helper.h"


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

    // ***** Grab object from User Data
    DrEngineObject *object = static_cast<DrEngineObject*>(cpBodyGetUserData(body));

    // Check if object ignores gravity, adjust gravity to zero if so
    if (object->ignoreGravity()) gravity = cpvzero;


    // ***** Get Keys - If player is still active get keyboard status
    int key_y =     0;
    int key_x =     0;
    int key_jump =  0;

    if (!object->hasLostControl()) {
        key_x =     g_keyboard_x;
        key_y =     g_keyboard_y;
        key_jump =  g_jump_button;

        if (object->shouldFlipImageX() && (key_x < 0 && !object->isFlippedX())) object->setFlipX(true);
        if (object->shouldFlipImageX() && (key_x > 0 &&  object->isFlippedX())) object->setFlipX(false);
        if (object->shouldFlipImageY() && (key_y < 0 && !object->isFlippedY())) object->setFlipY(true);
        if (object->shouldFlipImageY() && (key_y > 0 &&  object->isFlippedY())) object->setFlipY(false);
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
    if (key_jump && (object->getRemainingBoost() > 0.0 || object->getJumpTimeout() < 0.0)) {
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
            ///if (Dr::FuzzyCompare(jump_vx, 0.0) == false) player_v.x = 0;
            ///if (Dr::FuzzyCompare(jump_vy, 0.0) == false) player_v.y = 0;

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
    cpVect  velocity = cpBodyGetVelocity( object->body );


    // Movement Speed, adjust to angle if desired
    cpFloat move_speed_x = object->getMoveSpeedX() * key_x;
    cpFloat move_speed_y = object->getMoveSpeedY() * key_y;
    if (object->getAngleMovement()) {
        QPointF force(move_speed_x, move_speed_y);
        QTransform t = QTransform().rotate(object->getAngle());
        force = t.map(force);
        move_speed_x = force.x();
        move_speed_y = force.y();
    }


    // Calculate target velocity, includes any Forced Movement
    cpFloat pre_forced_target_vx = move_speed_x;
    cpFloat pre_forced_target_vy = move_speed_y;
    bool has_forced_x = !(Dr::FuzzyCompare(object->getForcedSpeedX(), 0.0));
    bool has_forced_y = !(Dr::FuzzyCompare(object->getForcedSpeedY(), 0.0));
    cpFloat target_vx = pre_forced_target_vx + object->getForcedSpeedX();
    cpFloat target_vy = pre_forced_target_vy + object->getForcedSpeedY();


    // This code subtracts gravity from target speed, not sure if we want to leave this in
    //      (useful for allowing movement force against gravity for m_cancel_gravity property, i.e. climbing up ladders)
    if (target_vy < 0 && gravity.y > 0) {
        target_vy += gravity.y * object->getTempGravityMultiplier();
        if (target_vy > 0) target_vy = 0;

    } else if (target_vy > 0 && gravity.y < 0) {
        target_vy += gravity.y * object->getTempGravityMultiplier();
        if (target_vy < 0) target_vy = 0;
    }

    if (target_vx < 0 && gravity.x > 0) {
        target_vx += gravity.x * object->getTempGravityMultiplier();
        if (target_vx > 0) target_vx = 0;

    } else if (target_vx > 0 && gravity.x < 0) {
        target_vx += gravity.x * object->getTempGravityMultiplier();
        if (target_vx < 0) target_vx = 0;
    }


    // ***** Some drag / acceleration multipliers
    double c_buffer_x =             0.001;
    double c_buffer_y =             0.001;

    double c_drag_ground =          0.005;
    double c_drag_air =             0.005;
    double c_drag_rotate =          0.025;

    double c_accel_key_pressed =    0.125;
    double c_accel_no_key =         1.250;


    // This increases slowdown speed while in contact with a ladder (cancel gravity object)
    if (Dr::FuzzyCompare(object->getTempGravityMultiplier(), 1.0) == false) {
        c_drag_air    = (c_drag_air *    object->getTempGravityMultiplier()) + 0.0001;
        c_drag_ground = (c_drag_ground * object->getTempGravityMultiplier()) + 0.0001;
    }


    // Air / Ground Drag
    double air_drag =       object->getAirDrag();
    double ground_drag =    object->getGroundDrag();
    double rotate_drag =    object->getRotateDrag();

    // Acceleration
    double air_accel =      object->getAcceleration();      // 1.0 is default, 5.0 is 5 times as slow, 0 is instant acceleration
    double ground_accel =   object->getAcceleration();      // 1.0 is default, 5.0 is 5 times as slow, 0 is instant acceleration
    ///double rotate_accel =   1.0;    // NOT IMPLEMENTED YET!!!!!

    // Air Acceleration
    double air_x_multiplier = ((pre_forced_target_vx > 0 || pre_forced_target_vx < 0 || has_forced_x) ? c_accel_key_pressed : c_accel_no_key) * air_accel;
    double air_y_multiplier = ((pre_forced_target_vy > 0 || pre_forced_target_vy < 0 || has_forced_y) ? c_accel_key_pressed : c_accel_no_key) * air_accel;
    double air_accel_x =    abs(move_speed_x / (sqrt(air_drag)*air_x_multiplier + c_buffer_x));
    double air_accel_y =    abs(move_speed_y / (sqrt(air_drag)*air_y_multiplier + c_buffer_y));

    // Ground Acceleration
    double grd_x_multiplier = ((pre_forced_target_vx > 0 || pre_forced_target_vx < 0) ? c_accel_key_pressed : c_accel_no_key) * ground_accel;
    double grd_y_multiplier = ((pre_forced_target_vy > 0 || pre_forced_target_vy < 0) ? c_accel_key_pressed : c_accel_no_key) * ground_accel;
    double ground_accel_x = abs(move_speed_x / (sqrt(ground_drag)*grd_x_multiplier + c_buffer_x));
    double ground_accel_y = abs(move_speed_y / (sqrt(ground_drag)*grd_y_multiplier + c_buffer_y));

    // Interpolate towards desired velocity if in air
    if (!object->isOnGround() && !object->isOnWall()) {
        // If key is pressed
        if ( Dr::FuzzyCompare(pre_forced_target_vx, 0.0) == false ) {
            velocity.x = cpflerpconst( velocity.x, target_vx, air_accel_x * dt);
        } else {
            velocity.x = cpflerpconst( velocity.x, object->getForcedSpeedX(), air_drag / c_drag_air * dt);
        }

        // If key is pressed
        if ( Dr::FuzzyCompare(pre_forced_target_vy, 0.0) == false ) {
            velocity.y = cpflerpconst( velocity.y, target_vy, air_accel_y * dt);
        } else {
            velocity.y = cpflerpconst( velocity.y, object->getForcedSpeedY(), air_drag / c_drag_air * dt);
        }

    // Interpolate towards desired velocity if on ground / wall
    } else {
        // If key is pressed
        if (Dr::FuzzyCompare(target_vx, 0.0) == false) {
            velocity.x = cpflerpconst( velocity.x, target_vx, ground_accel_x * dt);
        } else {
            velocity.x = cpflerpconst( velocity.x, target_vx, ground_drag / c_drag_ground * dt);
        }

        // If key is pressed
        if (Dr::FuzzyCompare(target_vy, 0.0) == false) {
            velocity.y = cpflerpconst( velocity.y, target_vy, ground_accel_y * dt);
        } else {
            velocity.y = cpflerpconst( velocity.y, target_vy, ground_drag / c_drag_ground * dt);
        }
    }

    // ***** Rotation drag
    double body_r = cpBodyGetAngularVelocity( body );
    ///if (g_pedal == Pedal::None)
    ///    body_r = cpflerpconst( body_r, 0, rotate_accel * dt);
    ///else
        body_r = cpflerpconst( body_r, 0, rotate_drag / c_drag_rotate * dt);
    cpBodySetAngularVelocity( body, body_r );


    // ***** Max Speed - Limit Velocity
    ///velocity.x += object->getForcedSpeedX();
    ///velocity.y += object->getForcedSpeedY();
    velocity.x = cpfclamp(velocity.x, -object->getMaxSpeedX(), object->getMaxSpeedX());
    velocity.y = cpfclamp(velocity.y, -object->getMaxSpeedY(), object->getMaxSpeedY());

    // ***** Fixed (Forced) Velocity
    ///bool fixed_x = (Dr::FuzzyCompare(object->getForcedSpeedX(), 0.0) == false);
    ///bool fixed_y = (Dr::FuzzyCompare(object->getForcedSpeedY(), 0.0) == false);
    ///if (fixed_x) velocity.x = object->getForcedSpeedX();
    ///if (fixed_y) velocity.y = object->getForcedSpeedY();


    // ***** Update Velocity - #NOTE: MUST CALL actual Update Velocity function some time during this callback!
    cpBodySetVelocity( object->body, velocity );

    if (object->ignoreGravity()) {
        cpBodyUpdateVelocityNoGravity(body, gravity, damping, dt);
    } else {
        cpVect multi_gravity = cpv(gravity.x * object->getTempGravityMultiplier(),
                                   gravity.y * object->getTempGravityMultiplier());
        cpBodyUpdateVelocity(body, multi_gravity, damping, dt);
    }

}


























