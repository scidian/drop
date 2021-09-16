//
//      Created by Stephens Nunnally on 4/24/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "core/dr_debug.h"
#include "engine/engine.h"
#include "engine/engine_message.h"
#include "engine/engine_texture.h"
#include "engine/thing/components/thing_comp_foliage.h"
#include "engine/thing/components/thing_comp_physics.h"
#include "engine/thing/components/thing_comp_player.h"
#include "engine/world/engine_world.h"


//####################################################################################
//##    Gets Arbiter Info for Messages
//####################################################################################
Collision_Info GetCollisionInfo(cpArbiter *arb, ThingCompPhysics *physics_a, ThingCompPhysics *physics_b, bool get_points) {
    CP_ARBITER_GET_SHAPES(arb, a, b);
    Collision_Info info;
    info.shape_a =      a;
    info.shape_b =      b;
    if (physics_a && physics_b)     info.collision_count =  physics_a->checkCollisionCountWithObject(physics_b->thing());
    if (physics_a)                  info.velocity_a =       cpBodyGetVelocity(physics_a->body);
    if (physics_b)                  info.velocity_b =       cpBodyGetVelocity(physics_b->body);
    if (get_points) {
        info.point_a =          cpArbiterGetPointA(arb, 0);
        info.point_b =          cpArbiterGetPointB(arb, 0);
        info.normal =           cpArbiterGetNormal(arb);
    }
    return info;
}


