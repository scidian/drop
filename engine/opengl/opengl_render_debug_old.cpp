//
//      Created by Stephens Nunnally on 5/7/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Draws the debug shapes
//
//
#include <QtMath>
#include <QBrush>
#include <QColor>
#include <QPainter>

#include <cmath>

#include "engine/engine.h"
#include "engine/engine_texture.h"
#include "engine/form_engine.h"
#include "engine/opengl/opengl.h"
#include "engine/things/engine_thing_object.h"
#include "engine/world/engine_world.h"


//####################################################################################
//##    Chipmunk Callbacks
//##        Grabs data from Space
//####################################################################################
// Used for Arbiter iterator to get a list of all arbiters (collision points) attached to a body
static void GetBodyContactPoints(cpBody *, cpArbiter *arb, std::vector<QPointF> *point_list) {
    cpContactPointSet contact = cpArbiterGetContactPointSet( arb );
    point_list->push_back( QPointF( contact.points->pointA.x, contact.points->pointA.y ) );
}
// Used for Arbiter iterator to get a list of Normals (angles) of all arbiters (collision points) attached to a body
static void GetBodyContactNormals(cpBody *, cpArbiter *arb, std::vector<cpVect> *normal_list) {
    cpVect normal = cpArbiterGetNormal( arb );
    normal_list->push_back( normal );
}
// Used for constraint iterator to get a list of all Constraints (joints) in cpSpace
static void GetSpaceJointList(cpConstraint *constraint, std::vector<cpConstraint*> *joint_list) {
    joint_list->push_back(constraint);
}


