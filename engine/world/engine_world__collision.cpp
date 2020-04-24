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

// Internal Linkage (File Scope) Forward Declarations
static void BodyAddRecoil(cpSpace *space, cpArbiter *arb, DrEngineThing *engine_thing);


//####################################################################################
//##    Set DrEngineObject and all physics children to designated group filter so they don't collide
//##        DEFAULTS:   reset_groups == true
//####################################################################################
void DrEngineWorld::applyCategoryMask(DrEngineThing *central, long group_id) {
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
    if (central->physics() == nullptr) return;
    unsigned int all_categories = ~(static_cast<unsigned int>(0));
    cpShapeFilter filter;
    filter.group =      group_id;               // Any int > 0, useful to use unique project id of parent (i.e. object->getKey())
    filter.categories = all_categories;         // CP_ALL_CATEGORIES
    filter.mask =       all_categories;         // CP_ALL_CATEGORIES
    for (auto shape : central->physics()->shapes) {
        cpShapeSetFilter(shape, filter);
    }
    if (central->compSoftBody() != nullptr) {
        if (central->compSoftBody()->soft_balls.size() > 0) {
            for (auto ball_number : central->compSoftBody()->soft_balls) {
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
//##    Chipmunk Collision Callbacks
//##        Support for object collisions, damage, recoil, one way platforms and more
//####################################################################################
extern cpBool BeginFuncWildcard(cpArbiter *arb, cpSpace *, void *) {
    CP_ARBITER_GET_SHAPES(arb, a, b)
    DrEngineThing *thing_a = static_cast<DrEngineThing*>(cpShapeGetUserData(a));
    DrEngineThing *thing_b = static_cast<DrEngineThing*>(cpShapeGetUserData(b));
    if (thing_a == nullptr || thing_b == nullptr) return cpTrue;
    ThingCompPhysics *physics_a = thing_a->physics();
    ThingCompPhysics *physics_b = thing_b->physics();
    if (physics_a == nullptr || physics_b == nullptr) return cpTrue;
    if (physics_a->isPhysicsChild()) { thing_a = physics_a->getPhysicsParent(); physics_a = thing_a->compPhysics(); }
    if (physics_b->isPhysicsChild()) { thing_b = physics_b->getPhysicsParent(); physics_b = thing_b->compPhysics(); }

    if (physics_a->shouldCollide(thing_b) == false) return cpArbiterIgnore(arb);
    if (physics_b->shouldCollide(thing_a) == false) return cpArbiterIgnore(arb);

    // Interactive foliage
    if (thing_a->compFoliage() != nullptr) {
        if (physics_a->checkCollisionCountWithObject(thing_b) == 0) {                           // Only react to an objects first shape that collides
            if (physics_b->body_type != Body_Type::Static) {
                double mass =  cpBodyGetMass(physics_a->body) * 2.0;
                double force = mass * thing_a->compFoliage()->getSpringiness();
                cpVect v = cpvmult(cpBodyGetVelocity(physics_b->body), force);
                       v.x = Dr::Clamp(v.x, -(mass * 500.0), (mass * 500.0));
                       v.y = Dr::Clamp(v.y, -(mass * 500.0), (mass * 500.0));
                cpBodyApplyImpulseAtWorldPoint(physics_a->body, v, cpArbiterGetPointA(arb, 0));
            }
            ///return cpArbiterIgnore(arb);
        }
    }

    // Some special Player collsion processing
    if (thing_a->compPlayer() != nullptr) {
        // Ledge Grabbing
        if (Dr::VectorContains(thing_a->compPlayer()->getLedgeGrabbers(), a)) {
            cpVect n = cpvneg(cpArbiterGetNormal(arb));                                         // Get normal vector of collision
            double dot = cpvdot(n, g_gravity_normal);                                           // Compare angle of gravity to angle of normal
            if (dot > -0.50) return cpArbiterIgnore(arb);                                       // Cancel collision if not in direction of gravity
        }
    }

    // Check for one way platform
    if (physics_a->getOneWay() == One_Way::Pass_Through) {                                      // Don't collide with something trying to pass through you
        if (cpvdot(cpArbiterGetNormal(arb), physics_a->getOneWayDirection()) <= 0.0)
            return cpArbiterIgnore(arb);
    }
    if (physics_b->getOneWay() == One_Way::Pass_Through) {                                      // Don't collide with something you want to pass through
        if (cpvdot(cpArbiterGetNormal(arb), physics_b->getOneWayDirection()) >= 0.0)
            return cpArbiterIgnore(arb);
    }

    // Keeps track number of shape collisions between objects
    if (cpArbiterIsFirstContact(arb)) {
        physics_a->increaseCollisionCountWithObject(thing_b);
    }

    // !!!!! #TEMP: Player collision tracking
    if (thing_a->compPlayer() != nullptr) {
        g_info = "Touching Shapes: " + std::to_string(physics_a->checkCollisionCountWithObject(thing_b));
    }

    return cpTrue;
}


extern cpBool PreSolveFuncWildcard(cpArbiter *arb, cpSpace *space, void *) {
    CP_ARBITER_GET_SHAPES(arb, a, b)
    DrEngineThing *thing_a = static_cast<DrEngineThing*>(cpShapeGetUserData(a));
    DrEngineThing *thing_b = static_cast<DrEngineThing*>(cpShapeGetUserData(b));
    if (thing_a == nullptr || thing_b == nullptr) return cpTrue;
    ThingCompPhysics *physics_a = thing_a->physics();
    ThingCompPhysics *physics_b = thing_b->physics();
    if (physics_a == nullptr || physics_b == nullptr) return cpTrue;
    if (physics_a->isPhysicsChild()) { thing_a = physics_a->getPhysicsParent(); physics_a = thing_a->compPhysics(); }
    if (physics_b->isPhysicsChild()) { thing_b = physics_b->getPhysicsParent(); physics_b = thing_b->compPhysics(); }

    if ( physics_a->isAlive() && physics_a->isDying()) return cpTrue;                           // Don't deal damage while dying
    if (!physics_a->isAlive()) return cpFalse;                                                  // If object a is dead, cancel collision
    if (!physics_b->isAlive()) return cpFalse;                                                   // If object b is dead, cancel collision

    // Some special Player collsion processing
    if (thing_a->compPlayer() != nullptr) {
        // Compare angle of button speed to angle of gravity normal
        cpVect but_cpv = cpv(thing_a->compPlayer()->getButtonSpeedX(), thing_a->compPlayer()->getButtonSpeedY());
        double but_dot = cpvdot(but_cpv, g_gravity_normal);

        // Allow for Drop Down
        if (physics_b->getDropDown() && but_dot > 0.0) {
            cpArbiterIgnore(arb);
            return cpFalse;
        }

        // Temp gravity multiplier if colliding with an object or sensor that should change its gravity
        if (Dr::FuzzyCompare(physics_b->getGravityMultiplier(), 1.0) == false) {
            // Apply temp gravity multiplier if button pressed away from gravity
            if (but_dot < 0.0 || but_dot > 0.0) thing_a->compPlayer()->setTempGravityMultiplier( physics_b->getGravityMultiplier() );
        }

    }

    if (!physics_a->doesDamage()) return cpTrue;                                                // Object does no damage, exit

    // Check for dealing damage
    bool should_damage = physics_a->shouldDamage(physics_b->getCollisionType());

    // Check for one way weak point
    if (physics_a->getOneWay() == One_Way::Weak_Spot) {                                         // Don't deal damage if something comes at your weak (vulnerable) spot
        if (cpvdot(cpArbiterGetNormal(arb), physics_a->getOneWayDirection()) < 0.0)
            should_damage = false;
    }
    if (physics_b->getOneWay() == One_Way::Weak_Spot) {                                         // Don't deal damage unless you are hitting its weak spot
        if (cpvdot(cpArbiterGetNormal(arb), physics_b->getOneWayDirection()) < 0.001)           // i.e. floating point for <= 0
            should_damage = false;
    }

    if (Dr::MillisecondsElapsed(physics_b->getDamageTimer()) < physics_b->getDamageDelay())
        should_damage = false;

    if (should_damage) {
        long damaged_by_key = thing_a->getKey();
        if (physics_a->isPhysicsChild() && physics_a->getPhysicsParent() != nullptr)
            damaged_by_key = physics_a->getPhysicsParent()->getKey();

        bool killed = physics_b->takeDamage(physics_a->getDamage(), true, physics_a->hasDeathTouch(), false, damaged_by_key);

        // If we killed object and object wants instant death, cancel collision
        if (killed && physics_b->getDeathDelay() == 0) return cpFalse;

        // Recoil force - if has and not invincible
        if ((Dr::FuzzyCompare(physics_b->getDamageRecoil(), 0.0) == false) && (physics_b->isInvincible() == false)) {
            physics_b->setReflectForce(physics_b->getDamageRecoil());
            BodyAddRecoil(space, arb, thing_b);
        }
    }

    // Interactive foliage
    if (thing_a->compFoliage() != nullptr) {
        return cpFalse;
    }

    // Repulse force
    if (Dr::FuzzyCompare(physics_a->getRepulseForce(), 0.0) == false) {
        physics_b->setReflectForce(physics_a->getRepulseForce());
        BodyAddRecoil(space, arb, thing_b);
    }

    // Normal collision
    return cpTrue;
}


extern void PostSolveFuncWildcard(cpArbiter *arb, cpSpace *space, void *) {
    CP_ARBITER_GET_SHAPES(arb, a, b)
    DrEngineThing *thing_a = static_cast<DrEngineThing*>(cpShapeGetUserData(a));
    DrEngineThing *thing_b = static_cast<DrEngineThing*>(cpShapeGetUserData(b));
    if (thing_a == nullptr || thing_b == nullptr) return;
    ThingCompPhysics *physics_a = thing_a->physics();
    ThingCompPhysics *physics_b = thing_b->physics();
    if (physics_a == nullptr || physics_b == nullptr) return;
    if (physics_a->isPhysicsChild()) { thing_a = physics_a->getPhysicsParent(); physics_a = thing_a->compPhysics(); }
    if (physics_b->isPhysicsChild()) { thing_b = physics_b->getPhysicsParent(); physics_b = thing_b->compPhysics(); }

    // We can react to collision force here, such as show an explosion based on impact force
    if (cpArbiterIsFirstContact(arb)) {
        // Divide the impulse by the timestep to get the collision force
        double impact = cpvlength(cpArbiterTotalImpulse(arb)) / cpSpaceGetCurrentTimeStep(space);

        if (impact > 100.0) {

        }
    }
}


extern void SeperateFuncWildcard(cpArbiter *arb, cpSpace *, void *) {
    CP_ARBITER_GET_SHAPES(arb, a, b)
    DrEngineThing *thing_a = static_cast<DrEngineThing*>(cpShapeGetUserData(a));
    DrEngineThing *thing_b = static_cast<DrEngineThing*>(cpShapeGetUserData(b));
    if (thing_a == nullptr || thing_b == nullptr) return;
    ThingCompPhysics *physics_a = thing_a->physics();
    ThingCompPhysics *physics_b = thing_b->physics();
    if (physics_a == nullptr || physics_b == nullptr) return;
    if (physics_a->isPhysicsChild()) { thing_a = physics_a->getPhysicsParent(); physics_a = thing_a->compPhysics(); }
    if (physics_b->isPhysicsChild()) { thing_b = physics_b->getPhysicsParent(); physics_b = thing_b->compPhysics(); }

    // Keeps track number of shape collisions between objects
    physics_a->decreaseCollisionCountWithObject(thing_b);

    // Stop canceling gravity when seperates
    if (thing_a->compPlayer() != nullptr) {
        thing_a->compPlayer()->setTempGravityMultiplier( 1.0 );
    }

    // !!!!! #TEMP: Player collision tracking
    if (thing_a->compPlayer() != nullptr) {
        g_info = "Touching Shapes: " + std::to_string(physics_a->checkCollisionCountWithObject(thing_b));
    }
}




//####################################################################################
//##    Applies Recoil Force after being damaged another object
//####################################################################################
static void BodyAddRecoil(cpSpace *space, cpArbiter *arb, DrEngineThing *engine_thing) {
    ThingCompPhysics *physics = engine_thing->physics();
    if (physics == nullptr) return;
    if (physics->getPhysicsParent() != nullptr) { engine_thing = physics->getPhysicsParent(); physics = engine_thing->physics(); }

    if (physics->body_type != Body_Type::Dynamic) return;
    if (cpBodyIsSleeping(physics->body)) {
        cpSpaceAddPostStepCallback(space, cpPostStepFunc(BodyAddRecoil), arb, engine_thing);
        return;
    }

    // METHOD: Apply damage_recoil opposite velocity
    cpVect n = cpArbiterGetNormal(arb);                                 // Get Normal of contact point
    cpVect velocity = cpBodyGetVelocity(physics->body);                 // Get current velocity of body
    if (abs(velocity.x) < 1.0 && abs(velocity.y) < 1.0) {               // If object isnt moving, give it the velocity towards the collision
        velocity = cpvneg(cpArbiterGetNormal(arb));
    }
    double dot = cpvdot(velocity, n);                                   // Calculate dot product (difference of angle from collision normal)
    if (dot < 0.0) {                                                    // If objects velocity if goings towards collision point, reflect it
        DrPointF v { velocity.x, velocity.y };                          // Convert to DrPointF for better vector math operators than cpVect
        v = v - (DrPointF(n.x, n.y) * 2.0 * dot);                       // Reflect velocity normal across the plane of the collision normal
        velocity = cpv(v.x, v.y);                                       // Convert back to cpVect
    }
    velocity = cpvnormalize(velocity);                                  // Normalize body velocity
    velocity.x *= physics->getReflectForce();                           // Apply reflect force x to new direction vector
    velocity.y *= physics->getReflectForce();                           // Apply reflect force y to new direction vector
    cpBodySetVelocity(physics->body, velocity);                         // Set body to new velocity
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






















