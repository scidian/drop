//
//      Created by Stephens Nunnally on 4/22/2020, (c) 2020 Scidian Software, All Rights Reserved
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
#include "engine/engine_texture.h"
#include "engine/mesh/engine_mesh.h"
#include "engine/thing/components/thing_comp_3d.h"
#include "engine/thing/components/thing_comp_physics.h"
#include "engine/thing/components/thing_comp_player.h"
#include "engine/thing/components/thing_comp_soft_body.h"
#include "engine/thing/engine_thing.h"
#include "engine/world/engine_world.h"
#include "project/dr_project.h"
#include "project/entities/dr_animation.h"
#include "project/entities_physics_2d/dr_asset.h"


//####################################################################################
//##    Constructor / Destructor
//####################################################################################
ThingCompPhysics::ThingCompPhysics(DrEngineWorld *engine_world, DrEngineThing *parent_thing,
                                   Body_Type body_type, long asset_key,
                                   double x, double y, double z, DrPointF scale, double friction, double bounce,
                                   bool should_collide, bool can_rotate, double angle, float opacity)
    : DrThingComponent(engine_world, parent_thing, Comps::Thing_Physics) {

    // Initialize Components
    if (thing()->comp3D() == nullptr) thing()->setComponent3D(new ThingComp3D(world(), thing()));

    // Initialize variables
    polygons.clear();

    // Thing Basics
    thing()->setOpacity(opacity);
    thing()->setScaleX(scale.x);                                           // Save x scale for later
    thing()->setScaleY(scale.y);                                           // Save y scale for later
    thing()->setZOrder(z);

    // Object Basics                 !!!!! #NOTE: texture_number == Asset Key
    long image_number = asset_key;
    if (asset_key <= 0) {
        image_number = asset_key;
    } else if (asset_key > 0) {
        DrAsset *asset = world()->getProject()->findAssetFromKey(asset_key);
        if (asset != nullptr) {
            image_number = asset->getIdleAnimationFirstFrameImageKey();
            DrAnimation *animation = world()->getProject()->findAnimationFromKey(asset->getBaseKey());
            if (animation != nullptr) {
                for (auto frame : animation->getFrames()) {
                    animation_idle_keys.push_back(frame->getKey());
                }
            }
        }
    }

    DrEngineTexture *texture = world()->getTexture(image_number);
    if (texture != nullptr) {
        thing()->setSize( DrPointF(texture->width(), texture->height()));
    } else {
        thing()->setSize( DrPointF(100, 100) );
    }
    this->setTextureNumber(image_number);                                       // Texture to render from
    this->updateBodyPosition( DrPointF(x, y), true );

    this->setCustomFriction(friction);
    this->setCustomBounce(bounce);
    this->setDoesCollide(should_collide);
    this->setCanRotate(can_rotate);

    // Create Body
    this->body_type = body_type;
    switch (body_type) {
        ///case Body_Type::Static:  this->body = cpSpaceGetStaticBody(getWorld()->getSpace());      break;
        case Body_Type::Static:     this->body = cpBodyNewStatic();                                 break;
        case Body_Type::Dynamic:    this->body = cpBodyNew( 0.0, 0.0 );                             break;
        case Body_Type::Kinematic:  this->body = cpBodyNewKinematic();                              break;
    }
    cpBodySetPosition( this->body, cpv(x, y) );
    thing()->setAngle( -angle );
    cpBodySetUserData( this->body, thing() );                                   // Set chipmunk User Data, store DrEngineThing* for use later

    if (body_type == Body_Type::Kinematic) {
        cpBodySetVelocityUpdateFunc(this->body, KinematicUpdateVelocity);
    } else if (body_type == Body_Type::Dynamic) {
        cpBodySetVelocityUpdateFunc(this->body, ObjectUpdateVelocity);
    }
}

ThingCompPhysics::~ThingCompPhysics() {
    // ***** Signal physics children to remove themselves
    if (thing()->compSoftBody() != nullptr) {
        for (auto &child : thing()->compSoftBody()->soft_balls) {
            if (child != nullptr) {
                if (child->compPhysics() != nullptr) {
                    child->compPhysics()->setPhysicsParent(nullptr);
                    child->remove();
                }
                child = nullptr;
            }
        }
    }

    // ***** Tell physics parent to remove itself
    if (isPhysicsChild()) {
        if (getPhysicsParent() != nullptr) {
            if (getPhysicsParent()->compSoftBody() != nullptr) {
                for (auto &child : getPhysicsParent()->compSoftBody()->soft_balls) {
                    if (child != nullptr) {
                        if (child->getKey() == thing()->getKey()) child = nullptr;
                    }
                }
            }
            getPhysicsParent()->remove();
        }
        setPhysicsParent(nullptr);
    }

    // ***** Delete cpBody and all cpShapes / cpConstraints (joints) associated with it
    this->body = world()->removeBody(this->body);
}



