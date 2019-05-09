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
            double radius = QLineF( mapToScreen( cpCircleShapeGetRadius(object->shape), 0.0, 0.0), mapToScreen( 0.0, 0.0, 0.0 )).length();
            QPointF point = mapToScreen( object->position.x(), object->position.y(), 0.0 );

            // Don't draw if not touching or inside of visible area
            QRect bounding_box = QRectF( point.x() - radius, point.y() - radius, radius * 2, radius * 2).toRect();
            if (rect().intersects(bounding_box) || rect().contains(bounding_box)) {
                painter.drawEllipse( point, radius, radius );
                QPointF top (point.x(), point.y() - radius);
                QTransform t = QTransform().translate(point.x(), point.y()).rotate(-object->angle).translate(-point.x(), -point.y());
                QPointF l2 = t.map( top );
                painter.drawLine( point, l2 );
            }

        } else if (object->shape_type == Shape_Type::Box) {
            double width =  QLineF( mapToScreen( object->width, 0.0, 0.0),  mapToScreen( 0.0, 0.0, 0.0) ).length();
            double height = QLineF( mapToScreen( object->height, 0.0, 0.0), mapToScreen( 0.0, 0.0, 0.0 )).length();
            QPointF point = mapToScreen( object->position.x(), object->position.y(), 0.0);
            QRectF  box = QRectF(point.x() - width / 2, point.y() - height / 2, width, height);
            QTransform t = QTransform().translate(point.x(), point.y()).rotate(-object->angle).translate(-point.x(), -point.y());
            QPolygonF rotated_box = t.map( box );

            // Don't draw if not touching or inside of visible area
            QRect bounding_box = rotated_box.boundingRect().toRect();
            if (rect().intersects(bounding_box) || rect().contains(bounding_box)) {
                painter.drawPolygon( rotated_box );
                QPointF top (point.x(), point.y() - height / 2);
                QPointF l2 = t.map( top );
                painter.drawLine( point, l2 );
            }

        } else if (object->shape_type == Shape_Type::Segment) {
            cpVect l1 = cpSegmentShapeGetA( object->shape);
            cpVect l2 = cpSegmentShapeGetB( object->shape);
            QPointF p1, p2;
            p1.setX(l1.x);  p1.setY(l1.y);
            p2.setX(l2.x);  p2.setY(l2.y);
            p1 = mapToScreen( p1.x(), p1.y(), 0.0 );
            p2 = mapToScreen( p2.x(), p2.y(), 0.0 );

            // Don't draw if not touching or inside of visible area
            QRect bounding_box = QRectF(p1, p2).toRect().normalized();
            if (rect().intersects(bounding_box) || rect().contains(bounding_box)) {
                QPen line_pen( QBrush(color), 2);
                painter.setPen( line_pen );
                painter.drawLine(p1, p2);
            }

        } else if (object->shape_type == Shape_Type::Polygon) {
            QPolygonF poly;
            for (int i = 0; i < cpPolyShapeGetCount( object->shape ); i++) {
                cpVect vert = cpPolyShapeGetVert( object->shape, i );
                poly.append( mapToScreen( vert.x + object->position.x(), vert.y + object->position.y(), 0 ));
            }
            QPointF center = mapToScreen( object->position.x(), object->position.y(), 0 );
            QTransform t = QTransform().translate(center.x(), center.y()).rotate(-object->angle).translate(-center.x(), -center.y());
            QPolygonF rotated_poly = t.map( poly );

            // Don't draw if not touching or inside of visible area
            QRect bounding_box = rotated_poly.boundingRect().toRect();
            if (rect().intersects(bounding_box) || rect().contains(bounding_box)) {
                painter.drawPolygon( rotated_poly );
            }
        }   // End If
    }   // End For

}







