//
//      Created by Stephens Nunnally on 5/6/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "engine.h"
#include "engine_texture.h"

//######################################################################################################
//##    Add Line
//######################################################################################################
SceneObject* Engine::addLine(BodyType body_type, QPointF p1, QPointF p2, double friction, double bounce, double mass) {
    SceneObject *line = new SceneObject();

    cpVect a, b;
    a.x = p1.x();   a.y = p1.y();
    b.x = p2.x();   b.y = p2.y();
    double moment = cpMomentForSegment( mass, a, b, 2);

    line->body_type = body_type;
    switch (body_type) {
        case BodyType::Static:      line->body = cpBodyNewStatic();             break;
        case BodyType::Dynamic:     line->body = cpBodyNew( mass, moment );     break;
        case BodyType::Kinematic:   line->body = cpBodyNewKinematic();          break;
    }
    cpSpaceAddBody(m_space, line->body);

    if (body_type != BodyType::Static) {
        QPointF center = QRectF( p1, p2 ).center();
        cpVect cv;
        cv.x = center.x();  cv.y = center.y();
        cpBodySetPosition( line->body, cv );
    }

    line->shape_type = ShapeType::Segment;
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
SceneObject* Engine::addCircle(BodyType body_type, long texture_number, double x, double y, double friction, double bounce, double mass, QPointF velocity) {
    SceneObject *ball = new SceneObject();

    // Ball Basics
    ball->texture_number = texture_number;
    cpFloat width =  m_textures[texture_number]->width() - (c_texture_border * 2);
    ///cpFloat height = txt->height() - (c_texture_border * 2);
    cpFloat radius = width / 2;
    ball->radius =   radius;
    cpFloat moment = cpMomentForCircle(mass, 0, radius, cpvzero);                       // The moment of inertia is like mass for rotation

    // Create the body for the ball
    ball->body_type = body_type;
    switch (body_type) {
        case BodyType::Static:      ball->body = cpBodyNewStatic();                 break;
        case BodyType::Dynamic:     ball->body = cpBodyNew( mass, moment );         break;
        case BodyType::Kinematic:   ball->body = cpBodyNewKinematic();              break;
    }
    cpSpaceAddBody(m_space, ball->body);
    cpBodySetPosition( ball->body, cpv( x, y));                                        // Coordinate is center of object
    cpBodySetVelocity( ball->body, cpv( velocity.x(), velocity.y()) );

    // Create the collision shape for the ball
    ball->shape_type = ShapeType::Circle;
    ball->shape = cpCircleShapeNew(ball->body, radius, cpvzero);
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
SceneObject* Engine::addBlock(BodyType body_type, long texture_number, double x, double y, double friction, double bounce, double mass, QPointF velocity) {
    SceneObject *block = new SceneObject();

    // Block basics
    block->texture_number = texture_number;
    cpFloat width =  m_textures[texture_number]->width() -  (c_texture_border * 2);
    cpFloat height = m_textures[texture_number]->height() - (c_texture_border * 2);
    block->radius =  width / 2;
    cpFloat moment = cpMomentForBox( mass, width, height);

    // Create the body for the block
    block->body_type = body_type;
    switch (body_type) {
        case BodyType::Static:      block->body = cpBodyNewStatic();                break;
        case BodyType::Dynamic:     block->body = cpBodyNew( mass, moment );        break;
        case BodyType::Kinematic:   block->body = cpBodyNewKinematic();             break;
    }
    cpSpaceAddBody(m_space, block->body);
    cpBodySetPosition( block->body, cpv( x, y));                                        // Coordinate is center of object
    cpBodySetVelocity( block->body, cpv( velocity.x(), velocity.y()) );

    // Create the collision shape for the block
    block->shape_type = ShapeType::Box;
    block->shape = cpBoxShapeNew(block->body, width, height, .01);
    cpSpaceAddShape(m_space, block->shape);
    cpShapeSetFriction(   block->shape, friction );
    cpShapeSetElasticity( block->shape, bounce );

    block->in_scene = true;
    objects.append( block );
    return block;
}


//######################################################################################################
//##    Add Flower
//######################################################################################################
SceneObject* Engine::addPolygon(BodyType body_type, long texture_number, double x, double y, QVector<QPointF> points,
                                      double friction, double bounce, double mass, QPointF velocity) {
    SceneObject *polygon = new SceneObject();

    // Polygon Basics
    polygon->texture_number = texture_number;
    ///cpFloat width =  txt->width() -  (c_texture_border * 2);
    ///cpFloat height = txt->height() - (c_texture_border * 2);

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
        case BodyType::Static:      polygon->body = cpBodyNewStatic();              break;
        case BodyType::Dynamic:     polygon->body = cpBodyNew( mass, moment );      break;
        case BodyType::Kinematic:   polygon->body = cpBodyNewKinematic();           break;
    }
    cpSpaceAddBody(m_space, polygon->body);
    cpBodySetPosition( polygon->body, cpv( x, y));                                        // Coordinate is center of object
    cpBodySetVelocity( polygon->body, cpv( velocity.x(), velocity.y()) );

    // Create the collision shape for the flower
    polygon->shape_type = ShapeType::Polygon;
    polygon->shape = cpPolyShapeNew( polygon->body, points.count(), verts.data(), cpTransformIdentity, .01);
    cpSpaceAddShape(m_space, polygon->shape);

    cpShapeSetFriction(   polygon->shape, friction );
    cpShapeSetElasticity( polygon->shape, bounce );

    polygon->in_scene = true;
    objects.append( polygon );
    return polygon;
}






