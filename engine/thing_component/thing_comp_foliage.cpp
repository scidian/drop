//
//      Created by Stephens Nunnally on 2/28/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "3rd_party/delaunator.h"
#include "3rd_party/chipmunk/chipmunk.h"
#include "core/dr_random.h"
#include "engine/engine.h"
#include "engine/engine_signal.h"
#include "engine/mesh/engine_mesh.h"
#include "engine/thing_component/thing_comp_foliage.h"
#include "engine/thing/engine_thing_object.h"
#include "engine/thing/engine_thing.h"
#include "engine/world/engine_world.h"


//####################################################################################
//##    Constructor / Destructor
//####################################################################################
ThingCompFoliage::ThingCompFoliage(DrEngineWorld *engine_world, DrEngineObject *parent_object)
    : DrEngineComponent(engine_world, parent_object, Comps::Thing_Soft_Body) {

    m_anchor_bottom = cpBodyNewKinematic();
    m_anchor_middle = cpBodyNewKinematic();
    cpVect bottom = cpv(parent_object->getPosition().x, parent_object->getPosition().y - 70);
    cpVect middle = cpv(parent_object->getPosition().x, parent_object->getPosition().y);
    //cpSpaceAddBody(engine_world->getSpace(), m_anchor_bottom );
    //cpSpaceAddBody(engine_world->getSpace(), m_anchor_middle );
    cpBodySetPosition( m_anchor_bottom, bottom);    cpBodySetUserData( m_anchor_bottom, nullptr);
    cpBodySetPosition( m_anchor_middle, middle);    cpBodySetUserData( m_anchor_middle, nullptr);

    // Pivot joint anchor to bottom body
    cpConstraint *pivot = cpPivotJointNew(parent_object->body, m_anchor_bottom, bottom);
    cpSpaceAddConstraint( engine_world->getSpace(), pivot );

    // Spring joint to middle body
    cpConstraint *slide_joint_1 =   cpSlideJointNew(parent_object->body,   m_anchor_middle, cpv(0, 0), cpv(0, 0), 0.0, 25.0);
    cpConstraint *damped_spring_1 = cpDampedSpringNew(parent_object->body, m_anchor_middle, cpv(0, 0), cpv(0, 0), 0.0, 4000.0, 100.0);
    cpSpaceAddConstraint( engine_world->getSpace(), slide_joint_1 );
    cpSpaceAddConstraint( engine_world->getSpace(), damped_spring_1 );

    //cpBodySetVelocityUpdateFunc(anchor, KinematicUpdateVelocity);
    //cpBodySetVelocityUpdateFunc(anchor, ObjectUpdateVelocity);
}

ThingCompFoliage::~ThingCompFoliage() {

}



//####################################################################################
//##    Basic Virtual Component Functions
//####################################################################################
// Called when component is first created
void ThingCompFoliage::init() {

}

// Called during DrEngineWorld->updateWorld() step
void ThingCompFoliage::update(double time_passed, double time_warp) {
    (void) time_passed;
    (void) time_warp;

}

// Called when component is destroyed
void ThingCompFoliage::destroy() {

}



















