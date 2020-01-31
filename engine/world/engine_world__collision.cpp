//
//      Created by Stephens Nunnally on 5/30/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "engine/engine.h"
#include "engine/things/engine_thing_object.h"
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

// Internal Linkage (File Scope) Forward Declarations
static void BodyAddRecoil(cpSpace *, cpArbiter *arb, DrEngineObject *object);


//####################################################################################
//##    Set DrEngineObject and all physics children to designated group filter so they don't collide
//##        DEFAULTS:   reset_groups == true
//####################################################################################
void DrEngineWorld::applyCategoryMask(DrEngineObject *central, long group_id) {
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
    unsigned int all_categories = ~(static_cast<unsigned int>(0));
    cpShapeFilter filter;
    filter.group =      group_id;               // Any int > 0, useful to use unique project id of parent (i.e. object->getKey())
    filter.categories = all_categories;         // CP_ALL_CATEGORIES
    filter.mask =       all_categories;         // CP_ALL_CATEGORIES
    for (auto shape : central->shapes) {
        cpShapeSetFilter(shape, filter);
    }
    if (central->soft_balls.size() > 0) {
        for (auto ball_number : central->soft_balls) {
            DrEngineObject *ball = this->findObjectByKey(ball_number);
            if (ball == nullptr) continue;
            for (auto shape : ball->shapes) {
                cpShapeSetFilter( shape, filter);
            }
        }
    }
}


//####################################################################################
//##    Chipmunk Collision Callbacks
//##        Support for object collisions, damage, recoil, one way platforms and more
//####################################################################################
extern cpBool BeginFuncWildcard(cpArbiter *arb, cpSpace *, void *) {
    CP_ARBITER_GET_SHAPES(arb, a, b)
    DrEngineObject *object_a = static_cast<DrEngineObject*>(cpShapeGetUserData(a));
    DrEngineObject *object_b = static_cast<DrEngineObject*>(cpShapeGetUserData(b));
    if (object_a == nullptr || object_b == nullptr) return cpTrue;

    if (object_a->shouldCollide(object_b) == false) return cpArbiterIgnore(arb);
    if (object_b->shouldCollide(object_a) == false) return cpArbiterIgnore(arb);

    // Temp cancel gravity on another object if colliding and should cancel it
    if ( Dr::FuzzyCompare(object_b->getGravityMultiplier(), 1.0) == false ) {
        object_a->setTempGravityMultiplier( object_b->getGravityMultiplier() );
    }

    // Check for one way platform
    if (object_a->getOneWay() == One_Way::Pass_Through) {                               // Don't collide with something trying to pass through you
        if (cpvdot(cpArbiterGetNormal(arb), object_a->getOneWayDirection()) <= 0.0)
            return cpArbiterIgnore(arb);
    }
    if (object_b->getOneWay() == One_Way::Pass_Through) {                               // Don't collide with something you want to pass through
        if (cpvdot(cpArbiterGetNormal(arb), object_b->getOneWayDirection()) >= 0.0)
            return cpArbiterIgnore(arb);
    }

    return cpTrue;
}


