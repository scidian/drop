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
//##    Chipmunk Callbacks
//##        Support for changing shape Friction and Bounce (Elasticity) during gameplay
//######################################################################################################
// Used for shape iterator to get a list of all shapes attached to a body
static void getShapeList(cpBody *, cpShape *shape, QVector<cpShape*> *shape_list) { shape_list->append(shape); }

void DrEngine::setObjectFriction(SceneObject *object, const cpFloat &friction) {
    QVector<cpShape*> shape_list;
    cpBodyEachShape(object->body, cpBodyShapeIteratorFunc(getShapeList), &shape_list);
    for (auto shape : shape_list)
        cpShapeSetFriction( shape, friction );
}
void DrEngine::setObjectBounce(SceneObject *object, const cpFloat &bounce) {
    QVector<cpShape*> shape_list;
    cpBodyEachShape(object->body, cpBodyShapeIteratorFunc(getShapeList), &shape_list);
    for (auto shape : shape_list)
        cpShapeSetElasticity( shape, bounce );
}

//######################################################################################################
//##    Update Space
//######################################################################################################
void DrEngine::updateSpace(double time_passed) {

    bool should_add_stage = false;

    // ***** Updates physics, time_passed is in milliseconds
    cpFloat step_time = time_passed / 1000.0 * m_time_warp;
    cpSpaceStep(m_space, step_time);

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
        cpFloat angle = cpBodyGetAngle( object->body );
            object->angle = qRadiansToDegrees( angle );
        cpVect  pos = cpBodyGetPosition( object->body );
            object->position.setX( pos.x );
            object->position.setY( pos.y );

        // ***** Update global friction and bounce to all objects if globals have changed (possibly due to Gameplay Action)
        cpFloat friction;
        cpFloat bounce;
        if ( object->shape ) {
            friction = cpShapeGetFriction( object->shape );
            bounce = cpShapeGetElasticity( object->shape );
            if (qFuzzyCompare(friction, m_friction) == false && object->custom_friction == false)
                setObjectFriction( object, m_friction );
            if (qFuzzyCompare(bounce, m_bounce) == false && object->custom_bounce == false)
                setObjectBounce( object, m_bounce );
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


        // ********** Update camera position and load more stages if necessary
        if (object->follow == true) {

            // ***** Camera updates
            ///info = ", " + QString::number( pos.x - getCameraPos().x() );
            setCameraPos( static_cast<float>(pos.x), static_cast<float>(pos.y), getCameraPos().z() );

//            avg_speed_x.push_back( pos.x - object->last_position_x );
//            avg_speed_y.push_back( pos.y - object->last_position_y );
//            if (avg_speed_x.size() > 60) avg_speed_x.pop_front();
//            if (avg_speed_y.size() > 60) avg_speed_y.pop_front();
//            object->last_position_x = pos.x;
//            object->last_position_y = pos.y;

//            double average_x = std::accumulate( avg_speed_x.begin(), avg_speed_x.end(), 0.0) / avg_speed_x.size();
//            double average_y = std::accumulate( avg_speed_y.begin(), avg_speed_y.end(), 0.0) / avg_speed_y.size();

//            m_camera_speed = QVector3D( static_cast<float>(average_x), static_cast<float>(average_y), 0);

//            info = ", " + QString::number( static_cast<double>(m_camera_speed.x()) );



            // ***** Calculate distance and load new stage if we need to
            if (demo_space == Demo_Space::Project && has_scene == true) {
                QTransform t = QTransform().translate(m_game_start.x(), m_game_start.y())
                                           .rotate(m_game_direction)
                                           .translate(-m_game_start.x(), -m_game_start.y());
                QPointF rotated = t.map( QPointF( static_cast<double>(m_camera_pos.x()), static_cast<double>(m_camera_pos.y()) ));
                m_game_distance = rotated.x() - m_game_start.x();
//                info = "Distance: " + QString::number(m_game_distance) + ", Loaded To: " + QString::number(m_loaded_to);

                if (m_loaded_to - m_game_distance < m_load_buffer)
                    should_add_stage = true;
            }

        }


        // ***** Update player velocity
        if (object->player_controls) {
            playerUpdateVelocity( player_body, gravity, step_time );

// Trying out some custom velocity updates
//            static double target_vx;
//            static int    last_key_x;
//            if (keyboard_x != 0 and keyboard_x != last_key_x) {
//                target_vx = 500 * keyboard_x;
//                last_key_x = keyboard_x;
//            }
//            cpVect p_vel = cpBodyGetVelocity( player_body );
//            p_vel.x = cpflerpconst( p_vel.x, target_vx, (500 / .35) * step_time);
//            cpBodySetVelocity( player_body, p_vel );

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











