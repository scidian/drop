//
//      Created by Stephens Nunnally on 5/7/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Draws the debug shapes
//
//
#include <QtMath>
#include <QPainter>
#include <cmath>

#include "engine/engine.h"
#include "engine/engine_texture.h"
#include "engine/engine_world.h"
#include "engine/form_engine.h"
#include "engine_things/engine_thing_object.h"
#include "helper.h"
#include "opengl/opengl.h"


//####################################################################################
//##    Chipmunk Callbacks
//##        Grabs data from Space
//####################################################################################
// Used for Arbiter iterator to get a list of all arbiters (collision points) attached to a body
static void GetBodyContactPoints(cpBody *, cpArbiter *arb, QVector<QPointF> *point_list) {
    cpContactPointSet contact = cpArbiterGetContactPointSet( arb );
    point_list->append( QPointF( contact.points->pointA.x, contact.points->pointA.y ) );
}
// Used for Arbiter iterator to get a list of Normals (angles) of all arbiters (collision points) attached to a body
static void GetBodyContactNormals(cpBody *, cpArbiter *arb, QVector<cpVect> *normal_list) {
    cpVect normal = cpArbiterGetNormal( arb );
    normal_list->append( normal );
}
// Used for constraint iterator to get a list of all Constraints (joints) in cpSpace
static void GetSpaceJointList(cpConstraint *constraint, QVector<cpConstraint*> *joint_list) { joint_list->append(constraint); }


//####################################################################################
//##    Assigns Debug color based on Collision Type
//####################################################################################
QColor DrOpenGL::objectDebugColor(DrEngineObject *object, bool text_color) {
    QColor color, font_color;

    switch (object->getCollisionType()) {
        case Collision_Type::Damage_None:       color = QColor(0, 255, 0);      break;          // Green
        case Collision_Type::Damage_Player:     color = QColor(255, 0, 0);      break;          // Red
        case Collision_Type::Damage_Enemy:      color = QColor(0, 0, 255);      break;          // Blue
        case Collision_Type::Damage_All:        color = QColor(128, 0, 128);    break;          // Purple
    }
    font_color = color;

    if (cpBodyIsSleeping(object->body)) {
        ///font_color = color;
        color = Qt::yellow;
    } else {
        ///font_color = QColor(255 - color.red(), 255 - color.green(), 255 - color.blue());
    }

    if (text_color) return font_color;
    else            return color;
}


//####################################################################################
//##    Main Debug / Text Rendering Function
//####################################################################################
void DrOpenGL::drawDebug(QPainter &painter) {
    painter.setRenderHint(QPainter::Antialiasing, true);
    if (m_form_engine->debug_shapes) {
        drawDebugShapes(painter);
        drawDebugJoints(painter);
        ///drawDebugHealth(painter);
        drawDebugHealthNative(painter);
    }
    if (m_form_engine->debug_collisions) {
        drawDebugCollisions(painter);
    }

    QFont font("Avenir", 12);
    painter.setFont(font);
    painter.setPen( Qt::white );
    painter.drawText( QPointF(20,  20), "FPS: \t" + QString::number(m_engine->getFormEngine()->fps_render) );               // Frames per second
    painter.drawText( QPointF(20,  40), "OBJ: \t" + QString::number(m_engine->getCurrentWorld()->getThings().count()) );    // Object count
    painter.drawText( QPointF(20,  60), "TRI: \t" + QString::number(getTriangleCount()) );                                  // Triangle count
    painter.drawText( QPointF(20,  80), "MAG: \t" + QString::number(double(m_scale)) );                                     // World scale
    //painter.drawText( QPointF(20, 100), "PHY: \t" + QString::number(m_engine->getFormEngine()->fps_physics) );            // Physics update
    //painter.drawText( QPointF(20, 120), g_info );                                                                         // Global debug string

    ///int max_sample = 0, max_text = 0, max_number_textures = 0, max_layers = 0;
    ///glGetIntegerv ( GL_MAX_SAMPLES, &max_sample );                                      // Finds max multi sampling available on system
    ///glGetIntegerv ( GL_MAX_TEXTURE_SIZE, &max_text );                                   // Finds max texture size available on system
    ///glGetIntegerv ( GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &max_number_textures );        // Finds max number of textures can bind at one time
    ///glGetIntegerv ( GL_MAX_ARRAY_TEXTURE_LAYERS, &max_layers );
    ///painter.drawText( QPointF(20, 80), "Max Texture Size: " +  QString::number(g_max_texture_size));
}


