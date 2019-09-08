//
//      Created by Stephens Nunnally on 8/30/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QtMath>
#include <QOpenGLFramebufferObject>
#include <QOpenGLPaintDevice>
#include <QPainter>
#include <vector>

#include "engine/engine.h"
#include "engine/engine_camera.h"
#include "engine/engine_texture.h"
#include "engine/engine_world.h"
#include "engine/form_engine.h"
#include "engine_things/engine_thing_light.h"
#include "engine_things/engine_thing_object.h"
#include "engine_things/engine_thing_water.h"
#include "helper.h"
#include "opengl/opengl.h"


//####################################################################################
//##    Renders Frame Buffer Object to screen buffer as a textured quad
//##        Post processing available through the fragment shader
//####################################################################################
void DrOpenGL::drawFrameBufferUsingDefaultShader(QOpenGLFramebufferObject *fbo) {

    if (!m_default_shader.bind()) return;
    if (!fbo) return;

    // Bind offscreen frame buffer object as a texture
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, fbo->texture());

    // Set Matrix for Shader, apply Orthographic Matrix to fill the viewport
    m_default_shader.setUniformValue( u_default_matrix, orthoMatrix(fbo->width(), fbo->height()) );

    // Set Texture Coordinates for Shader
    m_default_shader.setAttributeArray( a_default_texture_coord, m_whole_texture_coordinates.data(), 2 );
    m_default_shader.enableAttributeArray( a_default_texture_coord );

    // Load vertices for this object
    QVector<GLfloat> vertices;
    setQuadVertices(vertices, fbo->width(), fbo->height(), QPointF(0, 0), 0.0f);
    m_default_shader.setAttributeArray( a_default_vertex, vertices.data(), 3 );
    m_default_shader.enableAttributeArray( a_default_vertex );

    // Set variables for shader
    setShaderDefaultValues( fbo->width(), fbo->height() );

    // Set Bit Rate / Pixelation Variables for Shader
    m_default_shader.setUniformValue( u_default_bitrate,        m_engine->getCurrentWorld()->bitrate );
    m_default_shader.setUniformValue( u_default_pixel_x,        m_engine->getCurrentWorld()->pixel_x );
    m_default_shader.setUniformValue( u_default_pixel_y,        m_engine->getCurrentWorld()->pixel_y );
    m_default_shader.setUniformValue( u_default_pixel_offset,   m_engine->getCurrentWorld()->getCameraPos().x(), m_engine->getCurrentWorld()->getCameraPos().y() );

    // Set more Appearance Variables for Shader
    m_default_shader.setUniformValue( u_default_negative,   m_engine->getCurrentWorld()->negative );
    m_default_shader.setUniformValue( u_default_grayscale,  m_engine->getCurrentWorld()->grayscale );
    m_default_shader.setUniformValue( u_default_hue,        m_engine->getCurrentWorld()->hue );
    m_default_shader.setUniformValue( u_default_saturation, m_engine->getCurrentWorld()->saturation );
    m_default_shader.setUniformValue( u_default_contrast,   m_engine->getCurrentWorld()->contrast );
    m_default_shader.setUniformValue( u_default_brightness, m_engine->getCurrentWorld()->brightness );

    m_default_shader.setUniformValue( u_default_cartoon,    m_engine->getCurrentWorld()->cartoon );
    m_default_shader.setUniformValue( u_default_wavy,       m_engine->getCurrentWorld()->wavy );

    // Draw triangles using shader program
    glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

    // Disable arrays
    m_default_shader.disableAttributeArray( a_default_vertex );
    m_default_shader.disableAttributeArray( a_default_texture_coord );

    // Release Shader
    m_default_shader.release();
}