//####################################################################################
//##    Basic Virtual Component Functions
//####################################################################################
// Called when component is first created
void ThingCompPhysics::init() {

}

// Called when Thing is added to m_things DrEngineWorld vector
void ThingCompPhysics::addToWorld() {
    // Add Body and Shapes to Space
    cpSpaceAddBody(world()->getSpace(), this->body);
    for (auto shape : shapes) {
        cpSpaceAddShape( world()->getSpace(), shape );
    }

    // If we don't want the body to rotate, overwrite the precalculated moment of inertia with infinity
    if (this->canRotate() == false) {
        cpBodySetMoment( this->body, static_cast<double>(INFINITY) );
    }

    // Set initial velocity to forced velocity
    if (thing()->compPlayer() != nullptr) {
        cpVect forced_velocity = cpv( thing()->compPlayer()->getForcedSpeedX(), thing()->compPlayer()->getForcedSpeedY() );
        cpBodySetVelocity( body, forced_velocity );
    }

    // Reset Damage Clock
    Dr::ResetTimer(  m_damage_timer );
    Dr::AdjustTimer( m_damage_timer, 0, 0, 0, -static_cast<int>(m_damage_delay));

    // Reset Animation Timers
    Dr::ResetTimer( m_death_timer );
    Dr::ResetTimer( m_fade_timer );
}

// ***** Collide moved to: thing_comp_physics__collide.cpp
///bool ThingCompPhysics::collide(Collision_Step step, cpArbiter *arb, DrEngineThing *collided_with) { }

// Called when it is time to Render Thing
void ThingCompPhysics::draw() {

}

// ***** Update moved to: thing_comp_physics__update.cpp
///bool ThingCompPhysics::update(double time_passed, double time_warp) { }

// Called when component is destroyed
void ThingCompPhysics::destroy() {

}




//####################################################################################
//##    Collision Tracking with other DrEngineThing physics objects
//####################################################################################
long ThingCompPhysics::checkCollisionCountWithObject(DrEngineThing *object) {
    if (Dr::MapHasKey(this->m_colliding_bodies, object->getKey()) == false) {
        return 0;
    } else {
        return this->m_colliding_bodies[object->getKey()];
    }
}

void ThingCompPhysics::increaseCollisionCountWithObject(DrEngineThing *object) {
    if (Dr::MapHasKey(this->m_colliding_bodies, object->getKey()) == false) {
        this->m_colliding_bodies[object->getKey()] = 1;
    } else {
        this->m_colliding_bodies[object->getKey()]++;
    }
}

void ThingCompPhysics::decreaseCollisionCountWithObject(DrEngineThing *object) {
    if (Dr::MapHasKey(this->m_colliding_bodies, object->getKey()) == false) {
        return;
    } else {
        this->m_colliding_bodies[object->getKey()]--;
        if (this->m_colliding_bodies[object->getKey()] == 0) {
            this->m_colliding_bodies.erase(object->getKey());
        }
    }
}

// Returns a list of all Physics objects (DrEngineThings w/compPhysics) in DrEngineWorld that this object is currently colliding with (including sensor shapes)
std::vector<long> ThingCompPhysics::listOfCollidingObjectKeys() {
    std::vector<long> colliding_keys { };
    for (auto it = this->m_colliding_bodies.begin(); it != this->m_colliding_bodies.end(); ++it) {
        colliding_keys.push_back(it->first);
    }
    return colliding_keys;
}



//####################################################################################
//##    Misc Getters / Setters
//####################################################################################
const double& ThingCompPhysics::getDamageRecoil() {
    return (getPhysicsParent() == nullptr) ? m_damage_recoil : getPhysicsParent()->compPhysics()->getDamageRecoil();
}



//####################################################################################
//##    Collision Type of Object
//####################################################################################
void ThingCompPhysics::setCollisionType(Collision_Type what_should_collide) {
    m_collision_type = what_should_collide;
    for (auto shape : shapes)
        cpShapeSetCollisionType(shape, static_cast<cpCollisionType>(what_should_collide));
}

// Sets Surface Velocity
void ThingCompPhysics::setSurfaceVelocity(cpVect surface_vel) {
    m_surface_velocity = surface_vel;
    for (auto shape : shapes) {
        cpShapeSetSurfaceVelocity(shape, surface_vel);
    }
}

bool ThingCompPhysics::doesDamage() {
    return (m_damage > c_epsilon || m_damage < -c_epsilon || m_death_touch);
}

bool ThingCompPhysics::shouldDamage(Collision_Type check_damage) {
    if (m_collision_type == Collision_Type::Damage_Enemy  && (check_damage == Collision_Type::Damage_Player || check_damage == Collision_Type::Damage_All))
        return true;
    if (m_collision_type == Collision_Type::Damage_Player && (check_damage == Collision_Type::Damage_Enemy  || check_damage == Collision_Type::Damage_All))
        return true;
    if (m_collision_type == Collision_Type::Damage_All)
        return true;
    return false;
}

