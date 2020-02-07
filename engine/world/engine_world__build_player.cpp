//
//      Created by Stephens Nunnally on 5/10/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "core/dr_math.h"
#include "core/dr_random.h"
#include "engine/engine.h"
#include "engine/engine_camera.h"
#include "engine/engine_texture.h"
#include "engine/thing/engine_thing_light.h"
#include "engine/thing/engine_thing_object.h"
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
void DrEngineWorld::assignPlayerControls(DrEngineObject *object, bool has_controls_now, bool set_active_camera) {
    // Add Player Components
    if (object->compCamera() == nullptr) object->setComponentCamera(new ThingCompCamera(this, object));
    if (object->compPlayer() == nullptr) object->setComponentPlayer(new ThingCompPlayer(this, object));

    // Create Camera
    DrEngineCamera *camera = addCamera(object->getKey());
    if (set_active_camera) setActiveCamera( camera->getKey() );

    // Make Player Changes
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
        DrEngineObject *ball1 = new DrEngineObject(this, getNextKey(), c_no_key, Body_Type::Dynamic, Asset_Textures::Ball, -150, 100, 10, DrPointF(1.5,1.5), 0.25, 0.5, true, false);
        ball1->addShapeCircleFromTexture(Asset_Textures::Ball);
        addThing(ball1);
        assignPlayerControls(ball1, true, true);
        ball1->comp3D()->setDepth(30);
        ball1->compCamera()->setCameraPositionXY( DrPointF(0, 50) );
        ball1->compPlayer()->setJumpCount( 1 );
        ball1->compPlayer()->setCanAirJump( false );
        ball1->compPlayer()->setCanWallJump( true );
        ball1->setHealth( 80.0 );
        ///ball1->setDeathTouch( true );
        ball1->compPlayer()->setMoveSpeedY( 300 );
        ball1->setTouchDrag(true);
        ball1->setTouchDragForce(2000.0);

//        DrEngineObject *ball2 = new DrEngineObject(this, getNextKey(), c_no_key, Body_Type::Dynamic, Asset_Textures::Ball, -250, 100, 10, DrPointF(1.5,1.5), 1, 0.5);
//        ball2->addShapeCircleFromTexture(Asset_Textures::Ball);
//        ball2->setDepth(30);
//        addThing(ball2);
//        ball2->setCameraRotation( -25, -40, 0 );
//        assignPlayerControls(ball2, false, false);
//        ball2->setJumpCount( c_unlimited_jump );
//        ball2->setRotateSpeedZ( 20.0 );
//        ball2->setTouchDrag(true);
//        ball2->setTouchDragForce(2000.0);

        DrAsset *dragon = m_project->findAssetFromKey(1024);
        if (dragon != nullptr) {
            DrEngineObject *softy = addSoftBodyCircle(dragon->getIdleAnimationFirstFrameImageKey(), DrPointF(0, 100), 150, 0.8, 0.50, 0.25, false);
            if (softy != nullptr) {
                assignPlayerControls(softy, true, true);
                if (getCamerasFollowThing(softy->getKey()).size() > 0)
                    getCamerasFollowThing(softy->getKey())[0]->setLag(DrPointF(200, 200));
                softy->compPlayer()->setMoveSpeedX(800);
                softy->compPlayer()->setJumpForceY(350);
                softy->compPlayer()->setJumpTimeout(5000);
                softy->compPlayer()->setJumpCount( 1 );
                softy->compPlayer()->setCanWallJump(true);
                softy->compPlayer()->setCanWallJump(false);
                softy->setRotateSpeedZ( 7 );
            }

            DrEngineObject *softy2 = addSoftBodyMesh(dragon->getIdleAnimationFirstFrameImageKey(), DrPointF(200, 100), c_scale1x1, 0.8, 0.50, 0.25, false);
            if (softy2 != nullptr) {
                assignPlayerControls(softy2, false, false);
                if (getCamerasFollowThing(softy2->getKey()).size() > 0)
                    getCamerasFollowThing(softy2->getKey())[0]->setLag(DrPointF(200, 200));
                softy2->compPlayer()->setMoveSpeedX(800);
                ///softy2->compPlayer()->setJumpForceY(500);
                softy2->compPlayer()->setJumpTimeout(5000);
                softy2->compPlayer()->setJumpCount( -1 );
                softy2->compPlayer()->setCanWallJump(false);
                softy2->setRotateSpeedZ( 7 );
            }

//            DrEngineObject *softy3 = addSoftBodyCircle(dragon->getIdleAnimationFirstFrameImageKey(), DrPointF(-200, 250), 400, 0.8, 0.50, 0.25, false);
//            if (softy3 != nullptr) {
//                assignPlayerControls(softy3, false, false);
//                if (getCamerasFollowThing(softy3->getKey()).size() > 0)
//                    getCamerasFollowThing(softy3->getKey())[0]->setLag(DrPointF(200, 200));
//                softy3->compPlayer()->setMoveSpeedX(800);
//                ///softy3->compPlayer()->setJumpForceY(500);
//                softy3->compPlayer()->setJumpTimeout(5000);
//                softy3->compPlayer()->setRotateSpeedZ( 7 );
//                softy3->compPlayer()->setJumpCount( -1 );
//                softy3->compPlayer()->setCanWallJump(false);
//            }
        }

    } else if (new_player_type == Demo_Player::Light) {
        DrEngineObject *ball1 = new DrEngineObject(this, getNextKey(), c_no_key, Body_Type::Dynamic, Asset_Textures::Ball, 200, 350, 0, c_scale1x1, 0.25, 0.5, true, false);
        ball1->addShapeCircleFromTexture(Asset_Textures::Ball);
        addThing(ball1);

        assignPlayerControls(ball1, true, true);
        ball1->compCamera()->setCameraRotation( -15, 15, 0 );
        ball1->compPlayer()->setJumpCount( -1 );
        ball1->compPlayer()->setMoveSpeedY( 300 );
        ball1->compPlayer()->setMouseRotate(true);
        ball1->cast_shadows = false;

        if (this->getAmbientLight() > 3.0)
            this->setAmbientLight(3.0);
        this->setBackgroundColor(DrColor(32, 32, 32));
        this->setGlowZOrder(100.0);

        ball = ball1;
        light1 = new DrEngineLight(this, getNextKey(), c_no_key, 250, 325, -1, 1.0, Light_Type::Glow, DrColor(255, 255, 153), 3400, DrPointF(340,  20), 54, 50.0f, true, 15.0f, 0.00f, 0.00f);
        light2 = new DrEngineLight(this, getNextKey(), c_no_key, 250, 325,  1, 1.0, Light_Type::Glow, DrColor(255, 215, 215),  100, DrPointF(  0, 360), 60, 50.0f, true, 10.0f, 0.00f, 0.00f);
        ///light2->draw_shadows = false;
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
        rover->comp3D()->setDepth(30);
        addThing(rover);
        rover->setComponentCamera(new ThingCompCamera(this, rover));
        ///rover->setCameraRotation( -15, 15, 0 );
        rover->compCamera()->setCameraRotation( -15,  -25, 0 );
        rover->compCamera()->setCameraPositionXY( DrPointF(0, 200) );
        setActiveCamera( addCamera(rover->getKey())->getKey() );


        // Add wheels
        DrEngineObject *wheel1 = new DrEngineObject(this, getNextKey(), c_no_key, Body_Type::Dynamic, Asset_Textures::Wheel,  10,  345, 6, c_scale1x1, 3, 0.7);
        DrEngineObject *wheel2 = new DrEngineObject(this, getNextKey(), c_no_key, Body_Type::Dynamic, Asset_Textures::Wheel,  50,  345, 6, c_scale1x1, 3, 0.7);
        DrEngineObject *wheel3 = new DrEngineObject(this, getNextKey(), c_no_key, Body_Type::Dynamic, Asset_Textures::Wheel,  90,  345, 6, c_scale1x1, 3, 0.7);
        wheel1->addShapeCircleFromTexture(Asset_Textures::Wheel);
        wheel2->addShapeCircleFromTexture(Asset_Textures::Wheel);
        wheel3->addShapeCircleFromTexture(Asset_Textures::Wheel);
        wheel1->comp3D()->setDepth(30);
        wheel2->comp3D()->setDepth(30);
        wheel3->comp3D()->setDepth(30);
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
        cargo->comp3D()->setDepth(31);
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

        // Set body and wheels to same group so they don't collide
        applyCategoryMask(rover,  rover->getKey());
        applyCategoryMask(wheel1, rover->getKey());
        applyCategoryMask(wheel2, rover->getKey());
        applyCategoryMask(wheel3, rover->getKey());
        applyCategoryMask(spare1, rover->getKey());
    }
}















