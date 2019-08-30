//
//      Created by Stephens Nunnally on 8/30/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QVector3D>
#include <QDebug>

#include "engine/engine.h"
#include "engine/engine_thing_object.h"
#include "engine/engine_thing_light.h"
#include "engine/engine_world.h"
#include "opengl/opengl.h"


//####################################################################################
//##        Update the view matrices before rendering
//####################################################################################
void DrOpenGL::updateViewMatrix(Render_Type render_type, bool use_offset) {
    //          Axis:
    //              -X left,        +X right
    //              -Y down,        +Y up
    //              -Z back,        +Z front (close to camera)
    float aspect_ratio = static_cast<float>(width()) / static_cast<float>(height());

    // Orthographic
    m_model_view.setToIdentity();
    m_projection.setToIdentity();
    if (render_type == Render_Type::Orthographic) {
        float cam_x =  (m_engine->getCurrentWorld()->getCameraPos().x()) * m_scale;
        float cam_y =  (m_engine->getCurrentWorld()->getCameraPos().y() + 100) * m_scale;
        float left =   cam_x - (width() *  devicePixelRatio() / 2.0f);
        float right =  cam_x + (width() *  devicePixelRatio() / 2.0f);
        float top =    cam_y + (height() * devicePixelRatio() / 2.0f);
        float bottom = cam_y - (height() * devicePixelRatio() / 2.0f);
        m_projection.ortho( left, right, bottom, top, c_near_plane * m_scale, c_far_plane * m_scale);
        m_model_view.scale( m_scale );

    // Perspective
    } else {
        // Set camera position
        QVector3D  perspective_offset = use_offset ? QVector3D(200.0f, 200.0f, 0.0f) : QVector3D(0.0f, 0.0f, 0.0f);
        QVector3D  eye(     m_engine->getCurrentWorld()->getCameraPos().x()        * m_scale + perspective_offset.x(),
                           (m_engine->getCurrentWorld()->getCameraPos().y() + 100) * m_scale + perspective_offset.y(),
                            m_engine->getCurrentWorld()->getCameraPos().z() );
        QVector3D  look_at( m_engine->getCurrentWorld()->getCameraPos().x()        * m_scale,
                           (m_engine->getCurrentWorld()->getCameraPos().y() + 100) * m_scale,
                            0.0f );
        QVector3D  up(      0.0f, 1.0f, 0.0f);

        m_projection.perspective( c_field_of_view, aspect_ratio, 1.0f, (c_far_plane - c_near_plane) );
        m_model_view.lookAt(eye, look_at, up);
        m_model_view.scale( m_scale );

        // Rotates the camera around the center of the sceen
        ///m_angle += 1.0f;
        ///if (m_angle > 360) m_angle = 0;
        ///m_model_view.rotate( m_angle, 0.0f, 1.0f, 0.0f );
    }
}




//####################################################################################
//##        Occluder Map FBO Matrices
//####################################################################################
QMatrix4x4 DrOpenGL::occluderMatrix(Render_Type render_type, bool use_offset) {
    float aspect_ratio = static_cast<float>(m_occluder_fbo->width()) / static_cast<float>(m_occluder_fbo->height());

    // Orthographic
    QMatrix4x4 matrix, matrix2;
    matrix.setToIdentity();
    matrix2.setToIdentity();
    if (render_type == Render_Type::Orthographic) {
        float cam_x =  (m_engine->getCurrentWorld()->getCameraPos().x()) * m_scale * c_occluder_scale_ortho;
        float cam_y =  (m_engine->getCurrentWorld()->getCameraPos().y() + 100) * m_scale * c_occluder_scale_ortho;
        cam_x = (int(cam_x) / 5) * 5;
        cam_y = (int(cam_y) / 5) * 5;
        float left =   cam_x - (m_occluder_fbo->width() /  2.0f);
        float right =  cam_x + (m_occluder_fbo->width() /  2.0f);
        float top =    cam_y + (m_occluder_fbo->height() / 2.0f);
        float bottom = cam_y - (m_occluder_fbo->height() / 2.0f);
        matrix.ortho( left, right, bottom, top, c_near_plane * m_scale * c_occluder_scale_ortho, c_far_plane * m_scale * c_occluder_scale_ortho);
        matrix.scale( m_scale * c_occluder_scale_ortho );

    // Perspective
    } else {
        // Set camera position
        QVector3D  perspective_offset = use_offset? QVector3D(200.0f, 200.0f, 0.0f) : QVector3D(0.0f, 0.0f, 0.0f);
        float cam_x =  (m_engine->getCurrentWorld()->getCameraPos().x()) * m_scale * c_occluder_scale_proj;
        float cam_y =  (m_engine->getCurrentWorld()->getCameraPos().y() + 100) * m_scale * c_occluder_scale_proj;
        cam_x = (int(cam_x) / 5) * 5;
        cam_y = (int(cam_y) / 5) * 5;
        QVector3D  eye(     cam_x + perspective_offset.x(), cam_y + perspective_offset.y(), m_engine->getCurrentWorld()->getCameraPos().z());
        QVector3D  look_at( cam_x, cam_y, 0.0f );
        QVector3D  up(      0.0f,   1.0f, 0.0f );
        matrix.perspective( c_field_of_view, aspect_ratio, 1.0f, (c_far_plane - c_near_plane) );
        matrix2.lookAt(eye, look_at, up);
        matrix2.scale( m_scale * c_occluder_scale_proj);
        matrix *= matrix2;
    }
    return matrix;
}


