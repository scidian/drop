//
//      Created by Stephens Nunnally on 11/12/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QtMath>

#include "engine/engine.h"
#include "engine/engine_texture.h"
#include "engine/engine_world.h"
#include "engine_things/engine_thing_object.h"


//####################################################################################
//##    Updates - Override for DrEngineThing::update()
//####################################################################################
void DrEngineObject::updateBodyPosition(DrPointF updated_position, bool update_previous_position_also) {
    if (update_previous_position_also) {
        m_previous_position = Vec3(static_cast<float>(updated_position.x), static_cast<float>(updated_position.y), static_cast<float>(getZOrder()));
    } else {
        m_previous_position = Vec3(static_cast<float>(getPosition().x), static_cast<float>(getPosition().y), static_cast<float>(getZOrder()));
    }

    setPosition( updated_position );
}

bool DrEngineObject::update(double time_passed, double time_warp, QRectF &area) {
    bool remove = false;

    // ***** Get some info about the current object from the space and save it to the current DrEngineObject
    cpVect new_position = cpBodyGetPosition( body );
    cpVect new_velocity = cpBodyGetVelocity( body );
    updateBodyPosition( DrPointF( new_position.x, new_position.y ));
    setAngle( Dr::RadiansToDegrees( cpBodyGetAngle( body )) );
    setVelocityX( new_velocity.x );
    setVelocityY( new_velocity.y );

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
                case Pedal::Clockwise:          cpBodySetAngularVelocity( body, -getRotateSpeedZ() );    break;
                case Pedal::CounterClockwise:   cpBodySetAngularVelocity( body,  getRotateSpeedZ() );    break;
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













