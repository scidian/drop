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
    float left =   -(width  / 2.0f);
    float right =  +(width  / 2.0f);
    float top =    +(height / 2.0f);
    float bottom = -(height / 2.0f);
    QMatrix4x4 m;
    m.ortho( left, right, bottom, top, c_near_plane, c_far_plane);
    return m;
}


//####################################################################################
//##    Update the View / Projection matrices before rendering
//####################################################################################
void DrOpenGL::updateViewMatrix(Render_Type render_type) {
    //          Axis:
    //              -X left,        +X right
    //              -Y down,        +Y up
    //              -Z back,        +Z front (close to camera)
    float aspect_ratio = static_cast<float>(width()) / static_cast<float>(height());

    m_view.setToIdentity();
    m_projection.setToIdentity();

    // Set camera position
    float cam_x = (m_engine->getCurrentWorld()->getCameraPosition().x()) * m_scale;
    float cam_y = (m_engine->getCurrentWorld()->getCameraPosition().y()) * m_scale;
    m_eye =     QVector3D( cam_x, cam_y, m_engine->getCurrentWorld()->getCameraPosition().z() );
    m_look_at = QVector3D( cam_x, cam_y, 0.0f );
    m_up =      QVector3D(  0.0f,  1.0f, 0.0f );

    // Camera Rotation
    //      X Rotation, controls up / down
    //      Y Rotation, controls left / right
    QMatrix4x4 rotate_eye;
    rotate_eye.translate(m_look_at);
    rotate_eye.rotate(static_cast<float>(m_engine->getCurrentWorld()->getCameraRotationY()), 0.0f, 1.0f, 0.0f);
    rotate_eye.rotate(static_cast<float>(m_engine->getCurrentWorld()->getCameraRotationX()), 1.0f, 0.0f, 0.0f);
    rotate_eye.translate(-m_look_at);
    m_eye = rotate_eye * m_eye;

    //      Z Rotation, tilts head
    QMatrix4x4 rotate_up;
    rotate_up.rotate(static_cast<float>(m_engine->getCurrentWorld()->getCameraRotationZ()), 0.0f, 0.0f, 1.0f);
    m_up = rotate_up * m_up;

    // Orthographic
    if (render_type == Render_Type::Orthographic) {
        float left =   -(width()  * devicePixelRatio() / 2.0f);
        float right =  +(width()  * devicePixelRatio() / 2.0f);
        float top =    +(height() * devicePixelRatio() / 2.0f);
        float bottom = -(height() * devicePixelRatio() / 2.0f);
        m_projection.ortho( left, right, bottom, top, c_near_plane * m_scale, c_far_plane * m_scale);

    // Perspective
    } else {
        m_projection.perspective( c_field_of_view, aspect_ratio, 1.0f, (c_far_plane - c_near_plane) );
    }

    m_view.lookAt(m_eye, m_look_at, m_up);
    m_view.scale( m_scale );
}


//####################################################################################
//##    Occluder Map FBO Matrices
//##        RETURNS: View matrix and Projection matrix by reference
//####################################################################################
void DrOpenGL::occluderMatrix(Render_Type render_type, QMatrix4x4 &view_matrix, QMatrix4x4 &proj_matrix) {
    float aspect_ratio = static_cast<float>(m_occluder_fbo->width()) / static_cast<float>(m_occluder_fbo->height());

    view_matrix.setToIdentity();
    proj_matrix.setToIdentity();

    // Scale based on Render Type
    float scale = (render_type == Render_Type::Orthographic) ? (c_occluder_scale_ortho) : (c_occluder_scale_proj);
          scale *= m_scale;

    // Set camera position
    float cam_x = (m_engine->getCurrentWorld()->getCameraPosition().x()) * scale;
    float cam_y = (m_engine->getCurrentWorld()->getCameraPosition().y()) * scale;
    // Smooth position
    cam_x = (int(cam_x) / 5) * 5;
    cam_y = (int(cam_y) / 5) * 5;

    // Orthographic
    if (render_type == Render_Type::Orthographic) {
        float left =   -(m_occluder_fbo->width() /  2.0f);
        float right =  +(m_occluder_fbo->width() /  2.0f);
        float top =    +(m_occluder_fbo->height() / 2.0f);
        float bottom = -(m_occluder_fbo->height() / 2.0f);
        proj_matrix.ortho( left, right, bottom, top, c_near_plane * scale, c_far_plane * scale);

    // Perspective
    } else {
        proj_matrix.perspective( c_field_of_view, aspect_ratio, 1.0f, (c_far_plane - c_near_plane) );
    }

    view_matrix.lookAt(m_eye, m_look_at, m_up);
    view_matrix.scale(scale);
}











