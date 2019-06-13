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
#include "engine_world.h"
#include "forms/form_engine.h"
#include "helper.h"
#include "project/project.h"
#include "project/project_world.h"
#include "project/project_world_stage.h"


//######################################################################################################
//##    Update Space steps the physics calculations
//##        updateSpaceHelper meant to be called immediately afterwards
//######################################################################################################

// #NOTE: time_passed is in milliseconds
void DrEngineWorld::updateSpace(double time_passed) {
    double step_time = time_passed / 1000.0 * m_time_warp;
    ///double step_time = m_time_step * m_time_warp;
    cpSpaceStep(m_space, step_time);
}

void DrEngineWorld::updateSpaceHelper() {

    // ***** Update global variables for use in callbacks
    g_keyboard_x =  m_engine->keyboard_x;
    g_keyboard_y =  m_engine->keyboard_y;
    g_jump_button = m_engine->jump_button;

    switch (m_engine->gas_pedal) {
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
        if (!object->hasBeenProcessed()) {
            object->setHasBeenProcessed(true);
            object->setTimeSinceLastUpdate( 0.0 );
        } else {
            object->setTimeSinceLastUpdate( Dr::MillisecondsElapsed(object->getUpdateTimer()) );
        }
        Dr::ResetTimer(object->getUpdateTimer());

        // ***** Skip object if static; or if not yet in Space / no longer in Space
        if (!object->shouldProcess()) {
            it++;
            continue;
        }

        // ***** Get some info about the current object from the space and save it to the current DrEngineObject
        cpVect  new_position = cpBodyGetPosition( object->body );
        object->updateBodyPosition( QPointF( new_position.x, new_position.y ));
        object->updateBodyAngle( qRadiansToDegrees( cpBodyGetAngle( object->body )) );

        // **** Check that any object with custom PlayerUpdateVelocity callback is awake so it can access key / button events
        bool sleeping = cpBodyIsSleeping(object->body);
        if (object->hasKeyControls() && !object->hasLostControl() && sleeping) {
            cpBodyActivate(object->body);
        }

//        // ***** Update global friction and bounce to all objects if globals have changed (possibly due to Gameplay Action)
//        if (qFuzzyCompare(object->getCustomFriction(), c_friction) == false) {
//            for (auto shape : object->shapes) {
//                cpFloat friction = cpShapeGetFriction( shape );
//                if (qFuzzyCompare(friction, m_friction) == false) cpShapeSetFriction( shape, m_friction );
//            }
//        }
//        if (qFuzzyCompare(object->getCustomBounce(), c_bounce) == false) {
//            for (auto shape : object->shapes) {
//                cpFloat bounce = cpShapeGetElasticity( shape );
//                if (qFuzzyCompare(bounce, m_bounce) == false) cpShapeSetElasticity( shape, m_bounce );
//            }
//        }


        // ***** Process non-static object movement
        if ((object->body_type != Body_Type::Static)) {

            // ***** Process Button Presses
            // If is a wheel, apply gas pedal
            if (qFuzzyCompare(object->getRotateSpeed(), 0) == false) {
                switch (m_engine->gas_pedal) {
                    case Pedal::Clockwise:          cpBodySetAngularVelocity( object->body, -object->getRotateSpeed() );    break;
                    case Pedal::CounterClockwise:   cpBodySetAngularVelocity( object->body,  object->getRotateSpeed() );    break;
                    case Pedal::Brake:              cpBodySetAngularVelocity( object->body,  0 );                           break;
                    case Pedal::None:               break;
                }
            }


        }


        // ***** Auto Damage
        if (object->getHealth() > c_epsilon) {
            if (object->getAutoDamage() < -c_epsilon || object->getAutoDamage() > c_epsilon) {
                object->takeDamage( object->getAutoDamage() * (object->getTimeSinceLastUpdate() / 1000.0), false );
            }
        }


        // ***** Check for Object Death / Fade / Removal
        if (object->getHealth() <= c_epsilon && object->getHealth() > c_unlimited_health) {
            if (!object->isDying()) {
                object->setDying( true );
                Dr::ResetTimer(object->getDeathTimer());
            }
            if (object->isDying() && object->isAlive()) {
                if (Dr::MillisecondsElapsed(object->getDeathTimer()) >= object->getDeathDelay()) {
                    object->setAlive( false );
                    Dr::ResetTimer(object->getFadeTimer());
                }
            }
            if (!object->isAlive()) {
                if (Dr::MillisecondsElapsed(object->getFadeTimer()) >= object->getFadeDelay()) {
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
    if (m_engine->demo_space == Demo_Space::Project && has_scene == true) {
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
        DrWorld *world = m_project->getWorld(m_world);

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


    // ********** Updates Physics Frames per Second
    ++m_engine->getFormEngine()->fps_count_physics;
}











