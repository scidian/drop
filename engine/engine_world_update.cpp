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
#include "engine_thing_light.h"
#include "engine_thing_object.h"
#include "engine_world.h"
#include "form_engine.h"
#include "helper.h"
#include "opengl/opengl.h"
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

void DrEngineWorld::updateSpaceHelper(double time_passed) {

    // Calculate area that if Things are within, they can stay in the Space
    QRectF threshold(getCameraPosXD() - m_delete_threshold_x, getCameraPosYD() - m_delete_threshold_y, m_delete_threshold_x*2.0, m_delete_threshold_y*2.0);

    // ***** Update global variables for use in callbacks
    g_gravity_normal = cpvnormalize( cpSpaceGetGravity(m_space) );
    g_keyboard_x =  m_engine->keyboard_x;
    g_keyboard_y =  m_engine->keyboard_y;
    g_jump_button = m_engine->jump_button;
    g_pedal =       m_engine->gas_pedal;


    // ********** Iterate through Things, delete if they go off screen
    for ( auto it = m_things.begin(); it != m_things.end(); ) {
        DrEngineThing *thing = *it;

        // ***** Update Thing
        thing->calculateTimeSinceLastUpdate();
        bool remove = thing->update(time_passed, m_time_warp, threshold);

        // ***** Process Removal
        if (remove) {
            removeThing(thing);
            delete thing;
            it = m_things.erase(it);
        } else {
            it++;
        }
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











