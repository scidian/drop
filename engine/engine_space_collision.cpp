//
//      Created by Stephens Nunnally on 5/30/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QDebug>
#include "engine.h"

//  Arbiter Callbacks:
//      begin:      Called when objects first touch. Return true from the callback to process the collision normally or false to cause Chipmunk to ignore
//                      the collision entirely. If you return false, the preSolve() and postSolve() callbacks will never be run, but you will still recieve a
//                      separate event when the shapes stop overlapping.
//      presolve:   Called every frame, Two shapes are touching during this step. Return false from the callback to make Chipmunk ignore the collision this step
//                      or true to process it normally. Additionally, you may override collision values using cpArbiterSetFriction(), cpArbiterSetElasticity() or
//                      cpArbiterSetSurfaceVelocity()
//      postsolve:  Two shapes are touching and their collision response has been processed. You can retrieve the collision impulse or kinetic energy at this
//                      time if you want to use it to calculate sound volumes or damage amounts.
//      seperate:   Two shapes have just stopped touching for the first time this step. To ensure that begin() / separate() are always called in balanced pairs,
//                      it will also be called when removing a shape while its in contact with something or when deallocating the space.

enum class One_Way2 {                    // One Way Collide
    None,
    Pass_Through,                       // Objects can pass through going one_way_direction
    Weak_Point,                         // Only takes damage from one_way_direction
    Damage_Direction,                   // Only gives damage from one_way_direction (block with
};

//######################################################################################################
//##    Chipmunk Collision Callbacks
//##        Support for object collisions, damage, recoil, one way platforms and more
//######################################################################################################
static void BodyAddRecoil(cpSpace *, cpArbiter *arb, DrEngineObject *object) {
    // METHOD: Apply damage_recoil opposite velocity
    cpVect n = cpArbiterGetNormal(arb);                         // Get Normal of contact point
    cpVect velocity = cpBodyGetVelocity(object->body);          // Get current velocity of body
    if (abs(velocity.x) < 1.0 && abs(velocity.y) < 1.0)         // If object isnt moving, give it the velocity towards the collision
        velocity = cpvneg(cpArbiterGetNormal(arb));
    double dot = cpvdot(velocity, n);                           // Calculate dot product (difference of angle from collision normal)
    if (dot < 0.0) {                                            // If objects velocity if goings towards collision point, reflect it
        QPointF v = QPointF(velocity.x, velocity.y);                // Convert to QPointF for better vector math operators than cpVect
        v = v - (2.0 * dot * QPointF(n.x, n.y));                    // Reflect velocity normal across the plane of the collision normal
        velocity = cpv(v.x(), v.y());                               // Convert back to cpVect
    }
    velocity = cpvnormalize(velocity);                          // Normalize body velocity
    velocity.x *= object->damage_recoil;                        // Apply recoil force x to new direction vector
    velocity.y *= object->damage_recoil;                        // Apply recoil force y to new direction vector
    cpBodySetVelocity( object->body, velocity );                // Set body to new velocity
}

extern cpBool BeginFuncWildcard(cpArbiter *arb, cpSpace *, void *) {
    CP_ARBITER_GET_SHAPES(arb, a, b);
    DrEngineObject *object_a = static_cast<DrEngineObject*>(cpShapeGetUserData(a));
    DrEngineObject *object_b = static_cast<DrEngineObject*>(cpShapeGetUserData(b));

    // Check for one way platform
    if (object_a->one_way == One_Way::Pass_Through) {                               // Don't collide with something trying to pass through you
        if (cpvdot(cpArbiterGetNormal(arb), object_a->one_way_direction) < 0.0)
            return cpArbiterIgnore(arb);
    }
    if (object_b->one_way == One_Way::Pass_Through) {                               // Don't collide with something you want to pass through
        if (cpvdot(cpArbiterGetNormal(arb), object_b->one_way_direction) > 0.0)
            return cpArbiterIgnore(arb);
    }

    if ( object_a->damage <= c_epsilon) return cpTrue;                              // Object does no damage, exit
    if ( object_a->alive && object_a->dying) return cpTrue;                         // Don't deal damage while dying
    if (!object_a->alive) return cpFalse;                                           // If object a is dead, cancel collision
    if (!object_b->alive) return cpFalse;                                           // If object b is dead, cancel collision

    // Check for dealing damage
    bool should_damage = false;
    if ((object_a->collision_type == Collision_Type::Damage_Enemy) &&
        (object_b->collision_type == Collision_Type::Damage_Player || object_b->collision_type == Collision_Type::Damage_All))
        should_damage = true;

    if ((object_a->collision_type == Collision_Type::Damage_Player) &&
        (object_b->collision_type == Collision_Type::Damage_Enemy  || object_b->collision_type == Collision_Type::Damage_All))
        should_damage = true;

    if (object_a->collision_type == Collision_Type::Damage_All)
        should_damage = true;

    // Check for one way weak point
    if (object_a->one_way == One_Way::Weak_Spot) {                                  // Don't deal damage if something comes at your weak (vulnerable) spot
        if (cpvdot(cpArbiterGetNormal(arb), object_a->one_way_direction) < 0.0)
            should_damage = false;
    }
    if (object_b->one_way == One_Way::Weak_Spot) {                                  // Don't deal damage unless you are hitting its weak spot
        if (cpvdot(cpArbiterGetNormal(arb), object_b->one_way_direction) < 0.001)   // i.e. floating point for <= 0
            should_damage = false;
    }

    if (should_damage) {
        if (object_b->health > 0.0) {
            object_b->health -= object_a->damage;

            // If we killed object, cancel collision
            if (object_b->health <= c_epsilon) {
                object_b->health = 0.0;
                if (object_b->death_delay == 0) return cpFalse;
            }

            // Add recoil to object if necessary
            if (object_b->damage_recoil > 0.0 && object_b->body_type == Body_Type::Dynamic) {
                if (cpBodyIsSleeping(object_b->body))
                    cpSpaceAddPostStepCallback(cpBodyGetSpace(object_b->body), cpPostStepFunc(BodyAddRecoil), arb, object_b);
                else
                    BodyAddRecoil(cpBodyGetSpace(object_b->body), arb, object_b);
            }
        }
    }
    return cpTrue;
}


extern cpBool PreSolveFuncWildcard(cpArbiter *arb, cpSpace *, void *) {
    CP_ARBITER_GET_SHAPES(arb, a, b);
    DrEngineObject *object_a = static_cast<DrEngineObject*>(cpShapeGetUserData(a));
    ///DrEngineObject *object_b = static_cast<DrEngineObject*>(cpShapeGetUserData(b));

    if (!object_a->alive) return cpFalse;                                       // If object is dead, cancel collision

    return cpTrue;
}


void DrEngine::setCollisionType(DrEngineObject *object, Collision_Type type) {
    object->collision_type = type;
    for (auto shape : object->shapes) {
        cpShapeSetCollisionType(shape, static_cast<cpCollisionType>(type));
    }
}
























