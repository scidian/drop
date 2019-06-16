//
//      Created by Stephens Nunnally on 6/15/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QApplication>
#include <QGraphicsScene>

#include "form_playground.h"
#include "playground.h"
#include "playground_toy.h"

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
static void ChipmunkFreeSpaceChildren(cpSpace *space) {
    // Must remove these BEFORE freeing the body or you will access dangling pointers.
    cpSpaceEachShape(space, cpSpaceShapeIteratorFunc(PostShapeFree), space);
    cpSpaceEachConstraint(space, cpSpaceConstraintIteratorFunc(PostConstraintFree), space);
    cpSpaceEachBody(space, cpSpaceBodyIteratorFunc(PostBodyFree), space);
}

// Used for constraint iterator to get a list of all constraints attached to a body
static void GetBodyJointList(cpBody *, cpConstraint *constraint, QVector<cpConstraint*> *joint_list) { joint_list->append(constraint); }


//######################################################################################################
//##    Clear Space
//######################################################################################################
void DrPlayground::clearSpace() {
    if (has_scene) {
        has_scene = false;
        qApp->processEvents();
        ChipmunkFreeSpaceChildren(m_space);
        cpSpaceFree(m_space);
        objects.clear();

        for (auto graphic : m_form_playground->getScene()->items()) {
            m_form_playground->getScene()->removeItem( graphic );
            delete graphic;
        }
    }
}

//######################################################################################################
//##    Wake All Sleeping Bodies
//######################################################################################################
static void WakeBody(cpBody *body, cpSpace *) { cpBodyActivate(body); }
void DrPlayground::wakeAllBodies() { if (has_scene) cpSpaceEachBody(m_space, cpSpaceBodyIteratorFunc(WakeBody), m_space); }


//######################################################################################################
//##    Removes an object from the Space
//######################################################################################################
void DrPlayground::removeObject(DrToy *toy) {
    toy->m_should_process = false;

    cpSpaceRemoveShape(m_space, toy->shape);
    cpShapeFree(toy->shape);

    QVector<cpConstraint*> joint_list;
    cpBodyEachConstraint(toy->body, cpBodyConstraintIteratorFunc(GetBodyJointList), &joint_list);
    for (auto joint : joint_list) {
        cpSpaceRemoveConstraint(m_space, joint);
        cpConstraintFree(joint);
    }

    cpSpaceRemoveBody(m_space, toy->body);
    cpBodyFree(toy->body);

    if (toy->graphic->scene()) {
        toy->graphic->scene()->removeItem( toy->graphic );
        delete toy->graphic;
    }
}












