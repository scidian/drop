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
#include "engine/engine_signal.h"
#include "engine/mesh/engine_mesh.h"
#include "engine/thing_component/thing_comp_foliage.h"
#include "engine/thing/engine_thing_object.h"
#include "engine/thing/engine_thing.h"
#include "engine/world/engine_world.h"


//####################################################################################
//##    Constructor / Destructor
//####################################################################################
ThingCompFoliage::ThingCompFoliage(DrEngineWorld *engine_world, DrEngineObject *parent_object, double springiness)
    : DrThingComponent(engine_world, parent_object, Comps::Thing_Soft_Body), m_springiness(springiness) {

    double   scale_x =  static_cast<double>(parent_object->getScaleX());
    double   scale_y =  static_cast<double>(parent_object->getScaleY());
    double   rotation = parent_object->getAngle();
    DrPointF position = parent_object->getPosition();
    DrPointF size  =    parent_object->getSize();
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
    cpConstraint *pivot = cpPivotJointNew(parent_object->body, m_anchor_bottom, bottom);
    cpSpaceAddConstraint( engine_world->getSpace(), pivot );

    // Spring joint to middle body
    double max_slide = (abs(size.y / 4.0) * (abs(size.y) / abs(size.x)));
    if (max_slide > abs(size.y / 3.0)) max_slide = abs(size.y / 3.0);           // Slide distance of slide joint
    double force = 3000.0 + (cpBodyGetMass(parent_object->body) * 100.0);       // Force of spring, increases with additional mass
    cpConstraint *slide_joint_1 =   cpSlideJointNew(parent_object->body,   m_anchor_middle, cpv(0, 0), cpv(0, 0), 0.0, max_slide);
    cpConstraint *damped_spring_1 = cpDampedSpringNew(parent_object->body, m_anchor_middle, cpv(0, 0), cpv(0, 0), 0.0, force, 100.0);
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

// Called during DrEngineWorld->updateWorld() step
void ThingCompFoliage::update(double time_passed, double time_warp) {
    (void) time_passed;
    (void) time_warp;

}

// Called when component is destroyed
void ThingCompFoliage::destroy() {

}



















