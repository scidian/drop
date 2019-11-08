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

    // ***** Set camera position
    DrEngineWorld *world = m_engine->getCurrentWorld();
    if (world == nullptr) return;
    float cam_x = world->getCameraPosition().x() * combinedZoomScale();
    float cam_y = world->getCameraPosition().y() * combinedZoomScale();
    float cam_z = world->getCameraPosition().z();
    m_eye =     QVector3D( cam_x, cam_y, cam_z );
    m_look_at = QVector3D( cam_x, cam_y, world->getCameraFollowingZ() * combinedZoomScale() );
    m_up =      (world->getCameraUpVector() == Up_Vector::Y) ? c_up_vector_y : c_up_vector_z;

    // ***** Camera Rotation
    //          X Rotation, controls up / down
    //          Y Rotation, controls left / right
    QMatrix4x4 rotate_eye;
    rotate_eye.translate( m_look_at);
    rotate_eye.rotate(static_cast<float>(world->getCameraRotationX()), 1.0f, 0.0f, 0.0f);
    rotate_eye.rotate(static_cast<float>(world->getCameraRotationY()), 0.0f, 1.0f, 0.0f);
    rotate_eye.translate(-m_look_at);
    m_eye = rotate_eye * m_eye;

    //          Z Rotation, tilts head
    QMatrix4x4 rotate_up;
    rotate_up.rotate(static_cast<float>(world->getCameraRotationZ()), 0.0f, 0.0f, 1.0f);
    m_up = rotate_up * m_up;

    // ***** Orthographic
    if (render_type == Render_Type::Orthographic) {
        float left =   -(width()  * devicePixelRatio() / 2.0f);
        float right =  +(width()  * devicePixelRatio() / 2.0f);
        float top =    +(height() * devicePixelRatio() / 2.0f);
        float bottom = -(height() * devicePixelRatio() / 2.0f);
        m_projection.ortho( left, right, bottom, top, c_near_plane * combinedZoomScale(), c_far_plane * combinedZoomScale());

    // ***** Perspective
    } else {
        m_projection.perspective( c_field_of_view, aspect_ratio, 1.0f, (c_far_plane - c_near_plane) );
    }

    // ***** Rotation locked to Camera Follow Thing
    if (world->getCameraUpVector() == Up_Vector::Y) {
        rotate_up.setToIdentity();
        rotate_up.rotate(static_cast<float>(world->getCameraFollowingRotation()), 0.0f, 0.0f, 1.0f);
        m_up = rotate_up * m_up;
    } else if (world->getCameraUpVector() == Up_Vector::Z) {
        rotate_eye.setToIdentity();
        rotate_eye.translate( m_look_at);
        rotate_eye.rotate(static_cast<float>(world->getCameraFollowingRotation()), 0.0f, 0.0, 1.0f);
        rotate_eye.translate(-m_look_at);
        m_eye = rotate_eye * m_eye;
    }

    // ***** Set Look At and Scale
    m_view.lookAt(m_eye, m_look_at, m_up);
    m_view.scale( combinedZoomScale() );
}


//####################################################################################
//##    Occluder Map FBO Matrices
//##        RETURNS: View matrix and Projection matrix by reference
//####################################################################################
void DrOpenGL::occluderMatrix(Render_Type render_type, QMatrix4x4 &view_matrix, QMatrix4x4 &proj_matrix) {
    float aspect_ratio = static_cast<float>(m_occluder_fbo->width()) / static_cast<float>(m_occluder_fbo->height());

    view_matrix.setToIdentity();
    proj_matrix.setToIdentity();

    // ***** Scale based on Render Type
    float scale = (render_type == Render_Type::Orthographic) ? (c_occluder_scale_ortho) : (c_occluder_scale_proj);
          scale *= combinedZoomScale();

    // ***** Set camera position
    DrEngineWorld *world = m_engine->getCurrentWorld();
    if (world == nullptr) return;
    float cam_x = world->getCameraPosition().x() * scale;
    float cam_y = world->getCameraPosition().y() * scale;
    float cam_z = world->getCameraPosition().z();
    // Smooth position
    cam_x = (int(cam_x) / 5) * 5;
    cam_y = (int(cam_y) / 5) * 5;

    // ***** Orthographic
    if (render_type == Render_Type::Orthographic) {
        float left =   -(m_occluder_fbo->width() /  2.0f);
        float right =  +(m_occluder_fbo->width() /  2.0f);
        float top =    +(m_occluder_fbo->height() / 2.0f);
        float bottom = -(m_occluder_fbo->height() / 2.0f);
        proj_matrix.ortho( left, right, bottom, top, c_near_plane * scale, c_far_plane * scale);

    // ***** Perspective
    } else {
        proj_matrix.perspective( c_field_of_view, aspect_ratio, 1.0f, (c_far_plane - c_near_plane) );
    }

    // ***** Set Look At and Scale, Dont need extra rotation
    ///view_matrix.lookAt(m_eye, m_look_at, m_up);
    QVector3D eye =     QVector3D( cam_x, cam_y, cam_z );
    QVector3D look_at = QVector3D( cam_x, cam_y, world->getCameraFollowingZ() * scale );
    QVector3D up =      (world->getCameraUpVector() == Up_Vector::Y) ? c_up_vector_y : c_up_vector_z;
    view_matrix.lookAt(eye, look_at, up);
    view_matrix.scale(scale);
}











