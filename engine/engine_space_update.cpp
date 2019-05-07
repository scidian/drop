//
//      Created by Stephens Nunnally on 5/6/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QtMath>
#include "engine/engine.h"

//######################################################################################################
//##    Update Space
//######################################################################################################
void Engine::updateSpace() {

    // ***** Updates physics
    cpSpaceStep(m_space, m_time_step);


    // ***** Iterate through objects, delete if they go off screen
    for ( auto it = objects.begin(); it != objects.end(); ) {
        SceneObject *object = *it;

        if ((object->body_type == BodyType::Static) || (object->in_scene == false)) {
            it++;
            continue;
        }

        // Gets velocity
        cpVect  vel = cpBodyGetVelocity( object->body );
        object->velocity.setX( vel.x );
        object->velocity.setX( vel.y );

        // Gets position
        cpVect  pos = cpBodyGetPosition( object->body );
        object->position.setX( pos.x );
        object->position.setY( pos.y );

        // Gets angle
        cpFloat angle = cpBodyGetAngle( object->body );
        object->angle = qRadiansToDegrees( angle );

        // If is a wheel, apply gas pedal
        if (object->is_wheel) {
            switch (gas_pedal) {
                case Pedal::Clockwise:          cpBodySetAngularVelocity( object->body, -object->wheel_speed );     break;
                case Pedal::CounterClockwise:   cpBodySetAngularVelocity( object->body,  object->wheel_speed );     break;
                case Pedal::Brake:              cpBodySetAngularVelocity( object->body,  0 );                       break;
                case Pedal::None:               break;
            }
        }

        if (object->follow == true) {
            camera_pos.setX( pos.x );
            camera_pos.setY( pos.y );
        }

        // Delete object if it falls below y = 400
        if (pos.y < -400) {
            object->in_scene = false;

            cpSpaceRemoveShape( m_space, object->shape );   cpSpaceRemoveBody( m_space, object->body );
            cpShapeFree( object->shape );                   cpBodyFree( object->body );

            it = objects.erase(it);
        } else {
            it++;
        }
    }

}













