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

// Test Light Size
const float c_light_size = 1024;


//####################################################################################
//##        Allocate Light Occluder Frame Buffer Object
//####################################################################################
void OpenGL::bindLightBuffer(bool initialize_only) {
    int light_radius = static_cast<int>(c_light_size * m_scale);

    // Check Frame Buffer Object is initialized
    if (!m_light_fbo || m_light_fbo->width() != light_radius || m_light_fbo->height() != light_radius) {
        delete m_light_fbo;
        m_light_fbo =  new QOpenGLFramebufferObject(light_radius, light_radius);
    }

    // Bind and clear buffer
    if (!initialize_only == false) {
        m_light_fbo->bind();
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }
}

//####################################################################################
//##        Allocate Shadow Frame Buffer Object
//####################################################################################
void OpenGL::bindShadowBuffer(bool initialize_only) {
    int shadow_size =  static_cast<int>(c_light_size * m_scale);

    // Check Frame Buffer Object is initialized
    if (!m_shadow_fbo || m_shadow_fbo->width() != shadow_size) {
        delete m_shadow_fbo;
        m_shadow_fbo = new QOpenGLFramebufferObject(shadow_size, 1);
    }

    // Bind and clear buffer
    if (initialize_only == false) {
        m_shadow_fbo->bind();
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }
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
//    float left =   0.0f - (1 / 2.0f);
//    float right =  0.0f + (1 / m_scale / 2.0f);
//    float top =    0.0f + (1 / m_scale / 2.0f);
//    float bottom = 0.0f - (1 / m_scale / 2.0f);

    float left =   0.0f - ((m_light_fbo->width() )  / 2.0f);
    float right =  0.0f + ((m_light_fbo->width() )  / 2.0f);
    float top =    0.0f + ((m_light_fbo->height() ) / 2.0f);
    float bottom = 0.0f - ((m_light_fbo->height() ) / 2.0f);

    QMatrix4x4 m_matrix;
    m_matrix.ortho( left, right, bottom, top,  -1000.0f, 1000.0f);
    m_shadow_shader.setUniformValue( m_uniform_shadow_matrix, m_matrix );

    // Set Texture Coordinates for Shader
    std::vector<float> texture_coordinates;
    setWholeTextureCoordinates(texture_coordinates);
    m_shadow_shader.setAttributeArray(    m_attribute_shadow_tex_coord, texture_coordinates.data(), 2 );
    m_shadow_shader.enableAttributeArray( m_attribute_shadow_tex_coord );

    // Load vertices for this object
    QVector<GLfloat> vertices;
    setVertexFromSides(vertices, left, right, top, bottom);
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
    m_light_shader.setUniformValue( m_uniform_light_color, 0.75f, 0.2f, 0.75f );

    // Set Matrix for Shader, apply Orthographic Matrix to fill the viewport
    float left =   0.0f - (m_texture_fbo->width()  / 2.0f);
    float right =  0.0f + (m_texture_fbo->width()  / 2.0f);
    float top =    0.0f + (m_texture_fbo->height() / 2.0f);
    float bottom = 0.0f - (m_texture_fbo->height() / 2.0f);
    QMatrix4x4 m_matrix;
    m_matrix.ortho( left, right, bottom, top,  -1000.0f, 1000.0f);
    m_matrix.flipCoordinates();

    m_light_shader.setUniformValue( m_uniform_light_matrix, m_matrix );

    // Set Texture Coordinates for Shader
    std::vector<float> texture_coordinates;
    setWholeTextureCoordinates(texture_coordinates);
    m_light_shader.setAttributeArray(    m_attribute_light_tex_coord, texture_coordinates.data(), 2 );
    m_light_shader.enableAttributeArray( m_attribute_light_tex_coord );

    // Load vertices for this object
    left =   0.0f - ((m_light_fbo->width() )  / 2.0f);
    right =  0.0f + ((m_light_fbo->width() )  / 2.0f);
    top =    0.0f + ((m_light_fbo->height() ) / 2.0f);
    bottom = 0.0f - ((m_light_fbo->height() ) / 2.0f);
    QVector<GLfloat> vertices;
    setVertexFromSides(vertices, left, right, top, bottom);
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
























