//
//      Created by Stephens Nunnally on 5/6/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "engine/engine.h"
#include "engine_texture.h"

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
void DrEngine::buildSpace() {

    if (demo == Demo::Spawn) {
        // Set up physics world
        m_gravity = cpv(0, -500);                 // cpVect is a 2D vector and cpv() is a shortcut for initializing them
        m_space = cpSpaceNew();                   // Creates an empty space
        cpSpaceSetIterations(m_space, 10);        // 10 is default and should be good enough for most games
        cpSpaceSetGravity(m_space, m_gravity);

        // Add a static line segment shapes for the ground.
        this->addLine(Body_Type::Static, QPointF(-800,     0), QPointF( 300, -250), 1, .99, 1);
        this->addLine(Body_Type::Static, QPointF( 250,    50), QPointF(1000,  200), 1, .99, 1);
        this->addLine(Body_Type::Static, QPointF(-1100, -300), QPointF(-900, -300), 1, .99, 1);

        // Add one ball
        this->addCircle(Body_Type::Dynamic, Txt::Ball, -100,  100, .7, .5, 2, QPointF( 0, 0));

        SceneObject *ball = this->addCircle(Body_Type::Kinematic, Txt::Ball, -300,  150, .7, .5, 2, QPointF(25, 0));
        ball->follow = true;


    } else if (demo == Demo::Car) {
        // Set up physics world
        m_gravity = cpv(0, -500);                 // cpVect is a 2D vector and cpv() is a shortcut for initializing them
        m_space = cpSpaceNew();                   // Creates an empty space
        m_damping = .7;                           // Kind of like air drag
        cpSpaceSetIterations(m_space, 10);        // 10 is default and should be good enough for most games
        cpSpaceSetGravity(m_space, m_gravity);
        cpSpaceSetDamping(m_space, m_damping);


        // Add a static line segment shapes for the ground.
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

        // Sample object to find category and mask numbers
        SceneObject  *base = this->addBlock(Body_Type::Kinematic, Txt::Block, 300,   0, 0, QPointF(1, 1), 1, .75, 100, QPointF(0, 0));
        cpShapeFilter test = cpShapeGetFilter( base->shape);


        // Block alignment test
        this->addBlock(Body_Type::Kinematic, Txt::Block, -1000, 220, 0, QPointF(1, 1), 1, .75, 100, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Txt::Block, -1000, 160, 0, QPointF(1, 1), 1, .75, 100, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Txt::Block, -1000, 100, 0, QPointF(1, 1), 1, .75, 100, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Txt::Block, -1000,  40, 0, QPointF(1, 1), 1, .75, 100, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Txt::Block, -1000, -20, 0, QPointF(1, 1), 1, .75, 100, QPointF(0, 0));

        this->addBlock(Body_Type::Kinematic, Txt::Block,  -940, -20, 0, QPointF(1, 1), 1, .75, 100, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Txt::Block,  -880, -20, 0, QPointF(1, 1), 1, .75, 100, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Txt::Block,  -820, -20, 0, QPointF(1, 1), 1, .75, 100, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Txt::Block,  -760, -20, 0, QPointF(1, 1), 1, .75, 100, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Txt::Block,  -700, -20, 0, QPointF(1, 1), 1, .75, 100, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Txt::Block,  -640, -20, 0, QPointF(1, 1), 1, .75, 100, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Txt::Block,  -580, -20, 0, QPointF(1, 1), 1, .75, 100, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Txt::Block,  -520, -20, 0, QPointF(1, 1), 1, .75, 100, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Txt::Block,  -460, -20, 0, QPointF(1, 1), 1, .75, 100, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Txt::Block,  -400, -20, 0, QPointF(1, 1), 1, .75, 100, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Txt::Block,  -340, -20, 0, QPointF(1, 1), 1, .75, 100, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Txt::Block,  -280, -20, 0, QPointF(1, 1), 1, .75, 100, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Txt::Block,  -220, -20, 0, QPointF(1, 1), 1, .75, 100, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Txt::Block,  -160, -20, 0, QPointF(1, 1), 1, .75, 100, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Txt::Block,  -100, -20, 0, QPointF(1, 1), 1, .75, 100, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Txt::Block,   -39, -20, 0, QPointF(1, 1), 1, .75, 100, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Txt::Block,    22, -20, 0, QPointF(1, 1), 1, .75, 100, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Txt::Block,    83, -20, 0, QPointF(1, 1), 1, .75, 100, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Txt::Block,   144, -20, 0, QPointF(1, 1), 1, .75, 100, QPointF(0, 0));
        this->addBlock(Body_Type::Kinematic, Txt::Block,   205, -20, 0, QPointF(1, 1), 1, .75, 100, QPointF(0, 0));



        // Add body
        SceneObject *rover = this->addBlock(  Body_Type::Dynamic, Txt::Rover, -450,  75, 0, QPointF(1, 1),   .5, .1, 4, QPointF(0, 0));
        rover->follow = true;

        // Add wheels
        SceneObject *wheel1 = this->addCircle(Body_Type::Dynamic, Txt::Wheel, -490,  45, 3.4, .7,  2, QPointF(0, 0));
        SceneObject *wheel2 = this->addCircle(Body_Type::Dynamic, Txt::Wheel, -450,  45, 3.4, .7,  2, QPointF(0, 0));
        SceneObject *wheel3 = this->addCircle(Body_Type::Dynamic, Txt::Wheel, -410,  45, 3.4, .7,  2, QPointF(0, 0));
        wheel1->is_wheel = true;    wheel1->wheel_speed = 80;
        wheel2->is_wheel = true;    wheel2->wheel_speed = 40;
        wheel3->is_wheel = true;    wheel3->wheel_speed = 60;
        SceneObject *spare1 = this->addCircle(Body_Type::Dynamic, Txt::Spare, -510,  35, 3.4, .7, .5, QPointF(0, 0));

        //this->addCircle(Body_Type::Dynamic, Txt::Ball, -450, -80, 4, 0, 2, QPointF(0, 0));

        // Set body and wheels to same group so they don't collide
        //EX:
        //#define GRABBABLE_MASK_BIT (1<<31)
        //cpShapeFilter GRAB_FILTER = {CP_NO_GROUP, GRABBABLE_MASK_BIT, GRABBABLE_MASK_BIT};
        //cpShapeFilter NOT_GRABBABLE_FILTER = {CP_NO_GROUP, ~GRABBABLE_MASK_BIT, ~GRABBABLE_MASK_BIT};
        cpShapeFilter filter;
        filter.group = 43;
        filter.mask = test.mask;
        filter.categories = test.categories;
        cpShapeSetFilter( rover->shape,  filter);
        cpShapeSetFilter( wheel1->shape, filter);
        cpShapeSetFilter( wheel2->shape, filter);
        cpShapeSetFilter( wheel3->shape, filter);

        // Old solid pin joint
        //cpSpaceAddConstraint( m_space, cpPivotJointNew(rover->body, wheel1->body, cpBodyGetPosition(wheel1->body));
        //cpSpaceAddConstraint( m_space, cpPivotJointNew(rover->body, wheel2->body, cpBodyGetPosition(wheel2->body));

        // New bouncy shocks joint, Grooves a/b are relative to the car, anchor point B is on the wheel
        cpSpaceAddConstraint(m_space, cpGrooveJointNew( rover->body, wheel1->body, cpv(-40,  15), cpv(-40, -28), cpvzero));
        cpSpaceAddConstraint(m_space, cpGrooveJointNew( rover->body, wheel2->body, cpv(  0,  15), cpv(  0, -28), cpvzero));
        cpSpaceAddConstraint(m_space, cpGrooveJointNew( rover->body, wheel3->body, cpv( 40,  15), cpv( 40, -28), cpvzero));

        cpSpaceAddConstraint(m_space, cpGrooveJointNew( rover->body, spare1->body, cpv(-40,  15), cpv(-65, -30), cpvzero));
        //cpSpaceAddConstraint(m_space, cpGrooveJointNew( rover->body, spare2->body, cpv( 40,  15), cpv( 65, -30), cpvzero));

        cpSpaceAddConstraint(m_space, cpDampedSpringNew(rover->body, wheel1->body, cpv(-40, 0), cpvzero, 50.0, 90.0, 50.0)); // Higher damp = less mushy, 100 = pretty stiff
        cpSpaceAddConstraint(m_space, cpDampedSpringNew(rover->body, wheel2->body, cpv(  0, 0), cpvzero, 50.0, 90.0, 25.0));
        cpSpaceAddConstraint(m_space, cpDampedSpringNew(rover->body, wheel3->body, cpv( 40, 0), cpvzero, 50.0, 90.0, 40.0));

        cpSpaceAddConstraint(m_space, cpDampedSpringNew(rover->body, spare1->body, cpv(-40, 0), cpvzero, 40.0, 30.0,  5.0));
        //cpSpaceAddConstraint(m_space, cpDampedSpringNew(rover->body, spare2->body, cpv( 40, 0), cpvzero, 40.0, 30.0,  5.0));

        // Simple Motor Example, Applies constant speed to joint
        //cpConstraint *wheel_motor_1 = cpSimpleMotorNew(rover->body, wheel1->body, 25);
        //cpSpaceAddConstraint( m_space, wheel_motor_1);
        //cpConstraint *wheel_motor_2 = cpSimpleMotorNew(rover->body, wheel2->body, .2);
        //cpSpaceAddConstraint( m_space, wheel_motor_2);

    } else if (demo == Demo::Project) {

        loadSpace();

    }

    has_scene = true;
}








