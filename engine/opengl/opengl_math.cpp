//
//      Created by Stephens Nunnally on 5/7/2019, (c) 2019 Scidian Studios, All Rights Reserved
//
//  File:
//
//
//
#include "engine/engine.h"
#include "engine/opengl/opengl.h"
#include "engine/world/engine_world.h"


//####################################################################################
//##    Maps 3D Point to / from 2D FBO Map Coordinates
//####################################################################################
DrPointF DrOpenGL::mapToFBO(glm::vec3 point3D, QOpenGLFramebufferObject *fbo, QMatrix4x4 view_matrix, QMatrix4x4 proj_matrix) {

    float x_pos, y_pos, z_pos;
    x_pos = point3D.x;
    y_pos = point3D.y;
    z_pos = point3D.z;

    ///QRect viewport = QRect(0, 0, fbo->width(), fbo->height());
    ///QVector3D vec = QVector3D(x_pos, y_pos, z_pos).project( view_matrix, proj_matrix, viewport );

    glm::vec4 view(0, 0, fbo->width(), fbo->height());
    glm::vec3 object(x_pos, y_pos, z_pos);
    glm::mat4 model(glm::make_mat4(view_matrix.data()) );
    glm::mat4 proj( glm::make_mat4(proj_matrix.data()) );
    glm::vec3 v = glm::project(object, model, proj, view);

    ///return QPointF( static_cast<double>(vec.x()),  static_cast<double>(fbo->height() - vec.y()) );
    return DrPointF( static_cast<double>(v.x),  static_cast<double>(fbo->height() - v.y) );
}


//####################################################################################
//##    Maps 3D Point to / from 2D QOpenGLWidget Coordinates
//####################################################################################
DrPointF DrOpenGL::mapToScreen(double x, double y, double z) { return mapToScreen( glm::vec3(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z))); }
DrPointF DrOpenGL::mapToScreen(float  x, float  y, float  z) { return mapToScreen( glm::vec3(x, y, z )); }
DrPointF DrOpenGL::mapToScreen(glm::vec3 point3D) {

    float x_pos, y_pos, z_pos;
    x_pos = point3D.x;
    y_pos = point3D.y;
    z_pos = point3D.z;

    // Old Qt Way
    ///QRect viewport = QRect(0, 0, m_desired_fbo_width, m_desired_fbo_height);
    ///QVector3D vec = QVector3D(x_pos, y_pos, z_pos).project( m_view, m_projection, viewport );

    // Better Glm Way - No Qt
    glm::vec4 view(0, 0, m_desired_fbo_width, m_desired_fbo_height);
    glm::vec3 object(x_pos, y_pos, z_pos);
    glm::mat4 model(glm::make_mat4(m_view.data()) );
    glm::mat4 proj( glm::make_mat4(m_projection.data()) );
    glm::vec3 v = glm::project(object, model, proj, view);

    return DrPointF( static_cast<double>(v.x) / static_cast<double>(m_fbo_scale_x),
                     static_cast<double>((m_desired_fbo_height) - v.y) / static_cast<double>(m_fbo_scale_y));
}

