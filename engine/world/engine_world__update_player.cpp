//
//      Created by Stephens Nunnally on 5/20/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "core/dr_debug.h"
#include "engine/engine.h"
#include "engine/thing/engine_thing_object.h"
#include "engine/world/engine_world.h"


//####################################################################################
//##    Definition of Global Variables to be used to pass info to callbacks
//####################################################################################
double          g_keyboard_x =      0;                          // Set to -1 for left, 1 for right
double          g_keyboard_y =      0;                          // Set to -1 for down, 1 for up
bool            g_jump_button =     false;                      // Set to False for Not-Pressed, True for Pressed
bool            g_shoot_button =    false;                      // Set to False for Not-Pressed, True for Pressed
Pedal           g_pedal =           Pedal::None;                // Pedal enumeration for rotate key state

cpVect          g_gravity_normal =  cpv(0, 0);
DrPointF        g_player_position = DrPointF(0, 0);

DrPointF        g_mouse_position =  DrPointF(0, 0);

std::string     g_info =    "";

bool            g_bool =    false;
int             g_int =     1;
double          g_double =  1.0;


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
//##    Applies Jump Force
//####################################################################################
void ApplyJumpForce(DrEngineObject *object, cpVect player_vel, cpVect jump_vel, bool initial_jump) {
    player_vel = cpvadd(player_vel, jump_vel);

    // Soft Body Jump
    if ((object->body_style != Body_Style::Rigid_Body) && (object->compSoftBody() != nullptr)) {
        // Adjust initial velocity for soft bodies
        player_vel = player_vel * 0.85;

        // Apply force to Parent Object
        if (initial_jump) cpBodySetVelocity( object->body, player_vel );
        else cpBodyApplyForceAtWorldPoint( object->body, jump_vel * cpBodyGetMass(object->body) * 50.0, cpBodyGetPosition(object->body) );

        // Apply force to Children Soft Ball Objects
        for (auto ball_number : object->compSoftBody()->soft_balls) {
            DrEngineObject *soft_ball = object->world()->findObjectByKey(ball_number);
            if (soft_ball == nullptr) return;
            if (initial_jump) cpBodySetVelocity( soft_ball->body, player_vel );
            else cpBodyApplyForceAtWorldPoint( soft_ball->body, jump_vel * cpBodyGetMass(soft_ball->body) * 50.0, cpBodyGetPosition(soft_ball->body) );
        }

    // Normal Body Jump
    } else {
        cpBodySetVelocity( object->body, player_vel );
    }
}


