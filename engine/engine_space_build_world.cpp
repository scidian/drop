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
//##    Chipmunk Callbacks
//##        Support for object collisions and one way platforms
//######################################################################################################
static cpBool BeginFuncWildcard(cpArbiter *arb, cpSpace *, void *) {
    CP_ARBITER_GET_SHAPES(arb, a, b);
    DrEngineObject *object_a = static_cast<DrEngineObject*>(cpShapeGetUserData(a));
    DrEngineObject *object_b = static_cast<DrEngineObject*>(cpShapeGetUserData(b));

    // Check for one way platform
    if (object_a->one_way) {
        if (cpvdot(cpArbiterGetNormal(arb), object_a->one_way_direction) < 0.0)
            return cpArbiterIgnore(arb);
    } else if (object_b->one_way) {
        if (cpvdot(cpArbiterGetNormal(arb), object_b->one_way_direction) > 0.0)
            return cpArbiterIgnore(arb);
    }
    if (object_a->damage <= 0) return cpTrue;                                   // Object does no damage, exit
    if (!object_a->alive) return cpFalse;                                       // If object is dead, cancel collision

    bool should_damage = false;
    if ((object_a->collision_type == Collision_Type::Damage_Enemy  || object_a->collision_type == Collision_Type::Damage_Enemy_One_Way) &&
        (object_b->collision_type == Collision_Type::Damage_Player || object_b->collision_type == Collision_Type::Damage_Player_One_Way ||
         object_b->collision_type == Collision_Type::Damage_All    || object_b->collision_type == Collision_Type::Damage_All_One_Way))
        should_damage = true;

    if ((object_a->collision_type == Collision_Type::Damage_Player || object_a->collision_type == Collision_Type::Damage_Player_One_Way) &&
        (object_b->collision_type == Collision_Type::Damage_Enemy  || object_b->collision_type == Collision_Type::Damage_Enemy_One_Way ||
         object_b->collision_type == Collision_Type::Damage_All    || object_b->collision_type == Collision_Type::Damage_All_One_Way))
        should_damage = true;

    if ((object_a->collision_type == Collision_Type::Damage_All    || object_a->collision_type == Collision_Type::Damage_All_One_Way))
        should_damage = true;

    if (should_damage) {
        if (object_b->health > 0) {
            object_b->health -= object_a->damage;

            // If we killed object, cancel collision
            if (object_b->health <= 0) {
                object_b->health = 0;
                return cpFalse;
            }
        }
    }
    return cpTrue;
}

void DrEngine::oneWayPlatform(DrEngineObject *object, cpVect direction) {
    object->one_way = true;
    object->one_way_direction = direction;              // Let objects pass if going Direction
    switch (object->collision_type) {
        case Collision_Type::Damage_None:   case Collision_Type::Damage_None_One_Way:   setCollisionType(object, Collision_Type::Damage_None_One_Way);   break;
        case Collision_Type::Damage_Player: case Collision_Type::Damage_Player_One_Way: setCollisionType(object, Collision_Type::Damage_Player_One_Way); break;
        case Collision_Type::Damage_Enemy:  case Collision_Type::Damage_Enemy_One_Way:  setCollisionType(object, Collision_Type::Damage_Enemy_One_Way);  break;
        case Collision_Type::Damage_All:    case Collision_Type::Damage_All_One_Way:    setCollisionType(object, Collision_Type::Damage_All_One_Way);    break;
    }
}

void DrEngine::setCollisionType(DrEngineObject *object, Collision_Type type) {
    object->collision_type = type;
    for (auto shape : object->shapes) {
        cpShapeSetCollisionType(shape, static_cast<cpCollisionType>(type));
    }
}


