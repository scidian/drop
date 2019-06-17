//
//      Created by Stephens Nunnally on 6/17/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QOpenGLFramebufferObject>

#include "opengl.h"

const float c_light_size = 256;        // test size

//####################################################################################
//##        Allocate shadow Frame Buffer Object
//####################################################################################
void OpenGL::bindShadowBuffer() {
    // Check if exists first
    if (!m_shadow_fbo) {
        delete m_shadow_fbo;

        QOpenGLFramebufferObjectFormat format;
        format.setAttachment(QOpenGLFramebufferObject::Attachment::NoAttachment);
        format.setTextureTarget(GL_TEXTURE_2D);                      // This is set automatically, cannot be gl_texture_2d if multisampling is enabled
        format.setInternalTextureFormat(GL_RGBA32F_ARB);             // This is set automatically depending on the system
        m_shadow_fbo =  new QOpenGLFramebufferObject(static_cast<int>(c_light_size) * devicePixelRatio(), 1, format);
    }
    m_shadow_fbo->bind();

    // Clear the FBO
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);           // Texture X Plane
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);           // Texture Y Plane
}


void OpenGL::renderShadowMap() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, m_texture_fbo->texture());

    if (!m_shadow_shader.bind()) return;

    // Give the shader our light_size resolution
    m_shadow_shader.setUniformValue( m_uniform_shadow_resolution, c_light_size, c_light_size );

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

    // Draw triangles using shader program
    glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

    // Disable arrays
    m_shadow_shader.disableAttributeArray( m_attribute_shadow_vertex );
    m_shadow_shader.disableAttributeArray( m_attribute_shadow_tex_coord );

    // Release Shader
    m_shadow_shader.release();
}


























