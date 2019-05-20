//
//      Created by Stephens Nunnally on 5/6/2019, (c) 2019 Scidian Software, All Rights Reserved
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
#include "helper.h"
#include "project/project.h"
#include "project/project_world.h"
#include "project/project_world_stage.h"


// How fast we start to accelerate on the ground, smaller = faster
#define PLAYER_GROUND_ACCEL_TIME 0.1
#define PLAYER_GROUND_ACCEL (PLAYER_VELOCITY / PLAYER_GROUND_ACCEL_TIME)

// How fast we start to accelerate in the air, smaller = faster
#define PLAYER_AIR_ACCEL_TIME 0.35
#define PLAYER_AIR_ACCEL (PLAYER_VELOCITY / PLAYER_AIR_ACCEL_TIME)

#define PLAYER_VELOCITY 500.0                   // Movement speed
#define JUMP_HEIGHT      90.0                   // Jump force (for now only y)
#define JUMP_TIME_OUT   900.0                   // Milliseconds to allow for jump to continue to receive a boost when jump button is held down
#define FALL_VELOCITY  1000.0                   // Max fall speed


//######################################################################################################
//##    Update Space steps the physics calculations
//##        updateSpaceHelper meant to be called immediately afterwards
//######################################################################################################
void DrEngine::updateSpace(double time_passed) {

    // ***** Updates physics, time_passed is in milliseconds
    cpFloat step_time = time_passed / 1000.0 * m_time_warp;
    cpSpaceStep(m_space, step_time);

}

void DrEngine::updateSpaceHelper(double time_passed) {
    // Calculate step time
    cpFloat step_time = time_passed / 1000.0 * m_time_warp;

    // Get some Space info
    cpVect  gravity = cpSpaceGetGravity( m_space );

    // ********** Iterate through objects, delete if they go off screen
    for ( auto it = objects.begin(); it != objects.end(); ) {
        SceneObject *object = *it;
        object->has_been_processed = true;

        // ***** Skip object if static or no longer in Space
        if ((object->body_type == Body_Type::Static) || (object->in_scene == false)) {
            it++;
            continue;
        }

        // ***** Save objects last position, used by other objects
        object->last_position_x = object->position.x();
        object->last_position_y = object->position.y();

        // ***** Get some info about the current object from the space and save it to the current SceneObject
        cpVect  vel = cpBodyGetVelocity( object->body );
            object->velocity.setX( vel.x );
            object->velocity.setX( vel.y );
        cpFloat angle = cpBodyGetAngle( object->body );
            object->angle = qRadiansToDegrees( angle );
        cpVect  pos = cpBodyGetPosition( object->body );
            object->position.setX( pos.x );
            object->position.setY( pos.y );

        // ***** Update global friction and bounce to all objects if globals have changed (possibly due to Gameplay Action)
        cpFloat friction;
        cpFloat bounce;
        for (auto shape : object->shapes) {
            friction = cpShapeGetFriction( shape );
            bounce = cpShapeGetElasticity( shape );
            if (qFuzzyCompare(friction, m_friction) == false && object->custom_friction == false)
                cpShapeSetFriction( shape, friction );
            if (qFuzzyCompare(bounce, m_bounce) == false && object->custom_bounce == false)
                cpShapeSetElasticity( shape, bounce );
        }


        // ***** Process Button Presses
        // If is a wheel, apply gas pedal
        if (object->is_wheel) {
            switch (gas_pedal) {
                case Pedal::Clockwise:          cpBodySetAngularVelocity( object->body, -object->wheel_speed );     break;
                case Pedal::CounterClockwise:   cpBodySetAngularVelocity( object->body,  object->wheel_speed );     break;
                case Pedal::Brake:              cpBodySetAngularVelocity( object->body,  0 );                       break;
                case Pedal::None:               break;
            }
        }




        // ***** Update player velocity
        if (object->player_controls || object->lost_control) {
            playerUpdateVelocity( object, gravity, step_time );

// Trying out some custom velocity updates
//            static double target_vx;
//            static int    last_key_x;
//            if (keyboard_x != 0 and keyboard_x != last_key_x) {
//                target_vx = 500 * keyboard_x;
//                last_key_x = keyboard_x;
//            }
//            cpVect p_vel = cpBodyGetVelocity( object->body );
//            p_vel.x = cpflerpconst( p_vel.x, target_vx, (500 / .35) * step_time);
//            cpBodySetVelocity( object->body, p_vel );

            // ***** If the jump key was just pressed this frame and it wasn't pressed last frame, and we're on the ground, jump!
            if (object->remaining_boost > 0) object->remaining_boost -= time_passed;

            if (    (jump_button == true) && (object->jump_state == Jump_State::Need_To_Jump) && (object->lost_control == false) &&
                    (object->grounded || (object->jump_count == -1) || (object->remaining_jumps > 0))   ) {
                object->jump_state = Jump_State::Jumped;
                cpFloat jump_v = cpfsqrt(2.0 * JUMP_HEIGHT * -gravity.y);
                cpVect  player_v = cpBodyGetVelocity( object->body );
                player_v.y = 0;
                player_v = cpvadd(player_v, cpv(0.0, jump_v));
                cpBodySetVelocity( object->body, player_v );
                object->remaining_boost = JUMP_TIME_OUT;
                object->remaining_jumps--;
            }
            if (!jump_button) object->jump_state = Jump_State::Need_To_Jump;
        }


        // ***** Delete object if ends up outside the deletion threshold
        if ( (pos.y < static_cast<double>(getCameraPos().y()) - m_delete_threshold_y) ||
             (pos.y > static_cast<double>(getCameraPos().y()) + m_delete_threshold_y) ||
             (pos.x < static_cast<double>(getCameraPos().x()) - m_delete_threshold_x) ||
             (pos.x > static_cast<double>(getCameraPos().x()) + m_delete_threshold_x) ) {

            removeObject(object);

            delete object;
            it = objects.erase(it);
        } else {
            it++;
        }
    }   // End For






    // ***** Calculate distance and load new stage if we need to
    bool should_add_stage = false;
    if (demo_space == Demo_Space::Project && has_scene == true) {
        QTransform t = QTransform().translate(m_game_start.x(), m_game_start.y())
                                   .rotate(m_game_direction)
                                   .translate(-m_game_start.x(), -m_game_start.y());
        QPointF rotated = t.map( QPointF( static_cast<double>(getCameraPos().x()), static_cast<double>(getCameraPos().y()) ));
        m_game_distance = rotated.x() - m_game_start.x();
        ///info = "Distance: " + QString::number(m_game_distance) + ", Loaded To: " + QString::number(m_loaded_to);

        if (m_loaded_to - m_game_distance < m_load_buffer)
            should_add_stage = true;
    }


    // ********** Adds a Stage if necessary
    if (should_add_stage) {
        DrWorld *world = m_project->getWorld(m_current_world);

        // Pick a random stage other than start stage
        QVector<DrStage*> stages;
        for (auto stage_pair : world->getStageMap()) {
            if (stage_pair.second->getName() != "Start Stage") {
                stages.append(stage_pair.second);
            }
        }
        int stage_count = static_cast<int>(stages.size());
        int stage_num = QRandomGenerator::global()->bounded(0, stage_count);
        DrStage *stage = stages[stage_num];

        loadStageToSpace( stage, m_loaded_to, 0);
    }
}


