//
//      Created by Stephens Nunnally on 5/9/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QtMath>
#include "engine/engine.h"
#include "library.h"

#define PLAYER_VELOCITY 500.0

#define PLAYER_GROUND_ACCEL_TIME 0.1
#define PLAYER_GROUND_ACCEL (PLAYER_VELOCITY/PLAYER_GROUND_ACCEL_TIME)

#define PLAYER_AIR_ACCEL_TIME 0.25
#define PLAYER_AIR_ACCEL (PLAYER_VELOCITY/PLAYER_AIR_ACCEL_TIME)

#define JUMP_HEIGHT 50.0
#define JUMP_BOOST_HEIGHT 100.0
#define FALL_VELOCITY 900.0
#define GRAVITY 1000.0


//######################################################################################################
//##    Global Callback Functions and a pointer to active Engine for handling Player Jump
//######################################################################################################
extern DrEngine *engine { nullptr };

extern void selectPlayerGroundNormal(cpBody *, cpArbiter *arb, cpVect *ground_normal) {
    cpVect n = cpvneg( cpArbiterGetNormal(arb) );

    if (n.y > ground_normal->y) {
        (*ground_normal) = n;
    }
}

extern void playerUpdateVelocity(cpBody *body, cpVect gravity, cpFloat damping, cpFloat dt)
{
    bool jump_state = engine->keyboard_y > 0.0;

    // Grab the grounding normal from last frame
    cpVect ground_normal = cpvzero;
    cpBodyEachArbiter(engine->player_body, cpBodyArbiterIteratorFunc(selectPlayerGroundNormal), &ground_normal);

    engine->grounded = (ground_normal.y > 0.0);
    if (ground_normal.y < 0.0) engine->remaining_boost = 0.0;

    // Do a normal-ish update
    cpBool boost = (jump_state && engine->remaining_boost > 0.0);
    cpVect g = (boost ? cpvzero : gravity);
    cpBodyUpdateVelocity(body, g, damping, dt);

    // Target horizontal speed for air/ground control
    cpFloat target_vx = PLAYER_VELOCITY * -engine->keyboard_x;

    // Update the surface velocity and friction
    cpVect surface_v = cpv(target_vx, 0.0);
    cpShapeSetSurfaceVelocity( engine->player_shape, surface_v );
    cpShapeSetFriction( engine->player_shape, (engine->grounded ? PLAYER_GROUND_ACCEL / GRAVITY : 0.0));

    // Apply air control if not grounded
    if (!engine->grounded){
        // Smoothly accelerate the velocity
        cpVect body_v = cpBodyGetVelocity( engine->player_body );
        body_v.x = cpflerpconst( body_v.x, target_vx, PLAYER_AIR_ACCEL_TIME * dt);
        cpBodySetVelocity( engine->player_body, body_v );
    }

    cpVect body_v = cpBodyGetVelocity( body );
    body_v.y = cpfclamp(body_v.y, -FALL_VELOCITY, static_cast<double>(INFINITY));
    cpBodySetVelocity( body, body_v );
}










