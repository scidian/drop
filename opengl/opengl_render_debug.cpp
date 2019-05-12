//
//      Created by Stephens Nunnally on 5/7/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QtMath>
#include <QPainter>

#include "chipmunk/chipmunk.h"
#include "engine/engine.h"
#include "engine/engine_texture.h"
#include "opengl/opengl.h"

// Forward declare
static void drawContactPoints(cpBody *body, cpArbiter *arb, QVector<QPointF> *point_list);

// Used for shape iterator to get a list of all shapes attached to a body
static void getShapeList(cpBody *, cpShape *shape, QVector<cpShape*> *shape_list) { shape_list->append(shape); }

//####################################################################################
//##        Draws the Collision Shapes using QPainter
//####################################################################################
void OpenGL::drawDebugShapes(QPainter &painter) {

    for (auto object : m_engine->objects) {
        if (object->in_scene == false) continue;
        if (object->collide == false) continue;
        if (!object->has_been_processed) continue;

        QColor color;
        switch (object->body_type) {
            case Body_Type::Dynamic:        color = Qt::red;       break;
            case Body_Type::Static:         color = Qt::blue;      break;
            case Body_Type::Kinematic:      color = Qt::green;     break;
        }

        QPen cosmetic_pen( QBrush(color), 1);
        cosmetic_pen.setCosmetic(true);
        painter.setPen( cosmetic_pen );
        QColor brush_color = color;
        brush_color.setAlpha(64);
        painter.setBrush( QBrush( brush_color));

        if (object->shape_type == Shape_Type::Circle) {

            double radius = cpCircleShapeGetRadius(object->shape);
            QTransform t = QTransform().translate(object->position.x(), object->position.y());

            // Draw Normal circle since camera is facing straight on
            if (m_engine->render_type == Render_Type::Orthographic) {
                QPointF mid =   t.map(QPointF( 0, 0 ));
                QPointF top =   t.map(QPointF( 0,  radius ));
                QPointF l1 = mapToScreen( top.x(), top.y(), 0);
                QPointF l2 = mapToScreen( mid.x(), mid.y(), 0);
                double new_radius = QLineF(l1, l2).length();

                // Don't draw if not touching or inside of visible area
                QRect bounding_box = QRectF( l2.x() - new_radius, l2.y() - new_radius, new_radius * 2, new_radius * 2).toRect();
                if ((rect().intersects(bounding_box) || rect().contains(bounding_box)) &&
                    (bounding_box.width() * 0.1 < width()) && (bounding_box.height() * 0.1 < height())) {
                    painter.drawEllipse( l2, new_radius, new_radius );

                    // Draw orientation line
                    t = QTransform().translate(object->position.x(), object->position.y()).rotate( object->angle);
                    top = t.map(QPointF( 0, radius ));
                    l1 = mapToScreen( top.x(), top.y(), 0);
                    painter.drawLine( l1, l2 );
                }

            // Draw skewed circle with QPainterPath using bezier curves
            } if (m_engine->render_type == Render_Type::Perspective) {

                QPointF mid =   t.map(QPointF( 0, 0 ));
                QPointF top =   t.map(QPointF( 0,  radius ));
                QPointF bot =   t.map(QPointF( 0, -radius ));
                QPointF left =  t.map(QPointF( -radius, 0 ));
                QPointF right = t.map(QPointF(  radius, 0 ));
                QPointF tl =    t.map(QPointF( -radius * .92,  radius * .92 ));
                QPointF tr =    t.map(QPointF(  radius * .92,  radius * .92 ));
                QPointF bl =    t.map(QPointF( -radius * .92, -radius * .92 ));
                QPointF br =    t.map(QPointF(  radius * .92, -radius * .92 ));

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
                if ((rect().intersects(bounding_box) || rect().contains(bounding_box)) &&
                    (bounding_box.width() * 0.1 < width()) && (bounding_box.height() * 0.1 < height())) {
                    painter.drawPath( path );

                    // Draw orientation line
                    t = QTransform().translate(object->position.x(), object->position.y()).rotate( object->angle);
                    QPointF rotated_top = t.map(QPointF( 0, radius ));
                    rotated_top = mapToScreen( rotated_top.x(), rotated_top.y(), 0);
                    painter.drawLine( rotated_top, mid );
                }
            }


        } else if (object->shape_type == Shape_Type::Segment) {
            cpVect a = cpSegmentShapeGetA( object->shape);
            cpVect b = cpSegmentShapeGetB( object->shape);
            QPointF p1 = QPointF( a.x, a.y );
            QPointF p2 = QPointF( b.x, b.y );

            p1 = mapToScreen( p1.x(), p1.y(), 0.0 );
            p2 = mapToScreen( p2.x(), p2.y(), 0.0 );

            // Don't draw if not touching or inside of visible area
            QRect bounding_box = QRectF(p1, p2).toRect().normalized();
            if ((rect().intersects(bounding_box) || rect().contains(bounding_box)) &&
                (bounding_box.width() * 0.1 < width()) && (bounding_box.height() * 0.1 < height())) {
                QPen line_pen( QBrush(color), 2);
                painter.setPen( line_pen );
                painter.drawLine(p1, p2);
            }

        } else if (object->shape_type == Shape_Type::Polygon || object->shape_type == Shape_Type::Box) {

            QVector<cpShape*> shape_list;
            cpBodyEachShape(object->body, cpBodyShapeIteratorFunc(getShapeList), &shape_list);

            //if (object->follow) {
            //    painter.drawText( QPointF(20, 20), "Shapes: " + QString::number(shape_list.count()));
            // }

            for (auto shape : shape_list) {
                QTransform t = QTransform().translate(object->position.x(), object->position.y()).rotate( object->angle);
                QPolygonF polygon, mapped;
                for (int i = 0; i < cpPolyShapeGetCount( shape ); i++) {
                    cpVect  vert  = cpPolyShapeGetVert( shape, i );
                    polygon.append( QPointF( vert.x, vert.y ));

                    QPointF trans = t.map( polygon[i] );
                    mapped.append( mapToScreen(trans.x(), trans.y(), 0));
                }

                // Don't draw if not touching or inside of visible area
                QRect bounding_box = mapped.boundingRect().toRect();
                if ((rect().intersects(bounding_box) || rect().contains(bounding_box)) &&
                    (bounding_box.width() * 0.1 < width()) && (bounding_box.height() * 0.1 < height())) {
                    painter.drawPolygon( mapped );

                    // Draw orientation line
                    if (object->shape_type == Shape_Type::Box) {
                        QPointF mid = t.map(QPointF( 0, 0));
                        QPointF top = t.map(QPointF( 0, polygon.boundingRect().height() / 2 ));
                        QPointF l1 = mapToScreen( mid.x(),  mid.y(),  0);
                        QPointF l2 = mapToScreen( top.x(),  top.y(),  0);
                        painter.drawLine( l1, l2 );
                    }
                }
            }

        }   // End If
    }   // End For
}