//####################################################################################
//##    Draws the Collision Shapes using QPainter
//####################################################################################
void DrOpenGL::paintDebugShapes(QPainter &painter) {

    int open_gl_width =  width() *  devicePixelRatio();
    int open_gl_height = height() * devicePixelRatio();
    QRect open_gl_rect = QRect(0, 0, open_gl_width, open_gl_height);

    for (auto thing : m_engine->getCurrentWorld()->getThings()) {
        if (thing->getThingType() != DrThingType::Object) continue;
        DrEngineObject *object = dynamic_cast<DrEngineObject*>(thing);

        // Figure out what color to make the debug shapes
        DrColor color = objectDebugColor(object->getCollisionType(), false, cpBodyIsSleeping(object->body));

        if (object->isDying() || !object->isAlive()) color = Qt::gray;
        if (!object->doesCollide()) color = color.lighter();

        // Set up QPainter
        QPen cosmetic_pen( QBrush(QColor(color.red(), color.green(), color.blue())), 1);
        cosmetic_pen.setCosmetic(true);
        painter.setPen( cosmetic_pen );
        QColor brush_color = QColor(color.red(), color.green(), color.blue());
        brush_color.setAlpha(64);
        painter.setBrush( QBrush( brush_color));

        // Load Object Position
        DrPointF center = object->getPosition();

        // Used to store combined polygon of a multi-shape body
        QPolygonF object_poly;

        for (auto shape : object->shapes) {

            if (object->shape_type[shape] == Shape_Type::Circle) {
                cpVect offset = cpCircleShapeGetOffset(shape);
                double radius = cpCircleShapeGetRadius(shape);
                QTransform t = QTransform().translate(center.x, center.y).rotate(object->getAngle());

                // Draw Normal circle since Ortho does't skew angles
                if (m_engine->getCurrentWorld()->render_type == Render_Type::Orthographic) {
                    QPointF mid =   t.map( QPointF( offset.x, offset.y ) );
                    QPointF top =   t.map( QPointF( offset.x, offset.y + radius ));
                    QPointF l1 = mapToScreen( top.x(), top.y(), 0);
                    QPointF l2 = mapToScreen( mid.x(), mid.y(), 0);
                    double new_radius = QLineF(l1, l2).length();

                    // Don't draw if not touching or inside of visible area
                    QRect bounding_box = QRectF( l2.x() - new_radius, l2.y() - new_radius, new_radius * 2, new_radius * 2).toRect();
                    if ((open_gl_rect.intersects(bounding_box) || open_gl_rect.contains(bounding_box)) &&
                        (bounding_box.width() * 0.1 < open_gl_width) && (bounding_box.height() * 0.1 < open_gl_height)) {
                        // Draw circle
                        painter.drawEllipse( l2, new_radius, new_radius );
                        // Draw orientation line
                        painter.drawLine( l1, l2 );
                    }

                // Draw skewed circle with QPainterPath using bezier curves
                } if (m_engine->getCurrentWorld()->render_type == Render_Type::Perspective) {
                    QPointF mid =   t.map(QPointF( offset.x, offset.y ));
                    QPointF top =   t.map(QPointF( offset.x, offset.y + radius ));
                    QPointF bot =   t.map(QPointF( offset.x, offset.y - radius ));
                    QPointF left =  t.map(QPointF( offset.x - radius, offset.y ));
                    QPointF right = t.map(QPointF( offset.x + radius, offset.y ));
                    QPointF tl =    t.map(QPointF( (offset.x - radius) * .92, (offset.y + radius) * .92 ));
                    QPointF tr =    t.map(QPointF( (offset.x + radius) * .92, (offset.y + radius) * .92 ));
                    QPointF bl =    t.map(QPointF( (offset.x - radius) * .92, (offset.y - radius) * .92 ));
                    QPointF br =    t.map(QPointF( (offset.x + radius) * .92, (offset.y - radius) * .92 ));

                    mid =   mapToScreen( mid.x(),   mid.y(),   0);
                    top =   mapToScreen( top.x(),   top.y(),   0);
                    bot =   mapToScreen( bot.x(),   bot.y(),   0);
                    left =  mapToScreen( left.x(),  left.y(),  0);
                    right = mapToScreen( right.x(), right.y(), 0);
                    tl =    mapToScreen( tl.x(), tl.y(), 0);
                    tr =    mapToScreen( tr.x(), tr.y(), 0);
                    bl =    mapToScreen( bl.x(), bl.y(), 0);
                    br =    mapToScreen( br.x(), br.y(), 0);

                    QPainterPath path(top);
                    path.quadTo(tl, left);
                    path.quadTo(bl, bot);
                    path.quadTo(br, right);
                    path.quadTo(tr, top);

                    // Don't draw if not touching or inside of visible area
                    QRect bounding_box = path.boundingRect().normalized().toRect();
                    if ((open_gl_rect.intersects(bounding_box) || open_gl_rect.contains(bounding_box)) &&
                        (bounding_box.width() * 0.1 < open_gl_width) && (bounding_box.height() * 0.1 < open_gl_height)) {
                        // Draw Circle
                        painter.drawPath( path );
                        // Draw orientation line
                        painter.drawLine( top, mid );
                    }
                }


            } else if (object->shape_type[shape] == Shape_Type::Segment) {
                cpVect a = cpSegmentShapeGetA( shape );
                cpVect b = cpSegmentShapeGetB( shape );
                QPointF p1 = QPointF( a.x, a.y );
                QPointF p2 = QPointF( b.x, b.y );

                p1 = mapToScreen( p1.x(), p1.y(), 0.0 );
                p2 = mapToScreen( p2.x(), p2.y(), 0.0 );

                // Don't draw if not touching or inside of visible area
                QRect bounding_box = QRectF(p1, p2).toRect().normalized();
                if ((open_gl_rect.intersects(bounding_box) || open_gl_rect.contains(bounding_box)) &&
                    (bounding_box.width() * 0.1 < open_gl_width) && (bounding_box.height() * 0.1 < open_gl_height)) {
                    QPen line_pen( QBrush(QColor(color.red(), color.green(), color.blue())), 2);
                    painter.setPen( line_pen );
                    painter.drawLine(p1, p2);
                }

            } else if (object->shape_type[shape] == Shape_Type::Polygon || object->shape_type[shape] == Shape_Type::Box) {

                QTransform t = QTransform().translate(center.x, center.y).rotate(object->getAngle());
                QPolygonF polygon, mapped;
                for (int i = 0; i < cpPolyShapeGetCount( shape ); i++) {
                    cpVect  vert  = cpPolyShapeGetVert( shape, i );
                    polygon.append( QPointF( vert.x, vert.y ));

                    QPointF trans = t.map( polygon[i] );
                    mapped.append( mapToScreen(trans.x(), trans.y(), 0));
                }

                // Don't draw if not touching or inside of visible area
                QRect bounding_box = mapped.boundingRect().toRect();

                if ((open_gl_rect.intersects(bounding_box) || open_gl_rect.contains(bounding_box)) &&
                    (bounding_box.width() * 0.1 < open_gl_width) && (bounding_box.height() * 0.1 < open_gl_height)) {

                    // Draw orientation line
                    if (object->shape_type[shape] == Shape_Type::Box) {
                        QPointF mid = t.map(QPointF( 0, 0));
                        QPointF top = t.map(QPointF( 0, polygon.boundingRect().height() / 2 ));
                        QPointF l1 = mapToScreen( mid.x(),  mid.y(),  0);
                        QPointF l2 = mapToScreen( top.x(),  top.y(),  0);
                        painter.drawPolygon( mapped );
                        painter.drawLine( l1, l2 );
                    } else {

                        // If shape doesnt touch another shape, draw shapes we combined so far and reset to new shape
                        if (mapped.intersects(object_poly) == false) {
                            painter.drawPolygon( object_poly );
                            object_poly = mapped;
                        } else {
                            object_poly = object_poly.united( mapped );
                        }

                    }
                }

            }   // End If

        }   // End For shape

        // ***** Draw a multi shape Shape_Type::Polygon
        if (!object_poly.isEmpty()) {
            painter.drawPolygon( object_poly );
        }

    }   // End For object
}



//####################################################################################
//##    Draws the active Constraints using QPainter
//####################################################################################
void DrOpenGL::paintDebugJoints(QPainter &painter) {

    QPen pen( QBrush(QColor(255, 255, 255)), 1.25 * static_cast<double>(combinedZoomScale()), Qt::SolidLine, Qt::PenCapStyle::RoundCap);
    painter.setPen( pen );

    // Get a list of the constraints in the Space
    std::vector<cpConstraint*> joint_list;
    joint_list.clear();
    cpSpaceEachConstraint(m_engine->getCurrentWorld()->getSpace(), cpSpaceConstraintIteratorFunc(GetSpaceJointList), &joint_list);

    // Draw the Joints
    for (auto joint : joint_list) {
        cpBody *body_a = cpConstraintGetBodyA( joint );
        cpBody *body_b = cpConstraintGetBodyB( joint );

        // Grab DrEngineObjects from cpUserData
        DrEngineObject *object_a = static_cast<DrEngineObject*>(cpBodyGetUserData(body_a));
        DrEngineObject *object_b = static_cast<DrEngineObject*>(cpBodyGetUserData(body_b));

        // Load Object Positions
        DrPointF center_a = object_a->getPosition();
        DrPointF center_b = object_b->getPosition();
        QPointF  l1 = mapToScreen( center_a.x, center_a.y, 0);
        QPointF  l2 = mapToScreen( center_b.x, center_b.y, 0);

        painter.drawLine( l1, l2 );
    }
}













