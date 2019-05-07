//
//      Created by Stephens Nunnally on 5/7/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QVector3D>

#include "engine/engine.h"
#include "opengl/opengl.h"


//####################################################################################
//##        Maps 3D Point to 2d QOpenGLWidget Coordinates
//####################################################################################
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
