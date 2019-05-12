//
//      Created by Stephens Nunnally on 5/6/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QApplication>

#include "engine/engine.h"
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

        // Find current stage shown in editor
        long current_stage = m_project->getOption(Project_Options::Current_Stage).toInt();
        DrStage *stage = m_project->findStageFromKey( current_stage );
        DrWorld *world = stage->getParentWorld();

        if (world->getComponentPropertyValue(Components::World_Settings, Properties::World_Use_Background_Color).toBool())
            m_background_color =  QColor::fromRgba(world->getComponentPropertyValue(Components::World_Settings, Properties::World_Background_Color).toUInt());

        // Load objects
        for (auto object_pair : stage->getObjectMap()) {

            DrObject *object = object_pair.second;

            if (object->getType() != DrType::Object) continue;
            if (object->getObjectType() != DrObjectType::Object) continue;

            QPointF position = object->getComponentPropertyValue(Components::Object_Transform, Properties::Object_Position).toPointF();
            QPointF scale =    object->getComponentPropertyValue(Components::Object_Transform, Properties::Object_Scale).toPointF();
            double  angle =    object->getComponentPropertyValue(Components::Object_Transform, Properties::Object_Rotation).toDouble();
            double  z_order =  object->getComponentPropertyValue(Components::Object_Layering,  Properties::Object_Z_Order).toDouble();
            double  alpha =    object->getComponentPropertyValue(Components::Object_Layering,  Properties::Object_Opacity).toDouble() / 100;
            bool    collide =  object->getComponentPropertyValue(Components::Object_Settings,  Properties::Object_Collide).toBool();
            bool    physics =  object->getComponentPropertyValue(Components::Object_Settings,  Properties::Object_Physics).toBool();
            Body_Type btype =  physics? Body_Type::Dynamic : Body_Type::Kinematic;

            long    asset_key = object->getAssetKey();
            addBlock(btype, asset_key, position.x(), -position.y(), z_order, angle, scale, alpha, .5, .5, 25, QPointF(0, 0), collide);
        }


    } else if (demo_space == Demo_Space::Lines1) {
        // Add a static line segment shapes for the ground
        this->addLine(Body_Type::Static, QPointF(-800,     0), QPointF( 300, -250), 1, .99, 1);
        this->addLine(Body_Type::Static, QPointF( 250,    50), QPointF(1000,  200), 1, .99, 1);
        this->addLine(Body_Type::Static, QPointF(-1100, -300), QPointF(-900, -300), 1, .99, 1);

    } else if (demo_space == Demo_Space::Lines2) {
        // Add a static line segment shapes for the ground
        SceneObject *line1 = this->addLine(Body_Type::Static, QPointF(-1000, -200), QPointF(1000, -200), 2, .5, 1);
        SceneObject *line2 = this->addLine(Body_Type::Static, QPointF(  500, -100), QPointF( 700, -100), 2, .5, 1);
        this->addLine(Body_Type::Static, QPointF(  100,    0), QPointF( 300,    0), 2, .5, 1);
        this->addLine(Body_Type::Static, QPointF( -300,  150), QPointF(-100,  150), 2, .5, 1);

        // One way platform support
        oneWayPlatform(line1, cpv(0, 1));                        // Let objects pass upwards
        oneWayPlatform(line2, cpv(0, 1));                        // Let objects pass upwards


    } else if (demo_space == Demo_Space::Blocks) {
        // Add a static line segment shapes for the ground
        this->addLine(Body_Type::Static, QPointF(-1000,    0), QPointF( 2500,    0), 1, .75, 1);
        this->addLine(Body_Type::Static, QPointF( 2500,    0), QPointF( 2500, 100), 1, .75, 1);

        // Big ramp
        this->addLine(Body_Type::Static, QPointF(    0,    0), QPointF(300,   50), 1, .75, 1);
        this->addLine(Body_Type::Static, QPointF(  300,   50), QPointF(600,    0), 1, .75, 1);

        // Little bumps
        this->addLine(Body_Type::Static, QPointF( 1090,    0), QPointF(1120,   4), 1, .75, 1);
        this->addLine(Body_Type::Static, QPointF( 1120,    4), QPointF(1150,   0), 1, .75, 1);
        this->addLine(Body_Type::Static, QPointF( 1170,    0), QPointF(1200,   4), 1, .75, 1);
        this->addLine(Body_Type::Static, QPointF( 1200,    4), QPointF(1230,   0), 1, .75, 1);
        this->addLine(Body_Type::Static, QPointF( 1250,    0), QPointF(1280,   4), 1, .75, 1);
        this->addLine(Body_Type::Static, QPointF( 1280,    4), QPointF(1310,   0), 1, .75, 1);
        this->addLine(Body_Type::Static, QPointF( 1330,    0), QPointF(1360,   4), 1, .75, 1);
        this->addLine(Body_Type::Static, QPointF( 1360,    4), QPointF(1390,   0), 1, .75, 1);
        this->addLine(Body_Type::Static, QPointF( 1410,    0), QPointF(1440,   4), 1, .75, 1);
        this->addLine(Body_Type::Static, QPointF( 1440,    4), QPointF(1470,   0), 1, .75, 1);
        this->addLine(Body_Type::Static, QPointF( 1490,    0), QPointF(1520,   4), 1, .75, 1);
        this->addLine(Body_Type::Static, QPointF( 1520,    4), QPointF(1550,   0), 1, .75, 1);
        this->addLine(Body_Type::Static, QPointF( 1570,    0), QPointF(1600,   4), 1, .75, 1);
        this->addLine(Body_Type::Static, QPointF( 1600,    4), QPointF(1630,   0), 1, .75, 1);
        this->addLine(Body_Type::Static, QPointF( 1650,    0), QPointF(1680,   4), 1, .75, 1);
        this->addLine(Body_Type::Static, QPointF( 1680,    4), QPointF(1710,   0), 1, .75, 1);
        this->addLine(Body_Type::Static, QPointF( 1730,    0), QPointF(1760,   4), 1, .75, 1);
        this->addLine(Body_Type::Static, QPointF( 1760,    4), QPointF(1790,   0), 1, .75, 1);

        // Block alignment test
        this->addBlock(Body_Type::Kinematic, Test_Textures::Block, -1000, 220, 0, 0, QPointF(1, 1), 1, 1, .75, 100, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Test_Textures::Block, -1000, 160, 0, 0, QPointF(1, 1), 1, 1, .75, 100, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Test_Textures::Block, -1000, 100, 0, 0, QPointF(1, 1), 1, 1, .75, 100, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Test_Textures::Block, -1000,  40, 0, 0, QPointF(1, 1), 1, 1, .75, 100, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Test_Textures::Block, -1000, -20, 0, 0, QPointF(1, 1), 1, 1, .75, 100, QPointF(0, 0));

        SceneObject *block = this->addBlock(Body_Type::Kinematic, Test_Textures::Block, -400, 120, 100, 0, QPointF(1, 1), 1, 1, .75, 100, QPointF(0, 0));
        oneWayPlatform(block, cpv(0, 1));

        this->addBlock(Body_Type::Kinematic, Test_Textures::Block,  -940, -20, 0, 0, QPointF(1, 1), 1, 1, .75, 100, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Test_Textures::Block,  -880, -20, 0, 0, QPointF(1, 1), 1, 1, .75, 100, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Test_Textures::Block,  -820, -20, 0, 0, QPointF(1, 1), 1, 1, .75, 100, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Test_Textures::Block,  -760, -20, 0, 0, QPointF(1, 1), 1, 1, .75, 100, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Test_Textures::Block,  -700, -20, 0, 0, QPointF(1, 1), 1, 1, .75, 100, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Test_Textures::Block,  -640, -20, 0, 0, QPointF(1, 1), 1, 1, .75, 100, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Test_Textures::Block,  -580, -20, 0, 0, QPointF(1, 1), 1, 1, .75, 100, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Test_Textures::Block,  -520, -20, 0, 0, QPointF(1, 1), 1, 1, .75, 100, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Test_Textures::Block,  -460, -20, 0, 0, QPointF(1, 1), 1, 1, .75, 100, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Test_Textures::Block,  -400, -20, 0, 0, QPointF(1, 1), 1, 1, .75, 100, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Test_Textures::Block,  -340, -20, 0, 0, QPointF(1, 1), 1, 1, .75, 100, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Test_Textures::Block,  -280, -20, 0, 0, QPointF(1, 1), 1, 1, .75, 100, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Test_Textures::Block,  -220, -20, 0, 0, QPointF(1, 1), 1, 1, .75, 100, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Test_Textures::Block,  -160, -20, 0, 0, QPointF(1, 1), 1, 1, .75, 100, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Test_Textures::Block,  -100, -20, 0, 0, QPointF(1, 1), 1, 1, .75, 100, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Test_Textures::Block,   -39, -20, 0, 0, QPointF(1, 1), 1, 1, .75, 100, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Test_Textures::Block,    22, -20, 0, 0, QPointF(1, 1), 1, 1, .75, 100, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Test_Textures::Block,    83, -20, 0, 0, QPointF(1, 1), 1, 1, .75, 100, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Test_Textures::Block,   144, -20, 0, 0, QPointF(1, 1), 1, 1, .75, 100, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Test_Textures::Block,   205, -20, 0, 0, QPointF(1, 1), 1, 1, .75, 100, QPointF(0, 0));

    }
}














