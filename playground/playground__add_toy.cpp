//
//      Created by Stephens Nunnally on 6/15/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QtMath>
#include <QRectF>

#include "playground/form_playground.h"
#include "playground/playground.h"
#include "playground/playground_toy.h"


// Internal Constants
constexpr double c_extra_radius =    0.010;         // Radius added on to block and polygon shapes for better collisions
constexpr double c_mass_multiplier = 0.002;         // Shapes Area times this multiplier = shape mass

//####################################################################################
//##    Add Line
//####################################################################################
DrToy* DrPlayground::addLine(Body_Type body_type, QColor color, QPointF p1, QPointF p2, double friction, double bounce, double mass) {
    DrToy *line = new DrToy(this);

    // Check for default (-1) or custom friction / bounce settings
    line->m_custom_friction = friction;
    line->m_custom_bounce = bounce;
    if (friction < 0) friction = m_friction;
    if (bounce < 0)   bounce = m_bounce;

    cpVect a, b;
    a.x = p1.x();   a.y = p1.y();
    b.x = p2.x();   b.y = p2.y();
    double moment = cpMomentForSegment( mass, a, b, 2);

    line->body_type = body_type;
    switch (body_type) {
        case Body_Type::Static:      line->body = cpBodyNewStatic();             break;
        case Body_Type::Dynamic:     line->body = cpBodyNew( mass, moment );     break;
        case Body_Type::Kinematic:   line->body = cpBodyNewKinematic();          break;
    }
    cpSpaceAddBody(m_space, line->body);
    cpBodySetUserData(line->body, line);                        // Set chipmunk User Data, store DrEngineObject for later

    if (body_type != Body_Type::Static) {
        QPointF center = QRectF( p1, p2 ).center();
        cpVect cv;
        cv.x = center.x();  cv.y = center.y();
        cpBodySetPosition( line->body, cv );
    }

    cpShape *shape = cpSegmentShapeNew( line->body, cpv( p1.x(), p1.y()), cpv(p2.x(), p2.y()), 2) ;
    cpShapeSetFriction( shape, friction );
    cpShapeSetElasticity( shape, bounce );
    cpSpaceAddShape( m_space, shape );
    cpShapeSetUserData( shape, line );                                          // Set UserData to DrToy pointer

    // Save shape
    line->shape = shape;
    line->shape_type = Shape_Type::Segment;
    line->graphic = m_form_playground->addGraphicsLine(line, color);            // Create QGraphicItem for this line

    // Add line to list of objects in Space
    objects.append( line );
    return line;
}



//####################################################################################
//##    Add Circle
//##        Pass -1 for friction and/or bounce to use default world friction and bounce settings
//####################################################################################
DrToy* DrPlayground::addCircle(Body_Type body_type, QColor color, double x, double y, double angle, double shape_radius,
                               double friction, double bounce, bool should_collide, bool can_rotate) {
    DrToy *ball = new DrToy(this);

    // Check for default (-1) or custom friction / bounce settings
    ball->m_custom_friction = friction;
    ball->m_custom_bounce = bounce;
    if (friction < 0) friction = m_friction;
    if (bounce < 0)   bounce = m_bounce;

    // Ball Basics
    double radius = shape_radius;                                               // Radius of collision shape
    ball->m_position = QPointF(x, y);

    // Create the body for the ball
    ball->body_type = body_type;
    switch (body_type) {
        case Body_Type::Static:     ball->body = cpBodyNewStatic();                 break;
        case Body_Type::Dynamic:    ball->body = cpBodyNew( 0.0, 0.0 );             break;
        case Body_Type::Kinematic:  ball->body = cpBodyNewKinematic();              break;
    }
    cpSpaceAddBody(m_space, ball->body);
    cpBodySetUserData(ball->body, ball);                                        // Set UserData to DrToy pointer

    cpBodySetPosition( ball->body, cpv( x, y));                                 // Coordinate is center of object
    cpBodySetAngle(    ball->body, qDegreesToRadians(-angle) );

    // Create the collision shape for the ball
    cpShape *shape = cpCircleShapeNew(ball->body, radius, cpv(0, 0));

    // By setting mass of shape, body mass and moment of inertia are calculated for us
    double area = cpAreaForCircle( 0, radius ) * c_mass_multiplier;
    cpShapeSetMass( shape, area );
    cpShapeSetFriction( shape, friction  );
    cpShapeSetElasticity( shape, bounce );
    cpSpaceAddShape( m_space, shape );
    cpShapeSetUserData( shape, ball );                                          // Set UserData to DrToy pointer

    // Save shape
    ball->shape = shape;
    ball->shape_type = Shape_Type::Circle;

    // If we don't want the object to collide with other objects, set as sensor
    ball->m_does_collide = should_collide;
    cpShapeSetSensor( shape, !should_collide );

    // If we don't want the body to rotate, overwrite the precalculated moment of inertia with infinity
    ball->m_can_rotate = can_rotate;
    if (!can_rotate) cpBodySetMoment( ball->body, static_cast<double>(INFINITY) );

    // Add QGraphicItem for this circle
    ball->graphic = m_form_playground->addGraphicsCircle(ball, color);          // Create QGraphicItem for this circle

    // Add ball to the list of objects in the current Space
    objects.append( ball );
    return ball;
}


