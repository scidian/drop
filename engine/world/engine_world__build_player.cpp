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
#include "engine/thing/components/thing_comp_3d.h"
#include "engine/thing/components/thing_comp_camera.h"
#include "engine/thing/components/thing_comp_physics.h"
#include "engine/thing/components/thing_comp_player.h"
#include "engine/thing/components_effects/thing_comp_light.h"
#include "engine/thing/engine_thing.h"
#include "engine/world/engine_world.h"
#include "project/dr_project.h"
#include "project/entities/dr_font.h"
#include "project/entities/dr_image.h"
#include "project/entities/dr_stage.h"
#include "project/entities/dr_thing.h"
#include "project/entities/dr_world.h"
#include "project/entities_physics_2d/dr_asset.h"


//####################################################################################
//##    Sets up an object to be controlled as a "player"
//##        i.e. have PlayerUpdateVelocity function attached as a callback during cpSpaceStep
//####################################################################################
// Sets up an object to be controlled as a "player", i.e. have PlayerUpdateVelocity function attached as a callback during cpSpaceStep
void DrEngineWorld::assignPlayerControls(DrEngineThing *engine_thing, bool has_controls_now, bool set_active_camera) {
    ThingCompPhysics *physics = engine_thing->physics();
    if (physics == nullptr) return;

    // Add Player Components
    if (engine_thing->compCamera() == nullptr) engine_thing->setComponentCamera(new ThingCompCamera(this, engine_thing));
    if (engine_thing->compPlayer() == nullptr) engine_thing->setComponentPlayer(new ThingCompPlayer(this, engine_thing));

    // Create Camera
    DrEngineCamera *camera = addCamera(engine_thing->getKey());
    if (set_active_camera) setActiveCamera( camera->getKey() );

    // Make Player Changes
    physics->setCollisionType( Collision_Type::Damage_Enemy );
    physics->setKeyControls( true );
    physics->setLostControl( !has_controls_now );                           // Turn on jump / movement buttons
    cpBodySetVelocityUpdateFunc(physics->body, PlayerUpdateVelocity);       // Assign the PlayerUpdateVelocity callback function
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
        for (auto &thing_pair : stage->getThingMap()) {
            // Grab current Thing, check if Character so we can load it
            DrThing *thing = thing_pair.second;
            if (thing->getType() != DrType::Thing) continue;
            if (thing->getThingType() != DrThingType::Character) continue;

            loadCharacterToWorld(thing);
        }


    } else if (new_player_type == Demo_Player::Jump) {
        DrEngineThing *ball1 = new DrEngineThing(this, getNextKey(), c_no_key);
        ball1->setComponentPhysics(new ThingCompPhysics(this, ball1, Body_Type::Dynamic, Asset_Textures::Ball, -150, 100, 10, DrPointF(1.5,1.5), 0.25, 0.5, true, false));
        ball1->physics()->addShapeCircleFromTexture(Asset_Textures::Ball);
        addThing(ball1);
        assignPlayerControls(ball1, true, true);
        ball1->comp3D()->setDepth(30);
        ball1->compCamera()->setCameraPositionXY( DrPointF(0, 50) );
        ball1->compPlayer()->setJumpCount( 1 );
        ball1->compPlayer()->setCanAirJump( false );
        ball1->compPlayer()->setCanWallJump( true );
        ball1->physics()->setHealth( 80.0 );
        ///ball1->setDeathTouch( true );
        ball1->compPlayer()->setMoveSpeedY( 300 );
        ball1->physics()->setTouchDrag(true);

        DrEngineThing *ball2 = new DrEngineThing(this, getNextKey(), c_no_key);
        ball2->setComponentPhysics(new ThingCompPhysics(this, ball2, Body_Type::Dynamic, Asset_Textures::Ball, -250, 100, 10, DrPointF(1.5,1.5), 1, 0.5));
        ball2->physics()->addShapeCircleFromTexture(Asset_Textures::Ball);
        addThing(ball2);
        assignPlayerControls(ball2, false, false);
        ball2->comp3D()->setDepth(30);
        ball2->compCamera()->setCameraRotation( -25, -40, 0 );
        ball2->compPlayer()->setJumpCount( c_unlimited_jump );
        ball2->physics()->setRotateSpeedZ( 20.0 );
        ball2->physics()->setTouchDrag(true);

        DrAsset *dragon = m_project->findAssetFromKey(1024);
        if (dragon != nullptr) {
            DrEngineThing *softy = addSoftBodyCircle(c_no_key, dragon->getIdleAnimationFirstFrameImageKey(), 0, 100, 0,
                                                     DrPointF(dragon->getWidth(), dragon->getHeight()), c_scale1x1, 0.8, 0.50, 0.25, false);
            if (softy != nullptr) {
                assignPlayerControls(softy, false, false);
                if (getCamerasFollowThing(softy->getKey()).size() > 0)
                    getCamerasFollowThing(softy->getKey())[0]->setLag(DrPointF(200, 200));
                softy->compPlayer()->setMoveSpeedX(800);
                softy->compPlayer()->setJumpForceY(350);
                softy->compPlayer()->setJumpTimeout(5000);
                softy->compPlayer()->setJumpCount( 1 );
                softy->compPlayer()->setCanWallJump(true);
                softy->compPlayer()->setCanWallJump(false);
                softy->physics()->setRotateSpeedZ( 7 );
                softy->physics()->setTouchDrag(true);
            }

            DrEngineThing *softy2 = addSoftBodyMesh(c_no_key, dragon->getIdleAnimationFirstFrameImageKey(), 200, 100, 0,
                                                    DrPointF(dragon->getWidth(), dragon->getHeight()), c_scale1x1, 0.8, 0.50, 0.25, false);
            if (softy2 != nullptr) {
                assignPlayerControls(softy2, false, false);
                if (getCamerasFollowThing(softy2->getKey()).size() > 0)
                    getCamerasFollowThing(softy2->getKey())[0]->setLag(DrPointF(200, 200));
                softy2->compPlayer()->setMoveSpeedX(800);
                ///softy2->compPlayer()->setJumpForceY(500);
                softy2->compPlayer()->setJumpTimeout(5000);
                softy2->compPlayer()->setJumpCount( -1 );
                softy2->compPlayer()->setCanWallJump(false);
                softy2->physics()->setRotateSpeedZ( 7 );
                softy2->physics()->setTouchDrag(true);
            }
        }

    } else if (new_player_type == Demo_Player::Light) {
        DrEngineThing *ball1 = new DrEngineThing(this, getNextKey(), c_no_key);
        ball1->setComponentPhysics(new ThingCompPhysics(this, ball1, Body_Type::Dynamic, Asset_Textures::Ball, 200, 350, 0, c_scale1x1, 0.25, 0.5, true, false));
        ball1->physics()->addShapeCircleFromTexture(Asset_Textures::Ball);
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
        light1 = new DrEngineThing(this, getNextKey(), c_no_key, 250, 325, -1, c_scale1x1, 0.0, 1.0, DrPointF(3400, 3400));
        light2 = new DrEngineThing(this, getNextKey(), c_no_key, 250, 325,  1, c_scale1x1, 0.0, 1.0, DrPointF( 100,  100));

        light1->setComponent(Comps::Thing_Settings_Light,
                             new ThingCompLight(this, light1, Light_Type::Glow, DrColor(255, 255, 153), 3400, DrPointF(340,  20), 54, 50.0f, true, 15.0f, 0.00f, 0.00f));
        light2->setComponent(Comps::Thing_Settings_Light,
                             new ThingCompLight(this, light2, Light_Type::Glow, DrColor(255, 215, 215),  100, DrPointF(  0, 360), 60, 50.0f, true, 10.0f, 0.00f, 0.00f));
        ///light2->draw_shadows = false;
        addThings( { light1, light2 } );

        ///addThing(new DrEngineLight(this, getNextKey(), 1100, 350, 0, Light_Type::Glow,   DrColor(255, 87, 22),  1300, DrPointF(0, 360), 80, 50.0f, true, 50.0f, 70.0f, 300.0f, 1.0));
        ///addThing(new DrEngineLight(this, getNextKey(), 1100, 350, 0, Light_Type::Opaque, DrColor(255, 213, 79), 1350, DrPointF(0, 360), 50, 50.0f, true, 50.0f, 0.0f, 0.0f, 1.0));

    } else if (new_player_type == Demo_Player::Spawn) {
        DrEngineThing *ball1;
        ball1 = new DrEngineThing(this, getNextKey(), c_no_key);
        ball1->setComponentPhysics(new ThingCompPhysics(this, ball1, Body_Type::Kinematic, Asset_Textures::Ball, -300, 450, 0, c_scale1x1, 0.7, 0.5));
        ball1->physics()->addShapeCircleFromTexture(Asset_Textures::Ball);
        addThing(ball1);

        ball1->physics()->setOriginalVelocityX( 15 );
        ball1->physics()->setOriginalVelocityY(  0 );

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
        DrEngineThing *rover = new DrEngineThing(this, getNextKey(), c_no_key);
        rover->setComponentPhysics(new ThingCompPhysics(this, rover, Body_Type::Dynamic, Asset_Textures::Rover, 50, 375, 5, c_scale1x1, 5, 0.1, true));
        rover->physics()->addShapePolygon(points);
        rover->comp3D()->setDepth(30);
        addThing(rover);
        rover->setComponentCamera(new ThingCompCamera(this, rover));
        ///rover->setCameraRotation( -15, 15, 0 );
        rover->compCamera()->setCameraRotation( -15,  -25, 0 );
        rover->compCamera()->setCameraPositionXY( DrPointF(0, 50) );
        setActiveCamera( addCamera(rover->getKey())->getKey() );


        // Add wheels
        DrEngineThing *wheel1 = new DrEngineThing(this, getNextKey(), c_no_key);
        DrEngineThing *wheel2 = new DrEngineThing(this, getNextKey(), c_no_key);
        DrEngineThing *wheel3 = new DrEngineThing(this, getNextKey(), c_no_key);
        wheel1->setComponentPhysics(new ThingCompPhysics(this, wheel1, Body_Type::Dynamic, Asset_Textures::Wheel,  10,  345, 6, c_scale1x1, 3, 0.7));
        wheel2->setComponentPhysics(new ThingCompPhysics(this, wheel2, Body_Type::Dynamic, Asset_Textures::Wheel,  50,  345, 6, c_scale1x1, 3, 0.7));
        wheel3->setComponentPhysics(new ThingCompPhysics(this, wheel3, Body_Type::Dynamic, Asset_Textures::Wheel,  90,  345, 6, c_scale1x1, 3, 0.7));
        wheel1->physics()->addShapeCircleFromTexture(Asset_Textures::Wheel);
        wheel2->physics()->addShapeCircleFromTexture(Asset_Textures::Wheel);
        wheel3->physics()->addShapeCircleFromTexture(Asset_Textures::Wheel);
        wheel1->comp3D()->setDepth(30);
        wheel2->comp3D()->setDepth(30);
        wheel3->comp3D()->setDepth(30);
        addThings( { wheel1, wheel2, wheel3 } );
        wheel1->physics()->setRotateSpeedZ( 110.0 );
        wheel2->physics()->setRotateSpeedZ(  60.0 );
        wheel3->physics()->setRotateSpeedZ(  90.0 );

        DrEngineThing *spare1 = new DrEngineThing(this, getNextKey(), c_no_key);
        spare1->setComponentPhysics(new ThingCompPhysics(this, spare1, Body_Type::Dynamic, Asset_Textures::Spare, -11,  349, 4, c_scale1x1, 4, 0.7));
        spare1->physics()->addShapeCircleFromTexture(Asset_Textures::Spare);
        addThing(spare1);

        // Add Careful Cargo
        DrEngineThing *cargo = new DrEngineThing(this, getNextKey(), c_no_key);
        cargo->setComponentPhysics(new ThingCompPhysics(this, cargo, Body_Type::Dynamic, Asset_Textures::Ball, 30, 415, 5, c_scale1x1, 0.7, 0));
        cargo->physics()->addShapeCircleFromTexture(Asset_Textures::Ball);
        cargo->comp3D()->setDepth(31);
        addThing(cargo);

        // New bouncy shocks joint, Grooves a/b are relative to the car, anchor point B is on the wheel
        cpSpaceAddConstraint(m_space, cpGrooveJointNew( rover->physics()->body, wheel1->physics()->body, cpv(-40,  15), cpv(-40, -28), cpvzero));
        cpSpaceAddConstraint(m_space, cpGrooveJointNew( rover->physics()->body, wheel2->physics()->body, cpv(  0,  15), cpv(  0, -28), cpvzero));
        cpSpaceAddConstraint(m_space, cpGrooveJointNew( rover->physics()->body, wheel3->physics()->body, cpv( 40,  15), cpv( 40, -28), cpvzero));
        cpSpaceAddConstraint(m_space, cpDampedSpringNew(rover->physics()->body, wheel1->physics()->body, cpv(-40, 0), cpvzero, 50.0, 400.0, 50.0));
        cpSpaceAddConstraint(m_space, cpDampedSpringNew(rover->physics()->body, wheel2->physics()->body, cpv(  0, 0), cpvzero, 50.0, 400.0, 25.0));
        cpSpaceAddConstraint(m_space, cpDampedSpringNew(rover->physics()->body, wheel3->physics()->body, cpv( 40, 0), cpvzero, 50.0, 400.0, 40.0));

        // Old school solid pin joint
        ///cpSpaceAddConstraint( m_space, cpPivotJointNew(wheel1->body, rover->body, cpBodyGetPosition(wheel1->body)) );
        ///cpSpaceAddConstraint( m_space, cpPivotJointNew(wheel2->body, rover->body, cpBodyGetPosition(wheel2->body)) );
        ///cpSpaceAddConstraint( m_space, cpPivotJointNew(wheel3->body, rover->body, cpBodyGetPosition(wheel3->body)) );
        cpSpaceAddConstraint( m_space, cpPivotJointNew(rover->physics()->body, spare1->physics()->body, cpBodyGetPosition(spare1->physics()->body)) );

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















