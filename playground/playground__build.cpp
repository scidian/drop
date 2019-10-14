//
//      Created by Stephens Nunnally on 6/15/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "colors/colors.h"
#include "editor/tree_inspector.h"
#include "form_playground.h"
#include "helper.h"
#include "helper_qt.h"
#include "playground.h"
#include "playground_toy.h"


//####################################################################################
//##    Builds Playground World
//####################################################################################
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

    // ***** Add Mouse Body that is used to join to objects to move them around with the mouse
    mouse_body = cpBodyNewKinematic();
    cpSpaceAddBody(m_space, mouse_body);


    // Static line segment shapes for the walls
    this->addLine(Body_Type::Static, Qt::gray, QPointF(-1000,  1000), QPointF( 1000,  1000), c_friction, c_bounce, 1);      // Top
    this->addLine(Body_Type::Static, Qt::gray, QPointF(-1000, -1000), QPointF( 1000, -1000), c_friction, c_bounce, 1);      // Bottom
    this->addLine(Body_Type::Static, Qt::gray, QPointF(-1000, -1000), QPointF(-1000,  1000), c_friction, c_bounce, 1);      // Left
    this->addLine(Body_Type::Static, Qt::gray, QPointF( 1000, -1000), QPointF( 1000,  1000), c_friction, c_bounce, 1);      // Right

    // Static line segment shapes for the ground
    this->addLine(Body_Type::Static, Qt::red, QPointF(-750,    0), QPointF( 300, -250), c_friction, c_bounce, 1);
    this->addLine(Body_Type::Static, Qt::red, QPointF( 250,   50), QPointF( 900,  250), c_friction, c_bounce, 1);
    this->addLine(Body_Type::Static, Qt::red, QPointF(-900, -400), QPointF(-500, -400), c_friction, c_bounce, 1);

    QColor ball_color =  Dr::GetColor(Window_Colors::Icon_Light);

    this->addCircle(Body_Type::Dynamic, ball_color,    0,   0, 0, 50, c_friction, c_bounce, true, true);
    this->addCircle(Body_Type::Dynamic, ball_color, -800, 900, 0, 50, c_friction, c_bounce, true, true);
    this->addCircle(Body_Type::Dynamic, ball_color, -600, 900, 0, 50, c_friction, c_bounce, true, true);
    this->addCircle(Body_Type::Dynamic, ball_color, -400, 900, 0, 50, c_friction, c_bounce, true, true);
    this->addCircle(Body_Type::Dynamic, ball_color, -200, 900, 0, 50, c_friction, c_bounce, true, true);
    this->addCircle(Body_Type::Dynamic, ball_color,    0, 900, 0, 50, c_friction, c_bounce, true, true);
    this->addCircle(Body_Type::Dynamic, ball_color,  200, 900, 0, 50, c_friction, c_bounce, true, true);
    this->addCircle(Body_Type::Dynamic, ball_color,  400, 900, 0, 50, c_friction, c_bounce, true, true);
    this->addCircle(Body_Type::Dynamic, ball_color,  600, 900, 0, 50, c_friction, c_bounce, true, true);
    this->addCircle(Body_Type::Dynamic, ball_color,  800, 900, 0, 50, c_friction, c_bounce, true, true);

    QColor box_color =  Dr::GetColor(Window_Colors::Text);

    this->addBlock(Body_Type::Dynamic, box_color, -700, 800, 0, 100, 100, c_friction, c_bounce, true, true);
    this->addBlock(Body_Type::Dynamic, box_color, -500, 800, 0,  50,  50, c_friction, c_bounce, true, true);
    this->addBlock(Body_Type::Dynamic, box_color, -300, 800, 0,  50,  50, c_friction, c_bounce, true, true);
    this->addBlock(Body_Type::Dynamic, box_color, -100, 800, 0, 100, 100, c_friction, c_bounce, true, true);
    this->addBlock(Body_Type::Dynamic, box_color,  100, 800, 0,  50,  50, c_friction, c_bounce, true, true);
    this->addBlock(Body_Type::Dynamic, box_color,  300, 800, 0,  50,  50, c_friction, c_bounce, true, true);
    this->addBlock(Body_Type::Dynamic, box_color,  500, 800, 0, 100, 100, c_friction, c_bounce, true, true);
    this->addBlock(Body_Type::Dynamic, box_color,  700, 800, 0,  50,  50, c_friction, c_bounce, true, true);

    // Mark Space as Built
    has_scene = true;

    setWorldInfo();
}


//####################################################################################
//##    Populates World Info Label
//####################################################################################
void DrPlayground::setWorldInfo() {
    m_form_playground->m_spin_time_warp->blockSignals(true);
    m_form_playground->m_spin_time_warp->setValue(m_time_warp);
    m_form_playground->m_spin_time_warp->blockSignals(false);

    m_form_playground->m_spin_gravity_x->blockSignals(true);
    m_form_playground->m_spin_gravity_x->setValue(m_gravity.x);
    m_form_playground->m_spin_gravity_x->blockSignals(false);

    m_form_playground->m_spin_gravity_y->blockSignals(true);
    m_form_playground->m_spin_gravity_y->setValue(m_gravity.y);
    m_form_playground->m_spin_gravity_y->blockSignals(false);

    m_form_playground->m_world_info_damping->setText( "<b>Damping:</b> " +  Dr::RemoveTrailingDecimals(m_damping, 3));

    m_form_playground->m_world_info_friction->setText("<b>Friction:</b> " + Dr::RemoveTrailingDecimals(m_friction, 3));
    m_form_playground->m_world_info_bounce->setText(  "<b>Bounce:</b> " +   Dr::RemoveTrailingDecimals(m_bounce, 3));
}














