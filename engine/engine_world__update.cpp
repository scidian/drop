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
#include "engine_things/engine_thing_light.h"
#include "engine_things/engine_thing_object.h"
#include "engine_world.h"
#include "form_engine.h"
#include "helper.h"
#include "helper_qt.h"
#include "opengl/opengl.h"
#include "project/project.h"
#include "project/project_world.h"
#include "project/project_stage.h"


//####################################################################################
//##    Update Space steps the physics calculations
//##        updateWorld meant to be called immediately afterwards
//####################################################################################

// #NOTE: time_passed is in milliseconds
void DrEngineWorld::updateSpace(double time_passed) {
    double step_time = time_passed / 1000.0 * m_time_warp;
    ///double step_time = m_time_step * m_time_warp;
    cpSpaceStep(m_space, step_time);
}

void DrEngineWorld::updateWorld(double time_passed) {

    // Calculate area that if Things are within, they can stay in the Space
    QRectF threshold(getCameraPositionX() - getDeleteThresholdX(),
                     getCameraPositionY() - getDeleteThresholdY(),
                     getDeleteThresholdX()*2.0,
                     getDeleteThresholdY()*2.0);

    // ***** Update global variables for use in callbacks
    g_gravity_normal = cpvnormalize( cpSpaceGetGravity(m_space) );

    // ********** Iterate through Things, delete if they go off screen
    for ( auto it = m_things.begin(); it != m_things.end(); ) {
        DrEngineThing *thing = *it;

        // ***** Update Thing
        thing->calculateTimeSinceLastUpdate();
        bool remove = thing->update(time_passed, m_time_warp, threshold);

        // ***** Process Removal
        if (remove) {
            delete thing;
            it = m_things.erase(it);
            continue;
        }

        // ***** Increment for loop
        it++;
    }   // End For


    // ***** Calculate distance and load new stage if we need to
    bool should_add_stage = false;
    if (has_scene == true) {
        QTransform t = QTransform().translate( m_game_start.x,  m_game_start.y)
                                   .rotate(    m_game_direction)
                                   .translate(-m_game_start.x, -m_game_start.y);
        QPointF rotated = t.map( QPointF( getCameraPositionX(), getCameraPositionY() ));
        m_game_distance = rotated.x() - m_game_start.x;

        ///g_info = "Distance: \t" + QString::number(int(m_game_distance)) +
        ///       ", Loaded To: " + QString::number(m_loaded_to);

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

        // If there is at least one other stage available, load it up
        if (stages.count() >= 1) {
            int stage_count = static_cast<int>(stages.size());
            int stage_num = QRandomGenerator::global()->bounded(0, stage_count);
            DrStage *stage = stages[stage_num];

            QTransform t = QTransform().translate( m_game_start.x,  m_game_start.y)
                                       .rotate(   -m_game_direction)
                                       .translate(-m_game_start.x, -m_game_start.y);
            QPointF rotated = t.map( QPointF( m_loaded_to, 0 ));
            loadStageToWorld(stage, rotated.x(), rotated.y());
        }
    }


    // !!!!!!!!!! #TEMP: Light update for Demo
    if (demo_player == Demo_Player::Light) {
        light1->setPosition( ball->getPosition() );
        light2->setPosition( ball->getPosition() );
    }
}



//####################################################################################
//##    Updates Physics Object Velocity
//####################################################################################
extern void ObjectUpdateVelocity(cpBody *body, cpVect gravity, cpFloat damping, cpFloat dt) {
    // ***** Grab object from User Data
    DrEngineObject *object = static_cast<DrEngineObject*>(cpBodyGetUserData(body));
    if (object == nullptr) return;

    // ***** Update Velocity - #NOTE: MUST CALL actual Update Velocity function some time during this callback!
    if (object->ignoreGravity()) {
        cpBodyUpdateVelocityNoGravity(body, gravity, damping, dt);
    } else {
        cpBodyUpdateVelocity(body, gravity, damping, dt);
    }
}

//####################################################################################
//##    Updates Kinematic Angular Velocity
//####################################################################################
extern void KinematicUpdateVelocity(cpBody *body, cpVect gravity, cpFloat damping, cpFloat dt) {
    // Grab object from User Data
    DrEngineObject *object = static_cast<DrEngineObject*>(cpBodyGetUserData(body));
    if (object == nullptr) return;

    // Try and turn object to rotate toward player
    if (object->getRotateToPlayer()) {
        // Angle to Player
        cpVect my_pos =     cpBodyGetPosition(body);
        double my_angle =   cpBodyGetAngle(body);
        double angle1 =     Dr::CalcRotationAngleInDegrees(DrPointF(my_pos.x, my_pos.y), g_player_position);
               angle1 =     Dr::EqualizeAngle0to360(angle1);

        // Angle of Object
        DrPointF up =       Dr::RotatePointAroundOrigin(DrPointF(my_pos.x, my_pos.y - 1.0), DrPointF(my_pos.x, my_pos.y), my_angle, true);
        double angle2 =     Dr::CalcRotationAngleInDegrees(DrPointF(my_pos.x, my_pos.y), up);
               angle2 =     Dr::EqualizeAngle0to360(angle2);

        // Set Rotate Direction Towards Player
        double angle3 =     Dr::FindClosestAngle180(angle2, angle1);
        double angle_vel =  abs(object->getOriginalSpinVelocity());
        double new_spin =   (angle3 > angle2) ? angle_vel : -angle_vel;

        double angle_diff = abs(angle3 - angle2);
        if (angle_diff < 10.0) new_spin *= (angle_diff / 10.0);
        cpBodySetAngularVelocity(body, new_spin);

        ///g_info = "Angle 1: " +      QString::number(angle1,     'f', 2) +
        ///       ", Angle 3: " +      QString::number(angle2,     'f', 2) +
        ///       ", Angle Diff: " +   QString::number(angle_diff, 'f', 2);
    }

    // Figure out new velocity based on current object angle
    if (object->getUseAngleVelocity()) {
        double  angle = qRadiansToDegrees( cpBodyGetAngle(body) );
        QPointF original = QPointF( object->getOriginalVelocityX(), object->getOriginalVelocityY() );
        QPointF rotated = QTransform().rotate(angle).map(original);
        cpBodySetVelocity( body, cpv(rotated.x(), rotated.y()) );
    }  

    // Call real update function
    cpBodyUpdateVelocity(body, gravity, damping, dt);
}



