// Returns true if two objects should collide
bool ThingCompPhysics::shouldCollide(ThingCompPhysics *with_object_physics) {
    ThingCompPhysics *physics = with_object_physics;
    if (physics == nullptr) return false;
    switch (this->getCollidesWith()) {
        case Collision_Groups::None:                return false;
        case Collision_Groups::Active_Players_Only: return (physics->hasKeyControls() && (physics->hasLostControl() == false));
        case Collision_Groups::Players:             return (physics->hasKeyControls() || (physics->getCollisionType() == Collision_Type::Damage_Enemy));
        case Collision_Groups::Enemies:             return (physics->getCollisionType() == Collision_Type::Damage_Player);
        default: ;
    }
    return true;
}


//####################################################################################
//##    Take Damage / Heal,
//##        - If force_death is true, object dies no matter what
//##        - Returns
//##            TRUE:  just killed this object / object dead,
//##            FALSE: did not kill this object / object alive
//####################################################################################
bool ThingCompPhysics::takeDamage(double damage_to_take, bool reset_damage_timer, bool death_touch, bool force_death, long damaged_by_key) {
    // If dying or dead exit now
    if (!m_alive || m_dying) return true;
    if (force_death) {
        setHealth(0.0, damaged_by_key);
        return true;
    }

    // Check if unlimited health
    bool unlimited_health = (getHealth() < 0.0);

    // HEALING: If damage was negative (healing), add health, check max_health and exit
    if (damage_to_take < 0.0 && !unlimited_health) {
        double new_health = getHealth() + abs(damage_to_take);
        if (new_health > m_max_health && m_max_health > c_epsilon) new_health = m_max_health;
        setHealth(new_health, damaged_by_key);
        if (reset_damage_timer) Dr::ResetTimer(m_damage_timer);
        return false;
    }

    // INVINCIBLE: If can't be hurt exit
    if (m_invincible)
        return false;
    else if (reset_damage_timer)
        Dr::ResetTimer(m_damage_timer);

    // TAKE DAMAGE: If not unlimited health, apply damage, if killed exit
    if (!unlimited_health) {
        double new_health = getHealth() - damage_to_take;
        if (new_health <= c_epsilon) new_health = 0.0;
        setHealth(new_health, damaged_by_key);
        if (getHealth() <= c_epsilon) {
            return true;
        }
    }

    // DEATH TOUCH: If object was death touched, kill it and exit
    if (death_touch) {
        setHealth(0.0, damaged_by_key);
        return true;
    }

    return false;
}


//####################################################################################
//##    Sets Health of Object
//####################################################################################
void ThingCompPhysics::setHealth(double new_health, long damaged_by_key) {
    double health_change = m_health - new_health;
    m_health = new_health;
    if (isPhysicsChild() == false) {
        DrEngineThing *damaged_by = world()->findThingByKey(damaged_by_key);
        if      (Dr::FuzzyCompare(m_health,      0.0) == true)  emitSignal(Signals::ThingDied,    m_health,      damaged_by);
        else if (Dr::FuzzyCompare(health_change, 0.0) == false) emitSignal(Signals::ThingDamaged, health_change, damaged_by);
    }
}


//####################################################################################
//##
//##    Control Setters, Make sure we also pass to soft body Physics Children
//##
//####################################################################################
void ThingCompPhysics::setTouchDrag(bool touch_drag) {
    m_touch_drag = touch_drag;
    updatePhysicsChildren();
}
void ThingCompPhysics::setTouchDragForce(double drag_force) {
    m_touch_drag_force = drag_force;
    updatePhysicsChildren();
}
void ThingCompPhysics::setTouchDamage(bool touch_damage) {
    m_touch_damage = touch_damage;
    updatePhysicsChildren();
}
void ThingCompPhysics::setTouchDamagePoints(double damage) {
    m_touch_damage_points = damage;
    updatePhysicsChildren();
}

// Apply settings to physics children
void ThingCompPhysics::updatePhysicsChildren() {
    if (thing()->compSoftBody() != nullptr) {
        for (size_t i = 0; i < thing()->compSoftBody()->soft_balls.size(); ++i) {
            DrEngineThing *next_ball = thing()->compSoftBody()->soft_balls[i];
            if (next_ball == nullptr) continue;
            if (next_ball == thing()) continue;
            if (next_ball->compPhysics() == nullptr) continue;
            next_ball->compPhysics()->setRotateSpeedZ(      m_rotate_speed);
            next_ball->compPhysics()->setTouchDrag(         m_touch_drag);
            next_ball->compPhysics()->setTouchDragForce(    m_touch_drag_force);
            next_ball->compPhysics()->setTouchDamage(       m_touch_damage);
            next_ball->compPhysics()->setTouchDamagePoints( m_touch_damage_points);
        }
    }
}















