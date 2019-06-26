//
//      Created by Stephens Nunnally on 5/31/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QtMath>

#include "engine.h"
#include "engine_thing_object.h"


//######################################################################################################
//##    Chipmunk Callbacks
//######################################################################################################
// Used for constraint iterator to get a list of all constraints attached to a body
static void GetBodyJointList(cpBody *, cpConstraint *constraint, QVector<cpConstraint*> *joint_list) { joint_list->append(constraint); }
// Used for shape iterator to get a list of all shapes attached to a body
static void GetBodyShapeList(cpBody *, cpShape *shape, QVector<cpShape*> *shape_list) { shape_list->append(shape); }


//######################################################################################################
//##    Constructor / Destructor
//######################################################################################################
DrEngineObject::DrEngineObject(long unique_key) : DrEngineThing(unique_key) {

}

DrEngineObject::~DrEngineObject() {
    should_process = false;

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

//######################################################################################################
//##    Update Functions
//######################################################################################################
void DrEngineObject::updateBodyPosition(QPointF updated_position, bool update_previous_position_also) {
    m_previous_position = update_previous_position_also ? updated_position : getPosition();
    setPosition( updated_position );
}

bool DrEngineObject::update(double , double , QRectF &area) {
    if (!should_process) return false;
    bool remove = false;

    // ***** Get some info about the current object from the space and save it to the current DrEngineObject
    cpVect  new_position = cpBodyGetPosition( body );
    updateBodyPosition( QPointF( new_position.x, new_position.y ));
    updateBodyAngle( qRadiansToDegrees( cpBodyGetAngle( body )) );

    // **** Check that any object with custom PlayerUpdateVelocity callback is awake so it can access key / button events
    bool sleeping = cpBodyIsSleeping(body);
    if (hasKeyControls() && !hasLostControl() && sleeping) {
        cpBodyActivate(body);
    }

//    // ***** Update global friction and bounce to all objects if globals have changed (possibly due to Gameplay Action)
//    if (qFuzzyCompare(object->getCustomFriction(), c_friction) == false) {
//        for (auto shape : object->shapes) {
//            cpFloat friction = cpShapeGetFriction( shape );
//            if (qFuzzyCompare(friction, m_friction) == false) cpShapeSetFriction( shape, m_friction );
//        }
//    }
//    if (qFuzzyCompare(object->getCustomBounce(), c_bounce) == false) {
//        for (auto shape : object->shapes) {
//            cpFloat bounce = cpShapeGetElasticity( shape );
//            if (qFuzzyCompare(bounce, m_bounce) == false) cpShapeSetElasticity( shape, m_bounce );
//        }
//    }

    // ***** Process non-static object movement
    if (body_type != Body_Type::Static) {
        // If has rotate speed (wheels, etc.), apply gas pedal
        switch (g_pedal) {
            case Pedal::None:               break;
            case Pedal::Clockwise:          cpBodySetAngularVelocity( body, -getRotateSpeed() );    break;
            case Pedal::CounterClockwise:   cpBodySetAngularVelocity( body,  getRotateSpeed() );    break;
            case Pedal::Brake:              cpBodySetAngularVelocity( body,  0 );                   break;
        }
    }

    // ***** Auto Damage
    if (getHealth() > c_epsilon) {
        if (getAutoDamage() < -c_epsilon || getAutoDamage() > c_epsilon) {
            takeDamage( getAutoDamage() * (time_since_last_update / 1000.0), false );
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
            if (Dr::MillisecondsElapsed(getFadeTimer()) >= getFadeDelay()) {
                remove = true;
            }
        }
    }

    // Delete object if ends up outside the deletion threshold
    if (!area.contains(QPointF(new_position.x, new_position.y))) remove = true;

    return remove;
};


//######################################################################################################
//##    Collision Type of Object
//######################################################################################################
void DrEngineObject::setCollisionType(Collision_Type what_should_collide) {
    m_collision_type = what_should_collide;
    for (auto shape : shapes) {
        cpShapeSetCollisionType(shape, static_cast<cpCollisionType>(what_should_collide));
    }
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


//######################################################################################################
//##    Take Damage / Heal
//######################################################################################################
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


















