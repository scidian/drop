//
//      Created by Stephens Nunnally on 4/24/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "engine/engine.h"
#include "engine/engine_texture.h"
#include "engine/thing/components/thing_comp_foliage.h"
#include "engine/thing/components/thing_comp_physics.h"
#include "engine/thing/components/thing_comp_player.h"
#include "engine/world/engine_world.h"

// Internal Linkage (File Scope) Forward Declarations
static void BodyAddRecoil(cpSpace *space, cpArbiter *arb, DrEngineThing *engine_thing);


//####################################################################################
//##    Gets Arbiter Info for Signals
//####################################################################################
Collision_Info GetCollisionInfo(cpArbiter *arb, ThingCompPhysics *physics_a, ThingCompPhysics *physics_b) {
    CP_ARBITER_GET_SHAPES(arb, a, b);
    Collision_Info info;
    info.shape_a =          a;
    info.shape_b =          b;
    info.point_a =          cpArbiterGetPointA(arb, 0);
    info.point_b =          cpArbiterGetPointB(arb, 0);
    info.normal =           cpArbiterGetNormal(arb);
    info.collision_count =  physics_a->checkCollisionCountWithObject(physics_b->thing());
    info.impulse_force =    cpArbiterTotalImpulse(arb);
    return info;
}

//####################################################################################
//##    Collide Begin (BeginFuncWildcard)
//##        Called when collision first starts
//####################################################################################
bool ThingCompPhysics::collideBegin(cpArbiter *arb, DrEngineThing *thing_b) {
    DrEngineThing    *thing_a =   thing();
    ThingCompPhysics *physics_a = thing_a->physics();
    ThingCompPhysics *physics_b = thing_b->physics();
    if (physics_a->shouldCollide(physics_b) == false) return cpArbiterIgnore(arb);
    if (physics_b->shouldCollide(physics_a) == false) return cpArbiterIgnore(arb);
    Collision_Info collide_info = GetCollisionInfo(arb, physics_a, physics_b);

    // Some special Player collsion processing
    if (thing_a->compPlayer() != nullptr) {
        // Ledge Grabbing
        if (Dr::VectorContains(thing_a->compPlayer()->getLedgeGrabbers(), collide_info.shape_a)) {
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


    // ***** Emit collision as signal
    emitSignal(Signals::ThingCollideStart, collide_info, thing_b->getKey());

    // ***** Keeps track number of shape collisions between objects
    if (cpArbiterIsFirstContact(arb)) {
        physics_a->increaseCollisionCountWithObject(thing_b);
    }

    // !!!!! #TEMP: Player collision tracking
    if (thing_a->compPlayer() != nullptr) {
        g_info = "Touching Shapes: " + std::to_string(physics_a->checkCollisionCountWithObject(thing_b));
    }

    return true;
}


//####################################################################################
//##    Collide Step (PreSolveFuncWildcard)
//##        Called every update step
//####################################################################################
bool ThingCompPhysics::collideStep(cpArbiter *arb, DrEngineThing *thing_b) {
    CP_ARBITER_GET_SHAPES(arb, a, b);
    DrEngineThing    *thing_a =   thing();
    ThingCompPhysics *physics_a = thing_a->physics();
    ThingCompPhysics *physics_b = thing_b->physics();

    if ( physics_a->isAlive() && physics_a->isDying()) return cpTrue;                           // Don't deal damage while dying
    if (!physics_a->isAlive()) return cpFalse;                                                  // If object a is dead, cancel collision
    if (!physics_b->isAlive()) return cpFalse;                                                  // If object b is dead, cancel collision

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
            BodyAddRecoil(world()->getSpace(), arb, thing_b);
        }
    }

    // Interactive foliage
    if (thing_a->compFoliage() != nullptr) {
        return cpFalse;
    }

    // Repulse force
    if (Dr::FuzzyCompare(physics_a->getRepulseForce(), 0.0) == false) {
        physics_b->setReflectForce(physics_a->getRepulseForce());
        BodyAddRecoil(world()->getSpace(), arb, thing_b);
    }

    // Normal collision
    return cpTrue;
}


//####################################################################################
//##    Collide End (BeginFuncWildcard)
//##        Called when collision stops
//####################################################################################
bool ThingCompPhysics::collideEnd(cpArbiter *arb, DrEngineThing *thing_b) {
    CP_ARBITER_GET_SHAPES(arb, a, b);
    DrEngineThing    *thing_a =   thing();
    ThingCompPhysics *physics_a = thing_a->physics();
    ThingCompPhysics *physics_b = thing_b->physics();

    (void) physics_a;
    (void) physics_b;

    // We can react to collision force here, such as show an explosion based on impact force
    if (cpArbiterIsFirstContact(arb)) {
        // Divide the impulse by the timestep to get the collision force
        double impact = cpvlength(cpArbiterTotalImpulse(arb)) / cpSpaceGetCurrentTimeStep(world()->getSpace());
        if (impact > 100.0) {

        }
    }
    return cpTrue;
}


//####################################################################################
//##    Collide Seperate (BeginFuncWildcard)
//##        Called when two objects seperate
//####################################################################################
bool ThingCompPhysics::collideSeperate(cpArbiter *arb, DrEngineThing *thing_b) {
    CP_ARBITER_GET_SHAPES(arb, a, b);
    DrEngineThing    *thing_a =   thing();
    ThingCompPhysics *physics_a = thing_a->physics();
    ThingCompPhysics *physics_b = thing_b->physics();

    (void) physics_b;

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
    return cpTrue;
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




























