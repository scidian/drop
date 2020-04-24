//
//      Created by Stephens Nunnally on 4/22/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "engine/engine.h"
#include "engine/engine_texture.h"
#include "engine/thing/components/thing_comp_physics.h"
#include "engine/world/engine_world.h"



//####################################################################################
//##    Updates - Override for DrEngineThing::update()
//####################################################################################
void ThingCompPhysics::updateBodyPosition(DrPointF updated_position, bool update_previous_position_also) {
    if (update_previous_position_also) {
        m_previous_position = DrVec3(static_cast<float>(updated_position.x), static_cast<float>(updated_position.y), static_cast<float>(thing()->getZOrder()));
    } else {
        m_previous_position = DrVec3(static_cast<float>(thing()->getPosition().x), static_cast<float>(thing()->getPosition().y), static_cast<float>(thing()->getZOrder()));
    }

    thing()->setPosition( updated_position );
}

// Called during DrEngineWorld->updateWorld() step, returns true if parent DrEngineThing should be removed
bool ThingCompPhysics::update(double time_passed, double time_warp) {
    (void) time_warp;

    bool remove = false;

    // ***** Get some info about the current object from the space and save it to the current DrEngineObject
    cpVect new_position = cpBodyGetPosition( body );
    cpVect new_velocity = cpBodyGetVelocity( body );
    updateBodyPosition( DrPointF( new_position.x, new_position.y ));
    thing()->setVelocityX( new_velocity.x );
    thing()->setVelocityY( new_velocity.y );


    // ***** Animation Time Updating
    animation_idle_last_change += time_passed;
    if (animation_idle_last_change > (1000.0 / animation_speed)) {
        animation_idle_frame++;
        if (animation_idle_frame > static_cast<long>(animation_idle_keys.size())) animation_idle_frame = 1;
        animation_idle_last_change = 0;
    }


    // ***** Check that any object with custom PlayerUpdateVelocity callback is awake so it can access key / button events
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

    // ***** Process non-static, non-player object movement
    if (body_type != Body_Type::Static && !hasKeyControls()) {
        // If has rotate speed (wheels, etc.), apply gas pedal
        if (Dr::IsCloseTo(0.0, getRotateSpeedZ(), 0.001) == false) {
            switch (g_pedal) {
                case Pedal::None:               break;
                case Pedal::Clockwise:          cpBodySetAngularVelocity( body, -getRotateSpeedZ() );   break;
                case Pedal::CounterClockwise:   cpBodySetAngularVelocity( body,  getRotateSpeedZ() );   break;
                case Pedal::Brake:              cpBodySetAngularVelocity( body,  0 );                   break;
            }
        }
    }

    // ***** Auto Damage
    if (getHealth() > c_epsilon) {
        if (getAutoDamage() < -c_epsilon || getAutoDamage() > c_epsilon) {
            takeDamage(getAutoDamage() * (thing()->time_since_last_update / 1000.0), false);
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

    return remove;
};