extern cpBool PreSolveFuncWildcard(cpArbiter *arb, cpSpace *, void *) {
    CP_ARBITER_GET_SHAPES(arb, a, b)
    DrEngineObject *object_a = static_cast<DrEngineObject*>(cpShapeGetUserData(a));
    DrEngineObject *object_b = static_cast<DrEngineObject*>(cpShapeGetUserData(b));
    if (object_a == nullptr || object_b == nullptr) return cpTrue;

    if ( object_a->isAlive() && object_a->isDying()) return cpTrue;                     // Don't deal damage while dying
    if (!object_a->isAlive()) return cpFalse;                                           // If object a is dead, cancel collision
    if (!object_b->isAlive()) return cpFalse;                                           // If object b is dead, cancel collision
    if ( Dr::FuzzyCompare(object_b->getGravityMultiplier(), 1.0) == false )             // Temp cancel / reduce / increase gravity on another
        object_a->setTempGravityMultiplier( object_b->getGravityMultiplier() );         //      object if colliding and should cancel / adjust it
    if (!object_a->doesDamage()) return cpTrue;                                         // Object does no damage, exit

    // Check for dealing damage
    bool should_damage = object_a->shouldDamage(object_b->getCollisionType());

    // Check for one way weak point
    if (object_a->getOneWay() == One_Way::Weak_Spot) {                                  // Don't deal damage if something comes at your weak (vulnerable) spot
        if (cpvdot(cpArbiterGetNormal(arb), object_a->getOneWayDirection()) < 0.0)
            should_damage = false;
    }
    if (object_b->getOneWay() == One_Way::Weak_Spot) {                                  // Don't deal damage unless you are hitting its weak spot
        if (cpvdot(cpArbiterGetNormal(arb), object_b->getOneWayDirection()) < 0.001)    // i.e. floating point for <= 0
            should_damage = false;
    }

    if (Dr::MillisecondsElapsed(object_b->getDamageTimer()) < object_b->getDamageDelay())
        should_damage = false;

    if (should_damage) {
        bool killed = object_b->takeDamage(object_a->getDamage(), true, object_a->hasDeathTouch());

        // If we killed object and object wants instant death, cancel collision
        if (killed && object_b->getDeathDelay() == 0) return cpFalse;

        // Add recoil to object if necessary and not invincible
        if (!object_b->isInvincible() && object_b->getDamageRecoil() > 0.0 && object_b->body_type == Body_Type::Dynamic) {
            if (cpBodyIsSleeping(object_b->body))
                cpSpaceAddPostStepCallback(cpBodyGetSpace(object_b->body), cpPostStepFunc(BodyAddRecoil), arb, object_b);
            else
                BodyAddRecoil(cpBodyGetSpace(object_b->body), arb, object_b);
        }
    }
    return cpTrue;
}

extern void PostSolveFuncWildcard(cpArbiter *arb, cpSpace *space, void *) {
    CP_ARBITER_GET_SHAPES(arb, a, b)
    DrEngineObject *object_a = static_cast<DrEngineObject*>(cpShapeGetUserData(a));
    DrEngineObject *object_b = static_cast<DrEngineObject*>(cpShapeGetUserData(b));
    if (object_a == nullptr || object_b == nullptr) return;

    // We can react to collision force here, good place to work with for Interactive Foliage
    if (cpArbiterIsFirstContact(arb)) {
        // Divide the impulse by the timestep to get the collision force.
        double impact = cpvlength(cpArbiterTotalImpulse(arb)) / cpSpaceGetCurrentTimeStep(space);

        if (impact > 1.0) {

        }
    }
}


extern void SeperateFuncWildcard(cpArbiter *arb, cpSpace *, void *) {
    CP_ARBITER_GET_SHAPES(arb, a, b)
    DrEngineObject *object_a = static_cast<DrEngineObject*>(cpShapeGetUserData(a));
    DrEngineObject *object_b = static_cast<DrEngineObject*>(cpShapeGetUserData(b));
    if (object_a == nullptr || object_b == nullptr) return;

    // Stop canceling gravity when seperates
    object_a->setTempGravityMultiplier( 1.0 );
}


//####################################################################################
//##    Applies Recoil Force after being damaged another object
//####################################################################################
static void BodyAddRecoil(cpSpace *, cpArbiter *arb, DrEngineObject *object) {
    if (object->getPhysicsParent() != nullptr) object = object->getPhysicsParent();

    // METHOD: Apply damage_recoil opposite velocity
    cpVect n = cpArbiterGetNormal(arb);                         // Get Normal of contact point
    cpVect velocity = cpBodyGetVelocity(object->body);          // Get current velocity of body
    if (abs(velocity.x) < 1.0 && abs(velocity.y) < 1.0)         // If object isnt moving, give it the velocity towards the collision
        velocity = cpvneg(cpArbiterGetNormal(arb));
    double dot = cpvdot(velocity, n);                           // Calculate dot product (difference of angle from collision normal)
    if (dot < 0.0) {                                            // If objects velocity if goings towards collision point, reflect it
        DrPointF v = DrPointF(velocity.x, velocity.y);          // Convert to DrPointF for better vector math operators than cpVect
        v = v - (DrPointF(n.x, n.y) * 2.0 * dot);               // Reflect velocity normal across the plane of the collision normal
        velocity = cpv(v.x, v.y);                               // Convert back to cpVect
    }
    velocity = cpvnormalize(velocity);                          // Normalize body velocity
    velocity.x *= object->getDamageRecoil();                    // Apply recoil force x to new direction vector
    velocity.y *= object->getDamageRecoil();                    // Apply recoil force y to new direction vector
    cpBodySetVelocity( object->body, velocity );                // Set body to new velocity
}






