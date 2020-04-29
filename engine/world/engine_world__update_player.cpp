//
//      Created by Stephens Nunnally on 5/20/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "core/dr_debug.h"
#include "engine/engine.h"
#include "engine/thing/components/thing_comp_physics.h"
#include "engine/thing/components/thing_comp_player.h"
#include "engine/thing/components/thing_comp_soft_body.h"
#include "engine/thing/engine_thing.h"
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

static void SelectPlayerGroundNormal(cpBody *body, cpArbiter *arb, Ground_Data *ground) {
    CP_ARBITER_GET_BODIES(arb, a, b)

    // Don't allow self to count as touching ground
    if (body == b) return;

    // Get normal vector of collision
    cpVect n = cpvneg( cpArbiterGetNormal(arb) );

    // Compare angle of gravity to angle of normal
    double dot = cpvdot(n, g_gravity_normal);

    // Store the lowest dot product we find, 1 == gravity, -1 == opposite direction of gravity, 0 == perpendicular to gravity
    if (dot < (*ground).dot_product) {
        (*ground).dot_product = dot;
        (*ground).normal = n;
    }
}


//####################################################################################
//##    Applies Jump Force
//####################################################################################
void ApplyJumpForce(DrEngineThing *thing, cpVect player_vel, cpVect jump_vel, bool initial_jump) {
    if (thing->compPhysics() == nullptr) return;

    player_vel = cpvadd(player_vel, jump_vel);

    // Soft Body Jump
    if ((thing->compPhysics()->body_style != Body_Style::Rigid_Body) && (thing->compSoftBody() != nullptr)) {
        // Adjust initial velocity for soft bodies
        player_vel = player_vel * 0.85;

        // Apply force to Parent Object
        double body_r = cpBodyGetAngularVelocity( thing->compPhysics()->body );
        if (initial_jump) {
            cpBodySetVelocity( thing->compPhysics()->body, player_vel );
            cpBodySetAngularVelocity(thing->compPhysics()->body, body_r * 2.5);
        } else {
            cpBodyApplyForceAtWorldPoint( thing->compPhysics()->body, jump_vel * cpBodyGetMass(thing->compPhysics()->body) * 50.0,
                                          cpBodyGetPosition(thing->compPhysics()->body) );
        }

        // Apply force to Children Soft Ball Objects
        for (auto child : thing->compSoftBody()->soft_balls) {
            if (child == nullptr) return;
            if (child->physics() == nullptr) return;
            if (initial_jump) {
                cpBodySetVelocity( child->physics()->body, player_vel );
            } else {
                cpBodyApplyForceAtWorldPoint( child->physics()->body, jump_vel * cpBodyGetMass(child->physics()->body) * 50.0,
                                              cpBodyGetPosition(child->physics()->body) );
            }
        }

    // Normal Body Jump
    } else {
        cpBodySetVelocity( thing->compPhysics()->body, player_vel );
    }
}