//######################################################################################################
//##    Updates Jump Player Velocity
//######################################################################################################
static void selectPlayerGroundNormal(cpBody *, cpArbiter *arb, cpVect *ground_normal) {
    cpVect n = cpvneg( cpArbiterGetNormal(arb) );
    if (n.y > ground_normal->y)
        (*ground_normal) = n;
}

void DrEngine::playerUpdateVelocity(SceneObject *object, cpVect gravity, cpFloat dt)
{
    // If player is still active get keyboard status
    int key_y, key_x, key_jump;
    if (object->lost_control) {
        key_x =     0;
        key_y =     0;
        key_jump =  0;
    } else {
        key_x =     keyboard_x;
        key_y =     keyboard_y;
        key_jump =  jump_button;
    }

    // Check if jump key is active
    bool jump_button_down = key_jump;

    // Grab the grounding normal from last frame, if we hit the ground, turn of remaining_boost time
    cpVect ground_normal = cpvzero;
    cpBodyEachArbiter(object->body, cpBodyArbiterIteratorFunc(selectPlayerGroundNormal), &ground_normal);
    object->grounded = (ground_normal.y > 0.0);
    if (object->grounded) {
        object->remaining_jumps = object->jump_count;
        object->remaining_boost = 0.0;
    }
    if (ground_normal.y < 0.0) {
        object->remaining_boost = 0.0;
    }

    // Continues to provide jump velocity, although slowly fades
    cpBool boost = (jump_button_down && (object->remaining_boost > 0.0));

    // Alternative boost extension:
    ///cpVect g = (boost ? cpvzero : gravity);
    ///cpBodyUpdateVelocity(body, g, damping, dt);

    // Custom boost extension that provides a little more boost
    if (boost) {
        cpVect  player_v = cpBodyGetVelocity( object->body );
        cpFloat jump_v = cpfsqrt(2.0 * JUMP_HEIGHT * -gravity.y) * dt;
        player_v = cpvadd(player_v, cpv(0.0, jump_v));
        cpBodySetVelocity( object->body, player_v );
    }

    // Target horizontal speed for air / ground control
    cpFloat target_vx = PLAYER_VELOCITY * key_x;

    // Update the surface velocity and friction
    cpVect surface_v = cpv(-target_vx, 0.0);
    for (auto shape : object->shapes) {
        cpShapeSetSurfaceVelocity( shape, surface_v );
        cpShapeSetFriction( shape, (object->grounded ? PLAYER_GROUND_ACCEL / -gravity.y : 0.0));
    }

    // Apply air control if not grounded, smoothly accelerate the velocity
    if (object->grounded == false) {
        cpVect air_v = cpBodyGetVelocity( object->body );
        air_v.x = cpflerpconst( air_v.x, target_vx, PLAYER_AIR_ACCEL * dt);
        cpBodySetVelocity( object->body, air_v );
    }

    // Set a max fall speed of "FALL_VELOCITY"
    cpVect body_v = cpBodyGetVelocity( object->body );
    body_v.y = cpfclamp(body_v.y, -FALL_VELOCITY, static_cast<double>(INFINITY));
    cpBodySetVelocity( object->body, body_v );
}











