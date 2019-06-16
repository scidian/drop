//
//      Created by Stephens Nunnally on 6/15/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "playground.h"
#include "playground_toy.h"


//######################################################################################################
//##    Builds Playground World
//######################################################################################################
void DrPlayground::buildSpace() {

    // Sets up cpSpace object
    m_space = cpSpaceNew();                             // Creates an empty space
    cpSpaceSetIterations(m_space, m_iterations);        // Sets how many times physics are processed each update
    cpSpaceSetCollisionSlop(m_space, 0.5);              // Allows for a little overlap, makes sleep happier
    cpSpaceSetSleepTimeThreshold(m_space, 0.50);        // Objects will sleep after this long of not moving
    ///cpSpaceSetIdleSpeedThreshold(m_space, 25.0);     // Can set this manually, but also set automatically based on gravity

    // Default gravity / damping settings
    m_gravity = cpv(0, -1000);                          // cpVect is a 2D vector and cpv() is a shortcut for initializing them
    m_damping = 1;                                      // Kind of like air drag
    m_friction = 1.0;
    m_bounce =   0.8;

    cpSpaceSetGravity(m_space, m_gravity);
    cpSpaceSetDamping(m_space, m_damping);

    // Static line segment shapes for the walls
    this->addLine(Body_Type::Static, Qt::gray, QPointF(-1500,  1500), QPointF( 1500,  1500), c_friction, c_bounce, 1);      // Top
    this->addLine(Body_Type::Static, Qt::gray, QPointF(-1500, -1500), QPointF( 1500, -1500), c_friction, c_bounce, 1);      // Bottom
    this->addLine(Body_Type::Static, Qt::gray, QPointF(-1500, -1500), QPointF(-1500,  1500), c_friction, c_bounce, 1);      // Left
    this->addLine(Body_Type::Static, Qt::gray, QPointF( 1500, -1500), QPointF( 1500,  1500), c_friction, c_bounce, 1);      // Right

    // Static line segment shapes for the ground
    this->addLine(Body_Type::Static, Qt::blue, QPointF(-800,     0), QPointF( 300, -250), c_friction, c_bounce, 1);
    this->addLine(Body_Type::Static, Qt::blue, QPointF( 250,    50), QPointF(1750,  350), c_friction, c_bounce, 1);
    this->addLine(Body_Type::Static, Qt::blue, QPointF(-1100, -300), QPointF(-900, -300), c_friction, c_bounce, 1);



}
