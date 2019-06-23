//
//      Created by Stephens Nunnally on 5/31/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "engine_object.h"

//######################################################################################################
//##    Constructor / Destructor
//######################################################################################################
DrEngineObject::DrEngineObject() {

}

DrEngineObject::~DrEngineObject() {

}

//######################################################################################################
//##    Update Functions
//######################################################################################################
void DrEngineObject::updateBodyPosition(QPointF updated_position, bool update_previous_position_also) {
    m_previous_position = update_previous_position_also ? updated_position : m_position;
    m_position = updated_position;
}

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


















