//
//      Created by Stephens Nunnally on 5/30/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "core/dr_containers.h"
#include "core/dr_random.h"
#include "core/dr_time.h"
#include "engine/engine.h"
#include "engine/thing/components/thing_comp_foliage.h"
#include "engine/thing/components/thing_comp_physics.h"
#include "engine/thing/components/thing_comp_player.h"
#include "engine/thing/components/thing_comp_soft_body.h"
#include "engine/thing/engine_thing.h"
#include "engine/world/engine_world.h"
#include "project/properties/property_collision.h"


//  Arbiter Callbacks:
//      begin:      Called when objects first touch. Return true from the callback to process the collision normally or false to cause Chipmunk
//                      to ignore the collision entirely. If you return false, the preSolve() and postSolve() callbacks will never be run, but
//                      you will still recieve a separate event when the shapes stop overlapping.
//      presolve:   Called every frame, two shapes are touching during this step. Return false from the callback to make Chipmunk ignore the
//                      collision this step or true to process it normally. Additionally, you may override collision values using
//                      cpArbiterSetFriction(), cpArbiterSetElasticity() or cpArbiterSetSurfaceVelocity()
//      postsolve:  Two shapes are touching and their collision response has been processed. You can retrieve the collision impulse or kinetic
//                      energy at this time if you want to use it to calculate sound volumes or damage amounts.
//      seperate:   Two shapes have just stopped touching for the first time this step. To ensure that begin() / separate() are always called in
//                      balanced pairs, it will also be called when removing a shape while its in contact with something or when deallocating the space.


//####################################################################################
//##    Set DrEngineObject and all physics children to designated group filter so they don't collide
//##        DEFAULTS:   reset_groups == true
//####################################################################################
void DrEngineWorld::applyCategoryMask(DrEngineThing *parent_thing, long group_id) {
    // #define GRABBABLE_MASK_BIT (1<<31)
    // cpShapeFilter GRAB_FILTER = {CP_NO_GROUP, GRABBABLE_MASK_BIT, GRABBABLE_MASK_BIT};
    // cpShapeFilter NOT_GRABBABLE_FILTER = {CP_NO_GROUP, ~GRABBABLE_MASK_BIT, ~GRABBABLE_MASK_BIT};
    //
    // filter.group = no_group = 0 = could collide all
    //              = set all children to unique project_id to stop them from colliding with each other
    // categories   = what not to collide with  (player, player bullet, enemy, enemy bullet, wall, etc)
    // mask         = what to collide with      (walls, enemy bullet)
    //
    ///enum Shape_Groups {
    ///    A = 1 << 0,
    ///    B = 1 << 1,
    ///    C = 1 << 2,
    ///};
    if (parent_thing->physics() == nullptr) return;
    unsigned int all_categories = ~(static_cast<unsigned int>(0));
    cpShapeFilter filter;
    filter.group =      group_id;               // Any int > 0, useful to use unique project id of parent (i.e. object->getKey())
    filter.categories = all_categories;         // CP_ALL_CATEGORIES
    filter.mask =       all_categories;         // CP_ALL_CATEGORIES
    for (auto shape : parent_thing->physics()->shapes) {
        cpShapeSetFilter(shape, filter);
    }

    // Apply all shapes of physics children to same group filter
    if (parent_thing->compSoftBody() != nullptr) {
        if (parent_thing->compSoftBody()->soft_balls.size() > 0) {
            for (auto ball_number : parent_thing->compSoftBody()->soft_balls) {
                DrEngineThing *ball = this->findPhysicsObjectByKey(ball_number);
                if (ball == nullptr) continue;
                if (ball->physics() == nullptr) continue;
                for (auto shape : ball->physics()->shapes) {
                    cpShapeSetFilter( shape, filter);
                }
            }
        }
    }
}


