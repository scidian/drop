//
//      Created by Stephens Nunnally on 5/6/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QApplication>
#include <QDebug>

#include "engine.h"
#include "engine_things/engine_thing_light.h"
#include "engine_things/engine_thing_object.h"
#include "engine_texture.h"
#include "engine_world.h"
#include "form_engine.h"
#include "helper.h"
#include "opengl/opengl.h"
#include "project/project.h"
#include "project/project_asset.h"
#include "project/project_font.h"
#include "project/project_image.h"
#include "project/project_world.h"
#include "project/project_world_stage.h"
#include "project/project_world_stage_thing.h"
#include "project/thing_shape_list.h"


//####################################################################################
//##    Build Space
//####################################################################################
void DrEngineWorld::buildWorld(Demo_Space new_space_type, long world_id_to_build) {

    // ***** Set up physics world
    m_background_color = QColor(0, 0, 0);
    m_ambient_light = 50;
    demo_space = new_space_type;

    m_space = cpSpaceNew();                             // Creates an empty space
    cpSpaceSetIterations(m_space, m_iterations);        // Sets how many times physics are processed each update
    cpSpaceSetCollisionSlop(m_space, 0.5);              // Allows for a little overlap, makes sleep happier
    cpSpaceSetSleepTimeThreshold(m_space, 0.50);        // Objects will sleep after this long of not moving
    ///cpSpaceSetIdleSpeedThreshold(m_space, 25.0);     // Can set this manually, but also set automatically based on gravity

    // Default gravity / damping settings
    m_gravity = cpv(0, -1000);                          // cpVect is a 2D vector (aka 2D point) and cpv() is a shortcut for initializing them
    m_damping = 1;                                      // Kind of like air drag
    cpSpaceSetGravity(m_space, m_gravity);
    cpSpaceSetDamping(m_space, m_damping);



    // ***** Custom Wildcard beginFunc CollisionHandlers: Damage / Health    
    QVector<Collision_Type> collide_types { Collision_Type::Damage_None,
                                            Collision_Type::Damage_Player,
                                            Collision_Type::Damage_Enemy,
                                            Collision_Type::Damage_All };
    for (Collision_Type collision : collide_types) {
        cpCollisionHandler *custom_collision_handler = cpSpaceAddWildcardHandler(m_space, static_cast<cpCollisionType>(collision));
        custom_collision_handler->beginFunc =    BeginFuncWildcard;
        custom_collision_handler->preSolveFunc = PreSolveFuncWildcard;
        custom_collision_handler->separateFunc = SeperateFuncWildcard;
    }



    // ***** Build desired demo Space
    if (new_space_type == Demo_Space::Project) {

        // ***** Find current world, load Start Stage of that world
        DrWorld *world = m_project->getWorld(world_id_to_build);
        DrStage *stage = world->getStageFromKey(world->getFirstStageKey());

        // ***** World Settings
        m_game_direction += world->getComponentPropertyValue(Components::World_Settings, Properties::World_Game_Direction).toDouble();
        m_game_start = DrPointF(0, 0);      // Set starting load position
        m_loaded_to = 0;                    // Reset how far we've loaded

        if (world->getComponentPropertyValue(Components::World_Settings, Properties::World_Use_Background_Color).toBool())
            m_background_color = QColor::fromRgba(world->getComponentPropertyValue(Components::World_Settings, Properties::World_Background_Color).toUInt());

        // ***** World Physics Properties
        m_time_warp = world->getComponentPropertyValue(Components::World_Physics, Properties::World_Time_Warp).toDouble();
        QPointF get_gravity =   world->getComponentPropertyValue(Components::World_Physics, Properties::World_Gravity).toPointF();
        m_gravity = cpv(get_gravity.x(), get_gravity.y());
        m_damping =   world->getComponentPropertyValue(Components::World_Physics, Properties::World_Drag).toDouble();
        m_friction =  world->getComponentPropertyValue(Components::World_Physics, Properties::World_Friction).toDouble();
        m_bounce =    world->getComponentPropertyValue(Components::World_Physics, Properties::World_Bounce).toDouble();

        cpSpaceSetGravity(m_space, m_gravity);
        cpSpaceSetDamping(m_space, m_damping);

        // ***** World appearance settings
        m_ambient_light =       world->getComponentPropertyValue(Components::World_Lighting, Properties::World_Light_Ambient).toDouble();
        m_glow_light_z_order =  world->getComponentPropertyValue(Components::World_Lighting, Properties::World_Light_Layer).toDouble();
        m_glow_blend_mode =     world->getComponentPropertyValue(Components::World_Lighting, Properties::World_Light_Blend).toInt();

        bitrate =     world->getComponentPropertyValue(Components::World_Appearance, Properties::World_Filter_Bitrate).toList().first().toInt();
        QPointF pixelation = world->getComponentPropertyValue(Components::World_Appearance, Properties::World_Filter_Pixelation).toPointF();
        pixel_x =     static_cast<float>(pixelation.x());
        pixel_y =     static_cast<float>(pixelation.y());
        brightness =  world->getComponentPropertyValue(Components::World_Appearance, Properties::World_Filter_Brightness).toList().first().toInt() / 255.f;
        contrast =    world->getComponentPropertyValue(Components::World_Appearance, Properties::World_Filter_Contrast).toList().first().toInt() / 255.f;
        saturation =  world->getComponentPropertyValue(Components::World_Appearance, Properties::World_Filter_Saturation).toList().first().toInt() / 255.f;
        hue =         world->getComponentPropertyValue(Components::World_Appearance, Properties::World_Filter_Hue).toList().first().toInt() / 360.f;
        grayscale =   world->getComponentPropertyValue(Components::World_Appearance, Properties::World_Filter_Grayscale).toBool();
        negative =    world->getComponentPropertyValue(Components::World_Appearance, Properties::World_Filter_Negative).toBool();

        wireframe =   world->getComponentPropertyValue(Components::World_Special_Effects, Properties::World_Filter_Wireframe).toBool();
        cartoon =     world->getComponentPropertyValue(Components::World_Special_Effects, Properties::World_Filter_Cartoon).toBool();
        wavy =        world->getComponentPropertyValue(Components::World_Special_Effects, Properties::World_Filter_Wavy).toBool();

        // ***** Load Current Stage to origin position
        loadStageToWorld(stage, 0, 0);




//        // ********** Bouyancy Test
//        cpBody *staticBody = cpSpaceGetStaticBody(m_space);

//        // Add the sensor for the water
//        cpBB bb = cpBBNew(-2000, -1000, 0, -10);
//        cpShape *shape = cpSpaceAddShape(m_space, cpBoxShapeNew2(staticBody, bb, 0.0));
//        cpShapeSetSensor(shape, cpTrue);
//        cpShapeSetCollisionType(shape, 232323);     // 232323 is temp number for water

//        cpCollisionHandler *handler = cpSpaceAddCollisionHandler(m_space, 232323, 0);
//        handler->preSolveFunc = static_cast<cpCollisionPreSolveFunc>(WaterPreSolve);
//        for (Collision_Type collision : collide_types) {
//            cpCollisionHandler *handler = cpSpaceAddCollisionHandler(m_space, 232323, static_cast<cpCollisionType>(collision));
//            handler->preSolveFunc = static_cast<cpCollisionPreSolveFunc>(WaterPreSolve);
//        }




    } else if (new_space_type == Demo_Space::Lines1) {
        m_friction = 1.0;
        m_bounce =   0.8;

        // Static line segment shapes for the ground
        DrEngineObject *line1 = new DrEngineObject(this, getNextKey(), Body_Type::Static);  line1->addShapeSegment( DrPointF( -800,    0), DrPointF( 300, -250) );
        DrEngineObject *line2 = new DrEngineObject(this, getNextKey(), Body_Type::Static);  line2->addShapeSegment( DrPointF(  250,   50), DrPointF(1750,  350) );
        DrEngineObject *line3 = new DrEngineObject(this, getNextKey(), Body_Type::Static);  line3->addShapeSegment( DrPointF(-1100, -300), DrPointF(-900, -300) );
        addThings( { line1, line2, line3 } );


    } else if (new_space_type == Demo_Space::Lines2) {
        m_friction = 2.0;
        m_bounce =   0.5;

        // Static line segment shapes for the ground
        DrEngineObject *line1 = new DrEngineObject(this, getNextKey(), Body_Type::Static);  line1->addShapeSegment( DrPointF(-1000, -200), DrPointF(1000, -200) );
        DrEngineObject *line2 = new DrEngineObject(this, getNextKey(), Body_Type::Static);  line2->addShapeSegment( DrPointF(  500, -100), DrPointF( 700, -100) );
        DrEngineObject *line3 = new DrEngineObject(this, getNextKey(), Body_Type::Static);  line3->addShapeSegment( DrPointF(  100,    0), DrPointF( 300,    0) );
        DrEngineObject *line4 = new DrEngineObject(this, getNextKey(), Body_Type::Static);  line4->addShapeSegment( DrPointF( -300,  150), DrPointF(-100,  150) );
        addThings( { line1, line2, line3, line4 } );

        // One way platform support
        line1->setOneWay( One_Way::Pass_Through ); line1->setOneWayDirection( cpv(0, 1) );          // Let objects pass upwards
        line2->setOneWay( One_Way::Pass_Through ); line2->setOneWayDirection( cpv(0, 1) );          // Let objects pass upwards


    } else if (new_space_type == Demo_Space::Blocks) {
        m_friction = 0.5;
        m_bounce =   0.1;

        // ***** One Way Block
        DrEngineObject *block1 = new DrEngineObject(this, getNextKey(), Body_Type::Static, Asset_Textures::Block, -500, 150, 100);
        block1->addShapeBoxFromTexture(Asset_Textures::Block);
        block1->setCollisionType(Collision_Type::Damage_Player);
        block1->setOneWay( One_Way::Pass_Through );
        block1->setOneWayDirection( cpv(0, 1) );            // Let objects pass upwards
        ///block1->setInvincible(true);
        ///block1->setDamage(-1);
        addThing(block1);

        // ***** Rotating Block
        DrEngineObject *block2 = new DrEngineObject(this, getNextKey(), Body_Type::Kinematic, Asset_Textures::Block, -350, 150, 100);
        block2->addShapeBoxFromTexture(Asset_Textures::Block);
        block2->setRotateSpeed( 2.0 );
        block2->setHealth( 4.0 );
        block2->setAutoDamage( 0.2 );
        addThing(block2);

        // ***** Destroyable block
        DrEngineObject *block3 = new DrEngineObject(this, getNextKey(), Body_Type::Kinematic, Asset_Textures::Block, -200, 150, 100);
        block3->addShapeBoxFromTexture(Asset_Textures::Block);
        block3->setCollisionType(Collision_Type::Damage_Player);
        block3->setOneWay( One_Way::Weak_Spot );
        block3->setOneWayDirection( cpv(0, 1) );                                                    // Take damage from below only
        block3->setHealth( 1.0 );
        block3->setDamage( 1.0 );
        addThing(block3);

        // ***** Conveyor Belt Blocks
        DrEngineObject *belt1, *belt2, *belt3, *belt4;
        belt1 = new DrEngineObject(this, getNextKey(), Body_Type::Kinematic, Asset_Textures::Block, -680, 150, 100, c_scale1x1, 2.0);
        belt2 = new DrEngineObject(this, getNextKey(), Body_Type::Kinematic, Asset_Textures::Block, -740, 150, 100, c_scale1x1, 2.0);
        belt3 = new DrEngineObject(this, getNextKey(), Body_Type::Kinematic, Asset_Textures::Block, -800, 150, 100, c_scale1x1, 2.0);
        belt4 = new DrEngineObject(this, getNextKey(), Body_Type::Kinematic, Asset_Textures::Block, -860, 150, 100, c_scale1x1, 2.0);
        belt1->addShapeBoxFromTexture(Asset_Textures::Block);
        belt2->addShapeBoxFromTexture(Asset_Textures::Block);
        belt3->addShapeBoxFromTexture(Asset_Textures::Block);
        belt4->addShapeBoxFromTexture(Asset_Textures::Block);
        cpShapeSetSurfaceVelocity( belt1->shapes.first(), cpv(1000, 0) );
        cpShapeSetSurfaceVelocity( belt2->shapes.first(), cpv(1000, 0) );
        cpShapeSetSurfaceVelocity( belt3->shapes.first(), cpv(1000, 0) );
        cpShapeSetSurfaceVelocity( belt4->shapes.first(), cpv(1000, 0) );
        addThings( { belt1, belt2, belt3, belt4 } );

        // ***** Ladder / Sticky Blocks
        DrEngineObject *ladder1 = new DrEngineObject(this, getNextKey(), Body_Type::Kinematic, Asset_Textures::Block,
                                                     -980, 100, -1, DrPointF(1, 3), -1, -1, false);
        ladder1->addShapeBoxFromTexture(Asset_Textures::Block);
        ladder1->setGravityMultiplier( 0.0 );
        addThing(ladder1);

        DrEngineObject *ladder2 = new DrEngineObject(this, getNextKey(), Body_Type::Kinematic, Asset_Textures::Block, 300, 320, -1, DrPointF(6, 5));
        ladder2->addShapeBoxFromTexture(Asset_Textures::Block);
        ladder2->setGravityMultiplier( 0.5 );
        addThing(ladder2);

        DrEngineObject *ladder_ball = new DrEngineObject(this, getNextKey(), Body_Type::Kinematic, Asset_Textures::Ball, 800, 200, 0, DrPointF(3, 3));
        ladder_ball->addShapeCircleFromTexture(Asset_Textures::Block);
        ladder_ball->setGravityMultiplier( 0.0 );
        ladder_ball->setRotateSpeed(4);
        addThing(ladder_ball);

        DrEngineObject *ladder3 = new DrEngineObject(this, getNextKey(), Body_Type::Kinematic, Asset_Textures::Block, 1300, 325, -1, DrPointF(6, 6));
        ladder3->addShapeBoxFromTexture(Asset_Textures::Block);
        ladder3->setGravityMultiplier( 0.3 );
        addThing(ladder3);


        // ***** Bridge Test
        DrEngineObject *anchor_a = new DrEngineObject(this, getNextKey(), Body_Type::Kinematic, Asset_Textures::Block, 2500, -10, -1, DrPointF(1.5, .1));
        DrEngineObject *chain_1 =  new DrEngineObject(this, getNextKey(), Body_Type::Dynamic,   Asset_Textures::Block, 2600, -10, -1, DrPointF(1.5, .1));
        DrEngineObject *chain_2 =  new DrEngineObject(this, getNextKey(), Body_Type::Dynamic,   Asset_Textures::Block, 2700, -10, -1, DrPointF(1.5, .1));
        DrEngineObject *chain_3 =  new DrEngineObject(this, getNextKey(), Body_Type::Dynamic,   Asset_Textures::Block, 2800, -10, -1, DrPointF(1.5, .1));
        DrEngineObject *chain_4 =  new DrEngineObject(this, getNextKey(), Body_Type::Dynamic,   Asset_Textures::Block, 2900, -10, -1, DrPointF(1.5, .1));
        DrEngineObject *chain_5 =  new DrEngineObject(this, getNextKey(), Body_Type::Dynamic,   Asset_Textures::Block, 3000, -10, -1, DrPointF(1.5, .1));
        DrEngineObject *chain_6 =  new DrEngineObject(this, getNextKey(), Body_Type::Dynamic,   Asset_Textures::Block, 3100, -10, -1, DrPointF(1.5, .1));
        DrEngineObject *chain_7 =  new DrEngineObject(this, getNextKey(), Body_Type::Dynamic,   Asset_Textures::Block, 3200, -10, -1, DrPointF(1.5, .1));
        DrEngineObject *chain_8 =  new DrEngineObject(this, getNextKey(), Body_Type::Dynamic,   Asset_Textures::Block, 3300, -10, -1, DrPointF(1.5, .1));
        DrEngineObject *anchor_b = new DrEngineObject(this, getNextKey(), Body_Type::Kinematic, Asset_Textures::Block, 3400, -10, -1, DrPointF(1.5, .1));
        anchor_a->addShapeBoxFromTexture(Asset_Textures::Block);
        chain_1->addShapeBoxFromTexture( Asset_Textures::Block);
        chain_2->addShapeBoxFromTexture( Asset_Textures::Block);
        chain_3->addShapeBoxFromTexture( Asset_Textures::Block);
        chain_4->addShapeBoxFromTexture( Asset_Textures::Block);
        chain_5->addShapeBoxFromTexture( Asset_Textures::Block);
        chain_6->addShapeBoxFromTexture( Asset_Textures::Block);
        chain_7->addShapeBoxFromTexture( Asset_Textures::Block);
        chain_8->addShapeBoxFromTexture( Asset_Textures::Block);
        anchor_b->addShapeBoxFromTexture(Asset_Textures::Block);
        addThings( { anchor_a, chain_1, chain_2, chain_3, chain_4, chain_5, chain_6, chain_7, chain_8, anchor_b } );

        cpSpaceAddConstraint( m_space, cpPivotJointNew(anchor_a->body, chain_1->body, cpBodyGetPosition(chain_1->body)) );
        cpSpaceAddConstraint( m_space, cpPivotJointNew(chain_1->body,  chain_2->body, cpBodyGetPosition(chain_2->body)) );
        cpSpaceAddConstraint( m_space, cpPivotJointNew(chain_2->body,  chain_3->body, cpBodyGetPosition(chain_3->body)) );
        cpSpaceAddConstraint( m_space, cpPivotJointNew(chain_3->body,  chain_4->body, cpBodyGetPosition(chain_4->body)) );
        cpSpaceAddConstraint( m_space, cpPivotJointNew(chain_4->body,  chain_5->body, cpBodyGetPosition(chain_5->body)) );
        cpSpaceAddConstraint( m_space, cpPivotJointNew(chain_5->body,  chain_6->body, cpBodyGetPosition(chain_6->body)) );
        cpSpaceAddConstraint( m_space, cpPivotJointNew(chain_6->body,  chain_7->body, cpBodyGetPosition(chain_7->body)) );
        cpSpaceAddConstraint( m_space, cpPivotJointNew(chain_7->body,  chain_8->body, cpBodyGetPosition(chain_8->body)) );
        cpSpaceAddConstraint( m_space, cpPivotJointNew(chain_8->body, anchor_b->body, cpBodyGetPosition(anchor_b->body)) );

        unsigned int all_categories = ~(static_cast<unsigned int>(0));
        cpShapeFilter filter;
        filter.group = 56;                      // Any int > 0, maybe use unique project id of parent? or keep a key generator when Engine starts
        filter.categories = all_categories;     // CP_ALL_CATEGORIES
        filter.mask =       all_categories;     // CP_ALL_CATEGORIES
        for (auto shape : anchor_a->shapes) cpShapeSetFilter( shape, filter);
        for (auto shape : anchor_b->shapes) cpShapeSetFilter( shape, filter);
        for (auto shape : chain_1->shapes)  cpShapeSetFilter( shape, filter);
        for (auto shape : chain_2->shapes)  cpShapeSetFilter( shape, filter);
        for (auto shape : chain_3->shapes)  cpShapeSetFilter( shape, filter);
        for (auto shape : chain_4->shapes)  cpShapeSetFilter( shape, filter);
        for (auto shape : chain_5->shapes)  cpShapeSetFilter( shape, filter);
        for (auto shape : chain_6->shapes)  cpShapeSetFilter( shape, filter);
        for (auto shape : chain_7->shapes)  cpShapeSetFilter( shape, filter);
        for (auto shape : chain_8->shapes)  cpShapeSetFilter( shape, filter);

        // ***** Static line segment shapes for the ground
        DrEngineObject *line1 = new DrEngineObject(this, getNextKey(), Body_Type::Static);  line1->addShapeSegment( DrPointF(-1000,   0), DrPointF( 2500,   0) );
        DrEngineObject *line2 = new DrEngineObject(this, getNextKey(), Body_Type::Static);  line2->addShapeSegment( DrPointF( 3470,   0), DrPointF( 4500,   0) );
        DrEngineObject *line3 = new DrEngineObject(this, getNextKey(), Body_Type::Static);  line3->addShapeSegment( DrPointF( 4500,   0), DrPointF( 4500, 100) );
        addThings( { line1, line2, line3 } );

        // ***** Big ramp
        DrEngineObject *ramp1 = new DrEngineObject(this, getNextKey(), Body_Type::Static);  ramp1->addShapeSegment( DrPointF(    0,    0), DrPointF(300,   50) );
        DrEngineObject *ramp2 = new DrEngineObject(this, getNextKey(), Body_Type::Static);  ramp2->addShapeSegment( DrPointF(  300,   50), DrPointF(600,    0) );
        addThings( { ramp1, ramp2 } );

        // ***** Little bumps
        DrEngineObject *bump1 =  new DrEngineObject(this, getNextKey(), Body_Type::Static); bump1->addShapeSegment(  DrPointF( 1090,    0), DrPointF(1120,   4) );
        DrEngineObject *bump2 =  new DrEngineObject(this, getNextKey(), Body_Type::Static); bump2->addShapeSegment(  DrPointF( 1120,    4), DrPointF(1150,   0) );
        DrEngineObject *bump3 =  new DrEngineObject(this, getNextKey(), Body_Type::Static); bump3->addShapeSegment(  DrPointF( 1170,    0), DrPointF(1200,   4) );
        DrEngineObject *bump4 =  new DrEngineObject(this, getNextKey(), Body_Type::Static); bump4->addShapeSegment(  DrPointF( 1200,    4), DrPointF(1230,   0) );
        DrEngineObject *bump5 =  new DrEngineObject(this, getNextKey(), Body_Type::Static); bump5->addShapeSegment(  DrPointF( 1250,    0), DrPointF(1280,   4) );
        DrEngineObject *bump6 =  new DrEngineObject(this, getNextKey(), Body_Type::Static); bump6->addShapeSegment(  DrPointF( 1280,    4), DrPointF(1310,   0) );
        DrEngineObject *bump7 =  new DrEngineObject(this, getNextKey(), Body_Type::Static); bump7->addShapeSegment(  DrPointF( 1330,    0), DrPointF(1360,   4) );
        DrEngineObject *bump8 =  new DrEngineObject(this, getNextKey(), Body_Type::Static); bump8->addShapeSegment(  DrPointF( 1360,    4), DrPointF(1390,   0) );
        DrEngineObject *bump9 =  new DrEngineObject(this, getNextKey(), Body_Type::Static); bump9->addShapeSegment(  DrPointF( 1410,    0), DrPointF(1440,   4) );
        DrEngineObject *bump10 = new DrEngineObject(this, getNextKey(), Body_Type::Static); bump10->addShapeSegment( DrPointF( 1440,    4), DrPointF(1470,   0) );
        DrEngineObject *bump11 = new DrEngineObject(this, getNextKey(), Body_Type::Static); bump11->addShapeSegment( DrPointF( 1490,    0), DrPointF(1520,   4) );
        DrEngineObject *bump12 = new DrEngineObject(this, getNextKey(), Body_Type::Static); bump12->addShapeSegment( DrPointF( 1520,    4), DrPointF(1550,   0) );
        DrEngineObject *bump13 = new DrEngineObject(this, getNextKey(), Body_Type::Static); bump13->addShapeSegment( DrPointF( 1570,    0), DrPointF(1600,   4) );
        DrEngineObject *bump14 = new DrEngineObject(this, getNextKey(), Body_Type::Static); bump14->addShapeSegment( DrPointF( 1600,    4), DrPointF(1630,   0) );
        DrEngineObject *bump15 = new DrEngineObject(this, getNextKey(), Body_Type::Static); bump15->addShapeSegment( DrPointF( 1650,    0), DrPointF(1680,   4) );
        DrEngineObject *bump16 = new DrEngineObject(this, getNextKey(), Body_Type::Static); bump16->addShapeSegment( DrPointF( 1680,    4), DrPointF(1710,   0) );
        DrEngineObject *bump17 = new DrEngineObject(this, getNextKey(), Body_Type::Static); bump17->addShapeSegment( DrPointF( 1730,    0), DrPointF(1760,   4) );
        DrEngineObject *bump18 = new DrEngineObject(this, getNextKey(), Body_Type::Static); bump18->addShapeSegment( DrPointF( 1760,    4), DrPointF(1790,   0) );
        addThings( { bump1,  bump2,  bump3,  bump4,  bump5,  bump6,  bump7,  bump8,  bump9,  bump10,
                     bump11, bump12, bump13, bump14, bump15, bump16, bump17, bump18 } );
    }
}