//####################################################################################
//##    Updates Player Velocity / Handles Jump
//####################################################################################
// #NOTE: The order of the ground check, jump operations is important
extern void PlayerUpdateVelocity(cpBody *body, cpVect gravity, cpFloat damping, cpFloat dt) {

    // ********** Grab object from User Data
    DrEngineThing *thing = static_cast<DrEngineThing*>(cpBodyGetUserData(body));
    if (thing == nullptr) {            cpBodyUpdateVelocity(body, gravity, damping, dt);    return; }
    if (thing->physics() == nullptr) { cpBodyUpdateVelocity(body, gravity, damping, dt);    return; }
    if (thing->player() == nullptr) {  cpBodyUpdateVelocity(body, gravity, damping, dt);    return; }

    // Adjust object gravity
    gravity.x *= thing->physics()->getGravityScale().x;
    gravity.y *= thing->physics()->getGravityScale().y;

    // ********** Get Keys - If player is still active, get keyboard status
    double  key_x =     0;
    double  key_y =     0;
    bool    key_jump =  false;
    bool    key_shoot = false;
    Pedal   pedal = Pedal::None;

    if (!thing->physics()->hasLostControl()) {
        key_x =     g_keyboard_x;
        key_y =     g_keyboard_y;
        key_jump =  g_jump_button;
        key_shoot = g_shoot_button;
        pedal =     g_pedal;

        cpVect pos = cpBodyGetPosition(thing->physics()->body);
        g_player_position = DrPointF(pos.x, pos.y);

        if (thing->physics()->shouldFlipImageX() && (key_x < 0 && !thing->physics()->isFlippedX())) thing->physics()->setFlipX(true);
        if (thing->physics()->shouldFlipImageX() && (key_x > 0 &&  thing->physics()->isFlippedX())) thing->physics()->setFlipX(false);
        if (thing->physics()->shouldFlipImageY() && (key_y < 0 && !thing->physics()->isFlippedY())) thing->physics()->setFlipY(true);
        if (thing->physics()->shouldFlipImageY() && (key_y > 0 &&  thing->physics()->isFlippedY())) thing->physics()->setFlipY(false);
    }

    // ***** Mouse Rotate
    if (thing->player()->shouldMouseRotate()) {
        DrPointF pos = thing->mapPositionToScreen();
        double angle = Dr::CalcRotationAngleInDegrees(pos, g_mouse_position);
        cpBodySetAngle( thing->physics()->body, Dr::DegreesToRadians(-angle) );
    }


    // ********** Ground Check - Grab the grounding normal from last frame, if we hit the ground, turn off m_remaining_boost time
    Ground_Data ground;
    ground.dot_product = 2.0;                                                               // 2.0 is an impossible test number, dot products will always be between -1.0 to 1.0
    cpBodyEachArbiter(thing->physics()->body, cpBodyArbiterIteratorFunc(SelectPlayerGroundNormal), &ground);

    // Found no ground but in collision with something (likely sensor shape like ladder, etc)
    bool jump_from_sensor = false;
    if (Dr::FuzzyCompare(ground.dot_product, 2.0)) {
        if (thing->physics()->listOfCollidingObjectKeys().size() != 0) {
            jump_from_sensor = true;
            ground.dot_product = -1.0;
            //object->compPlayer()->setTempGravityMultiplier( 1.0 );
        }
    }

    // Ground Check for physics children of soft bodies
    if (thing->compSoftBody() != nullptr) {
        for (size_t i = 0; i < thing->compSoftBody()->soft_balls.size(); ++i) {
            DrEngineThing *next_ball = thing->compSoftBody()->soft_balls[i];
            if (next_ball == nullptr) continue;
            cpBodyEachArbiter(next_ball->physics()->body, cpBodyArbiterIteratorFunc(SelectPlayerGroundNormal), &ground);
        }
    }

    // Figure out if any collision points count as ground or as wall
    thing->player()->setOnWall( ground.dot_product <= 0.50 && thing->player()->canWallJump() );     //  0.50 == approx up to ~30 degrees roof (slightly overhanging)
    thing->player()->setOnGround( ground.dot_product < -0.30 );                                 // -0.30 == approx up to ~20 degrees slab (slightly less than vertical)
    if (thing->player()->isOnGround() || thing->player()->isOnWall()) {
        thing->player()->setLastTouchedGroundNormal( ground.normal );
        thing->player()->setLastTouchedGroundDot( ground.dot_product );
        thing->player()->setRemainingJumps( thing->player()->getJumpCount() );
        thing->player()->setRemainingBoost( 0.0 );
    }

    // Update wall jump time (gives player some time to do a wall jump), updated 11/12/2019: also give player time to do a ground jump (helps with bumpiness)
    thing->player()->setRemainingGroundTime( thing->player()->getRemainingGroundTime() - dt );
    thing->player()->setRemainingWallTime(   thing->player()->getRemainingWallTime() - dt );
    if (thing->player()->isOnWall()) {
        thing->player()->setRemainingGroundTime( 0.00 );
        thing->player()->setRemainingWallTime(   0.25 );
    }
    if (thing->player()->isOnGround()) {
        thing->player()->setRemainingGroundTime( 0.25 );
        thing->player()->setRemainingWallTime(   0.00 );
    }
    if (thing->player()->getRemainingGroundTime() < 0.0) thing->player()->setRemainingGroundTime( 0.00 );
    if (thing->player()->getRemainingWallTime()   < 0.0) thing->player()->setRemainingWallTime( 0.00 );


    // ********** Process Boost - Continues to provide jump velocity, although slowly fades
    if (thing->player()->getRemainingBoost() > 0) thing->player()->setRemainingBoost( thing->player()->getRemainingBoost() - dt );
    if (key_jump && (thing->player()->getRemainingBoost() > 0.0 || thing->player()->getJumpTimeout() < 0.0)) {
        cpVect player_v = cpBodyGetVelocity( thing->physics()->body );
        cpVect jump_v =   cpv(thing->player()->getJumpForceX() * 2.0 * dt, thing->player()->getJumpForceY() * 2.0 * dt);
        ApplyJumpForce(thing, player_v, jump_v, false);
    }

    // ********** Process Jump - If the jump key was just pressed this frame and it wasn't pressed last frame, jump!
    if ((key_jump == true) && (thing->player()->getJumpState() == Jump_State::Need_To_Jump)) {
        if ((thing->player()->getRemainingGroundTime() > 0.0) ||                                                                        // Jump from ground
            (thing->player()->getRemainingWallTime() > 0.0 ) ||                                                                         // Jump from wall
            ///(object->isOnWall() && object->getRemainingJumps() > 0) ||                                                               // Jump from wall if jumps remaining
            (thing->player()->canAirJump() && thing->player()->getRemainingJumps() > 0) ||                                              // Jump from air if jumps remaining
            (thing->player()->getRemainingJumps() > 0 && thing->player()->getJumpCount() != thing->player()->getRemainingJumps()) ||    // Already jumped once from ground and jumps remaining
            (thing->player()->getJumpCount() == -1) ) {                                                                                 // Unlimited jumping

            // Mark current jump button press as processed
            thing->player()->setJumpState( Jump_State::Jumped );

            // Calculate wall jump forces
            cpFloat jump_vx, jump_vy;
            if (thing->player()->getRemainingWallTime() > 0.0) {
                double angle = atan2(thing->player()->getLastTouchedGroundNormal().y, thing->player()->getLastTouchedGroundNormal().x) - atan2(g_gravity_normal.y, g_gravity_normal.x);
                angle = Dr::RadiansToDegrees( angle ) - 180;
                ///Dr::PrintDebug("Wall jump - Angle: " + std::to_string(angle) + ", Dot: " + std::to_string(object->getLastTouchedGroundDot()));
                if (angle < -180) angle += 360;
                if (angle >  180) angle -= 360;
                angle /= 3;

                DrPointF wall_jump_force = Dr::RotatePointAroundOrigin( DrPointF(thing->player()->getJumpForceX(), thing->player()->getJumpForceY()), DrPointF(0, 0), angle );
                jump_vx = wall_jump_force.x * 2.0;
                jump_vy = wall_jump_force.y * 2.0;

            // Calculate ground jump forces
            } else {
                jump_vx = thing->player()->getJumpForceX() * 2.0;   ///cpfsqrt(2.0 * object->getJumpForceX() * -gravity.x);
                jump_vy = thing->player()->getJumpForceY() * 2.0;   ///cpfsqrt(2.0 * object->getJumpForceY() * -gravity.y);

                // Cancel current lack of gravity to preform jump
                if (jump_from_sensor) thing->player()->setTempGravityMultiplier( 1.0 );
            }

            // Starting a new jump so partially cancel any previous jump forces
            cpVect  player_v = cpBodyGetVelocity( thing->physics()->body );
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

            ApplyJumpForce(thing, player_v, cpv(jump_vx, jump_vy), true);

            // Reset wall_timeout, jump timeout boost and subtract remaining jumps until ground
            thing->player()->setRemainingGroundTime( 0.0 );
            thing->player()->setRemainingWallTime( 0.0 );
            thing->player()->setRemainingBoost( static_cast<double>(thing->player()->getJumpTimeout()) / 1000.0 );
            thing->player()->setRemainingJumps( thing->player()->getRemainingJumps() - 1 );
            if (thing->player()->getRemainingJumps() < c_unlimited_jump) thing->player()->setRemainingJumps( c_unlimited_jump );
        }
    }

    // If jump button is let go, reset this objects ability to receive a jump command
    if (!g_jump_button) thing->player()->setJumpState( Jump_State::Need_To_Jump );



    // ********** Velocity - Target horizontal speed for air / ground control
    cpVect  velocity = cpBodyGetVelocity( thing->physics()->body );

    // Movement Speed, adjust to angle if desired
    double  button_speed_x = thing->player()->getMoveSpeedX() * key_x;
    double  button_speed_y = thing->player()->getMoveSpeedY() * key_y;
    double  forced_speed_x = thing->player()->getForcedSpeedX();
    double  forced_speed_y = thing->player()->getForcedSpeedY();
    double  rotate_speed =   thing->physics()->getRotateSpeedZ();
    if (thing->player()->getAngleMovement()) {
        DrPointF button_angle = Dr::RotatePointAroundOrigin( DrPointF(button_speed_x, button_speed_y), DrPointF(0, 0), thing->getAngle() );
            button_speed_x = button_angle.x;
            button_speed_y = button_angle.y;
        DrPointF forced_angle = Dr::RotatePointAroundOrigin( DrPointF(forced_speed_x, forced_speed_y), DrPointF(0, 0), thing->getAngle() );
            forced_speed_x = forced_angle.x;
            forced_speed_y = forced_angle.y;
    }
    bool has_key_x = (Dr::FuzzyCompare(button_speed_x, 0.0)) ? false : true;
    bool has_key_y = (Dr::FuzzyCompare(button_speed_y, 0.0)) ? false : true;
    double move_speed_x = forced_speed_x + button_speed_x;
    double move_speed_y = forced_speed_y + button_speed_y;

    // Store button direciton for use durin gother functions
    thing->player()->setButtonSpeedX(button_speed_x);
    thing->player()->setButtonSpeedY(button_speed_y);


    // ********** Drag / Acceleration
    double c_buffer =               0.001;
    double c_drag_ground =          0.005;
    double c_drag_air =             0.005;
    double c_drag_rotate =          0.100;
    double c_accel =                0.125;

    // Increase slowdown speed while in contact with a ladder (cancel gravity object)
    if (Dr::FuzzyCompare(thing->player()->getTempGravityMultiplier(), 1.0) == false) {
        c_drag_air    = (c_drag_air *    abs(thing->player()->getTempGravityMultiplier())) + 0.0001;
        c_drag_ground = (c_drag_ground * abs(thing->player()->getTempGravityMultiplier())) + 0.0001;
    }

    // Air / Ground / Rotation Drag
    double air_drag =       thing->player()->getAirDrag();
    double ground_drag =    thing->player()->getGroundDrag();
    double rotate_drag =    thing->player()->getRotateDrag();
    double acceleration =   thing->player()->getAcceleration();             // Acceleration, 1.0 is default, 5.0 is 5 times as slow, 0 is instant acceleration

    // Adjust Move Speeds for Drag
    double air_increase =    (air_drag    > 1.0) ? sqrt(air_drag) :    1.0;
    double ground_increase = (ground_drag > 1.0) ? sqrt(ground_drag) : 1.0;
    double rotate_increase = (rotate_drag > 1.0) ? sqrt(rotate_drag) : 1.0;
    if (!thing->player()->isOnGround() && !thing->player()->isOnWall()) {
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
        case Pedal::Brake:              target_spin = 0.0;                                  break;
        case Pedal::Clockwise:          target_spin = -thing->physics()->getRotateSpeedZ(); break;
        case Pedal::CounterClockwise:   target_spin =  thing->physics()->getRotateSpeedZ(); break;
        case Pedal::None:               break;
    }

    // This code subtracts gravity from target speed, not sure if we want to leave this in
    //      (useful for allowing movement force against gravity for m_cancel_gravity property, i.e. climbing up ladders)
    double actual_gravity_x = gravity.x * thing->player()->getTempGravityMultiplier();
    double actual_gravity_y = gravity.y * thing->player()->getTempGravityMultiplier();
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
    if (!thing->player()->isOnGround() && !thing->player()->isOnWall()) {
        if (target_x_is_zero || (has_key_x == false && speed_x_greater_than_forced)) {
            velocity.x = cpflerpconst(velocity.x, thing->player()->getForcedSpeedX(), air_drag / c_drag_air * dt);
        } else {
            velocity.x = cpflerpconst(velocity.x, target_vx, air_accel_x * dt);
        }

        if (target_y_is_zero || (has_key_y == false && speed_y_greater_than_forced)) {
            velocity.y = cpflerpconst(velocity.y, thing->player()->getForcedSpeedY(), air_drag / c_drag_air * dt);
        } else {
            velocity.y = cpflerpconst(velocity.y, target_vy, air_accel_y * dt);
        }

    // Interpolate towards desired velocity if on ground / wall
    } else {
        if (target_x_is_zero || (has_key_x == false && speed_x_greater_than_forced)) {
            velocity.x = cpflerpconst(velocity.x, thing->player()->getForcedSpeedX(), ground_drag / c_drag_ground * dt);
        } else {
            velocity.x = cpflerpconst(velocity.x, target_vx, ground_accel_x * dt);
        }

        if (target_y_is_zero || (has_key_y == false && speed_y_greater_than_forced)) {
            velocity.y = cpflerpconst(velocity.y, thing->player()->getForcedSpeedY(), ground_drag / c_drag_ground * dt);
        } else {
            velocity.y = cpflerpconst(velocity.y, target_vy, ground_accel_y * dt);
        }
    }



    // ********** Max Speed - Limit Velocity
    velocity.x = cpfclamp(velocity.x, -thing->player()->getMaxSpeedX(), thing->player()->getMaxSpeedX());
    velocity.y = cpfclamp(velocity.y, -thing->player()->getMaxSpeedY(), thing->player()->getMaxSpeedY());



    // ********** Update Velocity - #NOTE: MUST CALL actual Update Velocity function some time during this callback!
    cpBodySetVelocity( thing->physics()->body, velocity );

    // Apply movement force evenly to Mesh Cloth players
    if (thing->physics()->body_style == Body_Style::Mesh_Blob && (has_key_x || has_key_y)) {
        for (auto child : thing->compSoftBody()->soft_balls) {
            if (child == nullptr) return;
            cpVect soft_ball_velocity = cpBodyGetVelocity(child->physics()->body);
            double x_vel = cpflerp(soft_ball_velocity.x, velocity.x, 0.25);
            double y_vel = cpflerp(soft_ball_velocity.y, velocity.y, 0.25);
            cpBodySetVelocity( child->physics()->body, cpv(x_vel, y_vel));
        }
    }

    cpBodyUpdateVelocity(body, cpv(actual_gravity_x, actual_gravity_y), damping, dt);
}


