//####################################################################################
//##        Draws the Collision Points using QPainter
//####################################################################################
void OpenGL::drawDebugCollisions(QPainter &painter) {

    // ***** Draw debug collision points
    QPen line_pen( QBrush(QColor(128, 0, 128)), 4 * static_cast<double>(m_scale));
    painter.setPen( line_pen );

    for (auto object : m_engine->objects) {
        if (object->in_scene == false) continue;
        if (object->collide == false) continue;

        QVector<QPointF> point_list;
        point_list.clear();
        cpBodyEachArbiter(object->body, cpBodyArbiterIteratorFunc(drawContactPoints), &point_list);

        if (point_list.size() > 0) {
            for (auto contact : point_list) {
                QPointF point = mapToScreen( contact.x(), contact.y(), 0.0 );

                if (rect().contains(point.toPoint())) {
                    QTransform t = QTransform().translate(point.x(), point.y()).rotate(-object->angle).translate(-point.x(), -point.y());
                    QPoint dot = t.map( point ).toPoint();
                    painter.drawPoint( dot );
                    //// Draw crosses instead of dots
                    ///painter.drawLine( dot.x() - 1, dot.y() - 1, dot.x() + 1, dot.y() + 1);
                    ///painter.drawLine( dot.x() - 1, dot.y() + 1, dot.x() + 1, dot.y() - 1);
                }
            }
        } // End If
    }   // End For

}


static void drawContactPoints(cpBody *, cpArbiter *arb, QVector<QPointF> *point_list) {
    cpContactPointSet contact = cpArbiterGetContactPointSet( arb );
    point_list->append( QPointF( contact.points->pointA.x, contact.points->pointA.y ) );
}













