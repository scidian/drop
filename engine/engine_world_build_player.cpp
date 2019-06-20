//
//      Created by Stephens Nunnally on 5/10/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "engine.h"
#include "engine_object.h"
#include "engine_texture.h"
#include "engine_world.h"

//######################################################################################################
//##    Sets up an object to be controlled as a "player"
//##        i.e. have PlayerUpdateVelocity function attached as a callback during cpSpaceStep
//######################################################################################################
// Sets up an object to be controlled as a "player", i.e. have PlayerUpdateVelocity function attached as a callback during cpSpaceStep
void DrEngineWorld::assignPlayerControls(DrEngineObject *object, bool has_controls_now, bool add_camera, bool set_active_camera) {
    // Create camera
    if (add_camera) {
        long camera_key = addCamera(object);
        if (set_active_camera) setActiveCamera(camera_key);
    }
    object->setCollisionType( Collision_Type::Damage_Enemy );
    object->setKeyControls( true);
    object->setLostControl( !has_controls_now );                            // Turn on jump / movement buttons
    cpBodySetVelocityUpdateFunc(object->body, PlayerUpdateVelocity);        // Assign the PlayerUpdateVelocity callback function
}


//######################################################################################################
//##    Add Player to Space
//######################################################################################################
void DrEngineWorld::addPlayer(Demo_Player new_player_type) {

    m_engine->demo_player = new_player_type;

    if (m_engine->demo_player == Demo_Player::Jump) {

        double ball_radius = m_engine->getTextureMap()[Asset_Textures::Ball]->width() / 2.0;
        DrEngineObject *ball = this->addCircle(Body_Type::Dynamic, Asset_Textures::Ball, 200,  50, 0, c_norotate, c_scale1x1, c_opaque,
                                               ball_radius, c_center, 0.25, 0.5, QPointF( 0, 0), true, false);
        assignPlayerControls(ball, true, true, true);
        ball->setJumpCount( 1 );
        ball->setCanAirJump( false );
        ball->setCanWallJump( true );
        ball->setHealth( 80.0 );

        ///ball->setDeathTouch( true );
        ///ball->setIgnoreGravity( true );

        ball->setMoveSpeedY( 300 );


        DrEngineObject *ball2 = this->addCircle(Body_Type::Dynamic, Asset_Textures::Ball, 800,  50, 0, c_norotate, c_scale1x1, c_opaque,
                                                ball_radius, c_center, 1, 0.5, QPointF( 0, 0), true, true);
        assignPlayerControls(ball2, false, true, false);
        ball2->setJumpCount( c_unlimited_jump );
        ball2->setRotateSpeed( 20.0 );

        // !!!!! #TEMP: demo variables
        m_engine->demo_jumper_1 = ball;
        m_engine->demo_jumper_2 = ball2;


    } else if (m_engine->demo_player == Demo_Player::Spawn) {
        double ball_radius = m_engine->getTextureMap()[Asset_Textures::Ball]->width() / 2.0;
        DrEngineObject *ball = this->addCircle(Body_Type::Kinematic, Asset_Textures::Ball, -300,  150, 0, c_norotate, c_scale1x1, c_opaque, ball_radius, c_center,
                                               0.7, 0.5, QPointF(15, 0));
        setActiveCamera( addCamera(ball) );


    } else if (m_engine->demo_player == Demo_Player::Car) {
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
        DrEngineObject *rover = this->addPolygon(Body_Type::Dynamic, Asset_Textures::Rover,  50, 75, 0, c_norotate, c_scale1x1, c_opaque,
                                                 points, 0.5, 0.1, QPointF(0, 0));
        setActiveCamera( addCamera(rover) );

        // Add wheels
        double ball_radius =  m_engine->getTextureMap()[Asset_Textures::Ball]->width() /  2.0;
        double wheel_radius = m_engine->getTextureMap()[Asset_Textures::Wheel]->width() / 2.0;
        double spare_radius = m_engine->getTextureMap()[Asset_Textures::Spare]->width() / 2.0;
        DrEngineObject *wheel1 = this->addCircle(Body_Type::Dynamic, Asset_Textures::Wheel,  10,  45, .01, c_norotate, c_scale1x1, c_opaque,
                                                 wheel_radius, c_center, 3, 0.7, QPointF(0, 0));
        DrEngineObject *wheel2 = this->addCircle(Body_Type::Dynamic, Asset_Textures::Wheel,  50,  45, .01, c_norotate, c_scale1x1, c_opaque,
                                                 wheel_radius, c_center, 3, 0.7, QPointF(0, 0));
        DrEngineObject *wheel3 = this->addCircle(Body_Type::Dynamic, Asset_Textures::Wheel,  90,  45, .01, c_norotate, c_scale1x1, c_opaque,
                                                 wheel_radius, c_center, 3, 0.7, QPointF(0, 0));
        wheel1->setRotateSpeed( 110.0 );
        wheel2->setRotateSpeed(  60.0 );
        wheel3->setRotateSpeed(  90.0 );
        DrEngineObject *spare1 = this->addCircle(Body_Type::Dynamic, Asset_Textures::Spare, -11,  49, .01, c_norotate, c_scale1x1, c_opaque,
                                                 spare_radius, c_center, 4, 0.7, QPointF(0, 0));

        // Add Careful Cargo
        this->addCircle(Body_Type::Dynamic, Asset_Textures::Ball, 30, 115, 0, c_norotate, c_scale1x1, c_opaque,
                        ball_radius, c_center, 0.7, 0, QPointF(0, 0));

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
        ///cpConstraint *wheel_motor_1 = cpSimpleMotorNew(rover->body, wheel1->body, 25);
        ///cpSpaceAddConstraint( m_space, wheel_motor_1);
        ///cpConstraint *wheel_motor_2 = cpSimpleMotorNew(rover->body, wheel2->body, .2);
        ///cpSpaceAddConstraint( m_space, wheel_motor_2);

        // Set body and wheels to same group so they don't collide, example:
        //
        // #define GRABBABLE_MASK_BIT (1<<31)
        // cpShapeFilter GRAB_FILTER = {CP_NO_GROUP, GRABBABLE_MASK_BIT, GRABBABLE_MASK_BIT};
        // cpShapeFilter NOT_GRABBABLE_FILTER = {CP_NO_GROUP, ~GRABBABLE_MASK_BIT, ~GRABBABLE_MASK_BIT};
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
        filter.group = 43;                      // Any int > 0, maybe use unique project id of parent? or keep a key generator when Engine starts
        filter.categories = all_categories;     // CP_ALL_CATEGORIES
        filter.mask =       all_categories;     // CP_ALL_CATEGORIES
        for (auto shape : rover->shapes)  cpShapeSetFilter( shape, filter);
        for (auto shape : wheel1->shapes) cpShapeSetFilter( shape, filter);
        for (auto shape : wheel2->shapes) cpShapeSetFilter( shape, filter);
        for (auto shape : wheel3->shapes) cpShapeSetFilter( shape, filter);
        for (auto shape : spare1->shapes) cpShapeSetFilter( shape, filter);


        // Set constrained objects not to collide with each other, unfortunately they still collide with other parts of the car
        ///cpConstraintSetCollideBodies(groove1, cpFalse);
        ///cpConstraintSetCollideBodies(groove2, cpFalse);
        ///cpConstraintSetCollideBodies(groove3, cpFalse);
        ///cpConstraintSetCollideBodies(spring1, cpFalse);
        ///cpConstraintSetCollideBodies(spring2, cpFalse);
        ///cpConstraintSetCollideBodies(spring3, cpFalse);
        ///cpConstraintSetCollideBodies(joint1,  cpFalse);
    }
}