glm::vec3 DrOpenGL::mapFromScreen(double x, double y) { return mapFromScreen( DrPointF(x, y)); }
glm::vec3 DrOpenGL::mapFromScreen(float x, float y)   { return mapFromScreen( DrPointF(static_cast<double>(x), static_cast<double>(y)) ); }
glm::vec3 DrOpenGL::mapFromScreen(DrPointF point) {
                ///QRect viewport = QRect(0, 0, m_desired_fbo_width, m_desired_fbo_height);

    float x_pos = static_cast<float>(                         point.x * devicePixelRatio() * static_cast<double>(m_fbo_scale_x));
    float y_pos = static_cast<float>( m_desired_fbo_height - (point.y * devicePixelRatio() * static_cast<double>(m_fbo_scale_y)));

                // Old way of unprojecting in Orthographic before we 3D Ortho View was working
                ///QVector3D vec;
                ///if (m_engine->getCurrentWorld()->render_type == Render_Type::Orthographic) {
                ///    vec = QVector3D( x_pos, y_pos, 0 ).unproject( m_view, m_projection, viewport);
                ///    vec.setX( vec.x() );
                ///    vec.setY( vec.y() );
                ///    vec.setZ( vec.z() );
                ///} else {

                // Possible way to read from the depth buffer
                ///GLdouble depthScale;
                ///glGetDoublev( GL_DEPTH_SCALE, &depthScale );
                ///GLfloat z;
                ///glReadPixels( static_cast<int>(x_pos), static_cast<int>(y_pos), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &z );

                // Old way to unproject, returns unpredictable z coordinate that cannot easily be moved to z plane 0...
                //      Find what the Z value is for a projected vector
                ///QVector3D find_z(0, 0, 0);
                ///find_z = find_z.project(m_view, m_projection, viewport);
                //      Then we use that Z value to unproject
                ///vec = QVector3D( x_pos, y_pos, find_z.z()).unproject( m_view, m_projection, viewport);

                // ********** New Perspective / 3D Orthographic Unproject Method
                // Since two points determine a line, we actually need to call unproject() twice: once with screen z == 0.0, then again with screen z == 1.0
                // This will give us the world points that correspond to the mouse click on the near and far planes, respectively
                //
                // !!!!! #NOTE: This won't work too great if z plane 0 is not in view
                // **********

                // ***** Unproject at near and far plane
                // Old Qt Way
                ///QVector3D near_plane, far_plane;
                ///near_plane = QVector3D( x_pos, y_pos, 0.0f ).unproject( m_view, m_projection, viewport);
                ///far_plane =  QVector3D( x_pos, y_pos, 1.0f ).unproject( m_view, m_projection, viewport);

    // Better Glm Way - No Qt
    glm::vec4 view(0, 0, m_desired_fbo_width, m_desired_fbo_height);
    glm::vec3 window_near(x_pos, y_pos, 0.0f);
    glm::vec3 window_far( x_pos, y_pos, 1.0f);
    glm::mat4 model(glm::make_mat4(m_view.data()) );
    glm::mat4 proj( glm::make_mat4(m_projection.data()) );
    glm::vec3 near_plane = glm::unProject(window_near, model, proj, view);
    glm::vec3 far_plane =  glm::unProject(window_far,  model, proj, view);

    // Find distance to z plane 0 as a percentage, and interpolate between the two near and far plane mouse points
    float z_total = abs(near_plane.z) + abs(far_plane.z);
    float z_percent = abs(near_plane.z / z_total);
    float nx = near_plane.x + (z_percent * (far_plane.x - near_plane.x));
    float ny = near_plane.y + (z_percent * (far_plane.y - near_plane.y));

       ///qDebug() << "Near: " << near << "  -  Far: " << far << "  -  Vec: " << vec;
    return glm::vec3(nx, ny, 0);


    /*
     * The following is an attempt to convert mouse to
     * screen coordinates without using unproject

    float mouseX, mouseY;
    // these positions must be in range [-1, 1] (!!!), not [0, width] and [0, height]
    if (m_engine->getCurrentWorld()->render_type == Render_Type::Orthographic) {
        mouseX = x_pos / (m_desired_fbo_width  * 0.5f) - 1.0f;
        mouseY = y_pos / (m_desired_fbo_height * 0.5f) - 1.0f;
    }

    glm::mat4 proj( glm::make_mat4(m_projection.data()) );
    glm::mat4 view( glm::make_mat4(m_view.data()) );

    glm::mat4 invVP = glm::inverse(proj * view);
    glm::vec4 screenPos = glm::vec4(mouseX, mouseY, 0.0f, 1.0f);
    glm::vec4 worldPos = invVP * screenPos;

    glm::vec3 dir = worldPos;//glm::normalize(glm::vec3(worldPos));
    */

}











