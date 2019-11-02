//
//      Created by Stephens Nunnally on 5/31/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QtMath>

#include "engine/engine.h"
#include "engine/engine_texture.h"
#include "engine/engine_world.h"
#include "engine_thing_object.h"


//####################################################################################
//##    Chipmunk Callbacks
//####################################################################################
// Iterators to get a list of all constraints, shapes, etc attached to a body
static void GetBodyJointList(cpBody *, cpConstraint *constraint, QVector<cpConstraint*> *joint_list) { joint_list->append(constraint); }
static void GetBodyShapeList(cpBody *, cpShape *shape, QVector<cpShape*> *shape_list) { shape_list->append(shape); }


//####################################################################################
//##    Object Constructor
//##        Pass -1 for friction and/or bounce to use default world friction and bounce settings
//####################################################################################
DrEngineObject::DrEngineObject(DrEngineWorld *world, long unique_key) : DrEngineThing (world, unique_key) { }

DrEngineObject::DrEngineObject(DrEngineWorld *world, long unique_key, Body_Type body_type, long texture_number,
                               double x, double y, double z, DrPointF scale, double friction, double bounce,
                               bool should_collide, bool can_rotate, double angle, float opacity) : DrEngineThing (world, unique_key) {
    // Thing Basics
    this->setOpacity(opacity);
    this->setScaleX(scale.x);                                                   // Save x scale for later
    this->setScaleY(scale.y);                                                   // Save y scale for later
    this->setZOrder(z);

    // Object Basics                 !!!!! #NOTE: texture_number == Asset Key
    if (texture_number != 0) {
        DrEngineTexture *texture = world->getTexture(texture_number);
        if (texture != nullptr) {
            this->setSize( DrPointF(texture->width(), texture->height()));
        } else {
            this->setSize( DrPointF(100, 100) );
        }
    }
    this->setTextureNumber(texture_number);                                     // Texture to render from
    this->updateBodyPosition( DrPointF(x, y), true );

    this->setCustomFriction(friction);
    this->setCustomBounce(bounce);
    this->setDoesCollide(should_collide);
    this->setCanRotate(can_rotate);

    // Create Body
    this->body_type = body_type;
    switch (body_type) {
        case Body_Type::Static:      this->body = cpBodyNewStatic();                                break;
        ///case Body_Type::Static:      this->body = cpSpaceGetStaticBody(getWorld()->getSpace());     break;
        case Body_Type::Dynamic:     this->body = cpBodyNew( 0.0, 0.0 );                            break;
        case Body_Type::Kinematic:   this->body = cpBodyNewKinematic();                             break;
    }
    cpBodySetPosition( this->body, cpv(x, y));
    cpBodySetAngle(    this->body, qDegreesToRadians(-angle) );
    setAngle( -angle );
    cpBodySetUserData( this->body, this);                                       // Set chipmunk User Data, store DrEngineObject* for use later
}


//####################################################################################
//##    Destructor
//####################################################################################
DrEngineObject::~DrEngineObject() {
    if (body) {
        cpSpace *space = cpBodyGetSpace(body);

        QVector<cpShape*> shape_list;
        cpBodyEachShape(body, cpBodyShapeIteratorFunc(GetBodyShapeList), &shape_list);
        for (auto shape : shape_list) {
            cpSpaceRemoveShape(space, shape);
            cpShapeFree(shape);
        }
        QVector<cpConstraint*> joint_list;
        cpBodyEachConstraint(body, cpBodyConstraintIteratorFunc(GetBodyJointList), &joint_list);
        for (auto joint : joint_list) {
            cpSpaceRemoveConstraint(space, joint);
            cpConstraintFree(joint);
        }
        cpSpaceRemoveBody(space, body);
        cpBodyFree(body);
        body = nullptr;
    }
}


//####################################################################################
//##    Override for DrEngineThing::addToWorld()
//####################################################################################
void DrEngineObject::addToWorld() {
    // Add Body and Shapes to Space
    cpSpaceAddBody(getWorld()->getSpace(), this->body);
    for (auto shape : shapes)
        cpSpaceAddShape( getWorld()->getSpace(), shape );

    // If we don't want the body to rotate, overwrite the precalculated moment of inertia with infinity
    if (this->canRotate() == false)
        cpBodySetMoment( this->body, static_cast<double>(INFINITY) );

    // Reset Update Timer
    Dr::ResetTimer( update_timer );
}


//####################################################################################
//##    Updates - Override for DrEngineThing::update()
//####################################################################################
void DrEngineObject::updateBodyPosition(DrPointF updated_position, bool update_previous_position_also) {
    m_previous_position = update_previous_position_also ? updated_position : getPosition();
    setPosition( updated_position );
}

