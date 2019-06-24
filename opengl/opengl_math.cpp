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
#include "engine/engine_light.h"
#include "engine/engine_world.h"
#include "opengl/opengl.h"


//####################################################################################
//##        Maps 3D Point to / from 2D QOpenGLWidget Coordinates
//####################################################################################
QPointF OpenGL::mapToScreen(double x, double y, double z) { return mapToScreen( QVector3D(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z)) ); }
QPointF OpenGL::mapToScreen(float x,  float y,  float z)  { return mapToScreen( QVector3D(x, y, z )); }
QPointF OpenGL::mapToScreen(QVector3D point3D) {
    QRect viewport = QRect(0, 0, width() * devicePixelRatio(), height() * devicePixelRatio());

    float x_pos, y_pos, z_pos;
    x_pos = point3D.x() * viewScale();
    y_pos = point3D.y() * viewScale();
    z_pos = point3D.z() * viewScale();
    QVector3D vec = QVector3D( x_pos, y_pos, z_pos).project( m_model_view, m_projection, viewport);
    return QPointF( static_cast<double>(vec.x()),  static_cast<double>((height() * devicePixelRatio()) - vec.y()) );
}

QVector3D OpenGL::mapFromScreen(double x, double y) { return mapFromScreen( QPointF(x, y)); }
QVector3D OpenGL::mapFromScreen(float x, float y)   { return mapFromScreen( QPointF(static_cast<double>(x), static_cast<double>(y)) ); }
QVector3D OpenGL::mapFromScreen(QPointF point) {
    QRect viewport = QRect(0, 0, width() * devicePixelRatio(), height() * devicePixelRatio());

    float x_pos = static_cast<float>(             point.x()  * devicePixelRatio() );
    float y_pos = static_cast<float>( (height() - point.y()) * devicePixelRatio() );

    QVector3D vec;
    if (m_engine->getCurrentWorld()->render_type == Render_Type::Orthographic) {
        vec = QVector3D( x_pos, y_pos, 0 ).unproject( m_model_view, m_projection, viewport);
        vec.setX( vec.x() / m_scale );
        vec.setY( vec.y() / m_scale );
        vec.setZ( vec.z() / m_scale );
    } else {
        // Possible way to read from the depth buffer
        ///GLdouble depthScale;
        ///glGetDoublev( GL_DEPTH_SCALE, &depthScale );
        ///GLfloat z;
        ///glReadPixels( static_cast<int>(x_pos), static_cast<int>(y_pos), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &z );

        // Old way to unproject, returns unpredictable z coordinate that cannot easily be moved to z plane 0...
        ///// Find what the Z value is for a projected vector
        ///QVector3D find_z(0, 0, 0);
        ///find_z = find_z.project(m_model_view, m_projection, viewport);
        ///// Then we use that Z value to unproject
        ///vec = QVector3D( x_pos, y_pos, find_z.z()).unproject( m_model_view, m_projection, viewport);

        // ********** New Perspective Unproject Method
        // Since two points determine a line, we actually need to call unproject() twice: once with screen z == 0.0, then again with screen z == 1.0
        // This will give us the world points that correspond to the mouse click on the near and far planes, respectively
        //
        // !!!!! #NOTE: This won't work too great if z plane 0 is not in view
        // **********

        // Unproject at near and far plane
        QVector3D near, far;
        near = QVector3D( x_pos, y_pos, 0.0f ).unproject( m_model_view, m_projection, viewport);
        far =  QVector3D( x_pos, y_pos, 1.0f ).unproject( m_model_view, m_projection, viewport);

        // Find distance to z plane 0 as a percentage, and interpolate between the two near and far plane mouse points
        float z_total = abs(near.z()) + abs(far.z());
        float z_percent = abs(near.z() / z_total);
        float nx = near.x() + (z_percent * (far.x() - near.x()));
        float ny = near.y() + (z_percent * (far.y() - near.y()));
        vec = QVector3D(nx, ny, 0);

        ///qDebug() << "Near: " << near << "  -  Far: " << far << "  -  Vec: " << vec;
    }

    return vec;
}


//####################################################################################
//##        Maps 3D Point to / from 2D Occluder Map Coordinates
//####################################################################################
QPointF OpenGL::mapToOccluder(QVector3D point3D) {
    QRect viewport = QRect(0, 0, m_occluder_fbo->width() * devicePixelRatio(), m_occluder_fbo->height() * devicePixelRatio());

    float x_pos, y_pos, z_pos;
    x_pos = point3D.x() * c_occluder_scale * viewScale();
    y_pos = point3D.y() * c_occluder_scale * viewScale();
    z_pos = point3D.z() * c_occluder_scale * viewScale();

    QMatrix4x4 identity;
    identity.setToIdentity();
    QVector3D vec = QVector3D(x_pos, y_pos, z_pos).project( identity, occluderMatrix(), viewport);
    return QPointF( static_cast<double>(vec.x()),  static_cast<double>((height() * devicePixelRatio()) - vec.y()) );
}

