//####################################################################################
//##    Bouyancy Test
//####################################################################################
#define FLUID_DENSITY 0.004
#define FLUID_DRAG 2.0

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
    DrEngineObject *object_b = static_cast<DrEngineObject*>(cpShapeGetUserData(collider));
    if (object_b->body_type != Body_Type::Dynamic) return cpTrue;                                   // Only process dynamic bodies
    if (object_b->shape_type[collider] == Shape_Type::Segment) return cpTrue;                       // Don't precess segments

    // If shape is a circle, create a temporary polygon circle shape
    if (object_b->shape_type[collider] == Shape_Type::Circle) {
        cpVect  offset = cpCircleShapeGetOffset(collider);
        std::vector<DrPointF> points = object_b->createEllipseFromCircle(DrPointF(offset.x, offset.y), cpCircleShapeGetRadius(collider), 12);
        int old_point_count = static_cast<int>(points.size());

        // Copy polygon Vertices into a scaled cpVect array
        std::vector<cpVect> verts;
        verts.clear();
        verts.resize( static_cast<size_t>(old_point_count) );
        for (int i = 0; i < old_point_count; i++)
            verts[static_cast<size_t>(i)] = cpv( points[i].x * static_cast<double>(object_b->getScaleX()), points[i].y * static_cast<double>(object_b->getScaleY()));
        collider = cpPolyShapeNew( object_b->body, old_point_count, verts.data(), cpTransformIdentity, c_extra_radius);
    }

    // Clip the polygon against the water level
    std::vector<cpVect> clipped;
    clipped.clear();
    int  count = cpPolyShapeGetCount(collider);
    for (int i = 0, j = count - 1; i < count; j = i, i++) {
        cpVect a = cpBodyLocalToWorld(object_b->body, cpPolyShapeGetVert(collider, j));
        cpVect b = cpBodyLocalToWorld(object_b->body, cpPolyShapeGetVert(collider, i));
        if (a.y < level) clipped.push_back(a);

        cpFloat a_level = a.y - level;
        cpFloat b_level = b.y - level;
        if (a_level * b_level < 0.0) {
            cpFloat t = cpfabs(a_level) / (cpfabs(a_level) + cpfabs(b_level));
            clipped.push_back(cpvlerp(a, b, t));
        }
    }

    // Destroy temporary polygon circle shape
    if (object_b->shape_type[collider] == Shape_Type::Circle) cpShapeFree( collider );





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
    cpBodyApplyImpulseAtWorldPoint(object_b->body, impulse, centroid);


    // Apply linear damping for the fluid drag
    cpVect  v_centroid = cpBodyGetVelocityAtWorldPoint(object_b->body, centroid);
    cpFloat k =          k_scalar_body(object_b->body, centroid, cpvnormalize(v_centroid));
    cpFloat damping =    clipped_area * FLUID_DRAG * FLUID_DENSITY;
    cpFloat v_coef =     cpfexp(-damping * dt * k);                                     // linear drag
    impulse = cpvmult(cpvsub(cpvmult(v_centroid, v_coef), v_centroid), 1.0 / k);
    if (Dr::RealDouble(impulse.x) == false) impulse.x = 0.0;
    if (Dr::RealDouble(impulse.y) == false) impulse.y = 0.0;
    cpBodyApplyImpulseAtWorldPoint(object_b->body, impulse, centroid);


    // Apply angular damping for the fluid drag
    cpVect  cog =       cpBodyLocalToWorld(object_b->body, cpBodyGetCenterOfGravity(object_b->body));
    cpFloat w_damping = cpMomentForPoly(FLUID_DRAG * FLUID_DENSITY * clipped_area, static_cast<int>(clipped.size()), clipped.data(), cpvneg(cog), 0.01);
    cpFloat moment = cpBodyGetMoment(object_b->body);
    cpFloat angular_velocity = cpBodyGetAngularVelocity(object_b->body) * cpfexp(-w_damping*dt / moment);
    if (Dr::RealDouble(angular_velocity) == false) angular_velocity = 0.0;
    cpBodySetAngularVelocity(object_b->body, angular_velocity);

    return cpTrue;
}






















