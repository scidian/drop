//
//      Created by Stephens Nunnally on 5/20/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QtMath>
#include <QDebug>

#include "engine/engine.h"
#include "engine/things/engine_thing_object.h"
#include "engine/world/engine_world.h"


//####################################################################################
//##    Definition of Global Variables to be used to pass info to callbacks
//####################################################################################
double          g_keyboard_x = 0;                               // Set to -1 for left, 1 for right
double          g_keyboard_y = 0;                               // Set to -1 for down, 1 for up
bool            g_jump_button = false;                          // Set to False for Not-Pressed, True for Pressed
bool            g_shoot_button = false;                         // Set to False for Not-Pressed, True for Pressed
Pedal           g_pedal = Pedal::None;                          // Pedal enumeration for rotate key state

cpVect          g_gravity_normal = cpv(0, 0);
DrPointF        g_player_position = DrPointF(0, 0);

DrPointF        g_mouse_position = DrPointF(0, 0);

QString         g_info = "";


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

    // ********** Grab object from User Data
    DrEngineObject *object = static_cast<DrEngineObject*>(cpBodyGetUserData(body));

    // Adjust object gravity
    gravity.x *= object->getGravityScale().x;
    gravity.y *= object->getGravityScale().y;

    // ********** Get Keys - If player is still active, get keyboard status
    double  key_x =     0;
    double  key_y =     0;
    bool    key_jump =  false;
    bool    key_shoot = false;
    Pedal   pedal = Pedal::None;

    if (!object->hasLostControl()) {
        key_x =     g_keyboard_x;
        key_y =     g_keyboard_y;
        key_jump =  g_jump_button;
        key_shoot = g_shoot_button;
        pedal =     g_pedal;

        cpVect pos = cpBodyGetPosition(object->body);
        g_player_position = DrPointF(pos.x, pos.y);

        if (object->shouldFlipImageX() && (key_x < 0 && !object->isFlippedX())) object->setFlipX(true);
        if (object->shouldFlipImageX() && (key_x > 0 &&  object->isFlippedX())) object->setFlipX(false);
        if (object->shouldFlipImageY() && (key_y < 0 && !object->isFlippedY())) object->setFlipY(true);
        if (object->shouldFlipImageY() && (key_y > 0 &&  object->isFlippedY())) object->setFlipY(false);
    }

    // ***** Mouse Rotate
    if (object->shouldMouseRotate()) {
        DrPointF pos = object->mapPositionToScreen();
        double angle = QLineF( QPointF(pos.x, pos.y), QPointF(g_mouse_position.x, g_mouse_position.y)).angle();
        cpBodySetAngle( object->body, Dr::DegreesToRadians(angle - 90.0) );
    }


    // ********** Ground Check - Grab the grounding normal from last frame, if we hit the ground, turn off m_remaining_boost time
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

    // Update wall jump time (gives player some time to do a wall jump), updated 11/12/2019: also give player time to do a ground jump (helps with bumpiness)
    object->setRemainingGroundTime( object->getRemainingGroundTime() - dt );
    object->setRemainingWallTime(   object->getRemainingWallTime() - dt );
    if (object->isOnWall()) {
        object->setRemainingGroundTime( 0.00 );
        object->setRemainingWallTime(   0.25 );
    }
    if (object->isOnGround()) {
        object->setRemainingGroundTime( 0.25 );
        object->setRemainingWallTime(   0.00 );
    }
    if (object->getRemainingGroundTime() < 0.0) object->setRemainingGroundTime( 0.00 );
    if (object->getRemainingWallTime()   < 0.0) object->setRemainingWallTime( 0.00 );


    // ********** Process Boost - Continues to provide jump velocity, although slowly fades
    if (object->getRemainingBoost() > 0) object->setRemainingBoost( object->getRemainingBoost() - dt );
    if (key_jump && (object->getRemainingBoost() > 0.0 || object->getJumpTimeout() < 0.0)) {
        cpVect  player_v = cpBodyGetVelocity( object->body );
        cpFloat jump_vx = object->getJumpForceX() * 2.0 * dt;
        cpFloat jump_vy = object->getJumpForceY() * 2.0 * dt;
        player_v = cpvadd(player_v, cpv(jump_vx, jump_vy));
        cpBodySetVelocity( object->body, player_v );
    }

    // ********** Process Jump - If the jump key was just pressed this frame and it wasn't pressed last frame, jump!
    if ((key_jump == true) && (object->getJumpState() == Jump_State::Need_To_Jump)) {
        if ((object->getRemainingGroundTime() > 0.0) ||                                                     // Jump from ground
            (object->getRemainingWallTime() > 0.0 ) ||                                                      // Jump from wall
            ///(object->isOnWall() && object->getRemainingJumps() > 0) ||                                   // Jump from wall if jumps remaining
            (object->canAirJump() && object->getRemainingJumps() > 0) ||                                    // Jump from air if jumps remaining
            (object->getRemainingJumps() > 0 && object->getJumpCount() != object->getRemainingJumps()) ||   // Already jumped once from ground and jumps remaining
            (object->getJumpCount() == -1) ) {                                                              // Unlimited jumping

            // Mark current jump button press as processed
            object->setJumpState( Jump_State::Jumped );

            // Calculate wall jump forces
            cpFloat jump_vx, jump_vy;
            if (object->getRemainingWallTime() > 0.0) {
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
            object->setRemainingGroundTime( 0.0 );
            object->setRemainingWallTime( 0.0 );
            object->setRemainingBoost( static_cast<double>(object->getJumpTimeout()) / 1000.0 );
            object->setRemainingJumps( object->getRemainingJumps() - 1 );
            if (object->getRemainingJumps() < c_unlimited_jump) object->setRemainingJumps( c_unlimited_jump );
        }
    }

    // If jump button is let go, reset this objects ability to receive a jump command
    if (!g_jump_button) object->setJumpState( Jump_State::Need_To_Jump );



    // ********** Velocity - Target horizontal speed for air / ground control
    cpVect  velocity = cpBodyGetVelocity( object->body );

    // Movement Speed, adjust to angle if desired
    double  button_speed_x = object->getMoveSpeedX() * key_x;
    double  button_speed_y = object->getMoveSpeedY() * key_y;
    double  forced_speed_x = object->getForcedSpeedX();
    double  forced_speed_y = object->getForcedSpeedY();
    double  rotate_speed =   object->getRotateSpeedZ();
    if (object->getAngleMovement()) {
        QTransform t = QTransform().rotate(object->getAngle());
        QPointF button_angle = t.map( QPointF(button_speed_x, button_speed_y) );
            button_speed_x = button_angle.x();
            button_speed_y = button_angle.y();
        QPointF forced_angle = t.map( QPointF(forced_speed_x, forced_speed_y) );
            forced_speed_x = forced_angle.x();
            forced_speed_y = forced_angle.y();
    }
    bool has_key_x = (Dr::FuzzyCompare(button_speed_x, 0.0)) ? false : true;
    bool has_key_y = (Dr::FuzzyCompare(button_speed_y, 0.0)) ? false : true;
    double move_speed_x = forced_speed_x + button_speed_x;
    double move_speed_y = forced_speed_y + button_speed_y;


    // ********** Drag / Acceleration
    double c_buffer =               0.001;
    double c_drag_ground =          0.005;
    double c_drag_air =             0.005;
    double c_drag_rotate =          0.100;
    double c_accel =                0.125;

    // Increase slowdown speed while in contact with a ladder (cancel gravity object)
    if (Dr::FuzzyCompare(object->getTempGravityMultiplier(), 1.0) == false) {
        c_drag_air    = (c_drag_air *    abs(object->getTempGravityMultiplier())) + 0.0001;
        c_drag_ground = (c_drag_ground * abs(object->getTempGravityMultiplier())) + 0.0001;
    }

    // Air / Ground / Rotation Drag
    double air_drag =       object->getAirDrag();
    double ground_drag =    object->getGroundDrag();
    double rotate_drag =    object->getRotateDrag();
    double acceleration =   object->getAcceleration();              // Acceleration, 1.0 is default, 5.0 is 5 times as slow, 0 is instant acceleration

    // Adjust Move Speeds for Drag
    double air_increase =    (air_drag    > 1.0) ? sqrt(air_drag) :    1.0;
    double ground_increase = (ground_drag > 1.0) ? sqrt(ground_drag) : 1.0;
    double rotate_increase = (rotate_drag > 1.0) ? sqrt(rotate_drag) : 1.0;
    if (!object->isOnGround() && !object->isOnWall()) {
        move_speed_x /= air_increase;
        move_speed_y /= air_increase;
    } else {
        move_speed_x /= ground_increase;
        move_speed_y /= ground_increase;
    }
    rotate_speed /= rotate_increase;


    // ********** Calculate Target Velocity, includes any Forced Movement
    double  target_vx = move_speed_x;
    double  target_vy = move_speed_y;
    double  target_spin = 0.0;
    switch (pedal) {
        case Pedal::Brake:              target_spin = 0.0;                          break;
        case Pedal::Clockwise:          target_spin = -object->getRotateSpeedZ();   break;
        case Pedal::CounterClockwise:   target_spin =  object->getRotateSpeedZ();   break;
        case Pedal::None:               break;
    }

    // This code subtracts gravity from target speed, not sure if we want to leave this in
    //      (useful for allowing movement force against gravity for m_cancel_gravity property, i.e. climbing up ladders)
    double actual_gravity_x = gravity.x * object->getTempGravityMultiplier();
    double actual_gravity_y = gravity.y * object->getTempGravityMultiplier();
    if (target_vx < 0 && gravity.x > 0) {
        target_vx += actual_gravity_x;
        if (target_vx > 0) target_vx = 0;
    } else if (target_vx > 0 && gravity.x < 0) {
        target_vx += actual_gravity_x;
        if (target_vx < 0) target_vx = 0;
    }
    if (target_vy < 0 && gravity.y > 0) {
        target_vy += actual_gravity_y;
        if (target_vy > 0) target_vy = 0;
    } else if (target_vy > 0 && gravity.y < 0) {
        target_vy += actual_gravity_y;
        if (target_vy < 0) target_vy = 0;
    }

    // ********** Rotation drag
    double body_r = cpBodyGetAngularVelocity( body );
    double rotate_multiplier = c_accel * acceleration;
    double rotate_accel =   abs(rotate_speed / (sqrt(rotate_drag)*rotate_multiplier + c_buffer));
    if (pedal == Pedal::None) {
        body_r = cpflerpconst( body_r, 0, rotate_drag / c_drag_rotate * dt);
    } else {
        body_r = cpflerpconst( body_r, target_spin, rotate_accel * dt);
    }
    cpBodySetAngularVelocity( body, body_r );


    // ********** Calculate Air / Ground Acceleration
    double accel_multiplier = c_accel * acceleration;
    double air_accel_x =    abs(move_speed_x /    (sqrt(air_drag)*accel_multiplier + c_buffer));
    double air_accel_y =    abs(move_speed_y /    (sqrt(air_drag)*accel_multiplier + c_buffer));
    double ground_accel_x = abs(move_speed_x / (sqrt(ground_drag)*accel_multiplier + c_buffer));
    double ground_accel_y = abs(move_speed_y / (sqrt(ground_drag)*accel_multiplier + c_buffer));

    bool target_x_is_zero = Dr::FuzzyCompare(target_vx, forced_speed_x);
    bool target_y_is_zero = Dr::FuzzyCompare(target_vy, forced_speed_y);
    bool speed_x_greater_than_forced = (forced_speed_x > 0 && velocity.x > forced_speed_x) || (forced_speed_x < 0 && velocity.x < forced_speed_x);
    bool speed_y_greater_than_forced = (forced_speed_y > 0 && velocity.y > forced_speed_y) || (forced_speed_y < 0 && velocity.y < forced_speed_y);
    ///bool forced_x_is_zero = Dr::FuzzyCompare(forced_speed_x, 0.0);
    ///bool forced_y_is_zero = Dr::FuzzyCompare(forced_speed_y, 0.0);

    // Interpolate towards desired velocity if in air
    if (!object->isOnGround() && !object->isOnWall()) {
        if (target_x_is_zero || (has_key_x == false && speed_x_greater_than_forced)) {
            velocity.x = cpflerpconst(velocity.x, object->getForcedSpeedX(), air_drag / c_drag_air * dt);
        } else {
            velocity.x = cpflerpconst(velocity.x, target_vx, air_accel_x * dt);
        }

        if (target_y_is_zero || (has_key_y == false && speed_y_greater_than_forced)) {
            velocity.y = cpflerpconst(velocity.y, object->getForcedSpeedY(), air_drag / c_drag_air * dt);
        } else {
            velocity.y = cpflerpconst(velocity.y, target_vy, air_accel_y * dt);
        }

    // Interpolate towards desired velocity if on ground / wall
    } else {
        if (target_x_is_zero || (has_key_x == false && speed_x_greater_than_forced)) {
            velocity.x = cpflerpconst(velocity.x, object->getForcedSpeedX(), ground_drag / c_drag_ground * dt);
        } else {
            velocity.x = cpflerpconst(velocity.x, target_vx, ground_accel_x * dt);
        }

        if (target_y_is_zero || (has_key_y == false && speed_y_greater_than_forced)) {
            velocity.y = cpflerpconst(velocity.y, object->getForcedSpeedY(), ground_drag / c_drag_ground * dt);
        } else {
            velocity.y = cpflerpconst(velocity.y, target_vy, ground_accel_y * dt);
        }
    }



    // ********** Max Speed - Limit Velocity
    velocity.x = cpfclamp(velocity.x, -object->getMaxSpeedX(), object->getMaxSpeedX());
    velocity.y = cpfclamp(velocity.y, -object->getMaxSpeedY(), object->getMaxSpeedY());



    // ********** Update Velocity - #NOTE: MUST CALL actual Update Velocity function some time during this callback!
    cpBodySetVelocity( object->body, velocity );
    cpBodyUpdateVelocity(body, cpv(actual_gravity_x, actual_gravity_y), damping, dt);
}


























