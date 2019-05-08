//
//      Created by Stephens Nunnally on 5/7/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "project/project.h"
#include "project/project_asset.h"
#include "project/project_font.h"
#include "project/project_image.h"
#include "project/project_world.h"
#include "project/project_world_stage.h"
#include "project/project_world_stage_object.h"
#include "engine.h"
#include "engine_texture.h"
#include "library.h"

//######################################################################################################
//##    Load Current World, Scene into Space
//######################################################################################################
void DrEngine::loadSpace() {

    // Set up physics world
    m_gravity = cpv(0, -500);                 // cpVect is a 2D vector and cpv() is a shortcut for initializing them
    m_space = cpSpaceNew();                   // Creates an empty space
    m_damping = .7;                           // Kind of like air drag
    cpSpaceSetIterations(m_space, 10);        // 10 is default and should be good enough for most games
    cpSpaceSetGravity(m_space, m_gravity);
    cpSpaceSetDamping(m_space, m_damping);


    // Find current stage shown in editor
    long current_stage = m_project->getOption(Project_Options::Current_Stage).toInt();
    DrStage *stage = m_project->findStageFromKey( current_stage );

    // Load objects
    for (auto object_pair : stage->getObjectMap()) {

        DrObject *object = object_pair.second;

        if (object->getType() != DrType::Object) continue;
        if (object->getObjectType() != DrObjectType::Object) continue;

        QPointF position = object->getComponentPropertyValue(Components::Object_Transform, Properties::Object_Position).toPointF();
        QPointF scale =    object->getComponentPropertyValue(Components::Object_Transform, Properties::Object_Scale).toPointF();
        double  angle =    object->getComponentPropertyValue(Components::Object_Transform, Properties::Object_Rotation).toDouble();
        bool    collide =  object->getComponentPropertyValue(Components::Object_Settings,  Properties::Object_Collide).toBool();

        addBlock(Body_Type::Kinematic, Txt::Rover, position.x(), -position.y(), angle, scale, 1, .75, 100, QPointF(0, 0), collide);


    }





    // Add body
    SceneObject *rover = this->addBlock(  Body_Type::Dynamic, Txt::Rover,  350,  75,  0, QPointF(1, 1), .5, .1, 4, QPointF(0, 0));
    rover->follow = true;

    // Add wheels
    SceneObject *wheel1 = this->addCircle(Body_Type::Dynamic, Txt::Wheel,  310,  45, 3.4, .7,  2, QPointF(0, 0));
    SceneObject *wheel2 = this->addCircle(Body_Type::Dynamic, Txt::Wheel,  350,  45, 3.4, .7,  2, QPointF(0, 0));
    SceneObject *wheel3 = this->addCircle(Body_Type::Dynamic, Txt::Wheel,  390,  45, 3.4, .7,  2, QPointF(0, 0));
    wheel1->is_wheel = true;    wheel1->wheel_speed = 80;
    wheel2->is_wheel = true;    wheel2->wheel_speed = 40;
    wheel3->is_wheel = true;    wheel3->wheel_speed = 60;
    SceneObject *spare1 = this->addCircle(Body_Type::Dynamic, Txt::Spare,  290,  35, 3.4, .7, .5, QPointF(0, 0));

    cpShapeFilter test = cpShapeGetFilter( rover->shape );
    cpShapeFilter filter;
    filter.group = 43;
    filter.mask = test.mask;
    filter.categories = test.categories;
    cpShapeSetFilter( rover->shape,  filter);
    cpShapeSetFilter( wheel1->shape, filter);
    cpShapeSetFilter( wheel2->shape, filter);
    cpShapeSetFilter( wheel3->shape, filter);

    // New bouncy shocks joint, Grooves a/b are relative to the car, anchor point B is on the wheel
    cpSpaceAddConstraint(m_space, cpGrooveJointNew( rover->body, wheel1->body, cpv(-40,  15), cpv(-40, -28), cpvzero));
    cpSpaceAddConstraint(m_space, cpGrooveJointNew( rover->body, wheel2->body, cpv(  0,  15), cpv(  0, -28), cpvzero));
    cpSpaceAddConstraint(m_space, cpGrooveJointNew( rover->body, wheel3->body, cpv( 40,  15), cpv( 40, -28), cpvzero));

    cpSpaceAddConstraint(m_space, cpGrooveJointNew( rover->body, spare1->body, cpv(-40,  15), cpv(-65, -30), cpvzero));

    cpSpaceAddConstraint(m_space, cpDampedSpringNew(rover->body, wheel1->body, cpv(-40, 0), cpvzero, 50.0, 90.0, 50.0)); // Higher damp = less mushy, 100 = pretty stiff
    cpSpaceAddConstraint(m_space, cpDampedSpringNew(rover->body, wheel2->body, cpv(  0, 0), cpvzero, 50.0, 90.0, 25.0));
    cpSpaceAddConstraint(m_space, cpDampedSpringNew(rover->body, wheel3->body, cpv( 40, 0), cpvzero, 50.0, 90.0, 40.0));

    cpSpaceAddConstraint(m_space, cpDampedSpringNew(rover->body, spare1->body, cpv(-40, 0), cpvzero, 40.0, 30.0,  5.0));
}




