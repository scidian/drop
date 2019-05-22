//
//      Created by Stephens Nunnally on 5/10/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "engine.h"
#include "engine_texture.h"


//######################################################################################################
//##    Add Player to Space
//######################################################################################################
void DrEngine::addPlayer(Demo_Player new_player_type) {

    demo_player = new_player_type;

    if (demo_player == Demo_Player::Spawn) {
        m_gravity = cpv(0, -500);                           // cpVect is a 2D vector and cpv() is a shortcut for initializing them
        m_damping = 1;
        cpSpaceSetGravity(m_space, m_gravity);
        cpSpaceSetDamping(m_space, m_damping);

        double ball_radius = m_textures[Test_Textures::Ball]->width() / 2.0;
        SceneObject *ball = this->addCircle(Body_Type::Kinematic, Test_Textures::Ball, -300,  150, 0, c_norotate, c_scale1x1, c_opaque, ball_radius, c_center,
                                            0.7, 0.5, QPointF(15, 0));
        setActiveCamera( addCamera(ball) );


    } else if (demo_player == Demo_Player::Car) {
        m_gravity = cpv(0, -1000);                          // cpVect is a 2D vector and cpv() is a shortcut for initializing them
        m_damping = .8;                                     // Kind of like air drag
        cpSpaceSetGravity(m_space, m_gravity);
        cpSpaceSetDamping(m_space, m_damping);

        // Add body
        QVector<QPointF> points;
        points.append( QPointF( -45.5,  -5.0 ));
        points.append( QPointF( -45.5,  18.0 ));
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
        SceneObject *rover = this->addPolygon(Body_Type::Dynamic, Test_Textures::Rover,  50, 75, 0, c_norotate, c_scale1x1, c_opaque,
                                              points, 0.5, 0.1, QPointF(0, 0));
        setActiveCamera( addCamera(rover) );

        // Add wheels
        double ball_radius =  m_textures[Test_Textures::Ball]->width() /  2.0;
        double wheel_radius = m_textures[Test_Textures::Wheel]->width() / 2.0;
        double spare_radius = m_textures[Test_Textures::Spare]->width() / 2.0;
        SceneObject *wheel1 = this->addCircle(Body_Type::Dynamic, Test_Textures::Wheel,  10,  45, .01, c_norotate, c_scale1x1, c_opaque,
                                              wheel_radius, c_center, 3, 0.7, QPointF(0, 0));
        SceneObject *wheel2 = this->addCircle(Body_Type::Dynamic, Test_Textures::Wheel,  50,  45, .01, c_norotate, c_scale1x1, c_opaque,
                                              wheel_radius, c_center, 3, 0.7, QPointF(0, 0));
        SceneObject *wheel3 = this->addCircle(Body_Type::Dynamic, Test_Textures::Wheel,  90,  45, .01, c_norotate, c_scale1x1, c_opaque,
                                              wheel_radius, c_center, 3, 0.7, QPointF(0, 0));
        wheel1->rotate_speed = 110;
        wheel2->rotate_speed =  60;
        wheel3->rotate_speed =  90;
        SceneObject *spare1 = this->addCircle(Body_Type::Dynamic, Test_Textures::Spare, -11,  49, .01, c_norotate, c_scale1x1, c_opaque,
                                              spare_radius, c_center, 4, 0.7, QPointF(0, 0));

        // Add Careful Cargo
        this->addCircle(Body_Type::Dynamic, Test_Textures::Ball, 30, 115, 0, c_norotate, c_scale1x1, c_opaque,
                        ball_radius, c_center, 0.7, 0, QPointF(0, 0));

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

        unsigned int all_categories = ~(static_cast<unsigned int>(0));

        cpShapeFilter filter;
        filter.group = 43;
        filter.categories = all_categories;     // CP_ALL_CATEGORIES
        filter.mask =       all_categories;     // CP_ALL_CATEGORIES
        for (auto shape : rover->shapes)  cpShapeSetFilter( shape,  filter);
        for (auto shape : wheel1->shapes) cpShapeSetFilter( shape, filter);
        for (auto shape : wheel2->shapes) cpShapeSetFilter( shape, filter);
        for (auto shape : wheel3->shapes) cpShapeSetFilter( shape, filter);
        for (auto shape : spare1->shapes) cpShapeSetFilter( shape, filter);

        // New bouncy shocks joint, Grooves a/b are relative to the car, anchor point B is on the wheel
        cpSpaceAddConstraint(m_space, cpGrooveJointNew( rover->body, wheel1->body, cpv(-40,  15), cpv(-40, -28), cpvzero));
        cpSpaceAddConstraint(m_space, cpGrooveJointNew( rover->body, wheel2->body, cpv(  0,  15), cpv(  0, -28), cpvzero));
        cpSpaceAddConstraint(m_space, cpGrooveJointNew( rover->body, wheel3->body, cpv( 40,  15), cpv( 40, -28), cpvzero));

        cpSpaceAddConstraint(m_space, cpDampedSpringNew(rover->body, wheel1->body, cpv(-40, 0), cpvzero, 50.0, 300.0, 50.0));
        cpSpaceAddConstraint(m_space, cpDampedSpringNew(rover->body, wheel2->body, cpv(  0, 0), cpvzero, 50.0, 300.0, 25.0));
        cpSpaceAddConstraint(m_space, cpDampedSpringNew(rover->body, wheel3->body, cpv( 40, 0), cpvzero, 50.0, 300.0, 40.0));

        // Old school solid pin joint
        cpSpaceAddConstraint( m_space, cpPivotJointNew(rover->body, spare1->body, cpBodyGetPosition(spare1->body)) );

        // Simple Motor Example, Applies constant speed to joint
        //cpConstraint *wheel_motor_1 = cpSimpleMotorNew(rover->body, wheel1->body, 25);
        //cpSpaceAddConstraint( m_space, wheel_motor_1);
        //cpConstraint *wheel_motor_2 = cpSimpleMotorNew(rover->body, wheel2->body, .2);
        //cpSpaceAddConstraint( m_space, wheel_motor_2);


    } else if (demo_player == Demo_Player::Jump) {
        m_gravity = cpv(0, -1000);
        m_damping = 1;
        cpSpaceSetGravity(m_space, m_gravity);
        cpSpaceSetDamping(m_space, m_damping);

        double ball_radius = m_textures[Test_Textures::Ball]->width() / 2.0;



        SceneObject *ball2 = this->addCircle(Body_Type::Dynamic, Test_Textures::Ball, 800,  50, 0, c_norotate, c_scale1x1, c_opaque,
                                             ball_radius, c_center, 1, 0.25, QPointF( 0, 0), true, true);
        assignPlayerControls(ball2, false, true, false);
        ball2->jump_count = -1;
        ball2->rotate_speed = 20;




        SceneObject *ball = this->addCircle(Body_Type::Dynamic, Test_Textures::Ball, 200,  50, 0, c_norotate, c_scale1x1, c_opaque,
                                            ball_radius, c_center, 0.25, 0.25, QPointF( 0, 0), true, false);
        assignPlayerControls(ball, true, true, true);
        ball->jump_count = 1;
        ball->air_jump = false;
        ball->wall_jump = false;


        //ball->ignore_gravity = true;
        //ball->move_speed_y = 400;


        // TEMP demo variables
        demo_jumper_1 = ball;
        demo_jumper_2 = ball2;
    }

}


// Sets up an object to be controlled as a "player", i.e. have playerUpdateVelocity function attached as a callback during cpSpaceStep
void DrEngine::assignPlayerControls(SceneObject *object, bool has_controls_now, bool add_camera, bool set_active_camera) {
    // Create camera
    if (add_camera) {
        long camera_key = addCamera(object);
        if (set_active_camera) setActiveCamera(camera_key);
    }
    object->lost_control = !has_controls_now;                               // Turn on jump / movement buttons
    cpBodySetUserData(object->body, object);                                // Set chipmunk User Data, store SceneObject for later
    cpBodySetVelocityUpdateFunc(object->body, playerUpdateVelocity);        // Assign the playerUpdate callback function
}