//####################################################################################
//##    Grabs underlying physics objects (DrEngineThing physics components) from
//##    stored chipmunk user data pointer
//####################################################################################
bool GetPhysicsObjects(cpArbiter *arb, ThingCompPhysics *&physics_a, ThingCompPhysics *&physics_b) {
    CP_ARBITER_GET_SHAPES(arb, shape_a, shape_b)
    DrEngineThing *thing_a = static_cast<DrEngineThing*>(cpShapeGetUserData(shape_a));
    DrEngineThing *thing_b = static_cast<DrEngineThing*>(cpShapeGetUserData(shape_b));
    physics_a = nullptr;
    physics_b = nullptr;
    bool have_parents;
    do {
        have_parents = false;
        if (thing_a == nullptr || thing_b == nullptr) return cpTrue;
        physics_a = thing_a->physics();
        physics_b = thing_b->physics();
        if (physics_a == nullptr || physics_b == nullptr) return cpTrue;
        if (physics_a->isPhysicsChild()) { thing_a = physics_a->getPhysicsParent(); have_parents = true; }
        if (physics_b->isPhysicsChild()) { thing_b = physics_b->getPhysicsParent(); have_parents = true; }
    } while (have_parents);
    return false;
}


//####################################################################################
//##    Chipmunk Collision Callbacks
//##        Support for object collisions, damage, recoil, one way platforms and more
//####################################################################################
extern cpBool BeginFuncWildcard(cpArbiter *arb, cpSpace *, void *) {
    ThingCompPhysics *physics_a = nullptr;
    ThingCompPhysics *physics_b = nullptr;
    if (GetPhysicsObjects(arb, physics_a, physics_b)) return cpTrue;
    return physics_a->collideBegin(arb, physics_b->thing());
}

extern cpBool PreSolveFuncWildcard(cpArbiter *arb, cpSpace *, void *) {
    ThingCompPhysics *physics_a = nullptr;
    ThingCompPhysics *physics_b = nullptr;
    if (GetPhysicsObjects(arb, physics_a, physics_b)) return cpTrue;
    return physics_a->collideStep(arb, physics_b->thing());
}

extern void PostSolveFuncWildcard(cpArbiter *arb, cpSpace *, void *) {
    ThingCompPhysics *physics_a = nullptr;
    ThingCompPhysics *physics_b = nullptr;
    if (GetPhysicsObjects(arb, physics_a, physics_b)) return;
    physics_a->collideEnd(arb, physics_b->thing());
}

extern void SeperateFuncWildcard(cpArbiter *arb, cpSpace *, void *) {
    ThingCompPhysics *physics_a = nullptr;
    ThingCompPhysics *physics_b = nullptr;
    if (GetPhysicsObjects(arb, physics_a, physics_b)) return;
    physics_a->collideSeperate(arb, physics_b->thing());
}




//####################################################################################
//##    Bouyancy Test
//####################################################################################
#define FLUID_DENSITY 0.004
#define FLUID_DRAG    2.0

// Modified from chipmunk_private.h
static inline cpFloat k_scalar_body(cpBody *body, cpVect point, cpVect n) {
    cpFloat rcn =    cpvcross(cpvsub(point, cpBodyGetPosition(body)), n);
    cpFloat mass =   cpBodyGetMass(body);
    cpFloat moment = cpBodyGetMoment(body);
    return (1.0/mass + rcn*rcn/moment);
}

