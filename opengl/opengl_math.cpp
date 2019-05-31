//
//      Created by Stephens Nunnally on 5/7/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QVector3D>
#include <QDebug>

#include "engine/engine.h"
#include "engine/engine_object.h"
#include "opengl/opengl.h"


//####################################################################################
//##        Maps 3D Point to / from 2d QOpenGLWidget Coordinates
//####################################################################################
QPointF OpenGL::mapToScreen(double x, double y, double z) { return mapToScreen( QVector3D(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z)) ); }
QPointF OpenGL::mapToScreen(float x,  float y,  float z)  { return mapToScreen( QVector3D(x, y, z )); }
QPointF OpenGL::mapToScreen(QVector3D point3D) {
    float x_pos, y_pos, z_pos;

    if (m_engine->render_type == Render_Type::Orthographic) {
        x_pos = point3D.x() * m_scale;
        y_pos = point3D.y() * m_scale;
        z_pos = point3D.z() * m_scale;
    } else {
        x_pos = point3D.x();
        y_pos = point3D.y();
        z_pos = point3D.z();
    }
    QVector3D vec = QVector3D( x_pos, y_pos, z_pos).project( m_model_view, m_projection,
                                                             QRect(0, 0, width() * devicePixelRatio(), height() * devicePixelRatio()));
    return QPointF( static_cast<double>(vec.x()),  static_cast<double>(height() - vec.y()) );
}

QVector3D OpenGL::mapFromScreen(double x, double y) { return mapFromScreen( QPointF(x, y)); }
QVector3D OpenGL::mapFromScreen(float x, float y)   { return mapFromScreen( QPointF(static_cast<double>(x), static_cast<double>(y)) ); }
QVector3D OpenGL::mapFromScreen(QPointF point) {
    float x_pos = static_cast<float>( point.x() );
    float y_pos = static_cast<float>( height() - point.y() );

    QVector3D vec;
    if (m_engine->render_type == Render_Type::Orthographic) {
        vec = QVector3D( x_pos, y_pos, 0).unproject( m_model_view, m_projection, QRect(0, 0, width() * devicePixelRatio(), height() * devicePixelRatio()));
        vec.setX( vec.x() / m_scale );
        vec.setY( vec.y() / m_scale );
        vec.setZ( vec.z() / m_scale );
    } else {
        // Possible way to read from the depth buffer
        ///GLdouble depthScale;
        ///glGetDoublev( GL_DEPTH_SCALE, &depthScale );
        ///GLfloat z;
        ///glReadPixels( static_cast<int>(x_pos), static_cast<int>(y_pos), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &z );

        // In perspective mode we have to find what the Z value is for a projected vector first
        QVector3D find_z(0, 0, 0);
        find_z = find_z.project(m_model_view, m_projection, QRect(0, 0, width() * devicePixelRatio(), height() * devicePixelRatio()));

        // Then we use that Z value to unproject
        vec = QVector3D( x_pos, y_pos, find_z.z()).unproject( m_model_view, m_projection, QRect(0, 0, width() * devicePixelRatio(), height() * devicePixelRatio()));
    }

    return vec;
}














