//
//      Created by Stephens Nunnally on 6/15/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "colors/colors.h"
#include "form_playground.h"
#include "playground.h"
#include "playground_toy.h"


//######################################################################################################
//##    Builds Playground World
//######################################################################################################
void DrPlayground::buildSpace() {

    // ***** Sets up cpSpace object
    m_space = cpSpaceNew();                             // Creates an empty space
    cpSpaceSetIterations(m_space, m_iterations);        // Sets how many times physics are processed each update
    cpSpaceSetCollisionSlop(m_space, 0.5);              // Allows for a little overlap, makes sleep happier
    cpSpaceSetSleepTimeThreshold(m_space, 0.50);        // Objects will sleep after this long of not moving
    ///cpSpaceSetIdleSpeedThreshold(m_space, 25.0);     // Can set this manually, but also set automatically based on gravity

    // ***** Default gravity / damping settings
    m_gravity = cpv(0, -1000);                          // cpVect is a 2D vector and cpv() is a shortcut for initializing them
    m_damping = 1;                                      // Kind of like air drag
    m_friction = 0.8;
    m_bounce =   0.8;

    cpSpaceSetGravity(m_space, m_gravity);
    cpSpaceSetDamping(m_space, m_damping);
    setWorldInfo();

    // ***** Add Mouse Body that is used to join to objects to move them around with the mouse
    mouse_body = cpBodyNewKinematic();
    cpSpaceAddBody(m_space, mouse_body);


    // Static line segment shapes for the walls
    this->addLine(Body_Type::Static, Qt::gray, QPointF(-1000,  1000), QPointF( 1000,  1000), c_friction, c_bounce, 1);      // Top
    this->addLine(Body_Type::Static, Qt::gray, QPointF(-1000, -1000), QPointF( 1000, -1000), c_friction, c_bounce, 1);      // Bottom
    this->addLine(Body_Type::Static, Qt::gray, QPointF(-1000, -1000), QPointF(-1000,  1000), c_friction, c_bounce, 1);      // Left
    this->addLine(Body_Type::Static, Qt::gray, QPointF( 1000, -1000), QPointF( 1000,  1000), c_friction, c_bounce, 1);      // Right

    // Static line segment shapes for the ground
    this->addLine(Body_Type::Static, Qt::red, QPointF(-800,    0), QPointF( 300, -250), c_friction, c_bounce, 1);
    this->addLine(Body_Type::Static, Qt::red, QPointF( 250,   50), QPointF( 900,  250), c_friction, c_bounce, 1);
    this->addLine(Body_Type::Static, Qt::red, QPointF(-900, -300), QPointF(-500, -300), c_friction, c_bounce, 1);

    QColor ball_color =  Dr::GetColor(Window_Colors::Icon_Light);

    this->addCircle(Body_Type::Dynamic, ball_color,    0,   0, 0, 50, c_friction, c_bounce, true, true);

    this->addCircle(Body_Type::Dynamic, ball_color, -900, 900, 0, 50, c_friction, c_bounce, true, true);
    this->addCircle(Body_Type::Dynamic, ball_color, -800, 900, 0, 50, c_friction, c_bounce, true, true);
    this->addCircle(Body_Type::Dynamic, ball_color, -700, 900, 0, 50, c_friction, c_bounce, true, true);
    this->addCircle(Body_Type::Dynamic, ball_color, -600, 900, 0, 50, c_friction, c_bounce, true, true);
    this->addCircle(Body_Type::Dynamic, ball_color, -500, 900, 0, 50, c_friction, c_bounce, true, true);
    this->addCircle(Body_Type::Dynamic, ball_color, -400, 900, 0, 50, c_friction, c_bounce, true, true);
    this->addCircle(Body_Type::Dynamic, ball_color, -300, 900, 0, 50, c_friction, c_bounce, true, true);
    this->addCircle(Body_Type::Dynamic, ball_color, -200, 900, 0, 50, c_friction, c_bounce, true, true);
    this->addCircle(Body_Type::Dynamic, ball_color, -100, 900, 0, 50, c_friction, c_bounce, true, true);
    this->addCircle(Body_Type::Dynamic, ball_color,    0, 900, 0, 50, c_friction, c_bounce, true, true);
    this->addCircle(Body_Type::Dynamic, ball_color,  100, 900, 0, 50, c_friction, c_bounce, true, true);
    this->addCircle(Body_Type::Dynamic, ball_color,  200, 900, 0, 50, c_friction, c_bounce, true, true);
    this->addCircle(Body_Type::Dynamic, ball_color,  300, 900, 0, 50, c_friction, c_bounce, true, true);
    this->addCircle(Body_Type::Dynamic, ball_color,  400, 900, 0, 50, c_friction, c_bounce, true, true);
    this->addCircle(Body_Type::Dynamic, ball_color,  500, 900, 0, 50, c_friction, c_bounce, true, true);
    this->addCircle(Body_Type::Dynamic, ball_color,  600, 900, 0, 50, c_friction, c_bounce, true, true);
    this->addCircle(Body_Type::Dynamic, ball_color,  700, 900, 0, 50, c_friction, c_bounce, true, true);
    this->addCircle(Body_Type::Dynamic, ball_color,  800, 900, 0, 50, c_friction, c_bounce, true, true);
    this->addCircle(Body_Type::Dynamic, ball_color,  900, 900, 0, 50, c_friction, c_bounce, true, true);

    QColor box_color =  Dr::GetColor(Window_Colors::Text);

    this->addBlock(Body_Type::Dynamic, box_color, -900, 800, 0,  50,  50, c_friction, c_bounce, true, true);
    this->addBlock(Body_Type::Dynamic, box_color, -700, 800, 0, 100, 100, c_friction, c_bounce, true, true);
    this->addBlock(Body_Type::Dynamic, box_color, -500, 800, 0,  50,  50, c_friction, c_bounce, true, true);
    this->addBlock(Body_Type::Dynamic, box_color, -300, 800, 0,  50,  50, c_friction, c_bounce, true, true);
    this->addBlock(Body_Type::Dynamic, box_color, -100, 800, 0, 100, 100, c_friction, c_bounce, true, true);
    this->addBlock(Body_Type::Dynamic, box_color,  100, 800, 0,  50,  50, c_friction, c_bounce, true, true);
    this->addBlock(Body_Type::Dynamic, box_color,  300, 800, 0,  50,  50, c_friction, c_bounce, true, true);
    this->addBlock(Body_Type::Dynamic, box_color,  500, 800, 0, 100, 100, c_friction, c_bounce, true, true);
    this->addBlock(Body_Type::Dynamic, box_color,  700, 800, 0,  50,  50, c_friction, c_bounce, true, true);
    this->addBlock(Body_Type::Dynamic, box_color,  900, 800, 0,  50,  50, c_friction, c_bounce, true, true);

    // Mark Space as Built
    has_scene = true;
}


//######################################################################################################
//##    Populates World Info Label
//######################################################################################################
void DrPlayground::setWorldInfo() {
    QString info;
    info += "<center><b>World Info</b></center><br>";
    info += "<b>Time Warp</b>  x " + QString::number(m_time_warp) + "<br>";
    info += "<b>Gavity X:</b> " + QString::number(m_gravity.x) + ", <b>Y:</b> " + QString::number(m_gravity.y) + "<br>";
    info += "<b>Damping:</b> " + QString::number(m_damping) + "<br>";
    info += "<b>Friction:</b> " + QString::number(m_friction) + "<br>";
    info += "<b>Bounce:</b> " + QString::number(m_bounce) + "<br>";
    m_form_playground->setWorldInfo(info);
}