extern cpBool WaterPreSolve(cpArbiter *arb, cpSpace *space, void *) {
    // Get water and collider shape
    CP_ARBITER_GET_SHAPES(arb, water, collider)

    // Get the top of the water sensor bounding box to use as the water level
    cpFloat level = cpShapeGetBB(water).t;

    // Get DrEngineObject from shape user data, check body type and shape type
    DrEngineThing *thing_b = static_cast<DrEngineThing*>(cpShapeGetUserData(collider));
    if (thing_b == nullptr) return cpTrue;
    ThingCompPhysics *physics_b = thing_b->physics();
    if (physics_b == nullptr) return cpTrue;
    if (physics_b->body_type != Body_Type::Dynamic) return cpTrue;                                  // Only process dynamic bodies
    if (physics_b->shape_type[collider] == Shape_Type::Segment) return cpTrue;                      // Don't precess segments

    // If shape is a circle, create a temporary polygon circle shape
    if (physics_b->shape_type[collider] == Shape_Type::Circle) {
        cpVect  offset = cpCircleShapeGetOffset(collider);
        std::vector<DrPointF> points = physics_b->createEllipseFromCircle(DrPointF(offset.x, offset.y), cpCircleShapeGetRadius(collider), 12);
        int old_point_count = static_cast<int>(points.size());

        // Copy polygon Vertices into a scaled cpVect array
        std::vector<cpVect> verts;
        verts.clear();
        verts.resize( static_cast<size_t>(old_point_count) );
        for (int i = 0; i < old_point_count; i++)
            verts[static_cast<size_t>(i)] = cpv( points[i].x * static_cast<double>(thing_b->getScaleX()), points[i].y * static_cast<double>(thing_b->getScaleY()));
        collider = cpPolyShapeNew( physics_b->body, old_point_count, verts.data(), cpTransformIdentity, c_extra_radius);
    }

    // Clip the polygon against the water level
    std::vector<cpVect> clipped;
    clipped.clear();
    int  count = cpPolyShapeGetCount(collider);
    for (int i = 0, j = count - 1; i < count; j = i, i++) {
        cpVect a = cpBodyLocalToWorld(physics_b->body, cpPolyShapeGetVert(collider, j));
        cpVect b = cpBodyLocalToWorld(physics_b->body, cpPolyShapeGetVert(collider, i));
        if (a.y < level) clipped.push_back(a);

        cpFloat a_level = a.y - level;
        cpFloat b_level = b.y - level;
        if (a_level * b_level < 0.0) {
            cpFloat t = cpfabs(a_level) / (cpfabs(a_level) + cpfabs(b_level));
            clipped.push_back(cpvlerp(a, b, t));
        }
    }

    // Destroy temporary polygon circle shape
    if (physics_b->shape_type[collider] == Shape_Type::Circle) cpShapeFree( collider );





    // Calculate buoyancy from the clipped polygon area
    cpFloat clipped_area =   cpAreaForPoly(static_cast<int>(clipped.size()), clipped.data(), 0.0);
    cpFloat displaced_mass = clipped_area * FLUID_DENSITY;
    cpVect  centroid =       cpCentroidForPoly(static_cast<int>(clipped.size()), clipped.data());
    cpFloat dt = cpSpaceGetCurrentTimeStep(space);
    cpVect  g =  cpSpaceGetGravity(space);

    // Apply the buoyancy force as an impulse
    cpVect impulse = cpvmult(g, -displaced_mass * dt);
    if (Dr::RealDouble(impulse.x) == false) impulse.x = 0.0;
    if (Dr::RealDouble(impulse.y) == false) impulse.y = 0.0;
    cpBodyApplyImpulseAtWorldPoint(physics_b->body, impulse, centroid);


    // Apply linear damping for the fluid drag
    cpVect  v_centroid = cpBodyGetVelocityAtWorldPoint(physics_b->body, centroid);
    cpFloat k =          k_scalar_body(physics_b->body, centroid, cpvnormalize(v_centroid));
    cpFloat damping =    clipped_area * FLUID_DRAG * FLUID_DENSITY;
    cpFloat v_coef =     cpfexp(-damping * dt * k);                                     // linear drag
    impulse = cpvmult(cpvsub(cpvmult(v_centroid, v_coef), v_centroid), 1.0 / k);
    if (Dr::RealDouble(impulse.x) == false) impulse.x = 0.0;
    if (Dr::RealDouble(impulse.y) == false) impulse.y = 0.0;
    cpBodyApplyImpulseAtWorldPoint(physics_b->body, impulse, centroid);


    // Apply angular damping for the fluid drag
    cpVect  cog =       cpBodyLocalToWorld(physics_b->body, cpBodyGetCenterOfGravity(physics_b->body));
    cpFloat w_damping = cpMomentForPoly(FLUID_DRAG * FLUID_DENSITY * clipped_area, static_cast<int>(clipped.size()), clipped.data(), cpvneg(cog), 0.01);
    cpFloat moment = cpBodyGetMoment(physics_b->body);
    cpFloat angular_velocity = cpBodyGetAngularVelocity(physics_b->body) * cpfexp(-w_damping*dt / moment);
    if (Dr::RealDouble(angular_velocity) == false) angular_velocity = 0.0;
    cpBodySetAngularVelocity(physics_b->body, angular_velocity);

    return cpTrue;
}






















