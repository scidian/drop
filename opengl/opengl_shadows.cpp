//
//      Created by Stephens Nunnally on 6/17/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QOpenGLFramebufferObject>

#include "engine/engine.h"
#include "engine/engine_object.h"
#include "engine/engine_texture.h"
#include "engine/engine_world.h"
#include "engine/form_engine.h"
#include "helper.h"
#include "opengl/opengl.h"

const float c_light_size   = 1024;          // test size


//####################################################################################
//##        Allocate shadow Frame Buffer Object
//####################################################################################
void OpenGL::bindShadowBuffer() {

    int light_radius = static_cast<int>(c_light_size * m_scale);
    int shadow_size =  static_cast<int>(c_light_size * m_scale);

    if (!m_shadow_fbo || m_shadow_fbo->width() != shadow_size) {
        QOpenGLFramebufferObjectFormat format;
        format.setAttachment(QOpenGLFramebufferObject::Attachment::NoAttachment);
        delete m_shadow_fbo;
        m_shadow_fbo = new QOpenGLFramebufferObject(shadow_size, 1, format);
    }

    if (!m_light_fbo || m_light_fbo->width() != light_radius || m_light_fbo->height() != light_radius) {
        QOpenGLFramebufferObjectFormat format;
        format.setAttachment(QOpenGLFramebufferObject::Attachment::NoAttachment);
        delete m_light_fbo;
        m_light_fbo =  new QOpenGLFramebufferObject(light_radius, light_radius, format);
    }

    m_shadow_fbo->bind();

    // Clear the Shadow Frame Buffer Object
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);


    g_info = "W: " + QString::number(width()) + ", H: " + QString::number(height());
    g_info += ", LFBO W: " + QString::number(m_light_fbo->width()) + ", H: " + QString::number(m_light_fbo->height());
    g_info += ", SFBO W: " + QString::number(m_shadow_fbo->width()) + ", H: " + QString::number(m_shadow_fbo->height());
}


void OpenGL::drawShadowMap() {

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, m_light_fbo->texture());

//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);           // Texture X Plane
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);           // Texture Y Plane
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);           // Texture Z Plane

    if (!m_shadow_shader.bind()) return;

    // Give the shader our light_size resolution
    m_shadow_shader.setUniformValue( m_uniform_shadow_resolution, m_light_fbo->width(), m_light_fbo->height() );

    // Reset our projection matrix to the FBO size
    float left =   0.0f - (m_shadow_fbo->width()  / 2.0f);
    float right =  0.0f + (m_shadow_fbo->width()  / 2.0f);
    float top =    0.0f + (m_shadow_fbo->height() / 2.0f);
    float bottom = 0.0f - (m_shadow_fbo->height() / 2.0f);
    QMatrix4x4 m_matrix;
    m_matrix.ortho( left, right, bottom, top,  -1000.0f, 1000.0f);
    m_shadow_shader.setUniformValue( m_uniform_shadow_matrix, m_matrix );

    // Set Texture Coordinates for Shader
    std::vector<float> texture_coordinates;
    setWholeTextureCoordinates(texture_coordinates);
    m_shadow_shader.setAttributeArray(    m_attribute_shadow_tex_coord, texture_coordinates.data(), 2 );
    m_shadow_shader.enableAttributeArray( m_attribute_shadow_tex_coord );

    // Load vertices for this object
    left =   0.0f - ((m_light_fbo->width() )  / 2.0f);
    right =  0.0f + ((m_light_fbo->width() )  / 2.0f);
    top =    0.0f + ((m_light_fbo->height() ) / 2.0f);
    bottom = 0.0f - ((m_light_fbo->height() ) / 2.0f);
    QVector3D top_right = QVector3D( right, top, 0);
    QVector3D top_left =  QVector3D( left,  top, 0);
    QVector3D bot_right = QVector3D( right, bottom, 0);
    QVector3D bot_left =  QVector3D( left,  bottom, 0);

    QVector<GLfloat> vertices;
    vertices.clear();
    vertices.resize( 12 );              // in sets of x, y, z
    vertices[ 0] = top_right.x();       vertices[ 1] = top_right.y();       vertices[ 2] = 0;           // Top Right
    vertices[ 3] = top_left.x();        vertices[ 4] = top_left.y();        vertices[ 5] = 0;           // Top Left
    vertices[ 6] = bot_right.x();       vertices[ 7] = bot_right.y();       vertices[ 8] = 0;           // Bottom Right
    vertices[ 9] = bot_left.x();        vertices[10] = bot_left.y();        vertices[11] = 0;           // Bottom Left
    m_shadow_shader.setAttributeArray(    m_attribute_shadow_vertex, vertices.data(), 3 );
    m_shadow_shader.enableAttributeArray( m_attribute_shadow_vertex );

    m_shadow_shader.setUniformValue( m_uniform_shadow_texture, 0 );                                     // Use texture unit 0

    // Draw triangles using shader program
    glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

    // Disable arrays
    m_shadow_shader.disableAttributeArray( m_attribute_shadow_vertex );
    m_shadow_shader.disableAttributeArray( m_attribute_shadow_tex_coord );

    // Release Shader
    m_shadow_shader.release();
}


