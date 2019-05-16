//
//      Created by Stephens Nunnally on 5/6/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QApplication>

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
//##        Support for one way platform (collision)
//######################################################################################################
// Used for shape iterator to get a list of all shapes attached to a body
static void getShapeList(cpBody *, cpShape *shape, QVector<cpShape*> *shape_list) { shape_list->append(shape); }

static cpBool PreSolve(cpArbiter *arb, cpSpace *, void *) {
    CP_ARBITER_GET_SHAPES(arb, a, b);
    cpVect *direction = static_cast<cpVect*>(cpShapeGetUserData(a));

    if(cpvdot(cpArbiterGetNormal(arb), (*direction)) < 0)
        return cpArbiterIgnore(arb);
    return cpTrue;
}

void DrEngine::oneWayPlatform(SceneObject *object, cpVect direction) {
    object->one_way = true;
    object->one_way_direction = direction;                                  // Let objects pass upwards

    QVector<cpShape*> shape_list;
    cpBodyEachShape(object->body, cpBodyShapeIteratorFunc(getShapeList), &shape_list);
    for (auto shape : shape_list) {
        cpShapeSetCollisionType(shape, COLLISION_TYPE_ONE_WAY);           // We'll use the data pointer for the OneWayPlatform direction
        cpShapeSetUserData(shape, &object->one_way_direction);
    }
}

