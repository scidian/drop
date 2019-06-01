//
//      Created by Stephens Nunnally on 5/31/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//

#include "engine_object.h"





//######################################################################################################
//##    Set Collision Type of Object
//######################################################################################################
void DrEngineObject::setCollisionType(Collision_Type what_should_collide) {
    m_collision_type = what_should_collide;
    for (auto shape : shapes) {
        cpShapeSetCollisionType(shape, static_cast<cpCollisionType>(what_should_collide));
    }
}

bool DrEngineObject::shouldDamage(Collision_Type check_damage) {
    if ((m_collision_type == Collision_Type::Damage_Enemy) && (check_damage == Collision_Type::Damage_Player || check_damage == Collision_Type::Damage_All))
        return true;
    if ((m_collision_type == Collision_Type::Damage_Player) && (check_damage == Collision_Type::Damage_Enemy  || check_damage == Collision_Type::Damage_All))
        return true;
    if (m_collision_type == Collision_Type::Damage_All)
        return true;
    return false;
}


















