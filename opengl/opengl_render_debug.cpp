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
//            QTransform t = QTransform().translate(object->position.x(), object->position.y());
//            QPointF top = t.map(QPointF( 0, object->height / 2));
//            QPointF mid = t.map(QPointF( 0, 0));
//            QPointF l1 = mapToScreen( top.x(), top.y(), 0);
//            QPointF l2 = mapToScreen( mid.x(), mid.y(), 0);
//            double radius = QLineF(l1, l2).length();

//            // Don't draw if not touching or inside of visible area
//            QRect bounding_box = QRectF( l2.x() - radius, l2.y() - radius, radius * 2, radius * 2).toRect();
//            if ((rect().intersects(bounding_box) || rect().contains(bounding_box)) &&
//                (bounding_box.width() * 0.1 < width()) && (bounding_box.height() * 0.1 < height())) {
//                painter.drawEllipse( l2, radius, radius );
//                t = QTransform().translate(object->position.x(), object->position.y()).rotate( object->angle);
//                top = t.map(QPointF( 0, object->height / 2));
//                l1 = mapToScreen( top.x(), top.y(), 0);
//                painter.drawLine( l1, l2 );
//            }


            QTransform t = QTransform().translate(object->position.x(), object->position.y());
            QPointF top_left =  t.map(QPointF( -object->radius,  object->radius));
            QPointF top_right = t.map(QPointF(  object->radius,  object->radius));
            QPointF bot_left =  t.map(QPointF( -object->radius, -object->radius));
            QPointF bot_right = t.map(QPointF(  object->radius, -object->radius));
            top_left =  mapToScreen( top_left.x(),  top_left.y(),  0);
            top_right = mapToScreen( top_right.x(),  top_right.y(),  0);
            bot_left =  mapToScreen( bot_left.x(), bot_left.y(), 0);
            bot_right = mapToScreen( bot_right.x(), bot_right.y(), 0);
            top_left.setY ( (top_left.y() +  top_right.y()) / 2 );
            top_left.setX ( (top_left.x() +  bot_left.x()) /  2 );
            bot_right.setY ((bot_right.y() + bot_left.y()) /  2 );
            bot_right.setX ((bot_right.x() + top_right.x()) / 2 );

            // Don't draw if not touching or inside of visible area
            QRect bounding_box = QRectF(top_left, bot_right).normalized().toRect();
            if ((rect().intersects(bounding_box) || rect().contains(bounding_box)) &&
                (bounding_box.width() * 0.1 < width()) && (bounding_box.height() * 0.1 < height())) {
                //painter.drawPolygon( transformed );
                painter.drawEllipse( QRectF(top_left, bot_right).normalized() );

                t = QTransform().translate(object->position.x(), object->position.y()).rotate( object->angle);
                QPointF top = t.map(QPointF( 0, object->height / 2));
                QPointF mid = t.map(QPointF( 0, 0));
                QPointF l1 = mapToScreen( top.x(),  top.y(),  0);
                QPointF l2 = mapToScreen( mid.x(),  mid.y(),  0);
                painter.drawLine( l1, l2 );
            }


        } else if (object->shape_type == Shape_Type::Box) {
            QTransform t = QTransform().translate(object->position.x(), object->position.y()).rotate( object->angle);
            QPointF top_left =  t.map(QPointF( -object->width / 2,  object->height / 2));
            QPointF top_right = t.map(QPointF(  object->width / 2,  object->height / 2));
            QPointF bot_right = t.map(QPointF(  object->width / 2, -object->height / 2));
            QPointF bot_left =  t.map(QPointF( -object->width / 2, -object->height / 2));

            QPolygonF transformed;
            transformed.append( mapToScreen( top_left.x(),  top_left.y(),  0) );
            transformed.append( mapToScreen( top_right.x(), top_right.y(), 0) );
            transformed.append( mapToScreen( bot_right.x(), bot_right.y(), 0) );
            transformed.append( mapToScreen( bot_left.x(),  bot_left.y(),  0) );

            // Don't draw if not touching or inside of visible area
            QRect bounding_box = transformed.boundingRect().toRect();
            if ((rect().intersects(bounding_box) || rect().contains(bounding_box)) &&
                (bounding_box.width() * 0.1 < width()) && (bounding_box.height() * 0.1 < height())) {
                painter.drawPolygon( transformed );

                QPointF top = t.map(QPointF( 0, object->height / 2));
                QPointF mid = t.map(QPointF( 0, 0));
                QPointF l1 = mapToScreen( top.x(),  top.y(),  0);
                QPointF l2 = mapToScreen( mid.x(),  mid.y(),  0);
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

        } else if (object->shape_type == Shape_Type::Polygon) {
            QTransform t = QTransform().translate(object->position.x(), object->position.y()).rotate( object->angle);
            QPolygonF poly;
            for (int i = 0; i < cpPolyShapeGetCount( object->shape ); i++) {
                cpVect  vert  = cpPolyShapeGetVert( object->shape, i );
                QPointF trans = t.map(QPointF(vert.x, vert.y));
                poly.append( mapToScreen( trans.x(), trans.y(), 0 ));
            }

            // Don't draw if not touching or inside of visible area
            QRect bounding_box = poly.boundingRect().toRect();
            if ((rect().intersects(bounding_box) || rect().contains(bounding_box)) &&
                (bounding_box.width() * 0.1 < width()) && (bounding_box.height() * 0.1 < height())) {
                painter.drawPolygon( poly );
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













