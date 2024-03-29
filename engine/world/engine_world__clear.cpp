//
//      Created by Stephens Nunnally on 5/12/2019, (c) 2019 Scidian Studios, All Rights Reserved
//
//  File:
//
//
//
#include "engine/engine.h"
#include "engine/engine_camera.h"
#include "engine/engine_spawner.h"
#include "engine/form_engine.h"
#include "engine/opengl/opengl.h"
#include "engine/thing/engine_thing.h"
#include "engine/world/engine_world.h"


//####################################################################################
//##    Chipmunk Callbacks
//##        Clears bodies / shapes / constraints
//####################################################################################
static void ShapeFreeWrap(cpSpace *space, cpShape *shape, void *)                   { cpSpaceRemoveShape(space, shape); cpShapeFree(shape); }
static void ConstraintFreeWrap(cpSpace *space, cpConstraint *constraint, void *)    { cpSpaceRemoveConstraint(space, constraint); cpConstraintFree(constraint); }
static void BodyFreeWrap(cpSpace *space, cpBody *body, void *)                      { cpSpaceRemoveBody(space, body); cpBodyFree(body); }

static void PostShapeFree(cpShape *shape, cpSpace *space)                   { cpSpaceAddPostStepCallback(space, cpPostStepFunc(ShapeFreeWrap), shape, nullptr); }
static void PostConstraintFree(cpConstraint *constraint, cpSpace *space)    { cpSpaceAddPostStepCallback(space, cpPostStepFunc(ConstraintFreeWrap), constraint, nullptr); }
static void PostBodyFree(cpBody *body, cpSpace *space)                      { cpSpaceAddPostStepCallback(space, cpPostStepFunc(BodyFreeWrap), body, nullptr); }

// Safe and future proof way to remove and free all objects that have been added to the space
static void ChipmunkFreeSpaceChildren(cpSpace *space) {
    // Must remove Shape and Constraint BEFORE freeing the Body or you will access dangling pointers
    cpSpaceEachShape(space, cpSpaceShapeIteratorFunc(PostShapeFree), space);
    cpSpaceEachConstraint(space, cpSpaceConstraintIteratorFunc(PostConstraintFree), space);
    cpSpaceEachBody(space, cpSpaceBodyIteratorFunc(PostBodyFree), space);
}

// Iterators to get a list of all constraints, shapes, etc attached to a body
static void GetBodyJointList(cpBody *, cpConstraint *constraint, std::vector<cpConstraint*> *joint_list)    { joint_list->push_back(constraint); }
static void GetBodyShapeList(cpBody *, cpShape *shape, std::vector<cpShape*> *shape_list)                   { shape_list->push_back(shape); }


//####################################################################################
//##    Wake All Sleeping Bodies
//####################################################################################
static void WakeBody(cpBody *body, cpSpace *) {
    if (cpBodyGetType(body) == CP_BODY_TYPE_DYNAMIC) {
        if (cpBodyIsSleeping(body)) {
            cpBodyActivate(body);
        }
    }
}
void DrEngineWorld::wakeAllBodies() {
    if (has_scene) {
        cpSpaceEachBody(m_space, cpSpaceBodyIteratorFunc(WakeBody), m_space);
    }
}

//####################################################################################
//##    Clear World of All Things, Clears Space of All Bodies
//####################################################################################
void DrEngineWorld::clearWorld() {
    if (has_scene) {
        has_scene = false;

        // Clear all Things (objects, lights, etc.)
        for (auto &thing : m_things) {
            delete thing;
            thing = nullptr;
        }
        m_things.clear();

        // Reset active camera to none, clear all Cameras
        m_active_camera = c_no_key;
        for (auto &camera_pair : m_cameras) {
            delete camera_pair.second;
            camera_pair.second = nullptr;
        }
        m_cameras.clear();

        // Remove all Spawners
        for (auto &spawner : m_spawners) {
            delete spawner;
            spawner = nullptr;
        }
        m_spawners.clear();

        // Clear cpSpace from any remaining items, free Space
        ChipmunkFreeSpaceChildren(m_space);
        cpSpaceFree(m_space);
    }
}


//####################################################################################
//##    Removes a physics body and all joints attached to it from this World
//####################################################################################
cpBody* DrEngineWorld::removeBody(cpBody *body) {
    // If body is already null, return
    if (body == nullptr) return nullptr;

    // If body was never added to space, delete and return null
    cpSpace *space = cpBodyGetSpace(body);
    if (space == nullptr) {
        cpBodyFree(body);
        return nullptr;
    }

    // Remove all shapes attached to body
    std::vector<cpShape*> shape_list;
    cpBodyEachShape(body, cpBodyShapeIteratorFunc(GetBodyShapeList), &shape_list);
    for (auto shape : shape_list) {
        cpSpaceRemoveShape(space, shape);
        cpShapeFree(shape);
    }

    // Remove all joints attached to body
    std::vector<cpConstraint*> joint_list;
    cpBodyEachConstraint(body, cpBodyConstraintIteratorFunc(GetBodyJointList), &joint_list);
    for (auto joint : joint_list) {
        // Check if constraint to remove is mouse joint (drag joint)
        if (getEngine() != nullptr) {
            if (joint == getEngine()->mouse_joint) {
                getEngine()->mouse_joint = nullptr;
            }
        }
        cpSpaceRemoveConstraint(space, joint);
        cpConstraintFree(joint);
    }

    // Remove body
    cpSpaceRemoveBody(space, body);
    cpBodyFree(body);
    return nullptr;
}













