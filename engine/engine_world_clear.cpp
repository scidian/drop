//
//      Created by Stephens Nunnally on 5/12/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QApplication>
#include <QDebug>

#include "engine.h"
#include "engine_camera.h"
#include "engine_thing_light.h"
#include "engine_thing_object.h"
#include "engine_world.h"
#include "form_engine.h"
#include "opengl/opengl.h"

//####################################################################################
//##    Chipmunk Callbacks
//##        Clears bodies / shapes / constraints
//####################################################################################
static void ShapeFreeWrap(cpSpace *space, cpShape *shape, void *) { cpSpaceRemoveShape(space, shape);   cpShapeFree(shape); }
static void ConstraintFreeWrap(cpSpace *space, cpConstraint *constraint, void *) { cpSpaceRemoveConstraint(space, constraint); cpConstraintFree(constraint); }
static void BodyFreeWrap(cpSpace *space, cpBody *body, void *) { cpSpaceRemoveBody(space, body); cpBodyFree(body); }

static void PostShapeFree(cpShape *shape, cpSpace *space) { cpSpaceAddPostStepCallback(space, cpPostStepFunc(ShapeFreeWrap), shape, nullptr); }
static void PostConstraintFree(cpConstraint *constraint, cpSpace *space) { cpSpaceAddPostStepCallback(space, cpPostStepFunc(ConstraintFreeWrap), constraint, nullptr); }
static void PostBodyFree(cpBody *body, cpSpace *space) { cpSpaceAddPostStepCallback(space, cpPostStepFunc(BodyFreeWrap), body, nullptr); }

// Safe and future proof way to remove and free all objects that have been added to the space
static void ChipmunkFreeSpaceChildren(cpSpace *space) {
    // Must remove Shape and Constraint BEFORE freeing the Body or you will access dangling pointers
    cpSpaceEachShape(space, cpSpaceShapeIteratorFunc(PostShapeFree), space);
    cpSpaceEachConstraint(space, cpSpaceConstraintIteratorFunc(PostConstraintFree), space);
    cpSpaceEachBody(space, cpSpaceBodyIteratorFunc(PostBodyFree), space);
}


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
        for (auto thing : m_things)
            delete thing;
        m_things.clear();

        // Clear all Cameras
        for (auto camera_pair : m_cameras)
            delete camera_pair.second;
        m_cameras.clear();

        // Clear cpSpace from any remaining items, free Space
        ChipmunkFreeSpaceChildren(m_space);
        cpSpaceFree(m_space);
    }
}
