//####################################################################################
//##    Collide Begin (BeginFuncWildcard)
//##        Called when collision first starts
//####################################################################################
bool ThingCompPhysics::collideBegin(cpArbiter *arb, DrEngineThing *thing_b) {
    // ******************** Get Info ********************
    DrEngineThing    *thing_a =   thing();
    ThingCompPhysics *physics_a = thing_a->physics();
    ThingCompPhysics *physics_b = thing_b->physics();
    if (physics_a->shouldCollide(physics_b) == false) return cpArbiterIgnore(arb);
    if (physics_b->shouldCollide(physics_a) == false) return cpArbiterIgnore(arb);
    Collision_Info collide_info = GetCollisionInfo(arb, physics_a, physics_b, true);


    // ******************** Process ********************
    // Interactive foliage
    if (thing_a->compFoliage() != nullptr) {
        if (collide_info.collision_count == 0) {                                                // Only react to an objects first shape that collides
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
        if (Dr::VectorContains(thing_a->compPlayer()->getLedgeGrabbers(), collide_info.shape_a)) {
            cpVect n = cpvneg(cpArbiterGetNormal(arb));                                         // Get normal vector of collision
            double dot = cpvdot(n, g_gravity_normal);                                           // Compare angle of gravity to angle of normal
            if (dot > -0.50) return cpArbiterIgnore(arb);                                       // Cancel collision if not in direction of gravity
        }
    }

    // Check for one way platform
    if (physics_a->getOneWay() == One_Way::Pass_Through) {                                      // Don't collide with something trying to pass through you
        if (cpvdot(collide_info.normal, physics_a->getOneWayDirection()) <= 0.0)
            return cpArbiterIgnore(arb);
    }
    if (physics_b->getOneWay() == One_Way::Pass_Through) {                                      // Don't collide with something you want to pass through
        if (cpvdot(collide_info.normal, physics_b->getOneWayDirection()) >= 0.0)
            return cpArbiterIgnore(arb);
    }


    // ******************** Wrap up ********************
    // ***** Emit collision as message
    if (collide_info.collision_count == 0)      emitMessage(Messages::ThingCollide, collide_info, thing_b);

    // ***** Keeps track number of shape collisions between objects
    if (cpArbiterIsFirstContact(arb))           physics_a->increaseCollisionCountWithObject(thing_b);

    // !!!!! #TEMP: Player collision tracking
    if (thing_a->compPlayer() != nullptr)       { ;} ///g_info = "Touching Shapes: " + std::to_string(physics_a->checkCollisionCountWithObject(thing_b));

    return cpTrue;
}


//####################################################################################
//##    Collide Step (PreSolveFuncWildcard)
//##        Called every update step
//####################################################################################
bool ThingCompPhysics::collideStep(cpArbiter *arb, DrEngineThing *thing_b) {
    // ******************** Get Info ********************
    CP_ARBITER_GET_SHAPES(arb, a, b);
    DrEngineThing    *thing_a =   thing();
    ThingCompPhysics *physics_a = thing_a->physics();
    ThingCompPhysics *physics_b = thing_b->physics();


    // ******************** Process ********************
    // ***** Check if something has tried to cancel this collision elsewhere in game
    if (messageList(Messages::ThingCancelCollision, thing()->getKey()).size() > 0) {
        return cpArbiterIgnore(arb);
    }

    // Check health
    if ( physics_a->isAlive() && physics_a->isDying()) return cpTrue;                               // Don't deal damage while dying
    if (!physics_a->isAlive()) return cpFalse;                                                      // If object a is dead, cancel collision
    if (!physics_b->isAlive()) return cpFalse;                                                      // If object b is dead, cancel collision

    // Some special Player collision processing
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
            if (but_dot < 0.0 || but_dot > 0.0) {
                thing_a->compPlayer()->setTempGravityMultiplier( physics_b->getGravityMultiplier() );
            }
        }
    }

    // Object does no damage, exit
    if (!physics_a->doesDamage()) return cpTrue;

    // Check for dealing damage
    bool should_damage = physics_a->shouldDamage(physics_b->getCollisionType());

    // Check for one way weak point
    if (physics_a->getOneWay() == One_Way::Weak_Spot) {                                             // Don't deal damage if something comes at your weak (vulnerable) spot
        if (cpvdot(cpArbiterGetNormal(arb), physics_a->getOneWayDirection()) < 0.0)
            should_damage = false;
    }
    if (physics_b->getOneWay() == One_Way::Weak_Spot) {                                             // Don't deal damage unless you are hitting its weak spot
        if (cpvdot(cpArbiterGetNormal(arb), physics_b->getOneWayDirection()) < 0.001)               // i.e. floating point for <= 0
            should_damage = false;
    }

    if (Dr::MillisecondsElapsed(physics_b->getDamageTimer()) < physics_b->getDamageDelay())
        should_damage = false;

    if (should_damage) {
        long damaged_by_key = thing_a->getKey();
        if (physics_a->isPhysicsChild() && physics_a->getPhysicsParent() != nullptr) {
            damaged_by_key = physics_a->getPhysicsParent()->getKey();
        }

        bool killed = physics_b->takeDamage(physics_a->getDamage(), true, physics_a->hasDeathTouch(), false, damaged_by_key);

        // If we killed object and object wants instant death, cancel collision
        if (killed && physics_b->getDeathDelay() == 0) return cpFalse;

        // Recoil force - if has and not invincible
        if ((Dr::FuzzyCompare(physics_b->getDamageRecoil(), 0.0) == false) && (physics_b->isInvincible() == false)) {
            physics_b->setUseForce(physics_b->getDamageRecoil());

// !!!!!!!!!!!!!!!
            Collision_Info recoil_info = GetCollisionInfo(arb, physics_a, physics_b, true);
            ///physics_b->bodyAddRecoil( recoil_info );
// !!!!!!!!!!!!!!!

        }
    }

    // Interactive foliage
    if (thing_a->compFoliage() != nullptr) {
        return cpFalse;
    }


    // ******************** Wrap Up ********************
    // ***** Emit ThingCollideStep Message
    ///Collision_Info collide_info = GetCollisionInfo(arb, physics_a, physics_b, true);
    ///emitMessage(Messages::ThingCollideStep, collide_info, thing_b);

    // ***** Normal collision
    return cpTrue;
}


