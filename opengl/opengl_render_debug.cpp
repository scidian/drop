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

//####################################################################################
//##        Draws the Collision Shapes using QPainter
//####################################################################################
void OpenGL::drawDebugShapes(QPainter &painter) {

    for (auto object : m_engine->objects) {
        if (object->in_scene == false) continue;
        if (object->collide == false) continue;

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
            QPointF top = t.map(QPointF( 0, radius ));
            QPointF mid = t.map(QPointF( 0, 0));
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
            QTransform t = QTransform().translate(object->position.x(), object->position.y()).rotate( object->angle);
            QPolygonF polygon, mapped;
            for (int i = 0; i < cpPolyShapeGetCount( object->shape ); i++) {
                cpVect  vert  = cpPolyShapeGetVert( object->shape, i );
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

        }   // End If
    }   // End For
}


//####################################################################################
//##        Draws the Collision Points using QPainter
//####################################################################################
void OpenGL::drawDebugCollisions(QPainter &painter) {

    // ***** Draw debug collision points
    QPen line_pen( QBrush(QColor(128, 0, 128)), 3 * static_cast<double>(m_scale));
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













