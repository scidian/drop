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
#include "engine/engine_world.h"
#include "engine_things/engine_thing_object.h"
#include "engine_things/engine_thing_light.h"
#include "opengl/opengl.h"


//####################################################################################
//##    Returns a square matrix for rendering a flat quad (usually to fill screen)
//####################################################################################
QMatrix4x4 DrOpenGL::orthoMatrix(float width, float height) {
    float left =   0.0f - (width  / 2.0f);
    float right =  0.0f + (width  / 2.0f);
    float top =    0.0f + (height / 2.0f);
    float bottom = 0.0f - (height / 2.0f);
    QMatrix4x4 m;
    m.ortho( left, right, bottom, top, c_near_plane, c_far_plane);
    return m;
}


//####################################################################################
//##    Update the View / Projection matrices before rendering
//####################################################################################
void DrOpenGL::updateViewMatrix(Render_Type render_type, bool use_offset) {
    //          Axis:
    //              -X left,        +X right
    //              -Y down,        +Y up
    //              -Z back,        +Z front (close to camera)
    float aspect_ratio = static_cast<float>(width()) / static_cast<float>(height());

    m_view.setToIdentity();
    m_projection.setToIdentity();

    // Set camera position
    float cam_x, cam_y;
          cam_x = (m_engine->getCurrentWorld()->getCameraPosition().x())       * m_scale;
    if (render_type == Render_Type::Orthographic)
          cam_y = (m_engine->getCurrentWorld()->getCameraPosition().y() + 100) * m_scale;
    else
          cam_y = (m_engine->getCurrentWorld()->getCameraPosition().y()) * m_scale;
    m_eye =     QVector3D( cam_x, cam_y, m_engine->getCurrentWorld()->getCameraPosition().z() );
    m_look_at = QVector3D( cam_x, cam_y, 0.0f );
    m_up =      QVector3D(  0.0f,  1.0f, 0.0f );

    // Camera Rotation
    if (use_offset && render_type == Render_Type::Perspective) {
        // X Rotation, controls up / down
        // Y Rotation, controls left / right
        QMatrix4x4 rotate_eye;
        rotate_eye.translate(m_look_at);
        rotate_eye.rotate(static_cast<float>(m_engine->getCurrentWorld()->getCameraRotationY()), 0.0f, 1.0f, 0.0f);
        rotate_eye.rotate(static_cast<float>(m_engine->getCurrentWorld()->getCameraRotationX()), 1.0f, 0.0f, 0.0f);
        rotate_eye.translate(-m_look_at);
        m_eye = rotate_eye * m_eye;

        // Z Rotation, tilts head
        QMatrix4x4 rotate_up;
        rotate_up.rotate(static_cast<float>(m_engine->getCurrentWorld()->getCameraRotationZ()), 0.0f, 0.0f, 1.0f);
        m_up = rotate_up * m_up;
    }

    // Orthographic
    if (render_type == Render_Type::Orthographic) {
        QVector3D frame = QVector3D( cam_x, cam_y, 0.f );
        float left =   frame.x() - (width() *  devicePixelRatio() / 2.0f);
        float right =  frame.x() + (width() *  devicePixelRatio() / 2.0f);
        float top =    frame.y() + (height() * devicePixelRatio() / 2.0f);
        float bottom = frame.y() - (height() * devicePixelRatio() / 2.0f);
        m_projection.ortho( left, right, bottom, top, c_near_plane * m_scale, c_far_plane * m_scale);
        ///m_view.lookAt(m_eye, m_look_at, m_up);
        m_view.scale( m_scale );

    // Perspective
    } else {
        m_projection.perspective( c_field_of_view, aspect_ratio, 1.0f, (c_far_plane - c_near_plane) );
        m_view.lookAt(m_eye, m_look_at, m_up);
        m_view.scale( m_scale );
    }
}


//####################################################################################
//##    Occluder Map FBO Matrices
//####################################################################################
QMatrix4x4 DrOpenGL::occluderMatrix(Render_Type render_type, bool use_offset) {
    float aspect_ratio = static_cast<float>(m_occluder_fbo->width()) / static_cast<float>(m_occluder_fbo->height());

    // Orthographic
    QMatrix4x4 matrix, matrix2;
    matrix.setToIdentity();
    matrix2.setToIdentity();
    if (render_type == Render_Type::Orthographic) {
        float cam_x =  (m_engine->getCurrentWorld()->getCameraPosition().x()) * m_scale * c_occluder_scale_ortho;
        float cam_y =  (m_engine->getCurrentWorld()->getCameraPosition().y() + 100) * m_scale * c_occluder_scale_ortho;
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
        float cam_x =  (m_engine->getCurrentWorld()->getCameraPosition().x()) * m_scale * c_occluder_scale_proj;
        float cam_y =  (m_engine->getCurrentWorld()->getCameraPosition().y() + 100) * m_scale * c_occluder_scale_proj;
        cam_x = (int(cam_x) / 5) * 5;
        cam_y = (int(cam_y) / 5) * 5;
        QVector3D  eye(     cam_x + perspective_offset.x(), cam_y + perspective_offset.y(), m_engine->getCurrentWorld()->getCameraPosition().z());
        QVector3D  look_at( cam_x, cam_y, 0.0f );
        QVector3D  up(      0.0f,   1.0f, 0.0f );
        matrix.perspective( c_field_of_view, aspect_ratio, 1.0f, (c_far_plane - c_near_plane) );
        matrix2.lookAt(eye, look_at, up);
        matrix2.scale( m_scale * c_occluder_scale_proj);
        matrix *= matrix2;
    }
    return matrix;
}










