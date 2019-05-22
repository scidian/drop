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
#include "helper.h"
#include "library/poly_partition.h"

constexpr double c_extra_radius = 0.01;

//######################################################################################################
//##    Check for custom Friction and Bounce (pass in to add*** object call as negative values)
//######################################################################################################
void DrEngine::checkObjectCustomFrictionBounce(SceneObject *object, double &friction, double &bounce) {
    if (friction < 0) {
        object->custom_friction = true;
        friction = abs(friction);
    }
    if (bounce < 0) {
        object->custom_bounce = true;
        bounce = abs(bounce);
    }
}


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

    cpShape *shape = cpSegmentShapeNew( line->body, cpv( p1.x(), p1.y()), cpv(p2.x(), p2.y()), 2) ;
    cpShapeSetFriction( shape, friction );
    cpShapeSetElasticity( shape, bounce );          // Ideally between 0 and .99999
    cpSpaceAddShape( m_space, shape );

    line->shapes.push_back( shape );
    line->shape_type[shape] = Shape_Type::Segment;

    line->should_process = true;
    objects.append( line );
    return line;
}


QVector<QPointF> createEllipseFromCircle(const QPointF &center, const double &radius) {
    QVector<QPointF> ellipse;
    int count = 18;
    for (int i = 0; i < count; i++) {
        QTransform t = QTransform().translate(center.x(), center.y()).rotate(i * 360.0 / count);
        QPointF point = t.map(QPointF( 0, radius));
        ellipse.append( point );
    }
    return ellipse;
}


//######################################################################################################
//##    Add Circle
//##        Pass negative friction and negative bounce to enable custom per item friction / bounce
//######################################################################################################
SceneObject* DrEngine::addCircle(Body_Type body_type, long texture_number, double x, double y, double z, double angle, QPointF scale, double opacity,
                                 double shape_radius, QPointF shape_offset, double friction, double bounce, double mass, QPointF velocity,
                                 bool should_collide, bool can_rotate) {
    // Check if not square scale, if so call addPolygon with a polygon ellipse instead
    if (qFuzzyCompare(scale.x(), scale.y()) == false) {
        QVector<QPointF> ellipse = createEllipseFromCircle(shape_offset, shape_radius);
        return addPolygon(body_type, texture_number, x, y, z, angle, scale, opacity, ellipse, friction, bounce, mass, velocity, should_collide, can_rotate);
    }

    // Otherwise continue with circle
    SceneObject *ball = new SceneObject();
    checkObjectCustomFrictionBounce(ball, friction, bounce);

    // Ball Basics
    double radius = shape_radius * scale.x();                                   // Radius of collision shape
    cpVect offset = cpv(shape_offset.x(), shape_offset.y());                    // Offset of collision shape
    ball->scale_x = static_cast<float>(scale.x());                              // Save x scale for later
    ball->scale_y = static_cast<float>(scale.y());                              // Save y scale for later
    ball->texture_number = texture_number;                                      // Texture to render from
    ball->z_order = z;
    ball->alpha = static_cast<float>(opacity);
    ball->position.setX(x);
    ball->position.setY(y);
    ball->last_position.setX(x);
    ball->last_position.setY(y);

    // If we dont want an object to rotate, set moment of inertia to INFINITY
    cpFloat moment;
    ball->can_rotate = can_rotate;
    if (can_rotate) moment = cpMomentForCircle(mass, 0, radius, offset);                    // The moment of inertia is like mass for rotation
    else            moment = static_cast<double>(INFINITY);

    // Create the body for the ball
    ball->body_type = body_type;
    switch (body_type) {
        case Body_Type::Static:     ball->body = cpBodyNewStatic();                break;
        case Body_Type::Dynamic:    ball->body = cpBodyNew( mass, moment );        break;
        case Body_Type::Kinematic:  ball->body = cpBodyNewKinematic();             break;
    }
    cpSpaceAddBody(m_space, ball->body);
    cpBodySetPosition( ball->body, cpv( x, y));                                 // Coordinate is center of object
    cpBodySetAngle(    ball->body, qDegreesToRadians(-angle) );
    cpBodySetVelocity( ball->body, cpv( velocity.x(), velocity.y()) );

    // Create the collision shape for the ball
    cpShape *shape = cpCircleShapeNew(ball->body, radius, offset);
    cpShapeSetFriction( shape, friction  );
    cpShapeSetElasticity( shape, bounce );
    cpSpaceAddShape(m_space, shape);

    ball->shapes.push_back( shape );
    ball->shape_type[shape] = Shape_Type::Circle;

    ball->does_collide = should_collide;
    cpShapeSetSensor( shape, !should_collide );

    ball->should_process = true;
    objects.append( ball );
    return ball;
}