bool DrEngineObject::update(double time_passed, double time_warp, QRectF &area) {
    bool remove = false;

    // ***** Get some info about the current object from the space and save it to the current DrEngineObject
    cpVect new_position = cpBodyGetPosition( body );
    updateBodyPosition( DrPointF( new_position.x, new_position.y ));
    setAngle( Dr::RadiansToDegrees( cpBodyGetAngle( body )) );

    // **** Check that any object with custom PlayerUpdateVelocity callback is awake so it can access key / button events
    bool sleeping = cpBodyIsSleeping(body);
    if (hasKeyControls() && !hasLostControl() && sleeping) {
        cpBodyActivate(body);
    }

//    // ***** Update global friction and bounce to all objects if globals have changed (possibly due to Gameplay Action)
//    if (Dr::FuzzyCompare(object->getCustomFriction(), c_friction) == false) {
//        for (auto shape : object->shapes) {
//            cpFloat friction = cpShapeGetFriction( shape );
//            if (Dr::FuzzyCompare(friction, m_friction) == false) cpShapeSetFriction( shape, m_friction );
//        }
//    }
//    if (Dr::FuzzyCompare(object->getCustomBounce(), c_bounce) == false) {
//        for (auto shape : object->shapes) {
//            cpFloat bounce = cpShapeGetElasticity( shape );
//            if (Dr::FuzzyCompare(bounce, m_bounce) == false) cpShapeSetElasticity( shape, m_bounce );
//        }
//    }

    // ***** Process non-static object movement
    if (body_type != Body_Type::Static) {
        // If has rotate speed (wheels, etc.), apply gas pedal
        if (Dr::IsCloseTo(0.0, getRotateSpeed(), 0.001) == false) {
            switch (g_pedal) {
                case Pedal::None:               break;
                case Pedal::Clockwise:          cpBodySetAngularVelocity( body, -getRotateSpeed() );    break;
                case Pedal::CounterClockwise:   cpBodySetAngularVelocity( body,  getRotateSpeed() );    break;
                case Pedal::Brake:              cpBodySetAngularVelocity( body,  0 );                   break;
            }
        }
    }

    // ***** Auto Damage
    if (getHealth() > c_epsilon) {
        if (getAutoDamage() < -c_epsilon || getAutoDamage() > c_epsilon) {
            takeDamage(getAutoDamage() * (time_since_last_update / 1000.0), false);
        }
    }

    // ***** Check for Object Death / Fade / Removal
    if (getHealth() <= c_epsilon && getHealth() > c_unlimited_health) {
        if (!isDying()) {
            setDying( true );
            Dr::ResetTimer(getDeathTimer());
        }
        if (isDying() && isAlive()) {
            if (Dr::MillisecondsElapsed(getDeathTimer()) >= getDeathDelay()) {
                setAlive( false );
                Dr::ResetTimer(getFadeTimer());
            }
        }
        if (!isAlive()) {
            if (Dr::MillisecondsElapsed(getFadeTimer()) >= getDeathDuration()) {
                remove = true;
            }
        }
    }


    return (remove || DrEngineThing::update(time_passed, time_warp, area));
};


//####################################################################################
//##    Collision Type of Object
//####################################################################################
void DrEngineObject::setCollisionType(Collision_Type what_should_collide) {
    m_collision_type = what_should_collide;
    for (auto shape : shapes)
        cpShapeSetCollisionType(shape, static_cast<cpCollisionType>(what_should_collide));
}

// Sets Surface Velocity
void DrEngineObject::setSurfaceVelocity(cpVect surface_vel) {
    m_surface_velocity = surface_vel;
    for (auto shape : shapes)
        cpShapeSetSurfaceVelocity(shape, surface_vel);
}

bool DrEngineObject::doesDamage() {
    return (m_damage > c_epsilon || m_damage < -c_epsilon || m_death_touch);
}

bool DrEngineObject::shouldDamage(Collision_Type check_damage) {
    if (m_collision_type == Collision_Type::Damage_Enemy  && (check_damage == Collision_Type::Damage_Player || check_damage == Collision_Type::Damage_All))
        return true;
    if (m_collision_type == Collision_Type::Damage_Player && (check_damage == Collision_Type::Damage_Enemy  || check_damage == Collision_Type::Damage_All))
        return true;
    if (m_collision_type == Collision_Type::Damage_All)
        return true;
    return false;
}


//####################################################################################
//##    Take Damage / Heal
//####################################################################################
// Returns TRUE: just killed this object / object dead, FALSE: did not kill this object / object alive
bool DrEngineObject::takeDamage(double damage_to_take, bool reset_damage_timer, bool death_touch) {
    // If dying or dead exit now
    if (!m_alive || m_dying) return true;

    // Check if unlimited health
    bool unlimited_health = (m_health < 0.0);

    // HEALING: If damage was negative (healing), add health, check max_health and exit
    if (damage_to_take < 0.0 && !unlimited_health) {
        m_health += abs(damage_to_take);
        if (reset_damage_timer) Dr::ResetTimer(m_damage_timer);
        if (m_health > m_max_health && m_max_health > c_epsilon) m_health = m_max_health;
        return false;
    }

    // INVINCIBLE: If can't be hurt exit
    if (m_invincible)
        return false;
    else if (reset_damage_timer)
        Dr::ResetTimer(m_damage_timer);

    // TAKE DAMAGE: If not unlimited health, apply damage, if killed exit
    if (!unlimited_health) {
        m_health -= damage_to_take;
        if (m_health <= c_epsilon) {
            m_health = 0.0;
            return true;
        }
    }

    // DEATH TOUCH: If object was death touched, kill it and exit
    if (death_touch) {
        m_health = 0.0;
        return true;
    }

    return false;
}


















