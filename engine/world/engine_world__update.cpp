//
//      Created by Stephens Nunnally on 5/6/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <numeric>

#include "core/dr_random.h"
#include "engine/engine.h"
#include "engine/engine_camera.h"
#include "engine/engine_spawner.h"
#include "engine/form_engine.h"
#include "engine/opengl/opengl.h"
#include "engine/thing/components_effects/thing_comp_light.h"
#include "engine/thing/engine_thing.h"
#include "engine/world/engine_world.h"
#include "project/dr_project.h"
#include "project/entities/dr_stage.h"
#include "project/entities/dr_world.h"


// Local Structs
struct Space_Time {
    cpSpace *space;
    double   time_step;
};

//####################################################################################
//##    Check joint forces, break if necessary
//####################################################################################
//static void BreakJoint(cpSpace *space, cpConstraint *constraint, void *) {
//    bool was_mouse = (constraint == g_mouse_joint);
//    cpSpaceRemoveConstraint(space, constraint);
//    cpConstraintFree(constraint);
//    if (was_mouse) g_mouse_joint = nullptr;
//}
//static void CheckJointForce(cpConstraint *constraint, Space_Time *space_time) {
//    // The most recent impulse that constraint applied. To convert this to a force, divide by the timestep passed to cpSpaceStep()
//    cpFloat joint_force = cpConstraintGetImpulse(constraint) / (space_time->time_step);
//    cpBody *body_a = cpConstraintGetBodyA(constraint);
//    cpBody *body_b = cpConstraintGetBodyB(constraint);
//    double  mass_a = cpBodyGetMass(body_a);
//    double  mass_b = cpBodyGetMass(body_b);
//    double  total_mass = 1;
//    if (Dr::RealDouble(mass_a)) total_mass += mass_a;
//    if (Dr::RealDouble(mass_b)) total_mass += mass_b;
//    joint_force /= total_mass;

//    if (joint_force > 5000000) {
//        cpSpaceAddPostStepCallback(space_time->space, cpPostStepFunc(BreakJoint), constraint, nullptr);
//    }
//}


//####################################################################################
//##    Update Space steps the physics calculations
//##        #NOTE:  updateWorld meant to be called immediately afterwards
//##                time_passed is in milliseconds
//####################################################################################
// #NOTE: time_passed is in milliseconds
void DrEngineWorld::updateSpace(double time_passed) {
    double step_time = time_passed / 1000.0 * m_time_warp;
    ///double step_time = m_time_step * m_time_warp;
    cpSpaceStep(m_space, step_time);

    // Check Joint Forces
    ///Space_Time space_time { m_space, step_time };
    ///cpSpaceEachConstraint(m_space, cpSpaceConstraintIteratorFunc(CheckJointForce), &space_time);
}

void DrEngineWorld::updateWorld(double time_passed) {

    // ***** Calculate area that if Things are within, they can stay in the Space
    DrRectF threshold(getCameraPositionX() - getDeleteThresholdX(), getCameraPositionY() - getDeleteThresholdY(), getDeleteThresholdX()*2.0, getDeleteThresholdY()*2.0);

    // ***** Update global variables for use in callbacks
    g_gravity_normal = cpvnormalize( cpSpaceGetGravity(m_space) );


    // ***** Iterate through Things, delete if they go off screen
    updateThings(time_passed, m_time_warp, threshold);

    // ***** Iterate through Spawners, delete if they go off screen, run out of Spawns
    updateSpawners(time_passed, m_time_warp, threshold);


///    g_size_t = Dr::Max(engine()->signalList().size(), g_size_t);
///    g_info = "Signal Qty: " + std::to_string(engine()->signalList().size()) + ", Max: " + std::to_string(g_size_t);



    // ***** Calculate distance and Add New Stage if needed
    if (has_scene == true) {
        DrPointF rotated = Dr::RotatePointAroundOrigin( DrPointF(getCameraPositionX(), getCameraPositionY()), m_game_start, m_game_direction );
        m_game_distance = rotated.x - m_game_start.x;
        m_max_game_distance = Dr::Max(m_game_distance, m_max_game_distance);

        // Update local DrWorld User Variable that tracks Distance
        DrWorld *world = getProject()->findWorldFromKey(getWorldKey());
        world->setVariable(Variables::Distance, m_max_game_distance, Variable_Info::Current);
        if (m_max_game_distance > world->variable(Variables::Distance, Variable_Info::Best).toDouble()) {
            world->setVariable(Variables::Distance, m_max_game_distance, Variable_Info::Best);
        }


        if (m_loaded_to - m_game_distance < m_load_buffer) {
            addStage();
        }
    }


    // !!!!!!!!!! #TEMP: Light update for Demo
    if (demo_player == Demo_Player::Light) {
        light1->setPosition( ball->getPosition() );
        light2->setPosition( ball->getPosition() );

        // Get Light component of DrEngingThing
        DrThingComponent *component = light1->component(Comps::Thing_Settings_Light);
        if (component != nullptr) {
            ThingCompLight *light_settings = dynamic_cast<ThingCompLight*>(component);
            if (light_settings != nullptr) {
                light_settings->setAngle( ball->getAngle() + 90 );
            }
        }
    }
}


//####################################################################################
//##    Iterate through Things, delete if they go off screen
//####################################################################################
void DrEngineWorld::updateThings(double time_passed, double time_warp, DrRectF &area) {
    for (auto it = m_things.begin(); it != m_things.end(); ) {
        DrEngineThing *thing = *it;

        // ***** Update Thing
        thing->calculateTimeSinceLastUpdate();
        bool remove_thing = thing->update(time_passed, time_warp, area);

        // ***** Process Removal
        if (remove_thing) {
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
void DrEngineWorld::updateSpawners(double time_passed, double time_warp, DrRectF &area) {
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
    DrWorld *world = m_project->findWorldFromKey(m_world_key);

    // Find a list of possible Stages to pick from
    std::vector<DrStage*> stages;
    for (auto &stage_pair : world->getStageMap()) {
        DrStage *stage = stage_pair.second;
        if (stage == nullptr) continue;

        bool stage_enabled = stage->getComponentPropertyValue(Comps::Stage_Settings, Props::Stage_Enabled).toBool();
        int  stage_start =   stage->getComponentPropertyValue(Comps::Stage_Settings, Props::Stage_Start).toInt();
        int  stage_end =     stage->getComponentPropertyValue(Comps::Stage_Settings, Props::Stage_End).toInt();

        // !!!!! #TODO: NEED TO IMPLEMENT
        int  cooldown =      stage->getComponentPropertyValue(Comps::Stage_Settings, Props::Stage_Cooldown).toInt();

        // "Play Stage" Mode
        if (m_engine->getStageKey() != c_no_key) {
            if (stage->getKey() == m_engine->getStageKey()) {
                stages.push_back(stage_pair.second);
            }

        // Normal Mode
        } else if (stage_enabled && (stage->isStartStage() == false)) {
            if (m_loaded_to > stage_start) {
                if (m_loaded_to < stage_end || stage_end <= 0) {
                    stages.push_back(stage_pair.second);
                }
            }
        }
    }

    // If there is at least one other stage available, load it up
    if (stages.size() >= 1) {
        DrStage *stage = nullptr;
        int stage_count = static_cast<int>(stages.size());
        int stage_num = Dr::RandomInt(0, stage_count);
            stage = stages[stage_num];

        DrPointF rotated = Dr::RotatePointAroundOrigin( DrPointF(m_loaded_to, 0), m_game_start, -m_game_direction );
        loadStageToWorld(stage, rotated.x, rotated.y);
    }

}























