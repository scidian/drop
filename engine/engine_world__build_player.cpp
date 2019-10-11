//
//      Created by Stephens Nunnally on 5/10/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "engine.h"
#include "engine_camera.h"
#include "engine_texture.h"
#include "engine_things/engine_thing_light.h"
#include "engine_things/engine_thing_object.h"
#include "engine_world.h"
#include "project/project.h"
#include "project/project_asset.h"
#include "project/project_font.h"
#include "project/project_image.h"
#include "project/project_world.h"
#include "project/project_world_stage.h"
#include "project/project_world_stage_thing.h"


//####################################################################################
//##    Sets up an object to be controlled as a "player"
//##        i.e. have PlayerUpdateVelocity function attached as a callback during cpSpaceStep
//####################################################################################
// Sets up an object to be controlled as a "player", i.e. have PlayerUpdateVelocity function attached as a callback during cpSpaceStep
void DrEngineWorld::assignPlayerControls(DrEngineObject *object, bool has_controls_now, bool add_camera, bool set_active_camera) {
    // Create camera
    if (add_camera) {
        long camera_key = addCamera(object->getKey());
        if (set_active_camera) setActiveCamera(camera_key);
    }
    object->setCollisionType( Collision_Type::Damage_Enemy );
    object->setKeyControls( true);
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
        DrWorld *world = m_project->getWorld(m_engine->getCurrentEditorWorld());
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
        DrEngineObject *ball1 = new DrEngineObject(this, getNextKey(), Body_Type::Dynamic, Asset_Textures::Ball, 200, 50, 0, c_scale1x1, 0.25, 0.5, true, false);
        ball1->addShapeCircleFromTexture(Asset_Textures::Ball);
        addThing(ball1);

        assignPlayerControls(ball1, true, true, true);
        ball1->setJumpCount( 1 );
        ball1->setCanAirJump( false );
        ball1->setCanWallJump( true );
        ball1->setHealth( 80.0 );
        ///ball1->setDeathTouch( true );
        ///ball1->setIgnoreGravity( true );
        ball1->setMoveSpeedY( 300 );


        DrEngineObject *ball2 = new DrEngineObject(this, getNextKey(), Body_Type::Dynamic, Asset_Textures::Ball, 800, 50, 0, c_scale1x1, 1, 0.5);
        ball2->addShapeCircleFromTexture(Asset_Textures::Ball);
        addThing(ball2);

        assignPlayerControls(ball2, false, true, false);
        ball2->setJumpCount( c_unlimited_jump );
        ball2->setRotateSpeed( 20.0 );
        m_cameras[ball2->getActiveCameraKey()]->setRotation( -25, -40, 0 );


    } else if (new_player_type == Demo_Player::Light) {
        DrEngineObject *ball1 = new DrEngineObject(this, getNextKey(), Body_Type::Dynamic, Asset_Textures::Ball, 200, 50, 0, c_scale1x1, 0.25, 0.5, true, false);
        ball1->addShapeCircleFromTexture(Asset_Textures::Ball);
        addThing(ball1);

        assignPlayerControls(ball1, true, true, true);
        ball1->setJumpCount( -1 );
        ball1->setMoveSpeedY( 300 );
        ball1->cast_shadows = false;

        if (this->getAmbientLight() > 3.0)
            this->setAmbientLight(3.0);
        this->setBackgroundColor(QColor(32, 32, 32));
        this->setGlowZOrder(100.0);

        ball = ball1;
        light1 = new DrEngineLight(this, getNextKey(), 250, 25, -100, 1.0, Light_Type::Glow, QColor(255, 255, 153), 3400, DrPointF(340,  20), 54, 50.0f, true, 15.0f, 0.00f, 0.00f);
        light2 = new DrEngineLight(this, getNextKey(), 250, 25, -100, 1.0, Light_Type::Glow, QColor(255, 215, 215),  100, DrPointF(  0, 360), 60, 50.0f, true, 10.0f, 0.00f, 0.00f);
        light2->draw_shadows = false;
        addThings( { light1, light2 } );

        ///addThing(new DrEngineLight(this, getNextKey(), 1100, 350, 0, Light_Type::Glow,   QColor(255, 87, 22),  1300, DrPointF(0, 360), 80, 50.0f, true, 50.0f, 70.0f, 300.0f, 1.0));
        ///addThing(new DrEngineLight(this, getNextKey(), 1100, 350, 0, Light_Type::Opaque, QColor(255, 213, 79), 1350, DrPointF(0, 360), 50, 50.0f, true, 50.0f, 0.0f, 0.0f, 1.0));

    } else if (new_player_type == Demo_Player::Spawn) {
        DrEngineObject *ball1;
        ball1 = new DrEngineObject(this, getNextKey(), Body_Type::Kinematic, Asset_Textures::Ball, -300, 150, 0, c_scale1x1, 0.7, 0.5);
        ball1->addShapeCircleFromTexture(Asset_Textures::Ball);
        addThing(ball1);

        ball1->setOriginalVelocityX( 15 );
        ball1->setOriginalVelocityY(  0 );
        cpBodySetVelocityUpdateFunc(ball1->body, KinematicUpdateVelocity);
        setActiveCamera( addCamera(ball1->getKey()) );


    } else if (new_player_type == Demo_Player::Car) {
        // Add body
        QVector<DrPointF> points;
        points.append( DrPointF( -45.5,  -5.0 ));
        points.append( DrPointF( -45.5,  18.0 ));
        points.append( DrPointF( -54.5,  18.0 ));
        points.append( DrPointF( -57.0,  -5.0 ));
        points.append( DrPointF( -57.0, -24.0 ));
        points.append( DrPointF(  58.5, -24.0 ));
        points.append( DrPointF(  50.5,  -5.0 ));
        points.append( DrPointF(  48.0,  16.0 ));
        points.append( DrPointF(  37.0,  23.0 ));
        points.append( DrPointF(  31.0,  23.0 ));
        points.append( DrPointF(  20.0,  16.0 ));
        points.append( DrPointF(   5.5,  16.0 ));
        points.append( DrPointF(   5.5,  -5.0 ));
        DrEngineObject *rover = new DrEngineObject(this, getNextKey(), Body_Type::Dynamic, Asset_Textures::Rover, 50, 75, 0, c_scale1x1, 5, 0.1, true);
        rover->addShapePolygon(points);
        addThing(rover);
        setActiveCamera( addCamera(rover->getKey()) );


        // Add wheels
        DrEngineObject *wheel1 = new DrEngineObject(this, getNextKey(), Body_Type::Dynamic, Asset_Textures::Wheel,  10,  45, .01, c_scale1x1, 3, 0.7);
        DrEngineObject *wheel2 = new DrEngineObject(this, getNextKey(), Body_Type::Dynamic, Asset_Textures::Wheel,  50,  45, .01, c_scale1x1, 3, 0.7);
        DrEngineObject *wheel3 = new DrEngineObject(this, getNextKey(), Body_Type::Dynamic, Asset_Textures::Wheel,  90,  45, .01, c_scale1x1, 3, 0.7);
        wheel1->addShapeCircleFromTexture(Asset_Textures::Wheel);
        wheel2->addShapeCircleFromTexture(Asset_Textures::Wheel);
        wheel3->addShapeCircleFromTexture(Asset_Textures::Wheel);
        addThings( { wheel1, wheel2, wheel3 } );
        wheel1->setRotateSpeed( 110.0 );
        wheel2->setRotateSpeed(  60.0 );
        wheel3->setRotateSpeed(  90.0 );

        DrEngineObject *spare1 = new DrEngineObject(this, getNextKey(), Body_Type::Dynamic, Asset_Textures::Spare, -11,  49, .01, c_scale1x1, 4, 0.7);
        spare1->addShapeCircleFromTexture(Asset_Textures::Spare);
        addThing(spare1);

        // Add Careful Cargo
        DrEngineObject *cargo = new DrEngineObject(this, getNextKey(), Body_Type::Dynamic, Asset_Textures::Ball, 30, 115, 0, c_scale1x1, 0.7, 0);
        cargo->addShapeCircleFromTexture(Asset_Textures::Ball);
        addThing(cargo);

        // New bouncy shocks joint, Grooves a/b are relative to the car, anchor point B is on the wheel
        cpSpaceAddConstraint(m_space, cpGrooveJointNew( rover->body, wheel1->body, cpv(-40,  15), cpv(-40, -28), cpvzero));
        cpSpaceAddConstraint(m_space, cpGrooveJointNew( rover->body, wheel2->body, cpv(  0,  15), cpv(  0, -28), cpvzero));
        cpSpaceAddConstraint(m_space, cpGrooveJointNew( rover->body, wheel3->body, cpv( 40,  15), cpv( 40, -28), cpvzero));
        cpSpaceAddConstraint(m_space, cpDampedSpringNew(rover->body, wheel1->body, cpv(-40, 0), cpvzero, 50.0, 300.0, 50.0));
        cpSpaceAddConstraint(m_space, cpDampedSpringNew(rover->body, wheel2->body, cpv(  0, 0), cpvzero, 50.0, 300.0, 25.0));
        cpSpaceAddConstraint(m_space, cpDampedSpringNew(rover->body, wheel3->body, cpv( 40, 0), cpvzero, 50.0, 300.0, 40.0));

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