//####################################################################################
//##    Renders Frame Buffer Object to screen buffer as a textured quad
//##        Uses Kernel filter effect to blur, sharpen, etc
//####################################################################################
void DrOpenGL::drawFrameBufferUsingKernelShader(QOpenGLFramebufferObject *fbo) {

    if (!m_kernel_shader.bind()) return;
    if (!fbo) return;

    // Bind offscreen frame buffer object as a texture
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, fbo->texture());

    // Set Matrix for Shader, apply Orthographic Matrix to fill the viewport
    m_kernel_shader.setUniformValue( u_kernel_matrix, orthoMatrix(fbo->width(), fbo->height()) );

    // Set Texture Coordinates for Shader
    m_kernel_shader.setAttributeArray(    a_kernel_texture_coord, m_whole_texture_coordinates.data(), 2 );
    m_kernel_shader.enableAttributeArray( a_kernel_texture_coord );

    // Load vertices for this object
    QVector<GLfloat> vertices;
    setQuadVertices(vertices, fbo->width(), fbo->height(), QPointF(0, 0), 0.0f);
    m_kernel_shader.setAttributeArray(    a_kernel_vertex, vertices.data(), 3 );
    m_kernel_shader.enableAttributeArray( a_kernel_vertex );

    // Set variables for shader
    m_kernel_shader.setUniformValue( u_kernel_texture,    0 );                             // Use texture unit "0"
    m_kernel_shader.setUniformValue( u_kernel_alpha,      1.0f );
    m_kernel_shader.setUniformValue( u_kernel_width,      static_cast<float>(fbo->width()) );
    m_kernel_shader.setUniformValue( u_kernel_height,     static_cast<float>(fbo->height()) );

    // Draw triangles using shader program
    glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

    // Disable arrays
    m_kernel_shader.disableAttributeArray( a_kernel_vertex );
    m_kernel_shader.disableAttributeArray( a_kernel_texture_coord );

    // Release Shader
    m_kernel_shader.release();
}



//####################################################################################
//##    Renders Frame Buffer Object to screen buffer as a textured quad
//##        Uses "Screen" shader to multiply glow lights into texture
//####################################################################################
void DrOpenGL::drawFrameBufferUsingScreenShader(QOpenGLFramebufferObject *upper, QOpenGLFramebufferObject *lower, Blend_Mode mode) {

    if (!m_screen_shader.bind()) return;

    // Bind offscreen frame buffer objects as textures
    glEnable(GL_TEXTURE_2D);
    GLint upper_location = glGetUniformLocation(m_screen_shader.programId(), "u_upper");
    GLint lower_location = glGetUniformLocation(m_screen_shader.programId(), "u_lower");
    glUseProgram(m_screen_shader.programId());
    glUniform1i(upper_location, 0);
    glUniform1i(lower_location, 1);

    // Bind textures - !!!!! #NOTE: Must be called in descending order and end on 0
    glActiveTexture(GL_TEXTURE1);                           // Texture unit 1
    glBindTexture(GL_TEXTURE_2D, lower->texture());
    glActiveTexture(GL_TEXTURE0);                           // Texture unit 0
    glBindTexture(GL_TEXTURE_2D, upper->texture());

    // Set Matrix for Shader, apply Orthographic Matrix to fill the viewport
    m_screen_shader.setUniformValue( u_screen_matrix, orthoMatrix(lower->width(), lower->height()) );

    // Set Texture Coordinates for Shader
    m_screen_shader.setAttributeArray(    a_screen_texture_coord, m_whole_texture_coordinates.data(), 2 );
    m_screen_shader.enableAttributeArray( a_screen_texture_coord );

    // Load vertices for this object
    QVector<GLfloat> vertices;
    setQuadVertices(vertices, lower->width(), lower->height(), QPointF(0, 0), 0.0f);
    m_screen_shader.setAttributeArray(    a_screen_vertex, vertices.data(), 3 );
    m_screen_shader.enableAttributeArray( a_screen_vertex );

    // Set variables for shader
    m_screen_shader.setUniformValue( u_screen_width,    static_cast<float>(lower->width()) );
    m_screen_shader.setUniformValue( u_screen_height,   static_cast<float>(lower->height()) );
    m_screen_shader.setUniformValue( u_screen_blend,    static_cast<int>(mode) );

    // Draw triangles using shader program
    glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

    // Disable arrays
    m_screen_shader.disableAttributeArray( a_screen_vertex );
    m_screen_shader.disableAttributeArray( a_screen_texture_coord );

    // Release Shader
    m_screen_shader.release();
}