//####################################################################################
//##    Updates Player Velocity / Handles Jump
//####################################################################################
// #NOTE: The order of the ground check, jump operations is important
extern void PlayerUpdateVelocity(cpBody *body, cpVect gravity, cpFloat damping, cpFloat dt) {

    // ********** Grab object from User Data
    DrEngineObject  *object = static_cast<DrEngineObject*>(cpBodyGetUserData(body));
    if (object == nullptr) {        cpBodyUpdateVelocity(body, gravity, damping, dt);   return; }
    ThingCompPlayer *comp_player = object->compPlayer();
    if (comp_player == nullptr) {   cpBodyUpdateVelocity(body, gravity, damping, dt);   return; }

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
    if (comp_player->shouldMouseRotate()) {
        DrPointF pos = object->mapPositionToScreen();
        double angle = Dr::CalcRotationAngleInDegrees(pos, g_mouse_position);
        cpBodySetAngle( object->body, Dr::DegreesToRadians(-angle) );
    }


    // ********** Ground Check - Grab the grounding normal from last frame, if we hit the ground, turn off m_remaining_boost time
    Ground_Data ground;
    ground.dot_product = 2.0;                                                               // 2.0 is an impossible test number, dot products will always be between -1.0 to 1.0
    cpBodyEachArbiter(object->body, cpBodyArbiterIteratorFunc(SelectPlayerGroundNormal), &ground);

    // Figure out if any collision points count as ground or as wall
    comp_player->setOnWall( ground.dot_product <= 0.50 && comp_player->canWallJump() );     //  0.50 == approx up to ~30 degrees roof (slightly overhanging)
    comp_player->setOnGround( ground.dot_product < -0.30 );                                 // -0.30 == approx up to ~20 degrees slab (slightly less than vertical)
    if (comp_player->isOnGround() || comp_player->isOnWall()) {
        comp_player->setLastTouchedGroundNormal( ground.normal );
        comp_player->setLastTouchedGroundDot( ground.dot_product );
        comp_player->setRemainingJumps( comp_player->getJumpCount() );
        comp_player->setRemainingBoost( 0.0 );
    }

    // Update wall jump time (gives player some time to do a wall jump), updated 11/12/2019: also give player time to do a ground jump (helps with bumpiness)
    comp_player->setRemainingGroundTime( comp_player->getRemainingGroundTime() - dt );
    comp_player->setRemainingWallTime(   comp_player->getRemainingWallTime() - dt );
    if (comp_player->isOnWall()) {
        comp_player->setRemainingGroundTime( 0.00 );
        comp_player->setRemainingWallTime(   0.25 );
    }
    if (comp_player->isOnGround()) {
        comp_player->setRemainingGroundTime( 0.25 );
        comp_player->setRemainingWallTime(   0.00 );
    }
    if (comp_player->getRemainingGroundTime() < 0.0) comp_player->setRemainingGroundTime( 0.00 );
    if (comp_player->getRemainingWallTime()   < 0.0) comp_player->setRemainingWallTime( 0.00 );


    // ********** Process Boost - Continues to provide jump velocity, although slowly fades
    if (comp_player->getRemainingBoost() > 0) comp_player->setRemainingBoost( comp_player->getRemainingBoost() - dt );
    if (key_jump && (comp_player->getRemainingBoost() > 0.0 || comp_player->getJumpTimeout() < 0.0)) {
        cpVect player_v = cpBodyGetVelocity( object->body );
        cpVect jump_v =   cpv(comp_player->getJumpForceX() * 2.0 * dt, comp_player->getJumpForceY() * 2.0 * dt);
        ApplyJumpForce(object, player_v, jump_v, false);
    }

    // ********** Process Jump - If the jump key was just pressed this frame and it wasn't pressed last frame, jump!
    if ((key_jump == true) && (comp_player->getJumpState() == Jump_State::Need_To_Jump)) {
        if ((comp_player->getRemainingGroundTime() > 0.0) ||                                                                // Jump from ground
            (comp_player->getRemainingWallTime() > 0.0 ) ||                                                                 // Jump from wall
            ///(object->isOnWall() && object->getRemainingJumps() > 0) ||                                                   // Jump from wall if jumps remaining
            (comp_player->canAirJump() && comp_player->getRemainingJumps() > 0) ||                                          // Jump from air if jumps remaining
            (comp_player->getRemainingJumps() > 0 && comp_player->getJumpCount() != comp_player->getRemainingJumps()) ||    // Already jumped once from ground and jumps remaining
            (comp_player->getJumpCount() == -1) ) {                                                                         // Unlimited jumping

            // Mark current jump button press as processed
            comp_player->setJumpState( Jump_State::Jumped );

            // Calculate wall jump forces
            cpFloat jump_vx, jump_vy;
            if (comp_player->getRemainingWallTime() > 0.0) {
                double angle = atan2(comp_player->getLastTouchedGroundNormal().y, comp_player->getLastTouchedGroundNormal().x) - atan2(g_gravity_normal.y, g_gravity_normal.x);
                angle = Dr::RadiansToDegrees( angle ) - 180;
                ///Dr::PrintDebug("Wall jump - Angle: " + std::to_string(angle) + ", Dot: " + std::to_string(object->getLastTouchedGroundDot()));
                if (angle < -180) angle += 360;
                if (angle >  180) angle -= 360;
                angle /= 3;

                DrPointF wall_jump_force = Dr::RotatePointAroundOrigin( DrPointF(comp_player->getJumpForceX(), comp_player->getJumpForceY()), DrPointF(0, 0), angle );
                jump_vx = wall_jump_force.x * 2.0;
                jump_vy = wall_jump_force.y * 2.0;

            // Calculate ground jump forces
            } else {
                jump_vx = comp_player->getJumpForceX() * 2.0;   ///cpfsqrt(2.0 * object->getJumpForceX() * -gravity.x);
                jump_vy = comp_player->getJumpForceY() * 2.0;   ///cpfsqrt(2.0 * object->getJumpForceY() * -gravity.y);
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

            ApplyJumpForce(object, player_v, cpv(jump_vx, jump_vy), true);

            // Reset wall_timeout, jump timeout boost and subtract remaining jumps until ground
            comp_player->setRemainingGroundTime( 0.0 );
            comp_player->setRemainingWallTime( 0.0 );
            comp_player->setRemainingBoost( static_cast<double>(comp_player->getJumpTimeout()) / 1000.0 );
            comp_player->setRemainingJumps( comp_player->getRemainingJumps() - 1 );
            if (comp_player->getRemainingJumps() < c_unlimited_jump) comp_player->setRemainingJumps( c_unlimited_jump );
        }
    }

    // If jump button is let go, reset this objects ability to receive a jump command
    if (!g_jump_button) comp_player->setJumpState( Jump_State::Need_To_Jump );



    // ********** Velocity - Target horizontal speed for air / ground control
    cpVect  velocity = cpBodyGetVelocity( object->body );

    // Movement Speed, adjust to angle if desired
    double  button_speed_x = comp_player->getMoveSpeedX() * key_x;
    double  button_speed_y = comp_player->getMoveSpeedY() * key_y;
    double  forced_speed_x = comp_player->getForcedSpeedX();
    double  forced_speed_y = comp_player->getForcedSpeedY();
    double  rotate_speed =   object->getRotateSpeedZ();
    if (comp_player->getAngleMovement()) {
        DrPointF button_angle = Dr::RotatePointAroundOrigin( DrPointF(button_speed_x, button_speed_y), DrPointF(0, 0), object->getAngle() );
            button_speed_x = button_angle.x;
            button_speed_y = button_angle.y;
        DrPointF forced_angle = Dr::RotatePointAroundOrigin( DrPointF(forced_speed_x, forced_speed_y), DrPointF(0, 0), object->getAngle() );
            forced_speed_x = forced_angle.x;
            forced_speed_y = forced_angle.y;
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
    if (Dr::FuzzyCompare(comp_player->getTempGravityMultiplier(), 1.0) == false) {
        c_drag_air    = (c_drag_air *    abs(comp_player->getTempGravityMultiplier())) + 0.0001;
        c_drag_ground = (c_drag_ground * abs(comp_player->getTempGravityMultiplier())) + 0.0001;
    }

    // Air / Ground / Rotation Drag
    double air_drag =       comp_player->getAirDrag();
    double ground_drag =    comp_player->getGroundDrag();
    double rotate_drag =    comp_player->getRotateDrag();
    double acceleration =   comp_player->getAcceleration();              // Acceleration, 1.0 is default, 5.0 is 5 times as slow, 0 is instant acceleration

    // Adjust Move Speeds for Drag
    double air_increase =    (air_drag    > 1.0) ? sqrt(air_drag) :    1.0;
    double ground_increase = (ground_drag > 1.0) ? sqrt(ground_drag) : 1.0;
    double rotate_increase = (rotate_drag > 1.0) ? sqrt(rotate_drag) : 1.0;
    if (!comp_player->isOnGround() && !comp_player->isOnWall()) {
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
    double actual_gravity_x = gravity.x * comp_player->getTempGravityMultiplier();
    double actual_gravity_y = gravity.y * comp_player->getTempGravityMultiplier();
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
    if (!comp_player->isOnGround() && !comp_player->isOnWall()) {
        if (target_x_is_zero || (has_key_x == false && speed_x_greater_than_forced)) {
            velocity.x = cpflerpconst(velocity.x, comp_player->getForcedSpeedX(), air_drag / c_drag_air * dt);
        } else {
            velocity.x = cpflerpconst(velocity.x, target_vx, air_accel_x * dt);
        }

        if (target_y_is_zero || (has_key_y == false && speed_y_greater_than_forced)) {
            velocity.y = cpflerpconst(velocity.y, comp_player->getForcedSpeedY(), air_drag / c_drag_air * dt);
        } else {
            velocity.y = cpflerpconst(velocity.y, target_vy, air_accel_y * dt);
        }

    // Interpolate towards desired velocity if on ground / wall
    } else {
        if (target_x_is_zero || (has_key_x == false && speed_x_greater_than_forced)) {
            velocity.x = cpflerpconst(velocity.x, comp_player->getForcedSpeedX(), ground_drag / c_drag_ground * dt);
        } else {
            velocity.x = cpflerpconst(velocity.x, target_vx, ground_accel_x * dt);
        }

        if (target_y_is_zero || (has_key_y == false && speed_y_greater_than_forced)) {
            velocity.y = cpflerpconst(velocity.y, comp_player->getForcedSpeedY(), ground_drag / c_drag_ground * dt);
        } else {
            velocity.y = cpflerpconst(velocity.y, target_vy, ground_accel_y * dt);
        }
    }



    // ********** Max Speed - Limit Velocity
    velocity.x = cpfclamp(velocity.x, -comp_player->getMaxSpeedX(), comp_player->getMaxSpeedX());
    velocity.y = cpfclamp(velocity.y, -comp_player->getMaxSpeedY(), comp_player->getMaxSpeedY());



    // ********** Update Velocity - #NOTE: MUST CALL actual Update Velocity function some time during this callback!
    if (object->body_style != Body_Style::Rigid_Body) {
        // Apply force to Children Soft Ball Objects
        double velocity_ratio_x = cpBodyGetVelocity(object->body).x / velocity.x;
        double velocity_ratio_y = cpBodyGetVelocity(object->body).y / velocity.y;

        // Apply force to Parent Soft Body Object
        cpBodySetVelocity( object->body, velocity );

//        g_info = "Velocity X: " + std::to_string(cpBodyGetVelocity(object->body).x) + ", RatioX: " + std::to_string(velocity_ratio_x);

//        for (auto ball_number : object->soft_balls) {
//            DrEngineObject *soft_ball = object->getWorld()->findObjectByKey(ball_number);
//            if (soft_ball == nullptr) return;
//            cpVect soft_ball_velocity = cpBodyGetVelocity(soft_ball->body);
//            if (Dr::RealDouble(velocity_ratio_x)) soft_ball_velocity.x *= velocity_ratio_x;
//            if (Dr::RealDouble(velocity_ratio_y)) soft_ball_velocity.y *= velocity_ratio_y;
//            ///cpBodySetVelocity( soft_ball->body, soft_ball_velocity );
//        }

    // Normal Body Jump
    } else {
        cpBodySetVelocity( object->body, velocity );
    }

    cpBodyUpdateVelocity(body, cpv(actual_gravity_x, actual_gravity_y), damping, dt);
}


























