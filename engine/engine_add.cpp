//
//      Created by Stephens Nunnally on 5/6/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QtMath>
#include "engine.h"
#include "engine_texture.h"

//######################################################################################################
//##    Add Line
//######################################################################################################
SceneObject* DrEngine::addLine(Body_Type body_type, QPointF p1, QPointF p2, double friction, double bounce, double mass) {
    SceneObject *line = new SceneObject();

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

    if (body_type != Body_Type::Static) {
        QPointF center = QRectF( p1, p2 ).center();
        cpVect cv;
        cv.x = center.x();  cv.y = center.y();
        cpBodySetPosition( line->body, cv );
    }

    line->shape_type = Shape_Type::Segment;
    line->shape = cpSegmentShapeNew( line->body, cpv( p1.x(), p1.y()), cpv(p2.x(), p2.y()), 2) ;
    cpShapeSetFriction(     line->shape, friction );
    cpShapeSetElasticity(   line->shape, bounce );          // Ideally between 0 and .99999
    cpSpaceAddShape( m_space, line->shape );

    line->in_scene = true;
    objects.append( line );
    return line;
}


//######################################################################################################
//##    Add Circle
//######################################################################################################
SceneObject* DrEngine::addCircle(Body_Type body_type, long texture_number, double x, double y, double z, double opacity,
                                 double friction, double bounce, double mass, QPointF velocity, bool can_rotate) {
    SceneObject *ball = new SceneObject();

    //  !!!!! TEMP: Should be passed in by function to allow for custom radius
    double radius = m_textures[texture_number]->width() / 2;    // Radius of collision shape
    cpVect offset = cpv(0, 0);                                  // Offset of collision shape

    // Ball Basics
    ball->texture_number = texture_number;
    ball->z_order = z;
    ball->alpha = static_cast<float>(opacity);

    // If we dont want an object to rotate, set moment of inertia to INFINITY
    cpFloat moment;
    if (can_rotate)
        moment = cpMomentForCircle(mass, 0, radius, offset);                     // The moment of inertia is like mass for rotation
    else
        moment = static_cast<double>(INFINITY);

    // Create the body for the ball
    ball->body_type = body_type;
    switch (body_type) {
        case Body_Type::Static:     ball->body = cpBodyNewStatic();                break;
        case Body_Type::Dynamic:    ball->body = cpBodyNew( mass, moment );        break;
        case Body_Type::Kinematic:  ball->body = cpBodyNewKinematic();             break;
    }
    cpSpaceAddBody(m_space, ball->body);
    cpBodySetPosition( ball->body, cpv( x, y));                                         // Coordinate is center of object
    cpBodySetVelocity( ball->body, cpv( velocity.x(), velocity.y()) );

    // Create the collision shape for the ball
    ball->shape_type = Shape_Type::Circle;
    ball->shape = cpCircleShapeNew(ball->body, radius, offset);
    cpSpaceAddShape(m_space, ball->shape);
    cpShapeSetFriction(   ball->shape, friction  );
    cpShapeSetElasticity( ball->shape, bounce );

    ball->in_scene = true;
    objects.append( ball );
    return ball;
}


//######################################################################################################
//##    Add Block
//######################################################################################################
SceneObject* DrEngine::addBlock(Body_Type body_type, long texture_number, double x, double y, double z, double angle, QPointF scale, double opacity,
                                double friction, double bounce, double mass, QPointF velocity, bool should_collide) {
    SceneObject *block = new SceneObject();

    //  !!!!! TEMP: Should be passed in by function to allow for custom radius
    double width  = m_textures[texture_number]->width()  * scale.x();       // Width of collision shape
    double height = m_textures[texture_number]->height() * scale.y();       // Height of collision shape
    block->scale_x = static_cast<float>(scale.x());
    block->scale_y = static_cast<float>(scale.y());

    // Block basics
    block->texture_number = texture_number;
    block->z_order = z;
    block->alpha = static_cast<float>(opacity);
    cpFloat moment = cpMomentForBox( mass, width, height);

    // Create the body for the block
    block->body_type = body_type;
    switch (body_type) {
        case Body_Type::Static:     block->body = cpBodyNewStatic();               break;
        case Body_Type::Dynamic:    block->body = cpBodyNew( mass, moment );       break;
        case Body_Type::Kinematic:  block->body = cpBodyNewKinematic();            break;
    }
    cpSpaceAddBody(m_space, block->body);
    cpBodySetPosition( block->body, cpv( x, y));                                        // Coordinate is center of object
    cpBodySetAngle(    block->body, qDegreesToRadians(-angle) );
    cpBodySetVelocity( block->body, cpv( velocity.x(), velocity.y()) );

    // Create the collision shape for the block
    if (should_collide == true) {
        block->shape_type = Shape_Type::Box;
        block->shape = cpBoxShapeNew(block->body, width, height, .01);
        cpSpaceAddShape(m_space, block->shape);
        cpShapeSetFriction(   block->shape, friction );
        cpShapeSetElasticity( block->shape, bounce );
    } else {
        block->collide = false;
    }

    block->in_scene = true;
    objects.append( block );
    return block;
}


//######################################################################################################
//##    Add Flower
//######################################################################################################
SceneObject* DrEngine::addPolygon(Body_Type body_type, long texture_number, double x, double y, double z, double opacity, QVector<QPointF> points,
                                      double friction, double bounce, double mass, QPointF velocity) {
    SceneObject *polygon = new SceneObject();

    // Polygon Basics
    polygon->texture_number = texture_number;
    polygon->z_order = z;
    polygon->alpha = static_cast<float>(opacity);

    std::vector<cpVect> verts;
    verts.clear();
    verts.resize( static_cast<ulong>(points.count()) );
    for (int i = 0; i < points.count(); i++) {
        verts[static_cast<ulong>(i)] = cpv( points[i].x(), points[i].y() );
    }
    cpFloat moment = cpMomentForPoly( mass, points.count(), verts.data(), cpvzero, 0 );

    // Create the body for the polygon
    polygon->body_type = body_type;
    switch (body_type) {
        case Body_Type::Static:     polygon->body = cpBodyNewStatic();             break;
        case Body_Type::Dynamic:    polygon->body = cpBodyNew( mass, moment );     break;
        case Body_Type::Kinematic:  polygon->body = cpBodyNewKinematic();          break;
    }
    cpSpaceAddBody(m_space, polygon->body);
    cpBodySetPosition( polygon->body, cpv( x, y));                                        // Coordinate is center of object
    cpBodySetVelocity( polygon->body, cpv( velocity.x(), velocity.y()) );

    // Create the collision shape for the flower
    polygon->shape_type = Shape_Type::Polygon;
    polygon->shape = cpPolyShapeNew( polygon->body, points.count(), verts.data(), cpTransformIdentity, .01);
    cpSpaceAddShape(m_space, polygon->shape);

    cpShapeSetFriction(   polygon->shape, friction );
    cpShapeSetElasticity( polygon->shape, bounce );

    polygon->in_scene = true;
    objects.append( polygon );
    return polygon;
}