void OpenGL::draw2DLights() {

    // Enable alpha channel
///    glEnable(GL_BLEND);
///    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);                  // Standard blend function

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, m_shadow_fbo->texture());

    if (!m_light_shader.bind()) return;

    // Give the shader our light_size resolution, color
    m_light_shader.setUniformValue( m_uniform_light_resolution, m_light_fbo->width(), m_light_fbo->height() );
    m_light_shader.setUniformValue( m_uniform_light_color, 0.5f, 0.0f, 0.5f );

    // Set Matrix for Shader, apply Orthographic Matrix to fill the viewport
    float left =   0.0f - (m_texture_fbo->width()  / 2.0f);
    float right =  0.0f + (m_texture_fbo->width()  / 2.0f);
    float top =    0.0f + (m_texture_fbo->height() / 2.0f);
    float bottom = 0.0f - (m_texture_fbo->height() / 2.0f);
    QMatrix4x4 m_matrix;
    m_matrix.ortho( left, right, bottom, top,  -1000.0f, 1000.0f);

    m_matrix.flipCoordinates();
    //m_matrix.rotate(-30, 0, 0, 1);

    m_light_shader.setUniformValue( m_uniform_light_matrix, m_matrix );

    // Set Texture Coordinates for Shader
    std::vector<float> texture_coordinates;
    texture_coordinates.clear();
    texture_coordinates.resize( 8 );
    texture_coordinates[0] = 1;    texture_coordinates[1] = 1;
    texture_coordinates[2] = 0;    texture_coordinates[3] = 1;
    texture_coordinates[4] = 1;    texture_coordinates[5] = 0;
    texture_coordinates[6] = 0;    texture_coordinates[7] = 0;
    m_light_shader.setAttributeArray(    m_attribute_light_tex_coord, texture_coordinates.data(), 2 );
    m_light_shader.enableAttributeArray( m_attribute_light_tex_coord );

    // Load vertices for this object
    left =   0.0f - ((m_light_fbo->width() )  / 2.0f);
    right =  0.0f + ((m_light_fbo->width() )  / 2.0f);
    top =    0.0f + ((m_light_fbo->height() ) / 2.0f);
    bottom = 0.0f - ((m_light_fbo->height() ) / 2.0f);
    QVector3D top_right = QVector3D( right, top, 0);
    QVector3D top_left =  QVector3D( left,  top, 0);
    QVector3D bot_right = QVector3D( right, bottom, 0);
    QVector3D bot_left =  QVector3D( left,  bottom, 0);

    QVector<GLfloat> vertices;
    vertices.clear();
    vertices.resize( 12 );              // in sets of x, y, z
    vertices[ 0] = top_right.x();       vertices[ 1] = top_right.y();       vertices[ 2] = 0;           // Top Right
    vertices[ 3] = top_left.x();        vertices[ 4] = top_left.y();        vertices[ 5] = 0;           // Top Left
    vertices[ 6] = bot_right.x();       vertices[ 7] = bot_right.y();       vertices[ 8] = 0;           // Bottom Right
    vertices[ 9] = bot_left.x();        vertices[10] = bot_left.y();        vertices[11] = 0;           // Bottom Left
    m_light_shader.setAttributeArray(    m_attribute_light_vertex, vertices.data(), 3 );
    m_light_shader.enableAttributeArray( m_attribute_light_vertex );

    m_light_shader.setUniformValue( m_uniform_light_texture, 0 );                                     // Use texture unit 0

    // Draw triangles using shader program
    glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

    // Disable arrays
    m_light_shader.disableAttributeArray( m_attribute_light_vertex );
    m_light_shader.disableAttributeArray( m_attribute_light_tex_coord );

    // Release Shader
    m_light_shader.release();

}
