//######################################################################################################
//##    Build Space
//######################################################################################################
void DrEngine::buildSpace(Demo_Space new_space_type) {

    // ***** Set up physics world
    demo_space = new_space_type;                // Save Space type
    m_background_color = QColor(0,0,0);

    m_space = cpSpaceNew();                     // Creates an empty space
    cpSpaceSetIterations(m_space, 10);          // 10 is default and should be good enough for most games


    // ***** Add handler for one way collisions
    cpCollisionHandler *handler = cpSpaceAddWildcardHandler(m_space, COLLISION_TYPE_ONE_WAY);
    handler->preSolveFunc = PreSolve;


    // ***** Build desired demo Space
    if (demo_space == Demo_Space::Project) {

        m_friction = 0.5;
        m_bounce = 0.5;

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
        m_bounce = 0.999;

        // Static line segment shapes for the ground
        this->addLine(Body_Type::Static, QPointF(-800,     0), QPointF( 300, -250), m_friction, m_bounce, 1);
        this->addLine(Body_Type::Static, QPointF( 250,    50), QPointF(1000,  200), m_friction, m_bounce, 1);
        this->addLine(Body_Type::Static, QPointF(-1100, -300), QPointF(-900, -300), m_friction, m_bounce, 1);

    } else if (demo_space == Demo_Space::Lines2) {
        m_friction = 2;
        m_bounce = 0.5;

        // Static line segment shapes for the ground
        SceneObject *line1 = this->addLine(Body_Type::Static, QPointF(-1000, -200), QPointF(1000, -200), m_friction, m_bounce, 1);
        SceneObject *line2 = this->addLine(Body_Type::Static, QPointF(  500, -100), QPointF( 700, -100), m_friction, m_bounce, 1);
        this->addLine(Body_Type::Static, QPointF(  100,    0), QPointF( 300,    0), m_friction, m_bounce, 1);
        this->addLine(Body_Type::Static, QPointF( -300,  150), QPointF(-100,  150), m_friction, m_bounce, 1);

        // One way platform support
        oneWayPlatform(line1, cpv(0, 1));                        // Let objects pass upwards
        oneWayPlatform(line2, cpv(0, 1));                        // Let objects pass upwards


    } else if (demo_space == Demo_Space::Blocks) {
        m_friction = 1;
        m_bounce = 0.75;

        // Static line segment shapes for the ground
        this->addLine(Body_Type::Static, QPointF(-1000,   0), QPointF( 2500,   0), m_friction, m_bounce, 1);
        this->addLine(Body_Type::Static, QPointF( 2500,   0), QPointF( 2500, 100), m_friction, m_bounce, 1);

        // Big ramp
        this->addLine(Body_Type::Static, QPointF(    0,    0), QPointF(300,   50), m_friction, m_bounce, 1);
        this->addLine(Body_Type::Static, QPointF(  300,   50), QPointF(600,    0), m_friction, m_bounce, 1);

        // Little bumps
        this->addLine(Body_Type::Static, QPointF( 1090,    0), QPointF(1120,   4), m_friction, m_bounce, 1);
        this->addLine(Body_Type::Static, QPointF( 1120,    4), QPointF(1150,   0), m_friction, m_bounce, 1);
        this->addLine(Body_Type::Static, QPointF( 1170,    0), QPointF(1200,   4), m_friction, m_bounce, 1);
        this->addLine(Body_Type::Static, QPointF( 1200,    4), QPointF(1230,   0), m_friction, m_bounce, 1);
        this->addLine(Body_Type::Static, QPointF( 1250,    0), QPointF(1280,   4), m_friction, m_bounce, 1);
        this->addLine(Body_Type::Static, QPointF( 1280,    4), QPointF(1310,   0), m_friction, m_bounce, 1);
        this->addLine(Body_Type::Static, QPointF( 1330,    0), QPointF(1360,   4), m_friction, m_bounce, 1);
        this->addLine(Body_Type::Static, QPointF( 1360,    4), QPointF(1390,   0), m_friction, m_bounce, 1);
        this->addLine(Body_Type::Static, QPointF( 1410,    0), QPointF(1440,   4), m_friction, m_bounce, 1);
        this->addLine(Body_Type::Static, QPointF( 1440,    4), QPointF(1470,   0), m_friction, m_bounce, 1);
        this->addLine(Body_Type::Static, QPointF( 1490,    0), QPointF(1520,   4), m_friction, m_bounce, 1);
        this->addLine(Body_Type::Static, QPointF( 1520,    4), QPointF(1550,   0), m_friction, m_bounce, 1);
        this->addLine(Body_Type::Static, QPointF( 1570,    0), QPointF(1600,   4), m_friction, m_bounce, 1);
        this->addLine(Body_Type::Static, QPointF( 1600,    4), QPointF(1630,   0), m_friction, m_bounce, 1);
        this->addLine(Body_Type::Static, QPointF( 1650,    0), QPointF(1680,   4), m_friction, m_bounce, 1);
        this->addLine(Body_Type::Static, QPointF( 1680,    4), QPointF(1710,   0), m_friction, m_bounce, 1);
        this->addLine(Body_Type::Static, QPointF( 1730,    0), QPointF(1760,   4), m_friction, m_bounce, 1);
        this->addLine(Body_Type::Static, QPointF( 1760,    4), QPointF(1790,   0), m_friction, m_bounce, 1);

        // Block alignment test
        this->addBlock(Body_Type::Kinematic, Test_Textures::Block, -1000, 220, 0, 0, QPointF(1, 1), 1, m_friction, m_bounce, 100, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Test_Textures::Block, -1000, 160, 0, 0, QPointF(1, 1), 1, m_friction, m_bounce, 100, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Test_Textures::Block, -1000, 100, 0, 0, QPointF(1, 1), 1, m_friction, m_bounce, 100, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Test_Textures::Block, -1000,  40, 0, 0, QPointF(1, 1), 1, m_friction, m_bounce, 100, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Test_Textures::Block, -1000, -20, 0, 0, QPointF(1, 1), 1, m_friction, m_bounce, 100, QPointF(0, 0));

        SceneObject *block = this->addBlock(Body_Type::Kinematic, Test_Textures::Block, -400, 120, 100, 0,
                                            QPointF(1, 1), 1, m_friction, m_bounce, 100, QPointF(0, 0));
        oneWayPlatform(block, cpv(0, 1));

        this->addBlock(Body_Type::Kinematic, Test_Textures::Block,  -940, -20, 0, 0, QPointF(1, 1), 1, m_friction, m_bounce, 100, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Test_Textures::Block,  -880, -20, 0, 0, QPointF(1, 1), 1, m_friction, m_bounce, 100, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Test_Textures::Block,  -820, -20, 0, 0, QPointF(1, 1), 1, m_friction, m_bounce, 100, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Test_Textures::Block,  -760, -20, 0, 0, QPointF(1, 1), 1, m_friction, m_bounce, 100, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Test_Textures::Block,  -700, -20, 0, 0, QPointF(1, 1), 1, m_friction, m_bounce, 100, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Test_Textures::Block,  -640, -20, 0, 0, QPointF(1, 1), 1, m_friction, m_bounce, 100, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Test_Textures::Block,  -580, -20, 0, 0, QPointF(1, 1), 1, m_friction, m_bounce, 100, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Test_Textures::Block,  -520, -20, 0, 0, QPointF(1, 1), 1, m_friction, m_bounce, 100, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Test_Textures::Block,  -460, -20, 0, 0, QPointF(1, 1), 1, m_friction, m_bounce, 100, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Test_Textures::Block,  -400, -20, 0, 0, QPointF(1, 1), 1, m_friction, m_bounce, 100, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Test_Textures::Block,  -340, -20, 0, 0, QPointF(1, 1), 1, m_friction, m_bounce, 100, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Test_Textures::Block,  -280, -20, 0, 0, QPointF(1, 1), 1, m_friction, m_bounce, 100, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Test_Textures::Block,  -220, -20, 0, 0, QPointF(1, 1), 1, m_friction, m_bounce, 100, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Test_Textures::Block,  -160, -20, 0, 0, QPointF(1, 1), 1, m_friction, m_bounce, 100, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Test_Textures::Block,  -100, -20, 0, 0, QPointF(1, 1), 1, m_friction, m_bounce, 100, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Test_Textures::Block,   -39, -20, 0, 0, QPointF(1, 1), 1, m_friction, m_bounce, 100, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Test_Textures::Block,    22, -20, 0, 0, QPointF(1, 1), 1, m_friction, m_bounce, 100, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Test_Textures::Block,    83, -20, 0, 0, QPointF(1, 1), 1, m_friction, m_bounce, 100, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Test_Textures::Block,   144, -20, 0, 0, QPointF(1, 1), 1, m_friction, m_bounce, 100, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Test_Textures::Block,   205, -20, 0, 0, QPointF(1, 1), 1, m_friction, m_bounce, 100, QPointF(0, 0));

    }
}














