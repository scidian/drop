//
//      Created by Stephens Nunnally on 5/10/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "engine/engine.h"
#include "engine_texture.h"

// Used for shape iterator to get a list of all shapes attached to a body
static void getShapeList(cpBody *, cpShape *shape, QVector<cpShape*> *shape_list) { shape_list->append(shape); }

//######################################################################################################
//##    Add Player to Space
//######################################################################################################
void DrEngine::addPlayer(Demo_Player new_player_type) {

    demo_player = new_player_type;

    if (demo_player == Demo_Player::Spawn) {
        m_gravity = cpv(0, -500);                           // cpVect is a 2D vector and cpv() is a shortcut for initializing them
        cpSpaceSetGravity(m_space, m_gravity);

        SceneObject *ball = this->addCircle(Body_Type::Kinematic, Test_Textures::Ball, -300,  150, 0, 1, .7, .5, 2, QPointF(15, 0));
        ball->follow = true;


    } else if (demo_player == Demo_Player::Car) {
        m_gravity = cpv(0, -500);                           // cpVect is a 2D vector and cpv() is a shortcut for initializing them
        m_damping = .7;                                     // Kind of like air drag
        cpSpaceSetGravity(m_space, m_gravity);
        cpSpaceSetDamping(m_space, m_damping);

        // Add body
        QVector<QPointF> points;
        points.append( QPointF( -45.5,  -5.0 ));
        points.append( QPointF( -45.5,  16.0 ));
        points.append( QPointF( -54.5,  18.0 ));
        points.append( QPointF( -57.0,  -5.0 ));
        points.append( QPointF( -57.0, -24.0 ));
        points.append( QPointF(  58.5, -24.0 ));
        points.append( QPointF(  50.5,  -5.0 ));
        points.append( QPointF(  48.0,  16.0 ));
        points.append( QPointF(  37.0,  23.0 ));
        points.append( QPointF(  31.0,  23.0 ));
        points.append( QPointF(  20.0,  16.0 ));
        points.append( QPointF(   5.5,  16.0 ));
        points.append( QPointF(   5.5,  -5.0 ));
        SceneObject *rover = this->addPolygon(Body_Type::Dynamic, Test_Textures::Rover,  50, 75, 0, 1, points, .5, .1, 4, QPointF(0, 0));
        ///SceneObject *rover = this->addBlock(  Body_Type::Dynamic, Test_Textures::Rover,  50, 75, 0, 0, QPointF(1, 1), 1, .5, .1, 4, QPointF(0, 0));
        rover->follow = true;

        // Add wheels
        SceneObject *wheel1 = this->addCircle(Body_Type::Dynamic, Test_Textures::Wheel,   10,  45, .01, 1, 4, .7,  2, QPointF(0, 0));
        SceneObject *wheel2 = this->addCircle(Body_Type::Dynamic, Test_Textures::Wheel,   50,  45, .01, 1, 4, .7,  2, QPointF(0, 0));
        SceneObject *wheel3 = this->addCircle(Body_Type::Dynamic, Test_Textures::Wheel,   90,  45, .01, 1, 4, .7,  2, QPointF(0, 0));
        wheel1->is_wheel = true;    wheel1->wheel_speed = 80;
        wheel2->is_wheel = true;    wheel2->wheel_speed = 40;
        wheel3->is_wheel = true;    wheel3->wheel_speed = 60;
        SceneObject *spare1 = this->addCircle(Body_Type::Dynamic, Test_Textures::Spare,  -13,  45, .01, 1, 4, .7, .5, QPointF(0, 0));

        // Add Careful Cargo
        this->addCircle(Body_Type::Dynamic, Test_Textures::Ball, 30, 115, 0, 1, .7, 0, 2, QPointF(0, 0));

        // Set body and wheels to same group so they don't collide
        //EX:
        //#define GRABBABLE_MASK_BIT (1<<31)
        //cpShapeFilter GRAB_FILTER = {CP_NO_GROUP, GRABBABLE_MASK_BIT, GRABBABLE_MASK_BIT};
        //cpShapeFilter NOT_GRABBABLE_FILTER = {CP_NO_GROUP, ~GRABBABLE_MASK_BIT, ~GRABBABLE_MASK_BIT};
        //
        // filter.group = no_group = 0 = could collide all
        //              = set all children to unique project_id to stop them from colliding with each other
        // categories   = what not to collide with  (player, player bullet, enemy, enemy bullet, wall, etc)
        // mask         = what to collide with      (walls, enemy bullet)
        //
        enum Shape_Groups {
            A = 1 << 0,
            B = 1 << 1,
            C = 1 << 2,
        };

        cpShapeFilter filter;
        filter.group = 43;
        filter.categories = CP_ALL_CATEGORIES;
        filter.mask =       CP_ALL_CATEGORIES;
        QVector<cpShape*> shape_list;
        cpBodyEachShape(rover->body, cpBodyShapeIteratorFunc(getShapeList), &shape_list);
        for (auto shape : shape_list)
            cpShapeSetFilter( shape,  filter);
        cpShapeSetFilter( wheel1->shape, filter);
        cpShapeSetFilter( wheel2->shape, filter);
        cpShapeSetFilter( wheel3->shape, filter);
        cpShapeSetFilter( spare1->shape, filter);

        // New bouncy shocks joint, Grooves a/b are relative to the car, anchor point B is on the wheel
        cpSpaceAddConstraint(m_space, cpGrooveJointNew( rover->body, wheel1->body, cpv(-40,  15), cpv(-40, -28), cpvzero));
        cpSpaceAddConstraint(m_space, cpGrooveJointNew( rover->body, wheel2->body, cpv(  0,  15), cpv(  0, -28), cpvzero));
        cpSpaceAddConstraint(m_space, cpGrooveJointNew( rover->body, wheel3->body, cpv( 40,  15), cpv( 40, -28), cpvzero));

        cpSpaceAddConstraint(m_space, cpDampedSpringNew(rover->body, wheel1->body, cpv(-40, 0), cpvzero, 50.0, 90.0, 50.0)); // Higher damp = less mushy, 100 = pretty stiff
        cpSpaceAddConstraint(m_space, cpDampedSpringNew(rover->body, wheel2->body, cpv(  0, 0), cpvzero, 50.0, 90.0, 25.0));
        cpSpaceAddConstraint(m_space, cpDampedSpringNew(rover->body, wheel3->body, cpv( 40, 0), cpvzero, 50.0, 90.0, 40.0));

        // Old school solid pin joint
        cpSpaceAddConstraint( m_space, cpPivotJointNew(rover->body, spare1->body, cpBodyGetPosition(spare1->body)) );

        // Simple Motor Example, Applies constant speed to joint
        //cpConstraint *wheel_motor_1 = cpSimpleMotorNew(rover->body, wheel1->body, 25);
        //cpSpaceAddConstraint( m_space, wheel_motor_1);
        //cpConstraint *wheel_motor_2 = cpSimpleMotorNew(rover->body, wheel2->body, .2);
        //cpSpaceAddConstraint( m_space, wheel_motor_2);


    } else if (demo_player == Demo_Player::Jump) {
        m_gravity = cpv(0, -1000);
        cpSpaceSetGravity(m_space, m_gravity);
        m_damping = 1;
        cpSpaceSetDamping(m_space, m_damping);


        SceneObject *ball = this->addCircle(Body_Type::Dynamic, Test_Textures::Ball, 0,  50, 0, 1, 2, .01, 200, QPointF( 0, 0), false);

        // Camera should follow player
        ball->follow = true;

        // Turn on jump / movement buttons
        ball->player_controls = true;

        // Set player info
        player_body =  ball->body;
        player_shape = ball->shape;
        jump_count = 2;

        // Reset jump variables for this player
        remaining_jumps = jump_count;
        remaining_boost = 0;
        grounded = cpFalse;
        jump_state = Jump_State::Jumped;

    }

}








