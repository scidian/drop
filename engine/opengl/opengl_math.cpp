//
//      Created by Stephens Nunnally on 5/7/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "engine/engine.h"
#include "engine/opengl/opengl.h"
#include "engine/things/engine_thing_object.h"
#include "engine/things/engine_thing_light.h"
#include "engine/world/engine_world.h"


//####################################################################################
//##    Maps 3D Point to / from 2D FBO Map Coordinates
//####################################################################################
QPointF DrOpenGL::mapToFBO(QVector3D point3D, QOpenGLFramebufferObject *fbo, QMatrix4x4 view_matrix, QMatrix4x4 proj_matrix) {

    float x_pos, y_pos, z_pos;
    x_pos = point3D.x();
    y_pos = point3D.y();
    z_pos = point3D.z();

    ///QRect viewport = QRect(0, 0, fbo->width(), fbo->height());
    ///QVector3D vec = QVector3D(x_pos, y_pos, z_pos).project( view_matrix, proj_matrix, viewport );

    glm::vec4 view(0, 0, fbo->width(), fbo->height());
    glm::vec3 object(x_pos, y_pos, z_pos);
    glm::mat4 model(glm::make_mat4(view_matrix.data()) );
    glm::mat4 proj( glm::make_mat4(proj_matrix.data()) );
    glm::vec3 v = glm::project(object, model, proj, view);

    ///return QPointF( static_cast<double>(vec.x()),  static_cast<double>(fbo->height() - vec.y()) );
    return QPointF( static_cast<double>(v.x),  static_cast<double>(fbo->height() - v.y) );
}


//####################################################################################
//##    Maps 3D Point to / from 2D QOpenGLWidget Coordinates
//####################################################################################
QPointF DrOpenGL::mapToScreen(double x, double y, double z) { return mapToScreen( QVector3D(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z))); }
QPointF DrOpenGL::mapToScreen(float  x, float  y, float  z) { return mapToScreen( QVector3D(x, y, z )); }
QPointF DrOpenGL::mapToScreen(QVector3D point3D) {

    float x_pos, y_pos, z_pos;
    x_pos = point3D.x();
    y_pos = point3D.y();
    z_pos = point3D.z();

    // Old Qt Way
    ///QRect viewport = QRect(0, 0, width() * devicePixelRatio(), height() * devicePixelRatio());
    ///QVector3D vec = QVector3D(x_pos, y_pos, z_pos).project( m_view, m_projection, viewport );

    // Better Glm Way - No Qt
    glm::vec4 view(0, 0, width() * devicePixelRatio(), height() * devicePixelRatio());
    glm::vec3 object(x_pos, y_pos, z_pos);
    glm::mat4 model(glm::make_mat4(m_view.data()) );
    glm::mat4 proj( glm::make_mat4(m_projection.data()) );
    glm::vec3 v = glm::project(object, model, proj, view);

    return QPointF( static_cast<double>(v.x),  static_cast<double>((height() * devicePixelRatio()) - v.y) );
}

glm::vec3 DrOpenGL::mapFromScreen(double x, double y) { return mapFromScreen( QPointF(x, y)); }
glm::vec3 DrOpenGL::mapFromScreen(float x, float y)   { return mapFromScreen( QPointF(static_cast<double>(x), static_cast<double>(y)) ); }
glm::vec3 DrOpenGL::mapFromScreen(QPointF point) {
    ///QRect viewport = QRect(0, 0, width() * devicePixelRatio(), height() * devicePixelRatio());

    float x_pos = static_cast<float>(             point.x()  * devicePixelRatio() );
    float y_pos = static_cast<float>( (height() - point.y()) * devicePixelRatio() );

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
    glm::vec4 view(0, 0, width() * devicePixelRatio(), height() * devicePixelRatio());
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
}