//######################################################################################################
//##    Build Space
//######################################################################################################
void DrEngine::buildSpace(Demo_Space new_space_type) {

    // ***** Set up physics world
    demo_space = new_space_type;                        // Save Space type
    m_background_color = QColor(0, 0, 0);

    m_space = cpSpaceNew();                             // Creates an empty space
    cpSpaceSetIterations(m_space, m_iterations);        // Sets how many times physics are processed each update

    // Sleep settings
    cpSpaceSetSleepTimeThreshold(m_space, 0.5);         // Objects will sleep after this long of not moving
    ///cpSpaceSetIdleSpeedThreshold(m_space, 25.0);     // Set automatically based on gravity

    // Some other pretty good sleep settings
    ///cpSpaceSetSleepTimeThreshold(m_space, 0.20);     // Objects will sleep after this long of not moving
    ///cpSpaceSetIdleSpeedThreshold(m_space, 25.0);     // Speed at which an objet will start ot fall asleep

    cpSpaceSetCollisionSlop(m_space, 0.5);              // Allows for a little overlap, makes sleep happier


    // ***** Reset cameras
    clearCameras();


    // ***** Custom Wildcard beginFunc CollisionHandlers: Damage / Health
    cpCollisionHandler *damage_handler1 = cpSpaceAddWildcardHandler(m_space, static_cast<cpCollisionType>(Collision_Type::Damage_None));
    cpCollisionHandler *damage_handler2 = cpSpaceAddWildcardHandler(m_space, static_cast<cpCollisionType>(Collision_Type::Damage_None_One_Way));
    cpCollisionHandler *damage_handler3 = cpSpaceAddWildcardHandler(m_space, static_cast<cpCollisionType>(Collision_Type::Damage_Player));
    cpCollisionHandler *damage_handler4 = cpSpaceAddWildcardHandler(m_space, static_cast<cpCollisionType>(Collision_Type::Damage_Player_One_Way));
    cpCollisionHandler *damage_handler5 = cpSpaceAddWildcardHandler(m_space, static_cast<cpCollisionType>(Collision_Type::Damage_Enemy));
    cpCollisionHandler *damage_handler6 = cpSpaceAddWildcardHandler(m_space, static_cast<cpCollisionType>(Collision_Type::Damage_Enemy_One_Way));
    cpCollisionHandler *damage_handler7 = cpSpaceAddWildcardHandler(m_space, static_cast<cpCollisionType>(Collision_Type::Damage_All));
    cpCollisionHandler *damage_handler8 = cpSpaceAddWildcardHandler(m_space, static_cast<cpCollisionType>(Collision_Type::Damage_All_One_Way));
    damage_handler1->beginFunc = BeginFuncWildcard;
    damage_handler2->beginFunc = BeginFuncWildcard;
    damage_handler3->beginFunc = BeginFuncWildcard;
    damage_handler4->beginFunc = BeginFuncWildcard;
    damage_handler5->beginFunc = BeginFuncWildcard;
    damage_handler6->beginFunc = BeginFuncWildcard;
    damage_handler7->beginFunc = BeginFuncWildcard;
    damage_handler8->beginFunc = BeginFuncWildcard;


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
        m_friction = 1;
        m_bounce = 0.8;

        // Static line segment shapes for the ground
        this->addLine(Body_Type::Static, QPointF(-800,     0), QPointF( 300, -250), c_friction, c_bounce, 1);
        this->addLine(Body_Type::Static, QPointF( 250,    50), QPointF(1000,  200), c_friction, c_bounce, 1);
        this->addLine(Body_Type::Static, QPointF(-1100, -300), QPointF(-900, -300), c_friction, c_bounce, 1);

    } else if (demo_space == Demo_Space::Lines2) {
        m_friction = 2;
        m_bounce = 0.5;

        // Static line segment shapes for the ground
        DrEngineObject *line1 = this->addLine(Body_Type::Static, QPointF(-1000, -200), QPointF(1000, -200), c_friction, c_bounce, 1);
        DrEngineObject *line2 = this->addLine(Body_Type::Static, QPointF(  500, -100), QPointF( 700, -100), c_friction, c_bounce, 1);
        this->addLine(Body_Type::Static, QPointF(  100,    0), QPointF( 300,    0), c_friction, c_bounce, 1);
        this->addLine(Body_Type::Static, QPointF( -300,  150), QPointF(-100,  150), c_friction, c_bounce, 1);

        // One way platform support
        oneWayPlatform(line1, cpv(0, 1));                        // Let objects pass upwards
        oneWayPlatform(line2, cpv(0, 1));                        // Let objects pass upwards


    } else if (demo_space == Demo_Space::Blocks) {
        m_friction = 0.5;
        m_bounce = 0.5;

        // Test one way block
        DrEngineObject *block =  this->addBlock(Body_Type::Kinematic, Test_Textures::Block, -300, 120, 100, 0, QPointF(1, 1), 1, m_friction, m_bounce, QPointF(0, 0));
        block->collision_type = Collision_Type::Damage_Player;
        oneWayPlatform(block, cpv(0, 1));

        // Test rotate block
        DrEngineObject *block2 = this->addBlock(Body_Type::Kinematic, Test_Textures::Block, -150, 120, 100, 0, QPointF(1, 1), 1, m_friction, m_bounce, QPointF(0, 0));
        block->collision_type = Collision_Type::Damage_Player;
        block2->rotate_speed = 2;


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
        this->addBlock(Body_Type::Kinematic, Test_Textures::Block, -1000, 220, 0, 0, QPointF(1, 1), 1, c_friction, c_bounce, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Test_Textures::Block, -1000, 160, 0, 0, QPointF(1, 1), 1, c_friction, c_bounce, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Test_Textures::Block, -1000, 100, 0, 0, QPointF(1, 1), 1, c_friction, c_bounce, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Test_Textures::Block, -1000,  40, 0, 0, QPointF(1, 1), 1, c_friction, c_bounce, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Test_Textures::Block, -1000, -20, 0, 0, QPointF(1, 1), 1, c_friction, c_bounce, QPointF(0, 0));

        DrEngineObject *belt =  this->addBlock(Body_Type::Kinematic, Test_Textures::Block, -500, 120, 100, 0, QPointF(1, 1), 1, 2.0, m_bounce, QPointF(0, 0));
        DrEngineObject *belt2 = this->addBlock(Body_Type::Kinematic, Test_Textures::Block, -560, 120, 100, 0, QPointF(1, 1), 1, 2.0, m_bounce, QPointF(0, 0));
        DrEngineObject *belt3 = this->addBlock(Body_Type::Kinematic, Test_Textures::Block, -620, 120, 100, 0, QPointF(1, 1), 1, 2.0, m_bounce, QPointF(0, 0));
        DrEngineObject *belt4 = this->addBlock(Body_Type::Kinematic, Test_Textures::Block, -680, 120, 100, 0, QPointF(1, 1), 1, 2.0, m_bounce, QPointF(0, 0));

        cpShapeSetSurfaceVelocity( belt->shapes.first(),  cpv(1000, 0) );
        cpShapeSetSurfaceVelocity( belt2->shapes.first(), cpv(1000, 0) );
        cpShapeSetSurfaceVelocity( belt3->shapes.first(), cpv(1000, 0) );
        cpShapeSetSurfaceVelocity( belt4->shapes.first(), cpv(1000, 0) );

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














