//
//      Created by Stephens Nunnally on 5/10/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "engine/engine.h"
#include "engine/engine_camera.h"
#include "engine/engine_texture.h"
#include "engine/things/engine_thing_light.h"
#include "engine/things/engine_thing_object.h"
#include "engine/world/engine_world.h"
#include "project/dr_project.h"
#include "project/entities/dr_asset.h"
#include "project/entities/dr_font.h"
#include "project/entities/dr_image.h"
#include "project/entities/dr_world.h"
#include "project/entities/dr_stage.h"
#include "project/entities/dr_thing.h"


//####################################################################################
//##    Sets up an object to be controlled as a "player"
//##        i.e. have PlayerUpdateVelocity function attached as a callback during cpSpaceStep
//####################################################################################
// Sets up an object to be controlled as a "player", i.e. have PlayerUpdateVelocity function attached as a callback during cpSpaceStep
void DrEngineWorld::assignPlayerControls(DrEngineObject *object, bool has_controls_now, bool add_camera, bool set_active_camera) {
    // Create camera
    if (add_camera) {
        DrEngineCamera *camera = addCamera(object->getKey());
        if (set_active_camera) setActiveCamera( camera->getKey() );
    }
    object->setCollisionType( Collision_Type::Damage_Enemy );
    object->setKeyControls( true );
    object->setLostControl( !has_controls_now );                            // Turn on jump / movement buttons
    cpBodySetVelocityUpdateFunc(object->body, PlayerUpdateVelocity);        // Assign the PlayerUpdateVelocity callback function
}


