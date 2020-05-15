//
//      Created by Stephens Nunnally on 2/28/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "3rd_party/delaunator.h"
#include "3rd_party/chipmunk/chipmunk.h"
#include "core/dr_debug.h"
#include "core/dr_random.h"
#include "engine/engine.h"
#include "engine/engine_message.h"
#include "engine/mesh/engine_mesh.h"
#include "engine/thing/components/thing_comp_foliage.h"
#include "engine/thing/components/thing_comp_physics.h"
#include "engine/thing/engine_thing.h"
#include "engine/world/engine_world.h"


//####################################################################################
//##    Constructor / Destructor
//####################################################################################
ThingCompFoliage::ThingCompFoliage(DrEngineWorld *engine_world, DrEngineThing *parent_thing, double springiness)
    : DrThingComponent(engine_world, parent_thing, Comps::Thing_Foliage), m_springiness(springiness) {

    if (parent_thing == nullptr) return;
    if (parent_thing->compPhysics() == nullptr) return;
    ThingCompPhysics *physics = parent_thing->compPhysics();

    double   scale_x =  static_cast<double>(thing()->getScaleX());
    double   scale_y =  static_cast<double>(thing()->getScaleY());
    double   rotation = thing()->getAngle();
    DrPointF position = thing()->getPosition();
    DrPointF size  =    thing()->getSize();
             size.x *= scale_x;
             size.y *= scale_y;

    // Create foliage anchor point bodies and add them to space
    DrPointF bottom_pos(position.x, position.y - (size.y / 2.0));
             bottom_pos = Dr::RotatePointAroundOrigin(bottom_pos, position, rotation);
    cpVect   bottom = cpv(bottom_pos.x, bottom_pos.y);
    cpVect   middle = cpv(position.x, position.y);
    m_anchor_bottom = cpBodyNewKinematic();
    cpBodySetPosition( m_anchor_bottom, bottom);
    cpBodySetUserData( m_anchor_bottom, nullptr);
    cpSpaceAddBody(engine_world->getSpace(), m_anchor_bottom);

    m_anchor_middle = cpBodyNewKinematic();
    cpBodySetPosition( m_anchor_middle, middle);
    cpBodySetUserData( m_anchor_middle, nullptr);
    cpSpaceAddBody(engine_world->getSpace(), m_anchor_middle);

    // Pivot joint anchor to bottom body
    cpConstraint *pivot = cpPivotJointNew(physics->body, m_anchor_bottom, bottom);
    cpSpaceAddConstraint( engine_world->getSpace(), pivot );

    // Spring joint to middle body
    double max_slide = (abs(size.y / 4.0) * (abs(size.y) / abs(size.x)));
    if (max_slide > abs(size.y / 3.0)) max_slide = abs(size.y / 3.0);               // Slide distance of slide joint
    double force = 3000.0 + (cpBodyGetMass(physics->body) * 100.0);                 // Force of spring, increases with additional mass
    cpConstraint *slide_joint_1 =   cpSlideJointNew(physics->body,   m_anchor_middle, cpv(0, 0), cpv(0, 0), 0.0, max_slide);
    cpConstraint *damped_spring_1 = cpDampedSpringNew(physics->body, m_anchor_middle, cpv(0, 0), cpv(0, 0), 0.0, force, 100.0);
    cpSpaceAddConstraint( engine_world->getSpace(), slide_joint_1 );
    cpSpaceAddConstraint( engine_world->getSpace(), damped_spring_1 );
}

ThingCompFoliage::~ThingCompFoliage() {
    // Delete foliage cpBodies and all cpShapes / cpConstraints (joints) associated with them
    m_anchor_bottom = world()->removeBody(m_anchor_bottom);
    m_anchor_middle = world()->removeBody(m_anchor_middle);
}



//####################################################################################
//##    Basic Virtual Component Functions
//####################################################################################
// Called when component is first created
void ThingCompFoliage::init() {

}

// Called when Thing is added to m_things DrEngineWorld vector
void ThingCompFoliage::addToWorld() {

}

// Called when it is time to Render Thing
void ThingCompFoliage::draw() {

}

// Called during DrEngineWorld->updateWorld() step, returns true if parent DrEngineThing should be removed
bool ThingCompFoliage::update(double time_passed, double time_warp) {
    (void) time_passed;
    (void) time_warp;

    // If message exists with this Foliage that is started colliding with something, apply collision force
    for (auto &message : messageList(Messages::ThingCollide, thing()->getKey())) {
        DrEngineThing *thing_a = thing();
        DrEngineThing *thing_b = message->thingB();
        ThingCompPhysics *physics_a = thing_a->physics();
        ThingCompPhysics *physics_b = thing_b->physics();
        Collision_Info info = boost::any_cast<Collision_Info>(message->value().value());

        // Only react to an objects first shape that collides (i.e. collision_count == 0)
        if (info.collision_count == 0) {
            if (physics_b->body_type != Body_Type::Static) {
                double mass =  cpBodyGetMass(physics_a->body) * 2.0;
                double force = mass * thing_a->compFoliage()->getSpringiness();
                cpVect v = cpvmult(cpBodyGetVelocity(physics_b->body), force);
                       v.x = Dr::Clamp(v.x, -(mass * 500.0), (mass * 500.0));
                       v.y = Dr::Clamp(v.y, -(mass * 500.0), (mass * 500.0));
                cpBodyApplyImpulseAtWorldPoint(physics_a->body, v, info.point_a);
                emitMessage(Messages::ThingCancelCollision, info, thing_b);
            }
        }
    }

    return false;
}

// Called when component is destroyed
void ThingCompFoliage::destroy() {

}



















