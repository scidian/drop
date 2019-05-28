//
//      Created by Stephens Nunnally on 5/12/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QApplication>

#include "engine/engine.h"


//######################################################################################################
//##    Chipmunk Callbacks
//##        Clears bodies / shapes / constraints
//######################################################################################################
static void ShapeFreeWrap(cpSpace *space, cpShape *shape, void *) { cpSpaceRemoveShape(space, shape);   cpShapeFree(shape); }
static void ConstraintFreeWrap(cpSpace *space, cpConstraint *constraint, void *) { cpSpaceRemoveConstraint(space, constraint); cpConstraintFree(constraint); }
static void BodyFreeWrap(cpSpace *space, cpBody *body, void *) { cpSpaceRemoveBody(space, body); cpBodyFree(body); }

static void PostShapeFree(cpShape *shape, cpSpace *space) { cpSpaceAddPostStepCallback(space, cpPostStepFunc(ShapeFreeWrap), shape, nullptr); }
static void PostConstraintFree(cpConstraint *constraint, cpSpace *space) { cpSpaceAddPostStepCallback(space, cpPostStepFunc(ConstraintFreeWrap), constraint, nullptr); }
static void PostBodyFree(cpBody *body, cpSpace *space) { cpSpaceAddPostStepCallback(space, cpPostStepFunc(BodyFreeWrap), body, nullptr); }

// Safe and future proof way to remove and free all objects that have been added to the space.
void ChipmunkFreeSpaceChildren(cpSpace *space) {
    // Must remove these BEFORE freeing the body or you will access dangling pointers.
    cpSpaceEachShape(space, cpSpaceShapeIteratorFunc(PostShapeFree), space);
    cpSpaceEachConstraint(space, cpSpaceConstraintIteratorFunc(PostConstraintFree), space);
    cpSpaceEachBody(space, cpSpaceBodyIteratorFunc(PostBodyFree), space);
}

// Used for constarint iterator to get a list of all constraints attached to a body
static void getJointList(cpBody *, cpConstraint *constraint, QVector<cpConstraint*> *joint_list) { joint_list->append(constraint); }

// Used for shape iterator to get a list of all shapes attached to a body
static void getShapeList(cpBody *, cpShape *shape, QVector<cpShape*> *shape_list) { shape_list->append(shape); }


//######################################################################################################
//##    Clear Space
//######################################################################################################
void DrEngine::clearSpace() {
    if (has_scene) {
        has_scene = false;
        qApp->processEvents();
        ChipmunkFreeSpaceChildren(m_space);
        cpSpaceFree(m_space);
        objects.clear();
    }
}


//######################################################################################################
//##    Removes an object from the Space
//######################################################################################################
void DrEngine::removeObject(DrEngineObject *object) {
    object->should_process = false;

    QVector<cpShape*> shape_list;
    cpBodyEachShape(object->body, cpBodyShapeIteratorFunc(getShapeList), &shape_list);
    for (auto shape : shape_list) {
        cpSpaceRemoveShape(m_space, shape);
        cpShapeFree(shape);
    }

    QVector<cpConstraint*> joint_list;
    cpBodyEachConstraint(object->body, cpBodyConstraintIteratorFunc(getJointList), &joint_list);
    for (auto joint : joint_list) {
        cpSpaceRemoveConstraint(m_space, joint);
        cpConstraintFree(joint);
    }

    cpSpaceRemoveBody(m_space, object->body);
    cpBodyFree(object->body);
}