//####################################################################################
//##    Draws the Collision Shapes using QPainter
//####################################################################################
void DrOpenGL::drawDebugShapes(QPainter &painter) {

    int open_gl_width =  width() *  devicePixelRatio();
    int open_gl_height = height() * devicePixelRatio();
    QRect open_gl_rect = QRect(0, 0, open_gl_width, open_gl_height);

    for (auto thing : m_engine->getCurrentWorld()->getThings()) {
        if (thing->getThingType() != DrThingType::Object) continue;
        DrEngineObject *object = dynamic_cast<DrEngineObject*>(thing);

        // Figure out what color to make the debug shapes
        QColor color = objectDebugColor(object);
        if (object->isDying() || !object->isAlive()) color = Qt::gray;
        if (!object->doesCollide()) color = color.lighter();

        // Set up QPainter
        QPen cosmetic_pen( QBrush(color), 1);
        cosmetic_pen.setCosmetic(true);
        painter.setPen( cosmetic_pen );
        QColor brush_color = color;
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

                // Draw Normal circle since camera is facing straight on
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
                    QPen line_pen( QBrush(color), 2);
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

//                        object_poly = object_poly.united( mapped );

                        painter.drawPolygon( mapped );

                    }
                }

            }   // End If

        }   // End For shape


//        // ***** Draw a multi shape Shape_Type::Polygon
//        if (!object_poly.isEmpty())
//            painter.drawPolygon( object_poly );

    }   // End For object
}



//####################################################################################
//##    Draws the active Constraints using QPainter
//####################################################################################
void DrOpenGL::drawDebugJoints(QPainter &painter) {

    QPen pen( QBrush(QColor(255, 255, 255)), 1.25 * static_cast<double>(m_scale), Qt::SolidLine, Qt::PenCapStyle::RoundCap);
    painter.setPen( pen );

    // Get a list of the constraints in the Space
    QVector<cpConstraint*> joint_list;
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



//####################################################################################
//##    Draws the Collision Points using QPainter
//####################################################################################
void DrOpenGL::drawDebugCollisions(QPainter &painter) {

    int open_gl_width =  width() *  devicePixelRatio();
    int open_gl_height = height() * devicePixelRatio();
    QRect open_gl_rect = QRect(0, 0, open_gl_width, open_gl_height);

    QPen pen( QBrush(Qt::red), 2.5 * static_cast<double>(m_scale), Qt::SolidLine, Qt::PenCapStyle::RoundCap);
    painter.setPen( pen );

    for (auto thing : m_engine->getCurrentWorld()->getThings()) {
        if ( thing->getThingType() != DrThingType::Object)  continue;

        DrEngineObject *object = dynamic_cast<DrEngineObject*>(thing);
        if ( object->body_type != Body_Type::Dynamic)        continue;

        QVector<QPointF> point_list;    point_list.clear();
        QVector<cpVect>  normal_list;   normal_list.clear();
        cpBodyEachArbiter(object->body, cpBodyArbiterIteratorFunc(GetBodyContactPoints),  &point_list);
        cpBodyEachArbiter(object->body, cpBodyArbiterIteratorFunc(GetBodyContactNormals), &normal_list);

        for (int i = 0; i < point_list.size(); i++) {
            QPointF contact = point_list[i];
            QPointF point = mapToScreen( contact.x(), contact.y(), 0.0 );

            double angle_in_radians = std::atan2(normal_list[i].y, normal_list[i].x);
            double angle_in_degrees = (angle_in_radians / M_PI) * 180.0;

            if (open_gl_rect.contains(point.toPoint())) {
                QTransform t = QTransform().translate(point.x(), point.y()).rotate(-object->getAngle()).translate(-point.x(), -point.y());
                QPoint dot = t.map( point ).toPoint();

                //// Draw dots
                painter.translate(point.x(), point.y());
                painter.rotate(angle_in_degrees);
                painter.translate(-point.x(), -point.y());
                painter.drawLine( dot.x(), dot.y() - 4, dot.x(), dot.y() + 4);
                painter.resetTransform();
            }
        }   // End For
    }   // End For

}














