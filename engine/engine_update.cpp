//
//      Created by Stephens Nunnally on 5/6/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QtMath>
#include "engine/engine.h"
#include "library.h"

#define JUMP_HEIGHT 50.0
#define JUMP_BOOST_HEIGHT 100.0
#define GRAVITY 1000.0

//######################################################################################################
//##    Update Space
//######################################################################################################
void DrEngine::updateSpace(double time_passed) {

    // ***** Updates physics
    cpFloat step_time = time_passed / 1000 * m_time_warp;
    cpSpaceStep(m_space, step_time); ///m_time_step);


    // ********** Iterate through objects, delete if they go off screen
    for ( auto it = objects.begin(); it != objects.end(); ) {
        SceneObject *object = *it;

        // ***** Skip object if statc or no linger in Space
        if ((object->body_type == Body_Type::Static) || (object->in_scene == false)) {
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


        // ***** Process Button Presses
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
            // !!!!! Need to implement a smooth following camera
            setCameraPos( static_cast<float>(pos.x), static_cast<float>(pos.y), getCameraPos().z() );
        }



        // ***** If the jump key was just pressed this frame, jump!
        remaining_boost -= step_time;
        bool jump_state = keyboard_y > 0.0;
        if (jump_state && !last_jump_state && grounded) {
            cpFloat jump_v = cpfsqrt(2.0 * JUMP_HEIGHT * GRAVITY);
            cpVect  player_v = cpBodyGetVelocity( player_body );
            player_v = cpvadd(player_v, cpv(0.0, jump_v));
            cpBodySetVelocity( player_body, player_v );
            remaining_boost = JUMP_BOOST_HEIGHT / jump_v;
        }
        last_jump_state = jump_state;





        // ***** !!!!! TEMP: Delete object if it falls below y = 1000
        if (pos.y < -1000) {
            object->in_scene = false;

            cpSpaceRemoveShape( m_space, object->shape );   cpSpaceRemoveBody( m_space, object->body );
            cpShapeFree( object->shape );                   cpBodyFree( object->body );

            it = objects.erase(it);
        } else {
            it++;
        }
    }


}