//####################################################################################
//##    Add Block
//##        Pass -1 for friction and/or bounce to use default world friction and bounce settings
//####################################################################################
DrToy* DrPlayground::addBlock(Body_Type body_type, QColor color, double x, double y, double angle, double width, double height,
                              double friction, double bounce, bool should_collide, bool can_rotate) {
    DrToy *block = new DrToy(this);

    // Check for default (-1) or custom friction / bounce settings
    block->m_custom_friction = friction;
    block->m_custom_bounce = bounce;
    if (friction < 0) friction = m_friction;
    if (bounce < 0)   bounce = m_bounce;

    block->m_width = width;
    block->m_height = height;
    block->m_position = QPointF(x, y);

    // Create the body for the block
    block->body_type = body_type;
    switch (body_type) {
        case Body_Type::Static:     block->body = cpBodyNewStatic();                break;
        case Body_Type::Dynamic:    block->body = cpBodyNew( 0.0, 0.0 );            break;
        case Body_Type::Kinematic:  block->body = cpBodyNewKinematic();             break;
    }
    cpSpaceAddBody(m_space, block->body);
    cpBodySetUserData(block->body, block);                                          // Set UserData to DrToy pointer

    cpBodySetPosition( block->body, cpv( x, y));                                    // Coordinate is center of object
    cpBodySetAngle(    block->body, qDegreesToRadians(-angle) );

    // Create the collision shape for the block
    cpShape *shape = cpBoxShapeNew(block->body, width, height, c_extra_radius);

    // By setting mass of shape, body mass and moment of inertia are calculated for us
    double area = ( width * height ) * c_mass_multiplier;
    cpShapeSetMass( shape, area );
    cpShapeSetFriction( shape, friction );
    cpShapeSetElasticity( shape, bounce );
    cpSpaceAddShape( m_space, shape );
    cpShapeSetUserData( shape, block );                                             // Set UserData to DrToy pointer

    // Add shape to the list of shapes for this body
    block->shape = shape;
    block->shape_type = Shape_Type::Box;

    // If we don't want the object to collide with other objects, set as sensor
    block->m_does_collide = should_collide;
    cpShapeSetSensor( shape, !should_collide );

    // If we don't want the body to rotate, overwrite the precalculated moment of inertia with infinity
    block->m_can_rotate = can_rotate;
    if (!can_rotate) cpBodySetMoment( block->body, static_cast<double>(INFINITY) );

    // Add QGraphicItem for this box
    block->graphic = m_form_playground->addGraphicsBox(block, color);              // Create QGraphicItem for this block

    // Add block to the list of objects in the current Space
    objects.append( block );
    return block;
}


























