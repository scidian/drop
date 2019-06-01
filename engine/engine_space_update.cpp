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

#include "engine.h"
#include "engine_camera.h"
#include "engine_object.h"
#include "forms/form_engine.h"
#include "helper.h"
#include "project/project.h"
#include "project/project_world.h"
#include "project/project_world_stage.h"


//######################################################################################################
//##    Update Space steps the physics calculations
//##        updateSpaceHelper meant to be called immediately afterwards
//######################################################################################################

// NOTE: time_passed is in milliseconds
void DrEngine::updateSpace(double time_passed) {
    double step_time = time_passed / 1000.0 * m_time_warp;
    cpSpaceStep(m_space, step_time);
}

void DrEngine::updateSpaceHelper() {

    // ***** Update global variables for use in callbacks
    g_keyboard_x =  keyboard_x;
    g_keyboard_y =  keyboard_y;
    g_jump_button = jump_button;

    switch (gas_pedal) {
        case Pedal::Clockwise:          g_rotate_cw = true;     g_rotate_cw = false;        break;
        case Pedal::CounterClockwise:   g_rotate_cw = false;    g_rotate_cw = true;         break;
        default:                        g_rotate_cw = false;    g_rotate_cw = false;        break;
    }

    g_gravity_normal = cpvnormalize( cpSpaceGetGravity(m_space) );


    // ********** Iterate through objects, delete if they go off screen
    for ( auto it = objects.begin(); it != objects.end(); ) {

        // ***** Initial loop variables
        DrEngineObject *object = *it;
        bool remove = false;

        // ***** Get time since last update
        if (!object->has_been_processed) {
            object->has_been_processed = true;
            object->time_since_last_update = 0.0;
        } else {
            object->time_since_last_update = Dr::MillisecondsElapsed(object->update_timer);
        }
        Dr::ResetTimer(object->update_timer);

        // ***** Skip object if static; or if not yet in Space / no longer in Space
        if (object->should_process == false) {
            it++;
            continue;
        }

        // ***** Get some info about the current object from the space and save it to the current DrEngineObject
        object->angle = qRadiansToDegrees( cpBodyGetAngle( object->body ) );
        object->previous_position = object->position;
        cpVect  new_position = cpBodyGetPosition( object->body );
        object->position.setX( new_position.x );
        object->position.setY( new_position.y );

        // **** Check that any object with custom PlayerUpdateVelocity callback is awake so it can access key / button events
        bool sleeping = cpBodyIsSleeping(object->body);
        if (object->key_controls && !object->lost_control && sleeping) {
            cpBodyActivate(object->body);
        }

//        // ***** Update global friction and bounce to all objects if globals have changed (possibly due to Gameplay Action)
//        if (qFuzzyCompare(object->custom_friction, c_friction) == false) {
//            for (auto shape : object->shapes) {
//                cpFloat friction = cpShapeGetFriction( shape );
//                if (qFuzzyCompare(friction, m_friction) == false) cpShapeSetFriction( shape, m_friction );
//            }
//        }
//        if (qFuzzyCompare(object->custom_bounce, c_bounce) == false) {
//            for (auto shape : object->shapes) {
//                cpFloat bounce = cpShapeGetElasticity( shape );
//                if (qFuzzyCompare(bounce, m_bounce) == false) cpShapeSetElasticity( shape, m_bounce );
//            }
//        }


        // ***** Process non-static object movement
        if ((object->body_type != Body_Type::Static)) {

            // ***** Process Button Presses
            // If is a wheel, apply gas pedal
            if (qFuzzyCompare(object->rotate_speed, 0) == false) {
                switch (gas_pedal) {
                    case Pedal::Clockwise:          cpBodySetAngularVelocity( object->body, -object->rotate_speed );    break;
                    case Pedal::CounterClockwise:   cpBodySetAngularVelocity( object->body,  object->rotate_speed );    break;
                    case Pedal::Brake:              cpBodySetAngularVelocity( object->body,  0 );                       break;
                    case Pedal::None:               break;
                }
            }


        }


        // ***** Auto Damage
        if (object->health >= c_epsilon) {
            if (object->auto_damage < c_epsilon || object->auto_damage > c_epsilon) {
                object->health -= object->auto_damage * (object->time_since_last_update / 1000.0);
                if (object->health > object->max_health && object->max_health >= c_epsilon) object->health = object->max_health;
                if (object->health <= c_epsilon) object->health = 0.0;
            }
        }


        // ***** Check for Object Death / Fade / Removal
        if (object->health < c_epsilon) {
            if (!object->dying) {
                object->dying = true;
                Dr::ResetTimer(object->death_timer);
            }
            if (object->dying && object->alive) {
                if (Dr::MillisecondsElapsed(object->death_timer) >= object->death_delay) {
                    object->alive = false;
                    Dr::ResetTimer(object->fade_timer);
                }
            }
            if (!object->alive) {
                if (Dr::MillisecondsElapsed(object->fade_timer) >= object->fade_delay) {
                    remove = true;
                }
            }
        }

        // Delete object if ends up outside the deletion threshold
        QRectF threshold(getCameraPosXD() - m_delete_threshold_x, getCameraPosYD() - m_delete_threshold_y, m_delete_threshold_x*2.0, m_delete_threshold_y*2.0);
        if (!threshold.contains(QPointF(new_position.x, new_position.y))) remove = true;

        // Process removal
        if (remove) {
            removeObject(object);
            delete object;
            it = objects.erase(it);
        } else it++;
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

    // ********** Calculates Physics Updates Frames per Second
    static DrTime fps_time = Clock::now();
    static int fps_count = 0;
    ++fps_count;
    double fps_milli = Dr::MillisecondsElapsed(fps_time);
    if (fps_milli > 1000.0) {
        fps_physics = fps_count;
        fps_count = 0;
        Dr::ResetTimer(fps_time);
    }
}