//####################################################################################
//##    Add Player to Space
//####################################################################################
void DrEngineWorld::addPlayer(Demo_Player new_player_type) {

    demo_player = new_player_type;

    if (new_player_type == Demo_Player::Player) {

        // Find current world shown in editor, load Start Stage of that world
        DrWorld *world = m_project->findWorldFromKey(m_engine->getCurrentEditorWorld());
        DrStage *stage = world->getStageFromKey(world->getFirstStageKey());

        // Load Characters
        for (auto thing_pair : stage->getThingMap()) {
            // Grab current Thing, check if Character so we can load it
            DrThing *thing = thing_pair.second;
            if (thing->getType() != DrType::Thing) continue;
            if (thing->getThingType() != DrThingType::Character) continue;

            loadCharacterToWorld(thing);
        }


    } else if (new_player_type == Demo_Player::Jump) {
        DrEngineObject *ball1 = new DrEngineObject(this, getNextKey(), c_no_key, Body_Type::Dynamic, Asset_Textures::Ball, 200, 350, 10, c_scale1x1, 0.25, 0.5, true, false);
        ball1->addShapeCircleFromTexture(Asset_Textures::Ball);
        ball1->setDepth(30);
        addThing(ball1);

        ball1->setCameraPositionXY( DrPointF(0, 200) );
        assignPlayerControls(ball1, true, true, true);
        ball1->setJumpCount( 1 );
        ball1->setCanAirJump( false );
        ball1->setCanWallJump( true );
        ball1->setHealth( 80.0 );
        ///ball1->setDeathTouch( true );
        ball1->setMoveSpeedY( 300 );


        DrEngineObject *ball2 = new DrEngineObject(this, getNextKey(), c_no_key, Body_Type::Dynamic, Asset_Textures::Ball, 800, 350, 10, c_scale1x1, 1, 0.5);
        ball2->addShapeCircleFromTexture(Asset_Textures::Ball);
        ball2->setDepth(30);
        addThing(ball2);

        ball2->setCameraRotation( -25, -40, 0 );
        assignPlayerControls(ball2, false, true, false);
        ball2->setJumpCount( c_unlimited_jump );
        ball2->setRotateSpeedZ( 20.0 );


        // Soft Body Physics Playing Around

        std::vector<DrEngineObject*> balls;
        size_t row_size = 10;
        double ball_spacing =   30.0;
        double stiff =        1000.0;
        double damp =           10.0;
        size_t y = 0;
        for (size_t x = 0; x < row_size; x++) {
            Asset_Textures texture = Asset_Textures::Ball;
            Body_Type body_type =    Body_Type::Dynamic;
            double pos_x = (x * ball_spacing) + 400;
            double pos_y = (y * ball_spacing) + 300;
            double friction = 1.0;
            double bounce =   0.0;
            DrEngineObject *ball = new DrEngineObject(this, getNextKey(), c_no_key, body_type, texture, pos_x, pos_y, 0, DrPointF(0.5, 0.5), friction, bounce);
            ball->setTouchDrag(         true);
            ball->setTouchDragForce(    500.0);
            ball->addShapeCircleFromTexture(Asset_Textures::Ball);
            balls.push_back(ball);
            addThing( ball );

            if (x > 0) {
                cpBody *body1 = balls[x]->body;
                cpBody *body2 = balls[x-1]->body;

                // Simple Rope
                ///cpVect join_at = cpBodyGetPosition(body2);
                ///cpSpaceAddConstraint( m_space, cpPivotJointNew(  body1, body2, join_at));

                // Spring by Limiting
                cpVect join_at = cpv(pos_x, pos_y);
                       join_at = cpv(join_at.x - (ball_spacing / 2.0), join_at.y);
                cpSpaceAddConstraint( m_space, cpPivotJointNew(  body1, body2, join_at));
                //cpSpaceAddConstraint( m_space, cpRotaryLimitJointNew(body1, body2, Dr::DegreesToRadians(-35), Dr::DegreesToRadians(35)) );
                cpSpaceAddConstraint( m_space, cpDampedSpringNew(body1, body2, cpvzero, cpvzero, ball_spacing, stiff, damp) );


                //cpSpaceAddConstraint( m_space, cpSlideJointNew(  body1, body2, cpvzero, cpvzero, 20, 30));
            }
        }




    } else if (new_player_type == Demo_Player::Light) {
        DrEngineObject *ball1 = new DrEngineObject(this, getNextKey(), c_no_key, Body_Type::Dynamic, Asset_Textures::Ball, 200, 350, 0, c_scale1x1, 0.25, 0.5, true, false);
        ball1->addShapeCircleFromTexture(Asset_Textures::Ball);
        addThing(ball1);

        ball1->setCameraRotation( -15, 15, 0 );
        assignPlayerControls(ball1, true, true, true);
        ball1->setJumpCount( -1 );
        ball1->setMoveSpeedY( 300 );
        ball1->cast_shadows = false;
        ball1->setMouseRotate(true);

        if (this->getAmbientLight() > 3.0)
            this->setAmbientLight(3.0);
        this->setBackgroundColor(DrColor(32, 32, 32));
        this->setGlowZOrder(100.0);

        ball = ball1;
        light1 = new DrEngineLight(this, getNextKey(), c_no_key, 250, 325, -1, 1.0, Light_Type::Glow, DrColor(255, 255, 153), 3400, DrPointF(340,  20), 54, 50.0f, true, 15.0f, 0.00f, 0.00f);
        light2 = new DrEngineLight(this, getNextKey(), c_no_key, 250, 325,  1, 1.0, Light_Type::Glow, DrColor(255, 215, 215),  100, DrPointF(  0, 360), 60, 50.0f, true, 10.0f, 0.00f, 0.00f);
        light2->draw_shadows = false;
        addThings( { light1, light2 } );

        ///addThing(new DrEngineLight(this, getNextKey(), 1100, 350, 0, Light_Type::Glow,   DrColor(255, 87, 22),  1300, DrPointF(0, 360), 80, 50.0f, true, 50.0f, 70.0f, 300.0f, 1.0));
        ///addThing(new DrEngineLight(this, getNextKey(), 1100, 350, 0, Light_Type::Opaque, DrColor(255, 213, 79), 1350, DrPointF(0, 360), 50, 50.0f, true, 50.0f, 0.0f, 0.0f, 1.0));

    } else if (new_player_type == Demo_Player::Spawn) {
        DrEngineObject *ball1;
        ball1 = new DrEngineObject(this, getNextKey(), c_no_key, Body_Type::Kinematic, Asset_Textures::Ball, -300, 450, 0, c_scale1x1, 0.7, 0.5);
        ball1->addShapeCircleFromTexture(Asset_Textures::Ball);
        addThing(ball1);

        ball1->setOriginalVelocityX( 15 );
        ball1->setOriginalVelocityY(  0 );
        setActiveCamera( addCamera(ball1->getKey())->getKey() );


    } else if (new_player_type == Demo_Player::Car) {
        // Add body
        std::vector<DrPointF> points;
        points.push_back( DrPointF( -45.5,  -5.0 ));
        points.push_back( DrPointF( -45.5,  18.0 ));
        points.push_back( DrPointF( -54.5,  18.0 ));
        points.push_back( DrPointF( -57.0,  -5.0 ));
        points.push_back( DrPointF( -57.0, -24.0 ));
        points.push_back( DrPointF(  58.5, -24.0 ));
        points.push_back( DrPointF(  50.5,  -5.0 ));
        points.push_back( DrPointF(  48.0,  16.0 ));
        points.push_back( DrPointF(  37.0,  23.0 ));
        points.push_back( DrPointF(  31.0,  23.0 ));
        points.push_back( DrPointF(  20.0,  16.0 ));
        points.push_back( DrPointF(   5.5,  16.0 ));
        points.push_back( DrPointF(   5.5,  -5.0 ));
        DrEngineObject *rover = new DrEngineObject(this, getNextKey(), c_no_key, Body_Type::Dynamic, Asset_Textures::Rover, 50, 375, 5, c_scale1x1, 5, 0.1, true);
        rover->addShapePolygon(points);
        rover->setDepth(30);
        addThing(rover);
        ///rover->setCameraRotation( -15, 15, 0 );
        rover->setCameraRotation( -15,  -25, 0 );
        rover->setCameraPositionXY( DrPointF(0, 200) );
        setActiveCamera( addCamera(rover->getKey())->getKey() );


        // Add wheels
        DrEngineObject *wheel1 = new DrEngineObject(this, getNextKey(), c_no_key, Body_Type::Dynamic, Asset_Textures::Wheel,  10,  345, 6, c_scale1x1, 3, 0.7);
        DrEngineObject *wheel2 = new DrEngineObject(this, getNextKey(), c_no_key, Body_Type::Dynamic, Asset_Textures::Wheel,  50,  345, 6, c_scale1x1, 3, 0.7);
        DrEngineObject *wheel3 = new DrEngineObject(this, getNextKey(), c_no_key, Body_Type::Dynamic, Asset_Textures::Wheel,  90,  345, 6, c_scale1x1, 3, 0.7);
        wheel1->addShapeCircleFromTexture(Asset_Textures::Wheel);
        wheel2->addShapeCircleFromTexture(Asset_Textures::Wheel);
        wheel3->addShapeCircleFromTexture(Asset_Textures::Wheel);
        wheel1->setDepth(30);
        wheel2->setDepth(30);
        wheel3->setDepth(30);
        addThings( { wheel1, wheel2, wheel3 } );
        wheel1->setRotateSpeedZ( 110.0 );
        wheel2->setRotateSpeedZ(  60.0 );
        wheel3->setRotateSpeedZ(  90.0 );

        DrEngineObject *spare1 = new DrEngineObject(this, getNextKey(), c_no_key, Body_Type::Dynamic, Asset_Textures::Spare, -11,  349, 4, c_scale1x1, 4, 0.7);
        spare1->addShapeCircleFromTexture(Asset_Textures::Spare);
        addThing(spare1);

        // Add Careful Cargo
        DrEngineObject *cargo = new DrEngineObject(this, getNextKey(), c_no_key, Body_Type::Dynamic, Asset_Textures::Ball, 30, 415, 5, c_scale1x1, 0.7, 0);
        cargo->addShapeCircleFromTexture(Asset_Textures::Ball);
        cargo->setDepth(31);
        addThing(cargo);

        // New bouncy shocks joint, Grooves a/b are relative to the car, anchor point B is on the wheel
        cpSpaceAddConstraint(m_space, cpGrooveJointNew( rover->body, wheel1->body, cpv(-40,  15), cpv(-40, -28), cpvzero));
        cpSpaceAddConstraint(m_space, cpGrooveJointNew( rover->body, wheel2->body, cpv(  0,  15), cpv(  0, -28), cpvzero));
        cpSpaceAddConstraint(m_space, cpGrooveJointNew( rover->body, wheel3->body, cpv( 40,  15), cpv( 40, -28), cpvzero));
        cpSpaceAddConstraint(m_space, cpDampedSpringNew(rover->body, wheel1->body, cpv(-40, 0), cpvzero, 50.0, 400.0, 50.0));
        cpSpaceAddConstraint(m_space, cpDampedSpringNew(rover->body, wheel2->body, cpv(  0, 0), cpvzero, 50.0, 400.0, 25.0));
        cpSpaceAddConstraint(m_space, cpDampedSpringNew(rover->body, wheel3->body, cpv( 40, 0), cpvzero, 50.0, 400.0, 40.0));

        // Old school solid pin joint
        ///cpSpaceAddConstraint( m_space, cpPivotJointNew(wheel1->body, rover->body, cpBodyGetPosition(wheel1->body)) );
        ///cpSpaceAddConstraint( m_space, cpPivotJointNew(wheel2->body, rover->body, cpBodyGetPosition(wheel2->body)) );
        ///cpSpaceAddConstraint( m_space, cpPivotJointNew(wheel3->body, rover->body, cpBodyGetPosition(wheel3->body)) );
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
    }
}















