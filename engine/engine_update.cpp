//
//      Created by Stephens Nunnally on 5/6/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QtMath>
#include <QRandomGenerator>
#include "engine/engine.h"
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
//##    Update Space
//######################################################################################################
void DrEngine::updateSpace(double time_passed) {

    bool should_add_stage = false;

    // ***** Updates physics, time_passed is in milliseconds
    cpFloat step_time = time_passed / 1000 * m_time_warp;
    cpSpaceStep(m_space, step_time); ///m_time_step);

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

        // ***** Get some info about the current object from the space and save it to the current SceneObject
        cpVect  vel = cpBodyGetVelocity( object->body );
            object->velocity.setX( vel.x );
            object->velocity.setX( vel.y );
        cpVect  pos = cpBodyGetPosition( object->body );
            object->position.setX( pos.x );
            object->position.setY( pos.y );
        cpFloat angle = cpBodyGetAngle( object->body );
            object->angle = qRadiansToDegrees( angle );


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


        // ***** Update camera position and load more stages if necessary
        if (object->follow == true) {
            // !!!!! Need to implement a smooth following camera
            setCameraPos( static_cast<float>(pos.x), static_cast<float>(pos.y), getCameraPos().z() );

            // Calculate distance and load new stage if we need to
            if (demo_space == Demo_Space::Project && has_scene == true) {
                QTransform t = QTransform().translate(m_game_start.x(), m_game_start.y())
                                           .rotate(m_game_direction)
                                           .translate(-m_game_start.x(), -m_game_start.y());
                QPointF rotated = t.map( QPointF( static_cast<double>(m_camera_pos.x()), static_cast<double>(m_camera_pos.y()) ));
                m_game_distance = rotated.x() - m_game_start.x();
                info = "Distance: " + QString::number(m_game_distance) + ", Loaded To: " + QString::number(m_loaded_to);

                if (m_loaded_to - m_game_distance < m_load_buffer)
                    should_add_stage = true;
            }

        }



        // ***** Update player velocity
        if (object->player_controls) {
            playerUpdateVelocity( player_body, gravity, step_time );

            // ***** If the jump key was just pressed this frame and it wasn't pressed last frame, and we're on the ground, jump!
            if (remaining_boost > 0) remaining_boost -= time_passed;

            if ((jump_state == Jump_State::Need_To_Jump) && (grounded || (jump_count == -1) || (remaining_jumps > 0))) {
                cpFloat jump_v = cpfsqrt(2.0 * JUMP_HEIGHT * -gravity.y);
                cpVect  player_v = cpBodyGetVelocity( player_body );
                player_v.y = 0;
                player_v = cpvadd(player_v, cpv(0.0, jump_v));
                cpBodySetVelocity( player_body, player_v );
                remaining_boost = JUMP_TIME_OUT;
                --remaining_jumps;
            }
            jump_state = Jump_State::Jumped;
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

void DrEngine::playerUpdateVelocity(cpBody *body, cpVect gravity, cpFloat dt)
{
    // Check if jump key is active
    bool jump_button_down = keyboard_y > 0.0;

    // Grab the grounding normal from last frame, if we hit the ground, turn of remaining_boost time
    cpVect ground_normal = cpvzero;
    cpBodyEachArbiter(player_body, cpBodyArbiterIteratorFunc(selectPlayerGroundNormal), &ground_normal);
    grounded = (ground_normal.y > 0.0);
    if (grounded) {
        remaining_jumps = jump_count;
        remaining_boost = 0.0;
    }
    if (ground_normal.y < 0.0) {
        remaining_boost = 0.0;
    }

    // Continues to provide jump velocity, although slowly fades
    cpBool boost = (jump_button_down && (remaining_boost > 0.0));

    // Alternative boost extension:
    ///cpVect g = (boost ? cpvzero : gravity);
    ///cpBodyUpdateVelocity(body, g, damping, dt);

    // Custom boost extension that provides a little more boost
    if (boost) {
        cpVect  player_v = cpBodyGetVelocity( player_body );
        cpFloat jump_v = cpfsqrt(2.0 * JUMP_HEIGHT * -gravity.y) * dt;
        player_v = cpvadd(player_v, cpv(0.0, jump_v));
        cpBodySetVelocity( player_body, player_v );
    }

    // Target horizontal speed for air / ground control
    cpFloat target_vx = PLAYER_VELOCITY * keyboard_x;

    // Update the surface velocity and friction
    cpVect surface_v = cpv(-target_vx, 0.0);
    cpShapeSetSurfaceVelocity( player_shape, surface_v );
    cpShapeSetFriction( player_shape, (grounded ? PLAYER_GROUND_ACCEL / -gravity.y : 0.0));

    // Apply air control if not grounded, smoothly accelerate the velocity
    if (!grounded) {
        cpVect air_v = cpBodyGetVelocity( player_body );
        air_v.x = cpflerpconst( air_v.x, target_vx, PLAYER_AIR_ACCEL * dt);
        cpBodySetVelocity( player_body, air_v );
    }

    // Set a max fall speed of "FALL_VELOCITY"
    cpVect body_v = cpBodyGetVelocity( body );
    body_v.y = cpfclamp(body_v.y, -FALL_VELOCITY, static_cast<double>(INFINITY));
    cpBodySetVelocity( body, body_v );
}











