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
#include "engine_object.h"
#include "engine_texture.h"
#include "project/project.h"
#include "project/project_asset.h"
#include "project/project_font.h"
#include "project/project_image.h"
#include "project/project_world.h"
#include "project/project_world_stage.h"
#include "project/project_world_stage_object.h"
#include "helper.h"


//######################################################################################################
//##    Build Space
//######################################################################################################
void DrEngine::buildSpace(Demo_Space new_space_type) {

    // ***** Set up physics world
    demo_space = new_space_type;                        // Save Space type
    m_background_color = QColor(0, 0, 0);

    m_space = cpSpaceNew();                             // Creates an empty space
    cpSpaceSetIterations(m_space, m_iterations);        // Sets how many times physics are processed each update
    cpSpaceSetCollisionSlop(m_space, 0.5);              // Allows for a little overlap, makes sleep happier
    cpSpaceSetSleepTimeThreshold(m_space, 0.50);        // Objects will sleep after this long of not moving
    ///cpSpaceSetIdleSpeedThreshold(m_space, 25.0);     // Can set this manually, but also set automatically based on gravity


    // ***** Reset cameras
    clearCameras();


    // ***** Reset keys
    keyboard_x = 0;
    keyboard_y = 0;
    jump_button = false;
    gas_pedal = Pedal::None;


    // ***** Custom Wildcard beginFunc CollisionHandlers: Damage / Health    
    QVector<Collision_Type> collide_types { Collision_Type::Damage_None,
                                            Collision_Type::Damage_Player,
                                            Collision_Type::Damage_Enemy,
                                            Collision_Type::Damage_All };
    for (Collision_Type c : collide_types) {
        cpCollisionHandler *damage_handler = cpSpaceAddWildcardHandler(m_space, static_cast<cpCollisionType>(c));
        damage_handler->beginFunc = BeginFuncWildcard;
        damage_handler->preSolveFunc = PreSolveFuncWildcard;
    }


    // ***** Build desired demo Space
    if (demo_space == Demo_Space::Project) {

        m_friction = 0.5;
        m_bounce =   0.1;

        // Find current world shown in editor, load Start Stage of that world
        m_current_world = m_project->getOption(Project_Options::Current_World).toLongLong();
        DrWorld *world = m_project->getWorld(m_current_world);
        DrStage *stage = world->getStageFromKey(world->getFirstStageKey());

        if (world->getComponentPropertyValue(Components::World_Settings, Properties::World_Use_Background_Color).toBool())
            m_background_color =  QColor::fromRgba(world->getComponentPropertyValue(Components::World_Settings, Properties::World_Background_Color).toUInt());

        m_game_direction += world->getComponentPropertyValue(Components::World_Settings, Properties::World_Game_Direction).toDouble();
        m_game_start = QPointF(0, 0);       // Set starting load position
        m_loaded_to = 0;                    // Reset how far we've loaded

        loadStageToSpace(stage, 0, 0);      // Load current stage to origin position


    } else if (demo_space == Demo_Space::Lines1) {
        m_friction = 1.0;
        m_bounce =   0.8;

        // Static line segment shapes for the ground
        this->addLine(Body_Type::Static, QPointF(-800,     0), QPointF( 300, -250), c_friction, c_bounce, 1);
        this->addLine(Body_Type::Static, QPointF( 250,    50), QPointF(1750,  350), c_friction, c_bounce, 1);
        this->addLine(Body_Type::Static, QPointF(-1100, -300), QPointF(-900, -300), c_friction, c_bounce, 1);

    } else if (demo_space == Demo_Space::Lines2) {
        m_friction = 2.0;
        m_bounce =   0.5;

        // Static line segment shapes for the ground
        DrEngineObject *line1 = this->addLine(Body_Type::Static, QPointF(-1000, -200), QPointF(1000, -200), c_friction, c_bounce, 1);
        DrEngineObject *line2 = this->addLine(Body_Type::Static, QPointF(  500, -100), QPointF( 700, -100), c_friction, c_bounce, 1);
        this->addLine(Body_Type::Static, QPointF(  100,    0), QPointF( 300,    0), c_friction, c_bounce, 1);
        this->addLine(Body_Type::Static, QPointF( -300,  150), QPointF(-100,  150), c_friction, c_bounce, 1);

        // One way platform support
        line1->one_way = One_Way::Pass_Through; line1->one_way_direction = cpv(0, 1);                // Let objects pass upwards
        line2->one_way = One_Way::Pass_Through; line2->one_way_direction = cpv(0, 1);                // Let objects pass upwards

    } else if (demo_space == Demo_Space::Blocks) {
        m_friction = 0.5;
        m_bounce =   0.1;

        // Test one way block
        DrEngineObject *block = this->addBlock(Body_Type::Static, Test_Textures::Block, -500, 150, 100, 0, QPointF(1, 1), 1, m_friction, m_bounce, QPointF(0, 0));
        block->setCollisionType(Collision_Type::Damage_Player);
        block->one_way = One_Way::Pass_Through; block->one_way_direction = cpv(0, 1);                // Let objects pass upwards

        ///block->setInvincible(true);
        ///block->setDamage(-1);

        // Test rotate block
        DrEngineObject *block2 = this->addBlock(Body_Type::Kinematic, Test_Textures::Block, -350, 150, 100, 0, QPointF(1, 1), 1, m_friction, m_bounce, QPointF(0, 0));
        block2->rotate_speed = 2.0;
        block2->setHealth( 4.0 );
        block2->auto_damage = .2;


        // Test destroyable block
        DrEngineObject *block3 = this->addBlock(Body_Type::Kinematic, Test_Textures::Block, -200, 150, 100, 0, QPointF(1, 1), 1, m_friction, m_bounce, QPointF(0, 0));
        block3->setCollisionType(Collision_Type::Damage_Player);
        block3->one_way = One_Way::Weak_Spot;
        block3->one_way_direction = cpv(0, 1);                // Take damage from below only
        block3->setHealth( 1.0 );
        block3->setDamage( 1.0 );

        // Static line segment shapes for the ground
        this->addLine(Body_Type::Static, QPointF(-1000,   0), QPointF( 2500,   0), c_friction, c_bounce, 1);
        this->addLine(Body_Type::Static, QPointF( 2500,   0), QPointF( 2500, 100), c_friction, c_bounce, 1);

        // Big ramp
        this->addLine(Body_Type::Static, QPointF(    0,    0), QPointF(300,   50), c_friction, c_bounce, 1);
        this->addLine(Body_Type::Static, QPointF(  300,   50), QPointF(600,    0), c_friction, c_bounce, 1);

        // Little bumps
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

        // Block alignment test
        this->addBlock(Body_Type::Kinematic, Test_Textures::Block, -1240, 220, 0, 0, QPointF(1, 1), 1, c_friction, c_bounce, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Test_Textures::Block, -1240, 160, 0, 0, QPointF(1, 1), 1, c_friction, c_bounce, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Test_Textures::Block, -1240, 100, 0, 0, QPointF(1, 1), 1, c_friction, c_bounce, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Test_Textures::Block, -1240,  40, 0, 0, QPointF(1, 1), 1, c_friction, c_bounce, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Test_Textures::Block, -1240, -20, 0, 0, QPointF(1, 1), 1, c_friction, c_bounce, QPointF(0, 0));

        DrEngineObject *belt =  this->addBlock(Body_Type::Kinematic, Test_Textures::Block, -680, 150, 100, 0, QPointF(1, 1), 1, 2.0, m_bounce, QPointF(0, 0));
        DrEngineObject *belt2 = this->addBlock(Body_Type::Kinematic, Test_Textures::Block, -740, 150, 100, 0, QPointF(1, 1), 1, 2.0, m_bounce, QPointF(0, 0));
        DrEngineObject *belt3 = this->addBlock(Body_Type::Kinematic, Test_Textures::Block, -800, 150, 100, 0, QPointF(1, 1), 1, 2.0, m_bounce, QPointF(0, 0));
        DrEngineObject *belt4 = this->addBlock(Body_Type::Kinematic, Test_Textures::Block, -860, 150, 100, 0, QPointF(1, 1), 1, 2.0, m_bounce, QPointF(0, 0));

        cpShapeSetSurfaceVelocity( belt->shapes.first(),  cpv(1000, 0) );
        cpShapeSetSurfaceVelocity( belt2->shapes.first(), cpv(1000, 0) );
        cpShapeSetSurfaceVelocity( belt3->shapes.first(), cpv(1000, 0) );
        cpShapeSetSurfaceVelocity( belt4->shapes.first(), cpv(1000, 0) );

        this->addBlock(Body_Type::Kinematic, Test_Textures::Block, -1180, -20, 0, 0, QPointF(1, 1), 1, c_friction, c_bounce, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Test_Textures::Block, -1120, -20, 0, 0, QPointF(1, 1), 1, c_friction, c_bounce, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Test_Textures::Block, -1060, -20, 0, 0, QPointF(1, 1), 1, c_friction, c_bounce, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Test_Textures::Block, -1000, -20, 0, 0, QPointF(1, 1), 1, c_friction, c_bounce, QPointF(0, 0));

        this->addBlock(Body_Type::Kinematic, Test_Textures::Block,  -940, -20, 0, 0, QPointF(1, 1), 1, c_friction, c_bounce, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Test_Textures::Block,  -880, -20, 0, 0, QPointF(1, 1), 1, c_friction, c_bounce, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Test_Textures::Block,  -820, -20, 0, 0, QPointF(1, 1), 1, c_friction, c_bounce, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Test_Textures::Block,  -760, -20, 0, 0, QPointF(1, 1), 1, c_friction, c_bounce, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Test_Textures::Block,  -700, -20, 0, 0, QPointF(1, 1), 1, c_friction, c_bounce, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Test_Textures::Block,  -640, -20, 0, 0, QPointF(1, 1), 1, c_friction, c_bounce, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Test_Textures::Block,  -580, -20, 0, 0, QPointF(1, 1), 1, c_friction, c_bounce, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Test_Textures::Block,  -520, -20, 0, 0, QPointF(1, 1), 1, c_friction, c_bounce, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Test_Textures::Block,  -460, -20, 0, 0, QPointF(1, 1), 1, c_friction, c_bounce, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Test_Textures::Block,  -400, -20, 0, 0, QPointF(1, 1), 1, c_friction, c_bounce, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Test_Textures::Block,  -340, -20, 0, 0, QPointF(1, 1), 1, c_friction, c_bounce, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Test_Textures::Block,  -280, -20, 0, 0, QPointF(1, 1), 1, c_friction, c_bounce, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Test_Textures::Block,  -220, -20, 0, 0, QPointF(1, 1), 1, c_friction, c_bounce, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Test_Textures::Block,  -160, -20, 0, 0, QPointF(1, 1), 1, c_friction, c_bounce, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Test_Textures::Block,  -100, -20, 0, 0, QPointF(1, 1), 1, c_friction, c_bounce, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Test_Textures::Block,   -39, -20, 0, 0, QPointF(1, 1), 1, c_friction, c_bounce, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Test_Textures::Block,    22, -20, 0, 0, QPointF(1, 1), 1, c_friction, c_bounce, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Test_Textures::Block,    83, -20, 0, 0, QPointF(1, 1), 1, c_friction, c_bounce, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Test_Textures::Block,   144, -20, 0, 0, QPointF(1, 1), 1, c_friction, c_bounce, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Test_Textures::Block,   205, -20, 0, 0, QPointF(1, 1), 1, c_friction, c_bounce, QPointF(0, 0));

    }
}














