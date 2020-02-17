//
//      Created by Stephens Nunnally on 8/30/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QVector3D>

#include "core/dr_debug.h"
#include "engine/engine.h"
#include "engine/opengl/opengl.h"
#include "engine/thing/engine_thing_object.h"
#include "engine/thing/engine_thing_light.h"
#include "engine/world/engine_world.h"


const float c_not_zero = 0.0001f;

//####################################################################################
//##    Returns a square matrix for rendering a flat quad (usually to fill screen)
//####################################################################################
QMatrix4x4 DrOpenGL::orthoMatrix(float width, float height) {
    float left =   -(width  / 2.0f);
    float right =  +(width  / 2.0f);
    float top =    +(height / 2.0f);
    float bottom = -(height / 2.0f);
    QMatrix4x4 m;
    m.ortho(left, right, bottom, top, c_near_plane, c_far_plane);
    return m;
}


//####################################################################################
//##    Update the View / Projection matrices before rendering
//####################################################################################
void DrOpenGL::updateViewMatrix(Render_Type render_type, float target_width, float target_height) {
    //          Axis:
    //              -X left,        +X right
    //              -Y down,        +Y up
    //              -Z back,        +Z front (close to camera)
    float aspect_ratio = static_cast<float>(target_width) / static_cast<float>(target_height);

    m_view.setToIdentity();
    m_projection.setToIdentity();

    // ***** Camera View Type
    DrEngineWorld *world = m_engine->getCurrentWorld();
    if (world == nullptr) return;
    float cam_x = (world->getCameraPosition().x * combinedZoomScale());
    float cam_y = (world->getCameraPosition().y * combinedZoomScale());
    float cam_z =  world->getCameraPosition().z;

    float plane_scale = combinedZoomScale();
    if (plane_scale < 1.0f) plane_scale = 1.0f;

    //      Orthographic
    if (render_type == Render_Type::Orthographic) {
        float height_difference = 1.0f;;//static_cast<float>(height / (this->height() * devicePixelRatio())) / m_engine->getCurrentWorld()->pixel_y;// / 800.0f;
        float left =   -(width()  * devicePixelRatio() / 2.0f) * height_difference;
        float right =  +(width()  * devicePixelRatio() / 2.0f) * height_difference;
        float top =    +(height() * devicePixelRatio() / 2.0f) * height_difference;
        float bottom = -(height() * devicePixelRatio() / 2.0f) * height_difference;
        m_projection.ortho( left, right, bottom, top, c_near_plane * plane_scale, c_far_plane * plane_scale);

    //      Perspective
    } else {
        m_projection.perspective( c_field_of_view, aspect_ratio, c_perspective_near, (c_far_plane - c_near_plane) * plane_scale );
    }


    // ***** Camera Position
    m_eye =     QVector3D( cam_x, cam_y, cam_z );
    m_look_at = QVector3D( cam_x, cam_y, world->getCameraFollowingZ() * combinedZoomScale() );
    m_up =      QVector3D( world->getCameraUpVector().x, world->getCameraUpVector().y, world->getCameraUpVector().z );


    // ***** Camera Rotation
    //          X Rotation, controls up / down
    //          Y Rotation, controls left / right
    QMatrix4x4 rotate_eye;
    rotate_eye.translate( m_look_at);
    rotate_eye.rotate(static_cast<float>(world->getCameraRotationY()) + c_not_zero, 0.0f, 1.0f, 0.0f);
    rotate_eye.rotate(static_cast<float>(world->getCameraRotationX()) + c_not_zero, 1.0f, 0.0f, 0.0f);
    rotate_eye.translate(-m_look_at);
    m_eye =    rotate_eye * m_eye;

    //          Find up vector (from a max as distance of square root of 2)
    float sqrt_2 = 1.4142135623f; ///same as sqrt(2.0);
    float dist_y = glm::distance(c_up_vector_y, world->getCameraUpVector());
    float dist_z = glm::distance(c_up_vector_z, world->getCameraUpVector());

    //          Origin Point, used to calculate relative camera position for pixelation and such
    QVector3D  origin = rotate_eye * QVector3D(0.0, 0.0, 0.0);

    //          Z Rotation, tilts head, apply rotation as a percentage of distance from square root of 2, useful for camera tweening between camera up vectors
    QMatrix4x4 rotate_up;
    if (dist_y < sqrt_2) { rotate_up.rotate(static_cast<float>(-world->getCameraRotationZ()) * ((sqrt_2 - dist_y) / sqrt_2), 0.0f, 0.0f, 1.0f); }
    if (dist_z < sqrt_2) { rotate_up.rotate(static_cast<float>( world->getCameraRotationZ()) * ((sqrt_2 - dist_z) / sqrt_2), 0.0f, 1.0f, 0.0f); }
    m_up = rotate_up * m_up;


    // ***** Rotation locked to Camera Follow Thing
    if (world->getCameraMatching()) {
        // Apply rotation as a percentage of distance from square root of 2, useful for camera tweening between camera up vectors
        if (dist_y < sqrt_2) {
            // Camera Z Rotate
            float tilt_amount = static_cast<float>(world->getCameraFollowingRotation()) * ((sqrt_2 - dist_y) / sqrt_2);
            rotate_up.setToIdentity();
            rotate_up.rotate(tilt_amount,  0.0f, 0.0f, 1.0f);
            m_up = rotate_up * m_up;
        }
        if (dist_z < sqrt_2) {
            // Camera Z Rotate
            float tilt_amount = static_cast<float>(world->getCameraFollowingRotation()) * ((sqrt_2 - dist_z) / sqrt_2) + c_not_zero;
            rotate_eye.setToIdentity();
            rotate_eye.translate( m_look_at);
            rotate_eye.rotate(tilt_amount, 0.0f, 0.0, 1.0f);
            rotate_eye.translate(-m_look_at);
            m_eye =  rotate_eye * m_eye;
        }
    }

    // ***** Round camera during pixelation
    m_origin = QVector3D(m_eye.x() - origin.x(), m_eye.y() - origin.y(), m_eye.z() - origin.z());
    if (m_engine->getCurrentWorld()->pixel_x > 1.0f || m_engine->getCurrentWorld()->pixel_y > 1.0f) {
        m_eye.setX(     Dr::RoundToMultiple(m_eye.x(),      1.0) );
        m_eye.setY(     Dr::RoundToMultiple(m_eye.y(),      1.0) );
        m_eye.setZ(     Dr::RoundToMultiple(m_eye.z(),      1.0) );
        m_look_at.setX( Dr::RoundToMultiple(m_look_at.x(),  1.0) );
        m_look_at.setY( Dr::RoundToMultiple(m_look_at.y(),  1.0) );
        m_look_at.setZ( Dr::RoundToMultiple(m_look_at.z(),  1.0) );
    }

    // ***** Set Look At and Scale
    m_view.lookAt(m_eye, m_look_at, m_up);
    m_view.scale(combinedZoomScale());
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

    // ***** Camera position
    DrEngineWorld *world = m_engine->getCurrentWorld();
    if (world == nullptr) return;
    float cam_x = world->getCameraPosition().x * scale;
    float cam_y = world->getCameraPosition().y * scale;
    float cam_z = world->getCameraPosition().z;

    float plane_scale = scale;
    if (plane_scale < 1.0f) plane_scale = 1.0f;

    // Orthographic
    if (render_type == Render_Type::Orthographic) {
        float left =   -(m_occluder_fbo->width()  * devicePixelRatio() / 2.0f);
        float right =  +(m_occluder_fbo->width()  * devicePixelRatio() / 2.0f);
        float top =    +(m_occluder_fbo->height() * devicePixelRatio() / 2.0f);
        float bottom = -(m_occluder_fbo->height() * devicePixelRatio() / 2.0f);
        proj_matrix.ortho( left, right, bottom, top, c_near_plane * plane_scale, c_far_plane * plane_scale);

    // Perspective
    } else {
        proj_matrix.perspective( c_field_of_view, aspect_ratio, c_perspective_near, (c_far_plane - c_near_plane) * plane_scale );
    }

    // ***** Set Look At and Scale, Don't need extra rotation
    ///view_matrix.lookAt(m_eye, m_look_at, m_up);
    QVector3D eye =     QVector3D( cam_x, cam_y, cam_z );
    QVector3D look_at = QVector3D( cam_x, cam_y, world->getCameraFollowingZ() * scale );
    QVector3D up =      QVector3D( c_up_vector_y.x, c_up_vector_y.y, c_up_vector_y.z ); ///world->getCameraUpVector();

    // ***** Smooth positions
    eye.setX(     Dr::RoundToMultiple(eye.x(),      1.0) );
    eye.setY(     Dr::RoundToMultiple(eye.y(),      1.0) );
    eye.setZ(     Dr::RoundToMultiple(eye.z(),      1.0) );
    look_at.setX( Dr::RoundToMultiple(look_at.x(),  1.0) );
    look_at.setY( Dr::RoundToMultiple(look_at.y(),  1.0) );
    look_at.setZ( Dr::RoundToMultiple(look_at.z(),  1.0) );

    // ***** Set Look At and Scale
    view_matrix.lookAt(eye, look_at, up);
    view_matrix.scale(scale);
}











