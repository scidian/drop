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
#include "engine_light.h"
#include "engine_object.h"
#include "engine_texture.h"
#include "engine_world.h"
#include "form_engine.h"
#include "opengl/opengl.h"
#include "project/project.h"
#include "project/project_asset.h"
#include "project/project_font.h"
#include "project/project_image.h"
#include "project/project_world.h"
#include "project/project_world_stage.h"
#include "project/project_world_stage_thing.h"
#include "helper.h"


//######################################################################################################
//##    Build Space
//######################################################################################################
void DrEngineWorld::buildSpace(Demo_Space new_space_type) {

    // ***** Set up physics world
    m_engine->demo_space = new_space_type;              // Save Space type
    m_background_color = QColor(0, 0, 0);

    m_space = cpSpaceNew();                             // Creates an empty space
    cpSpaceSetIterations(m_space, m_iterations);        // Sets how many times physics are processed each update
    cpSpaceSetCollisionSlop(m_space, 0.5);              // Allows for a little overlap, makes sleep happier
    cpSpaceSetSleepTimeThreshold(m_space, 0.50);        // Objects will sleep after this long of not moving
    ///cpSpaceSetIdleSpeedThreshold(m_space, 25.0);     // Can set this manually, but also set automatically based on gravity

    // Default gravity / damping settings
    m_gravity = cpv(0, -1000);                          // cpVect is a 2D vector and cpv() is a shortcut for initializing them
    m_damping = 1;                                      // Kind of like air drag
    cpSpaceSetGravity(m_space, m_gravity);
    cpSpaceSetDamping(m_space, m_damping);

    // ***** Reset cameras
    clearCameras();


    // ***** Reset keys
    m_engine->keyboard_x = 0;
    m_engine->keyboard_y = 0;
    m_engine->jump_button = false;
    m_engine->gas_pedal = Pedal::None;


    // ***** Custom Wildcard beginFunc CollisionHandlers: Damage / Health    
    QVector<Collision_Type> collide_types { Collision_Type::Damage_None,
                                            Collision_Type::Damage_Player,
                                            Collision_Type::Damage_Enemy,
                                            Collision_Type::Damage_All };
    for (Collision_Type c : collide_types) {
        cpCollisionHandler *custom_collision_handler = cpSpaceAddWildcardHandler(m_space, static_cast<cpCollisionType>(c));
        custom_collision_handler->beginFunc = BeginFuncWildcard;
        custom_collision_handler->preSolveFunc = PreSolveFuncWildcard;
        custom_collision_handler->separateFunc = SeperateFuncWildcard;
    }



    // !!!!! #TEMP add lights
    m_engine->getFormEngine()->getOpenGL()->makeCurrent();

    DrEngineLight *light1 = new DrEngineLight();
    light1->position = QPointF(-350, 300);
    light1->color = Qt::blue;
    light1->intensity = 3.0;

    DrEngineLight *light2 = new DrEngineLight();
    light2->position = QPointF(1000, 250);
    light2->color = Qt::yellow;
    light2->intensity = 2.0;

    DrEngineLight *light3 = new DrEngineLight();
    light3->position = QPointF(2000, 1000);
    light3->color = Qt::red;
    light3->intensity = 1.0;
    light3->shadows = 5.0;
    light3->light_size = 5000;


    lights.append(light1);
    lights.append(light2);
    lights.append(light3);
    m_engine->getFormEngine()->getOpenGL()->doneCurrent();



    // ***** Build desired demo Space
    if (m_engine->demo_space == Demo_Space::Project) {

        // ***** Find current world shown in editor, load Start Stage of that world

        DrWorld *world = m_project->getWorld(m_engine->getCurrentEditorWorld());
        DrStage *stage = world->getStageFromKey(world->getFirstStageKey());

        // ***** World Settings
        m_game_direction += world->getComponentPropertyValue(Components::World_Settings, Properties::World_Game_Direction).toDouble();
        m_game_start = QPointF(0, 0);       // Set starting load position
        m_loaded_to = 0;                    // Reset how far we've loaded

        if (world->getComponentPropertyValue(Components::World_Settings, Properties::World_Use_Background_Color).toBool())
            m_background_color =  QColor::fromRgba(world->getComponentPropertyValue(Components::World_Settings, Properties::World_Background_Color).toUInt());

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
        bitrate =     world->getComponentPropertyValue(Components::World_Appearance, Properties::World_Filter_Bitrate).toInt();
        QPointF pixelation = world->getComponentPropertyValue(Components::World_Appearance, Properties::World_Filter_Pixelation).toPointF();
        pixel_x =     static_cast<float>(pixelation.x());
        pixel_y =     static_cast<float>(pixelation.y());
        brightness =  world->getComponentPropertyValue(Components::World_Appearance, Properties::World_Filter_Brightness).toInt() / 255.f;
        contrast =    world->getComponentPropertyValue(Components::World_Appearance, Properties::World_Filter_Contrast).toInt() / 255.f;
        saturation =  world->getComponentPropertyValue(Components::World_Appearance, Properties::World_Filter_Saturation).toInt() / 255.f;
        hue =         world->getComponentPropertyValue(Components::World_Appearance, Properties::World_Filter_Hue).toInt() / 360.f;
        grayscale =   world->getComponentPropertyValue(Components::World_Appearance, Properties::World_Filter_Grayscale).toBool();
        negative =    world->getComponentPropertyValue(Components::World_Appearance, Properties::World_Filter_Negative).toBool();

        // ***** Load Current Stage to origin position
        loadStageToSpace(stage, 0, 0);


    } else if (m_engine->demo_space == Demo_Space::Lines1) {
        m_friction = 1.0;
        m_bounce =   0.8;

        // Static line segment shapes for the ground
        this->addLine(Body_Type::Static, QPointF(-800,     0), QPointF( 300, -250), c_friction, c_bounce, 1);
        this->addLine(Body_Type::Static, QPointF( 250,    50), QPointF(1750,  350), c_friction, c_bounce, 1);
        this->addLine(Body_Type::Static, QPointF(-1100, -300), QPointF(-900, -300), c_friction, c_bounce, 1);

    } else if (m_engine->demo_space == Demo_Space::Lines2) {
        m_friction = 2.0;
        m_bounce =   0.5;

        // Static line segment shapes for the ground
        DrEngineObject *line1 = this->addLine(Body_Type::Static, QPointF(-1000, -200), QPointF(1000, -200), c_friction, c_bounce, 1);
        DrEngineObject *line2 = this->addLine(Body_Type::Static, QPointF(  500, -100), QPointF( 700, -100), c_friction, c_bounce, 1);
        this->addLine(Body_Type::Static, QPointF(  100,    0), QPointF( 300,    0), c_friction, c_bounce, 1);
        this->addLine(Body_Type::Static, QPointF( -300,  150), QPointF(-100,  150), c_friction, c_bounce, 1);

        // One way platform support
        line1->setOneWay( One_Way::Pass_Through ); line1->setOneWayDirection( cpv(0, 1) );          // Let objects pass upwards
        line2->setOneWay( One_Way::Pass_Through ); line2->setOneWayDirection( cpv(0, 1) );          // Let objects pass upwards


    } else if (m_engine->demo_space == Demo_Space::Blocks) {
        m_friction = 0.5;
        m_bounce =   0.1;

        // ***** One Way Block
        DrEngineObject *block = this->addBlock(Body_Type::Static, Asset_Textures::Block, -500, 150, 100, 0, c_scale1x1, 1, m_friction, m_bounce, QPointF(0, 0));
        block->setCollisionType(Collision_Type::Damage_Player);
        block->setOneWay( One_Way::Pass_Through ); block->setOneWayDirection( cpv(0, 1) );          // Let objects pass upwards
        ///block->setInvincible(true);
        ///block->setDamage(-1);

        // ***** Rotating Block
        DrEngineObject *block2 = this->addBlock(Body_Type::Kinematic, Asset_Textures::Block, -350, 150, 100, 0, c_scale1x1, 1, m_friction, m_bounce, QPointF(0, 0));
        block2->setRotateSpeed( 2.0 );
        block2->setHealth( 4.0 );
        block2->setAutoDamage( 0.2 );

        // ***** Destroyable block
        DrEngineObject *block3 = this->addBlock(Body_Type::Kinematic, Asset_Textures::Block, -200, 150, 100, 0, c_scale1x1, 1, m_friction, m_bounce, QPointF(0, 0));
        block3->setCollisionType(Collision_Type::Damage_Player);
        block3->setOneWay( One_Way::Weak_Spot );
        block3->setOneWayDirection( cpv(0, 1) );                                                    // Take damage from below only
        block3->setHealth( 1.0 );
        block3->setDamage( 1.0 );

        // ***** Conveyor Belt Blocks
        DrEngineObject *belt =  this->addBlock(Body_Type::Kinematic, Asset_Textures::Block, -680, 150, 100, 0, c_scale1x1, 1, 2.0, m_bounce, QPointF(0, 0));
        DrEngineObject *belt2 = this->addBlock(Body_Type::Kinematic, Asset_Textures::Block, -740, 150, 100, 0, c_scale1x1, 1, 2.0, m_bounce, QPointF(0, 0));
        DrEngineObject *belt3 = this->addBlock(Body_Type::Kinematic, Asset_Textures::Block, -800, 150, 100, 0, c_scale1x1, 1, 2.0, m_bounce, QPointF(0, 0));
        DrEngineObject *belt4 = this->addBlock(Body_Type::Kinematic, Asset_Textures::Block, -860, 150, 100, 0, c_scale1x1, 1, 2.0, m_bounce, QPointF(0, 0));

        cpShapeSetSurfaceVelocity( belt->shapes.first(),  cpv(1000, 0) );
        cpShapeSetSurfaceVelocity( belt2->shapes.first(), cpv(1000, 0) );
        cpShapeSetSurfaceVelocity( belt3->shapes.first(), cpv(1000, 0) );
        cpShapeSetSurfaceVelocity( belt4->shapes.first(), cpv(1000, 0) );

        // ***** Ladder / Sticky Blocks
        DrEngineObject *ladder = this->addBlock(Body_Type::Kinematic, Asset_Textures::Block, -980, 100, -1, 0, QPointF(1, 3), 1,
                                                m_friction, m_bounce, QPointF(0, 0), false);
        ladder->setGravityMultiplier( 0.0 );

        DrEngineObject *ladder2 = this->addBlock(Body_Type::Kinematic, Asset_Textures::Block, 300, 225, -1, 0, QPointF(6, 2), 1,
                                                m_friction, m_bounce, QPointF(0, 0), true);
        ladder2->setGravityMultiplier( 0.1 );

        double ball_radius = m_engine->getTextureMap()[Asset_Textures::Ball]->width() / 2.0;
        DrEngineObject *ladder_ball = this->addCircle(Body_Type::Kinematic, Asset_Textures::Ball, 800, 200, 0, c_norotate, QPointF(3, 3), c_opaque,
                                                      ball_radius, c_center, m_friction, 0, QPointF(0, 0));
        ladder_ball->setGravityMultiplier( 0.0 );
        ladder_ball->setRotateSpeed(4);

        DrEngineObject *ladder3 = this->addBlock(Body_Type::Kinematic, Asset_Textures::Block, 1300, 325, -1, 0, QPointF(6, 6), 1,
                                                m_friction, m_bounce, QPointF(0, 0), true);
        ladder3->setGravityMultiplier( 0.3 );

        // ***** Bridge Test
        DrEngineObject *anchor_a = this->addBlock(Body_Type::Kinematic, Asset_Textures::Block, 2500, -10, -1, 0, QPointF(1.5, .1), 1, m_friction, m_bounce, QPointF(0, 0));
        DrEngineObject *chain_1 =  this->addBlock(Body_Type::Dynamic,   Asset_Textures::Block, 2600, -10, -1, 0, QPointF(1.5, .1), 1, m_friction, m_bounce, QPointF(0, 0));
        DrEngineObject *chain_2 =  this->addBlock(Body_Type::Dynamic,   Asset_Textures::Block, 2700, -10, -1, 0, QPointF(1.5, .1), 1, m_friction, m_bounce, QPointF(0, 0));
        DrEngineObject *chain_3 =  this->addBlock(Body_Type::Dynamic,   Asset_Textures::Block, 2800, -10, -1, 0, QPointF(1.5, .1), 1, m_friction, m_bounce, QPointF(0, 0));
        DrEngineObject *chain_4 =  this->addBlock(Body_Type::Dynamic,   Asset_Textures::Block, 2900, -10, -1, 0, QPointF(1.5, .1), 1, m_friction, m_bounce, QPointF(0, 0));
        DrEngineObject *chain_5 =  this->addBlock(Body_Type::Dynamic,   Asset_Textures::Block, 3000, -10, -1, 0, QPointF(1.5, .1), 1, m_friction, m_bounce, QPointF(0, 0));
        DrEngineObject *chain_6 =  this->addBlock(Body_Type::Dynamic,   Asset_Textures::Block, 3100, -10, -1, 0, QPointF(1.5, .1), 1, m_friction, m_bounce, QPointF(0, 0));
        DrEngineObject *chain_7 =  this->addBlock(Body_Type::Dynamic,   Asset_Textures::Block, 3200, -10, -1, 0, QPointF(1.5, .1), 1, m_friction, m_bounce, QPointF(0, 0));
        DrEngineObject *chain_8 =  this->addBlock(Body_Type::Dynamic,   Asset_Textures::Block, 3300, -10, -1, 0, QPointF(1.5, .1), 1, m_friction, m_bounce, QPointF(0, 0));
        DrEngineObject *anchor_b = this->addBlock(Body_Type::Kinematic, Asset_Textures::Block, 3400, -10, -1, 0, QPointF(1.5, .1), 1, m_friction, m_bounce, QPointF(0, 0));

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
        this->addLine(Body_Type::Static, QPointF(-1000,   0), QPointF( 2500,   0), c_friction, c_bounce, 1);
        this->addLine(Body_Type::Static, QPointF( 3470,   0), QPointF( 4500,   0), c_friction, c_bounce, 1);
        this->addLine(Body_Type::Static, QPointF( 4500,   0), QPointF( 4500, 100), c_friction, c_bounce, 1);

        // ***** Big ramp
        this->addLine(Body_Type::Static, QPointF(    0,    0), QPointF(300,   50), c_friction, c_bounce, 1);
        this->addLine(Body_Type::Static, QPointF(  300,   50), QPointF(600,    0), c_friction, c_bounce, 1);

        // ***** Little bumps
        this->addLine(Body_Type::Static, QPointF( 1090,    0), QPointF(1120,   4), c_friction, c_bounce, 1);
        this->addLine(Body_Type::Static, QPointF( 1120,    4), QPointF(1150,   0), c_friction, c_bounce, 1);
        this->addLine(Body_Type::Static, QPointF( 1170,    0), QPointF(1200,   4), c_friction, c_bounce, 1);
        this->addLine(Body_Type::Static, QPointF( 1200,    4), QPointF(1230,   0), c_friction, c_bounce, 1);
        this->addLine(Body_Type::Static, QPointF( 1250,    0), QPointF(1280,   4), c_friction, c_bounce, 1);
        this->addLine(Body_Type::Static, QPointF( 1280,    4), QPointF(1310,   0), c_friction, c_bounce, 1);
        this->addLine(Body_Type::Static, QPointF( 1330,    0), QPointF(1360,   4), c_friction, c_bounce, 1);
        this->addLine(Body_Type::Static, QPointF( 1360,    4), QPointF(1390,   0), c_friction, c_bounce, 1);
        this->addLine(Body_Type::Static, QPointF( 1410,    0), QPointF(1440,   4), c_friction, c_bounce, 1);
        this->addLine(Body_Type::Static, QPointF( 1440,    4), QPointF(1470,   0), c_friction, c_bounce, 1);
        this->addLine(Body_Type::Static, QPointF( 1490,    0), QPointF(1520,   4), c_friction, c_bounce, 1);
        this->addLine(Body_Type::Static, QPointF( 1520,    4), QPointF(1550,   0), c_friction, c_bounce, 1);
        this->addLine(Body_Type::Static, QPointF( 1570,    0), QPointF(1600,   4), c_friction, c_bounce, 1);
        this->addLine(Body_Type::Static, QPointF( 1600,    4), QPointF(1630,   0), c_friction, c_bounce, 1);
        this->addLine(Body_Type::Static, QPointF( 1650,    0), QPointF(1680,   4), c_friction, c_bounce, 1);
        this->addLine(Body_Type::Static, QPointF( 1680,    4), QPointF(1710,   0), c_friction, c_bounce, 1);
        this->addLine(Body_Type::Static, QPointF( 1730,    0), QPointF(1760,   4), c_friction, c_bounce, 1);
        this->addLine(Body_Type::Static, QPointF( 1760,    4), QPointF(1790,   0), c_friction, c_bounce, 1);

        // ***** Block alignment test
        this->addBlock(Body_Type::Kinematic, Asset_Textures::Block, -1240, 220, 0, 0, QPointF(1, 1), 1, c_friction, c_bounce, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Asset_Textures::Block, -1240, 160, 0, 0, QPointF(1, 1), 1, c_friction, c_bounce, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Asset_Textures::Block, -1240, 100, 0, 0, QPointF(1, 1), 1, c_friction, c_bounce, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Asset_Textures::Block, -1240,  40, 0, 0, QPointF(1, 1), 1, c_friction, c_bounce, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Asset_Textures::Block, -1240, -20, 0, 0, QPointF(1, 1), 1, c_friction, c_bounce, QPointF(0, 0));

        // ***** Ground / Left Wall Blocks
        this->addBlock(Body_Type::Kinematic, Asset_Textures::Block, -1180, -20, 0, 0, QPointF(1, 1), 1, c_friction, c_bounce, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Asset_Textures::Block, -1120, -20, 0, 0, QPointF(1, 1), 1, c_friction, c_bounce, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Asset_Textures::Block, -1060, -20, 0, 0, QPointF(1, 1), 1, c_friction, c_bounce, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Asset_Textures::Block, -1000, -20, 0, 0, QPointF(1, 1), 1, c_friction, c_bounce, QPointF(0, 0));

        this->addBlock(Body_Type::Kinematic, Asset_Textures::Block,  -940, -20, 0, 0, QPointF(1, 1), 1, c_friction, c_bounce, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Asset_Textures::Block,  -880, -20, 0, 0, QPointF(1, 1), 1, c_friction, c_bounce, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Asset_Textures::Block,  -820, -20, 0, 0, QPointF(1, 1), 1, c_friction, c_bounce, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Asset_Textures::Block,  -760, -20, 0, 0, QPointF(1, 1), 1, c_friction, c_bounce, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Asset_Textures::Block,  -700, -20, 0, 0, QPointF(1, 1), 1, c_friction, c_bounce, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Asset_Textures::Block,  -640, -20, 0, 0, QPointF(1, 1), 1, c_friction, c_bounce, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Asset_Textures::Block,  -580, -20, 0, 0, QPointF(1, 1), 1, c_friction, c_bounce, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Asset_Textures::Block,  -520, -20, 0, 0, QPointF(1, 1), 1, c_friction, c_bounce, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Asset_Textures::Block,  -460, -20, 0, 0, QPointF(1, 1), 1, c_friction, c_bounce, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Asset_Textures::Block,  -400, -20, 0, 0, QPointF(1, 1), 1, c_friction, c_bounce, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Asset_Textures::Block,  -340, -20, 0, 0, QPointF(1, 1), 1, c_friction, c_bounce, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Asset_Textures::Block,  -280, -20, 0, 0, QPointF(1, 1), 1, c_friction, c_bounce, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Asset_Textures::Block,  -220, -20, 0, 0, QPointF(1, 1), 1, c_friction, c_bounce, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Asset_Textures::Block,  -160, -20, 0, 0, QPointF(1, 1), 1, c_friction, c_bounce, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Asset_Textures::Block,  -100, -20, 0, 0, QPointF(1, 1), 1, c_friction, c_bounce, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Asset_Textures::Block,   -39, -20, 0, 0, QPointF(1, 1), 1, c_friction, c_bounce, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Asset_Textures::Block,    22, -20, 0, 0, QPointF(1, 1), 1, c_friction, c_bounce, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Asset_Textures::Block,    83, -20, 0, 0, QPointF(1, 1), 1, c_friction, c_bounce, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Asset_Textures::Block,   144, -20, 0, 0, QPointF(1, 1), 1, c_friction, c_bounce, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Asset_Textures::Block,   205, -20, 0, 0, QPointF(1, 1), 1, c_friction, c_bounce, QPointF(0, 0));
    }
}














