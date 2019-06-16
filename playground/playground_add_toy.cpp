//
//      Created by Stephens Nunnally on 6/15/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QRectF>

#include "forms/form_playground.h"
#include "playground.h"
#include "playground_toy.h"

// Local Constants
constexpr double c_extra_radius =    0.010;         // Radius added on to block and polygon shapes for better collisions
constexpr double c_mass_multiplier = 0.002;         // Shapes Area times this multiplier = shape mass

//######################################################################################################
//##    Add Line
//######################################################################################################
DrToy* DrPlayground::addLine(Body_Type body_type, QColor color, QPointF p1, QPointF p2, double friction, double bounce, double mass) {
    DrToy *line = new DrToy();

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
    cpShapeSetElasticity( shape, bounce );                      // Ideally between 0 and .99999
    cpSpaceAddShape( m_space, shape );
    cpShapeSetUserData( shape, line );                          // Set UserData to DrToy pointer

    line->shape = shape;
    line->shape_type = Shape_Type::Segment;

    line->graphic = m_form_playground->addGraphicsLine(line, color);

    objects.append( line );
    return line;
}


