//####################################################################################
//##    Collide End (BeginFuncWildcard)
//##        Called when collision stops
//####################################################################################
bool ThingCompPhysics::collideEnd(cpArbiter *arb, DrEngineThing *thing_b) {
    // ******************** Get Info ********************
    CP_ARBITER_GET_SHAPES(arb, a, b);
    DrEngineThing    *thing_a =   thing();
    ThingCompPhysics *physics_a = thing_a->physics();
    ThingCompPhysics *physics_b = thing_b->physics();
    (void) physics_a;
    (void) physics_b;


    // ******************** Process ********************
    // We can react to collision force here, such as show an explosion based on impact force
    /**
    if (cpArbiterIsFirstContact(arb)) {
        // Divide the impulse by the timestep to get the collision force
        double impact = cpvlength(cpArbiterTotalImpulse(arb)) / cpSpaceGetCurrentTimeStep(world()->getSpace());
        if (impact > 100.0) {

        }
    }
    */


    // ******************** Wrap Up ********************
    // ***** Emit ThingCollideEnd Message
    ///Collision_Info collide_info = GetCollisionInfo(arb, physics_a, physics_b, true);
    ///emitMessage(Messages::ThingCollideEnd, collide_info, thing_b);

    // ***** End collision, return value for this function has no effect
    return cpTrue;
}


//####################################################################################
//##    Collide Seperate (BeginFuncWildcard)
//##        Called when two objects seperate
//####################################################################################
bool ThingCompPhysics::collideSeperate(cpArbiter *arb, DrEngineThing *thing_b) {
    // ******************** Get Info ********************
    CP_ARBITER_GET_SHAPES(arb, a, b);
    DrEngineThing    *thing_a =   thing();
    ThingCompPhysics *physics_a = thing_a->physics();
    ThingCompPhysics *physics_b = thing_b->physics();

    // Keeps track number of shape collisions between objects
    physics_a->decreaseCollisionCountWithObject(thing_b);


    // ******************** Process ********************
    // Stop canceling gravity when seperates
    if (thing_a->compPlayer() != nullptr) {
        thing_a->compPlayer()->setTempGravityMultiplier( 1.0 );
    }


    // ******************** Wrap Up ********************
    // ***** Emit seperation message
    Collision_Info collide_info = GetCollisionInfo(arb, physics_a, physics_b, false);
    if (collide_info.collision_count == 0)      emitMessage(Messages::ThingSeperate, collide_info, thing_b);

    // !!!!! #TEMP: Player collision tracking
    if (thing_a->compPlayer() != nullptr)       { ;} ///g_info = "Touching Shapes: " + std::to_string(physics_a->checkCollisionCountWithObject(thing_b));

    // ***** End collision, return value for this function has no effect
    return cpTrue;
}



//####################################################################################
//##    Applies Recoil Force after being damaged another object
//####################################################################################
void ThingCompPhysics::bodyAddRecoil(Collision_Info info) {
    if (this->body_type != Body_Type::Dynamic) return;
    cpVect normal =   info.normal;                                      // Get normal of contact point
    cpVect velocity = info.velocity_b;                                  // Get velocity of body on impact

    // If object isnt moving, give it the velocity towards the collision
    if (abs(velocity.x) < 1.0 && abs(velocity.y) < 1.0) {
        velocity = cpvneg(info.normal);
    }

    // Reflect velocity
    double dot = cpvdot(velocity, normal);                              // Calculate dot product (difference of angle from collision normal)
    if (dot < 0.0) {                                                    // If objects velocity if goings towards collision point, reflect it
        DrPointF v { velocity.x, velocity.y };                          // Convert to DrPointF for better vector math operators than cpVect
        v = v - (DrPointF(normal.x, normal.y) * 2.0 * dot);             // Reflect velocity normal across the plane of the collision normal
        velocity = cpv(v.x, v.y);                                       // Convert back to cpVect
    }

    // Set velocity to new directional Force velocity
    velocity = cpvnormalize(velocity);                                  // Normalize body velocity
    velocity.x *= this->getUseForce();                                  // Apply reflect force x to new direction vector
    velocity.y *= this->getUseForce();                                  // Apply reflect force y to new direction vector
    cpBodySetVelocity(this->body, velocity);                            // Set body to new velocity
}




























