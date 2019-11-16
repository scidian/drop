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
#include "engine/engine_spawner.h"
#include "engine_things/engine_thing_light.h"
#include "engine_things/engine_thing_object.h"
#include "engine/engine_world.h"
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

    // ***** Calculate area that if Things are within, they can stay in the Space
    QRectF threshold(getCameraPositionX() - getDeleteThresholdX(),
                     getCameraPositionY() - getDeleteThresholdY(),
                     getDeleteThresholdX()*2.0, getDeleteThresholdY()*2.0);

    // ***** Update global variables for use in callbacks
    g_gravity_normal = cpvnormalize( cpSpaceGetGravity(m_space) );


    // ***** Iterate through Things, delete if they go off screen
    updateThings(time_passed, m_time_warp, threshold);

    // ***** Iterate through Spawners, delete if they go off screen, run out of Spawns
    updateSpawners(time_passed, m_time_warp, threshold);


    // ***** Calculate distance and Add New Stage if needed
    if (has_scene == true) {
        QTransform t = QTransform().translate( m_game_start.x,  m_game_start.y)
                                   .rotate(    m_game_direction)
                                   .translate(-m_game_start.x, -m_game_start.y);
        QPointF rotated = t.map( QPointF( getCameraPositionX(), getCameraPositionY() ));
        m_game_distance = rotated.x() - m_game_start.x;

        if (m_loaded_to - m_game_distance < m_load_buffer) {
            addStage();
        }
    }


    // !!!!!!!!!! #TEMP: Light update for Demo
    if (demo_player == Demo_Player::Light) {
        light1->setPosition( ball->getPosition() );
        light2->setPosition( ball->getPosition() );
        light1->setAngle( ball->getAngle() + 90 );
    }
}


//####################################################################################
//##    Iterate through Things, delete if they go off screen
//####################################################################################
void DrEngineWorld::updateThings(double time_passed, double time_warp, QRectF &area) {
    for (auto it = m_things.begin(); it != m_things.end(); ) {
        DrEngineThing *thing = *it;

        // ***** Update Thing
        thing->calculateTimeSinceLastUpdate();
        bool remove = thing->update(time_passed, time_warp, area);

        // ***** Process Removal
        if (remove) {
            // Let Spawners know that the Object they're attached to is being deleted
            for (auto spawner : thing->getSpawners()) {
                spawner->setAttachedThing(nullptr);
            }

            // Delete Thing, #NOTE: Remember that delete recursively calls children destructors
            delete thing;
            it = m_things.erase(it);
            continue;
        }

        // ***** Increment for loop
        it++;
    }
}

//####################################################################################
//##    Iterate through Spawners, delete if they go off screen or run out of Spawns
//####################################################################################
void DrEngineWorld::updateSpawners(double time_passed, double time_warp, QRectF &area) {
    for (auto it = m_spawners.begin(); it != m_spawners.end(); ) {
        DrEngineSpawner *spawner = *it;

        // ***** Update Spawner
        spawner->update(time_passed, time_warp, area);

        // ***** Process Removal
        if (spawner->readyForRemoval()) {
            // Let Attached Object know Spawner is being deleted
            if (spawner->getAttachedThing() != nullptr) {
                spawner->getAttachedThing()->removeSpawner(spawner);
            }

            // Delete Spawner
            delete spawner;
            it = m_spawners.erase(it);
            continue;
        }

        // ***** Increment for loop
        it++;
    }
}


//####################################################################################
//##    Finds a Stage, if possible, and Loads it to this World
//####################################################################################
void DrEngineWorld::addStage() {
    // Get DrWorld from Project
    DrWorld *world = m_project->getWorld(m_world);

    // Find a list of possible Stages to pick from
    QVector<DrStage*> stages;
    for (auto &stage_pair : world->getStageMap()) {
        DrStage *stage = stage_pair.second;
        if (stage == nullptr) continue;

        bool stage_enabled = stage->getComponentPropertyValue(Components::Stage_Settings, Properties::Stage_Enabled).toBool();
        int  stage_start =   stage->getComponentPropertyValue(Components::Stage_Settings, Properties::Stage_Start).toInt();
        int  stage_end =     stage->getComponentPropertyValue(Components::Stage_Settings, Properties::Stage_End).toInt();

        // !!!!! NEED TO IMPLEMENT
        int  cooldown =      stage->getComponentPropertyValue(Components::Stage_Settings, Properties::Stage_Cooldown).toInt();

        if (stage_enabled && stage->getName() != "Start Stage") {
            if (m_loaded_to > stage_start) {
                if (m_loaded_to < stage_end || stage_end < 0) {
                    stages.append(stage_pair.second);
                }
            }
        }
    }

    // If there is at least one other stage available, load it up
    if (stages.count() >= 1) {
        DrStage *stage = nullptr;
        int stage_count = static_cast<int>(stages.size());
        int stage_num = QRandomGenerator::global()->bounded(0, stage_count);
            stage = stages[stage_num];

        QTransform t = QTransform().translate( m_game_start.x,  m_game_start.y)
                                   .rotate(   -m_game_direction)
                                   .translate(-m_game_start.x, -m_game_start.y);
        QPointF rotated = t.map( QPointF( m_loaded_to, 0 ));
        loadStageToWorld(stage, rotated.x(), rotated.y());
    }

}