//######################################################################################################
//##    Add Block - This is a TEMP call, better to just use addPolygon with 4 points passed in
//######################################################################################################
SceneObject* DrEngine::addBlock(Body_Type body_type, long texture_number, double x, double y, double z, double angle, QPointF scale, double opacity,
                                double friction, double bounce, double mass, QPointF velocity, bool should_collide, bool can_rotate) {
    SceneObject *block = new SceneObject();

    checkObjectCustomFrictionBounce(block, friction, bounce);

    double width  = m_textures[texture_number]->width() * scale.x();        // Width of collision shape
    double height = m_textures[texture_number]->height() * scale.y();       // Height of collision shape
    block->scale_x = static_cast<float>(scale.x());
    block->scale_y = static_cast<float>(scale.y());
    block->position.setX(x);
    block->position.setY(y);
    block->last_position.setX(x);
    block->last_position.setY(y);

    // Block basics
    block->texture_number = texture_number;
    block->z_order = z;
    block->alpha = static_cast<float>(opacity);

    cpFloat moment;
    block->can_rotate = can_rotate;
    if (can_rotate) moment = cpMomentForBox( mass, width, height);          // The moment of inertia is like mass for rotation
    else            moment = static_cast<double>(INFINITY);

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
    cpShape *shape = cpBoxShapeNew(block->body, width, height, c_extra_radius);
    cpShapeSetFriction( shape, friction );
    cpShapeSetElasticity( shape, bounce );
    cpSpaceAddShape(m_space, shape);

    block->shapes.push_back( shape );
    block->shape_type[shape] = Shape_Type::Box;

    block->does_collide = should_collide;
    cpShapeSetSensor( shape, !should_collide );

    block->should_process = true;
    objects.append( block );
    return block;
}


