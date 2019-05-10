//
//      Created by Stephens Nunnally on 5/6/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
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

static void ShapeFreeWrap(cpSpace *space, cpShape *shape, void *) { cpSpaceRemoveShape(space, shape);   cpShapeFree(shape); }
static void PostShapeFree(cpShape *shape, cpSpace *space) { cpSpaceAddPostStepCallback(space, cpPostStepFunc(ShapeFreeWrap), shape, nullptr); }
static void ConstraintFreeWrap(cpSpace *space, cpConstraint *constraint, void *) { cpSpaceRemoveConstraint(space, constraint); cpConstraintFree(constraint); }
static void PostConstraintFree(cpConstraint *constraint, cpSpace *space) { cpSpaceAddPostStepCallback(space, cpPostStepFunc(ConstraintFreeWrap), constraint, nullptr); }
static void BodyFreeWrap(cpSpace *space, cpBody *body, void *) { cpSpaceRemoveBody(space, body); cpBodyFree(body); }
static void PostBodyFree(cpBody *body, cpSpace *space) { cpSpaceAddPostStepCallback(space, cpPostStepFunc(BodyFreeWrap), body, nullptr); }

// Safe and future proof way to remove and free all objects that have been added to the space.
void ChipmunkFreeSpaceChildren(cpSpace *space) {
    // Must remove these BEFORE freeing the body or you will access dangling pointers.
    cpSpaceEachShape(space, cpSpaceShapeIteratorFunc(PostShapeFree), space);
    cpSpaceEachConstraint(space, cpSpaceConstraintIteratorFunc(PostConstraintFree), space);
    cpSpaceEachBody(space, cpSpaceBodyIteratorFunc(PostBodyFree), space);
}

//######################################################################################################
//##    Clear Space
//######################################################################################################
void DrEngine::clearSpace() {
    if (has_scene) {
        ChipmunkFreeSpaceChildren(m_space);
        cpSpaceFree(m_space);
        objects.clear();
        has_scene = false;
    }
}

//######################################################################################################
//##    Build Space
//######################################################################################################
void DrEngine::buildSpace(Demo_Space new_space_type) {

    // Set up physics world
    demo_space = new_space_type;                // Save Space type
    clearSpace();                               // Clear existing Space
    m_background_color = QColor(0,0,0);

    m_space = cpSpaceNew();                     // Creates an empty space
    cpSpaceSetIterations(m_space, 10);          // 10 is default and should be good enough for most games

    // Build desired demo Space
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

            long    asset_key = object->getAssetKey();
            addBlock(Body_Type::Kinematic, asset_key, position.x(), -position.y(), z_order, angle, scale, alpha, 1, .75, 100, QPointF(0, 0), collide);
        }


    } else if (demo_space == Demo_Space::Lines1) {
        // Add a static line segment shapes for the ground
        this->addLine(Body_Type::Static, QPointF(-800,     0), QPointF( 300, -250), 1, .99, 1);
        this->addLine(Body_Type::Static, QPointF( 250,    50), QPointF(1000,  200), 1, .99, 1);
        this->addLine(Body_Type::Static, QPointF(-1100, -300), QPointF(-900, -300), 1, .99, 1);

    } else if (demo_space == Demo_Space::Lines2) {
        // Add a static line segment shapes for the ground
        this->addLine(Body_Type::Static, QPointF(-1000, -200), QPointF(1000, -200), 2, .5, 1);
        this->addLine(Body_Type::Static, QPointF( -300,  150), QPointF(-100,  150), 2, .5, 1);
        this->addLine(Body_Type::Static, QPointF(  100,    0), QPointF( 300,    0), 2, .5, 1);
        this->addLine(Body_Type::Static, QPointF(  500, -100), QPointF( 700, -100), 2, .5, 1);

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

    has_scene = true;
}














