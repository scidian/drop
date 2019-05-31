//
//      Created by Stephens Nunnally on 5/30/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
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
//##    Chipmunk Callbacks
//##        Support for object collisions and one way platforms
//######################################################################################################
extern cpBool BeginFuncWildcard(cpArbiter *arb, cpSpace *, void *) {
    CP_ARBITER_GET_SHAPES(arb, a, b);
    DrEngineObject *object_a = static_cast<DrEngineObject*>(cpShapeGetUserData(a));
    DrEngineObject *object_b = static_cast<DrEngineObject*>(cpShapeGetUserData(b));

    // Check for one way platform
    if (object_a->one_way == One_Way::Pass_Through) {                               // Don't deal damage if something is passing through you
        if (cpvdot(cpArbiterGetNormal(arb), object_a->one_way_direction) < 0.0)
            return cpArbiterIgnore(arb);
    }
    if (object_b->one_way == One_Way::Pass_Through) {                               // Don't hurt it if you're just passing through it
        if (cpvdot(cpArbiterGetNormal(arb), object_b->one_way_direction) > 0.0)
            return cpArbiterIgnore(arb);
    }

    if ( object_a->damage <= 0) return cpTrue;                                      // Object does no damage, exit
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
    if (object_a->one_way == One_Way::Weak_Point) {                                 // Don't deal damage if coming from any other direction but one_way_direction
        if (cpvdot(cpArbiterGetNormal(arb), object_a->one_way_direction) < 0.0)
            should_damage = false;
    }
    if (object_b->one_way == One_Way::Weak_Point) {                                 // Don't deal damage if coming from any other direction but one_way_direction
        if (cpvdot(cpArbiterGetNormal(arb), object_b->one_way_direction) < 0.001)
            should_damage = false;
    }

    if (should_damage) {
        if (object_b->health > 0) {
            object_b->health -= object_a->damage;

            // If we killed object, cancel collision
            if (object_b->health <= 0) {
                object_b->health = 0;
                if (object_b->death_delay == 0) return cpFalse;
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
