//######################################################################################################
//##    Add Polygon
//##        ***** NOTE: Vertices must be in COUNTER-CLOCKWISE ordering
//######################################################################################################
SceneObject* DrEngine::addPolygon(Body_Type body_type, long texture_number, double x, double y, double z, double angle, QPointF scale, double opacity,
                                  QVector<QPointF> points, double friction, double bounce, double mass, QPointF velocity,
                                  bool should_collide, bool can_rotate) {
    SceneObject *polygon = new SceneObject();

    checkObjectCustomFrictionBounce(polygon, friction, bounce);

    // Polygon Basics
    polygon->scale_x = static_cast<float>(scale.x());                              // Save x scale for later
    polygon->scale_y = static_cast<float>(scale.y());                              // Save y scale for later
    polygon->texture_number = texture_number;
    polygon->z_order = z;
    polygon->alpha = static_cast<float>(opacity);
    polygon->position.setX(x);
    polygon->position.setY(y);
    polygon->last_position.setX(x);
    polygon->last_position.setY(y);

    int old_point_count =static_cast<int>(points.size());

    std::vector<cpVect> verts;
    verts.clear();
    verts.resize( static_cast<ulong>(old_point_count) );
    for (int i = 0; i < old_point_count; i++)
        verts[static_cast<ulong>(i)] = cpv( points[i].x() * scale.x(), points[i].y() * scale.y());

    cpFloat moment;
    polygon->can_rotate = can_rotate;
    if (can_rotate) moment = cpMomentForPoly( mass, old_point_count, verts.data(), cpvzero, 0 );
    else            moment = static_cast<double>(INFINITY);

    // Create the body for the polygon
    polygon->body_type = body_type;
    switch (body_type) {
        case Body_Type::Static:     polygon->body = cpBodyNewStatic();             break;
        case Body_Type::Dynamic:    polygon->body = cpBodyNew( mass, moment );     break;
        case Body_Type::Kinematic:  polygon->body = cpBodyNewKinematic();          break;
    }
    cpSpaceAddBody(m_space, polygon->body);
    cpBodySetPosition( polygon->body, cpv( x, y));                                        // Coordinate is center of object
    cpBodySetAngle(    polygon->body, qDegreesToRadians(-angle) );
    cpBodySetVelocity( polygon->body, cpv( velocity.x(), velocity.y()) );

    // ***** Determine if polygon is concave, if it is create multiple shapes, otherwise create one shape
    std::list<TPPLPoly> testpolys, result;                              // Used by library Poly Partition
    TPPLPoly poly;
    poly.Init(old_point_count);
    for (int i = 0; i < old_point_count; i++) {
        poly[i].x = points[i].x() * scale.x();
        poly[i].y = points[i].y() * scale.y();
    }
    testpolys.push_back( poly );

    // Calculate the convex hull of a given set of points. Returns the count of points in the hull. Result must be a pointer to a cpVect array
    // with at least count elements. If result is NULL, then verts array wil be reduced instead. first is an optional pointer to an integer to store
    // where the first vertex in the hull came from (i.e. verts[first] == result[0]) tol is the allowed amount to shrink the hull when simplifying it.
    // A tolerance of 0.0 creates an exact hull.
    cpVect hull[99];
    int first = 0;
    int new_point_count = cpConvexHull(old_point_count, verts.data(), hull, &first, 0.0);

    // Shape is convex or could not determine convex hull
    if (new_point_count == 0) Dr::ShowMessageBox("Warning! Could not form convex hull!");
    cpShape *shape;
    if ((new_point_count == old_point_count || (new_point_count == 0))) {
        shape = cpPolyShapeNew( polygon->body, old_point_count, verts.data(), cpTransformIdentity, c_extra_radius);
        cpShapeSetFriction( shape, friction );
        cpShapeSetElasticity( shape, bounce );
        cpSpaceAddShape(m_space, shape);

        polygon->shapes.push_back( shape );
        polygon->shape_type[shape] = Shape_Type::Polygon;

        polygon->does_collide = should_collide;
        cpShapeSetSensor( shape, !should_collide );

    // Shape is concave
    } else {
        TPPLPartition pp;
        pp.ConvexPartition_OPT(&(*testpolys.begin()), &result);
        ///pp.ConvexPartition_HM(&testpolys, &result);

        for (auto poly : result) {
            std::vector<cpVect> verts;
            verts.clear();
            verts.resize( static_cast<ulong>( poly.GetNumPoints()) );
            for (int i = 0; i < poly.GetNumPoints(); i++)
                verts[static_cast<ulong>(i)] = cpv( poly[i].x, poly[i].y );
            shape = cpPolyShapeNew( polygon->body, static_cast<int>(poly.GetNumPoints()), verts.data(), cpTransformIdentity, c_extra_radius);
            cpShapeSetFriction( shape, friction );
            cpShapeSetElasticity( shape, bounce );
            cpSpaceAddShape(m_space, shape);

            polygon->shapes.push_back( shape );
            polygon->shape_type[shape] = Shape_Type::Polygon;

            polygon->does_collide = should_collide;
            cpShapeSetSensor( shape, !should_collide );
        }
    }

    polygon->should_process = true;
    objects.append( polygon );
    return polygon;
}











