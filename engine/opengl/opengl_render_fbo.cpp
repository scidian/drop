//
//      Created by Stephens Nunnally on 8/30/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QOpenGLFramebufferObject>

#include <vector>

#include "engine/engine.h"
#include "engine/engine_camera.h"
#include "engine/engine_texture.h"
#include "engine/form_engine.h"
#include "engine/opengl/opengl.h"
#include "engine/thing/engine_thing_light.h"
#include "engine/thing/engine_thing_object.h"
#include "engine/thing/engine_thing_water.h"
#include "engine/world/engine_world.h"


//####################################################################################
//##    Renders Frame Buffer Object to screen buffer as a textured quad
//##        Post processing available through the fragment shader
//####################################################################################
void DrOpenGL::drawFrameBufferUsingDefaultShader(QOpenGLFramebufferObject *fbo) {

    if (!m_default_shader.bind()) return;
    if (!fbo) return;

    // ***** Bind offscreen frame buffer objects as textures
    glEnable(GL_TEXTURE_2D);
    GLint texture = glGetUniformLocation(m_default_shader.programId(), "u_texture");
    GLint pixel =   glGetUniformLocation(m_default_shader.programId(), "u_texture_pixel");
    glUseProgram(m_default_shader.programId());
    glUniform1i(texture,    0);
    glUniform1i(pixel,      1);
    // Bind textures - !!!!! #NOTE: Must be called in descending order and end on 0
    glActiveTexture(GL_TEXTURE1);                           // Texture unit 1
    if (m_engine->getCurrentWorld()->pixel_texture == Pixel_Texture::Tile) {
        glBindTexture(GL_TEXTURE_2D, m_engine->getTexture(Asset_Textures::Pixel_Tile_1)->texture()->textureId());
        m_engine->getTexture(Asset_Textures::Pixel_Tile_1)->texture()->setWrapMode(QOpenGLTexture::WrapMode::Repeat);
    } else if (m_engine->getCurrentWorld()->pixel_texture == Pixel_Texture::Cross) {
        glBindTexture(GL_TEXTURE_2D, m_engine->getTexture(Asset_Textures::Pixel_Cross_1)->texture()->textureId());
        m_engine->getTexture(Asset_Textures::Pixel_Cross_1)->texture()->setWrapMode(QOpenGLTexture::WrapMode::Repeat);
    } else if (m_engine->getCurrentWorld()->pixel_texture == Pixel_Texture::Knitted) {
        glBindTexture(GL_TEXTURE_2D, m_engine->getTexture(Asset_Textures::Pixel_Sitch_1)->texture()->textureId());
        m_engine->getTexture(Asset_Textures::Pixel_Sitch_1)->texture()->setWrapMode(QOpenGLTexture::WrapMode::Repeat);
    } else if (m_engine->getCurrentWorld()->pixel_texture == Pixel_Texture::Woven) {
        glBindTexture(GL_TEXTURE_2D, m_engine->getTexture(Asset_Textures::Pixel_Woven_1)->texture()->textureId());
        m_engine->getTexture(Asset_Textures::Pixel_Woven_1)->texture()->setWrapMode(QOpenGLTexture::WrapMode::Repeat);
    } else if (m_engine->getCurrentWorld()->pixel_texture == Pixel_Texture::Wood) {
        glBindTexture(GL_TEXTURE_2D, m_engine->getTexture(Asset_Textures::Pixel_Wood_1)->texture()->textureId());
        m_engine->getTexture(Asset_Textures::Pixel_Wood_1)->texture()->setWrapMode(QOpenGLTexture::WrapMode::Repeat);
    }
    glActiveTexture(GL_TEXTURE0);                           // Texture unit 0
    glBindTexture(GL_TEXTURE_2D, fbo->texture());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // GL_CLAMP_TO_BORDER  // GL_MIRRORED_REPEAT
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // GL_CLAMP_TO_BORDER  // GL_MIRRORED_REPEAT

    // ***** Set Matrix for Shader, apply Orthographic Matrix to fill the viewport
    DrPointF center(0, 0);
    // Adjust center for pixelation
    if (m_engine->getCurrentWorld()->pixel_x > 1.0f || m_engine->getCurrentWorld()->pixel_y > 1.0f) {
        // Woven cloth uses 1 texture spread across 2 pixels, etc
        float spread = 1.0f;
        if (m_engine->getCurrentWorld()->pixel_texture == Pixel_Texture::Woven) spread = 2.0f;
        if (m_engine->getCurrentWorld()->pixel_texture == Pixel_Texture::Wood)  spread = 5.0f;

        double x_pos = static_cast<double>(m_engine->getCurrentWorld()->getCameraPosition().x * combinedZoomScale());
        double y_pos = static_cast<double>(m_engine->getCurrentWorld()->getCameraPosition().y * combinedZoomScale());
        if (x_pos < 0)  center.x =  fmod(abs(x_pos), m_engine->getCurrentWorld()->pixel_x*spread) - static_cast<double>(m_engine->getCurrentWorld()->pixel_x*spread);
        else            center.x = -fmod(    x_pos,  m_engine->getCurrentWorld()->pixel_x*spread);
        if (y_pos < 0)  center.y =  fmod(abs(y_pos), m_engine->getCurrentWorld()->pixel_y*spread) - static_cast<double>(m_engine->getCurrentWorld()->pixel_y*spread);
        else            center.y = -fmod(    y_pos,  m_engine->getCurrentWorld()->pixel_y*spread);
        m_default_shader.setUniformValue( u_default_matrix, orthoMatrix(fbo->width()  - (m_engine->getCurrentWorld()->pixel_x*2.0f*spread),
                                                                        fbo->height() - (m_engine->getCurrentWorld()->pixel_y*2.0f*spread)) );
    // No pixelation
    } else {
        m_default_shader.setUniformValue( u_default_matrix, orthoMatrix(fbo->width(), fbo->height()) );
    }

    // Set Texture / Barycentric Coordinates for Shader
    m_default_shader.enableAttributeArray( a_default_texture_coord );
    m_default_shader.enableAttributeArray( a_default_barycentric );
    m_default_shader.setAttributeArray(    a_default_texture_coord, m_quad_texture_coordinates.data(), 2 );
    m_default_shader.setAttributeArray(    a_default_barycentric,   m_quad_barycentric.data(),         3 );

    // Load vertices for this object
    std::vector<GLfloat> vertices;
    setQuadVertices(vertices, fbo->width(), fbo->height(), center, 0.0f);
    m_default_shader.setAttributeArray(    a_default_vertex, vertices.data(), 3 );
    m_default_shader.enableAttributeArray( a_default_vertex );

    // Set variables for shader
    setShaderDefaultValues( fbo->width(), fbo->height() );

    // Set Bit Rate / Pixelation Variables for Shader
    m_default_shader.setUniformValue( u_default_bitrate,            m_engine->getCurrentWorld()->bitrate );
    m_default_shader.setUniformValue( u_default_pixel_x,            m_engine->getCurrentWorld()->pixel_x );
    m_default_shader.setUniformValue( u_default_pixel_y,            m_engine->getCurrentWorld()->pixel_y );
    m_default_shader.setUniformValue( u_default_pixel_offset,       m_engine->getCurrentWorld()->getCameraPosition().x,
                                                                    m_engine->getCurrentWorld()->getCameraPosition().y );
    m_default_shader.setUniformValue( u_default_pixel_type,         static_cast<float>(m_engine->getCurrentWorld()->pixel_texture) );

    // Set more Appearance Variables for Shader
    m_default_shader.setUniformValue( u_default_negative,           m_engine->getCurrentWorld()->negative );
    m_default_shader.setUniformValue( u_default_grayscale,          m_engine->getCurrentWorld()->grayscale );
    m_default_shader.setUniformValue( u_default_hue,                m_engine->getCurrentWorld()->hue );
    m_default_shader.setUniformValue( u_default_saturation,         m_engine->getCurrentWorld()->saturation );
    m_default_shader.setUniformValue( u_default_contrast,           m_engine->getCurrentWorld()->contrast );
    m_default_shader.setUniformValue( u_default_brightness,         m_engine->getCurrentWorld()->brightness );

    m_default_shader.setUniformValue( u_default_cartoon,            m_engine->getCurrentWorld()->cartoon );
    m_default_shader.setUniformValue( u_default_cartoon_width,      m_engine->getCurrentWorld()->cartoon_width );
    m_default_shader.setUniformValue( u_default_cross_hatch,        m_engine->getCurrentWorld()->cross_hatch );
    m_default_shader.setUniformValue( u_default_cross_hatch_width,  m_engine->getCurrentWorld()->cross_hatch_width );
    m_default_shader.setUniformValue( u_default_wavy,               m_engine->getCurrentWorld()->wavy );
    m_default_shader.setUniformValue( u_default_wireframe,          false );
    m_default_shader.setUniformValue( u_default_wireframe_width,    1.0f );

    // Draw triangles using shader program
    glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

    // Disable arrays
    m_default_shader.disableAttributeArray( a_default_vertex );
    m_default_shader.disableAttributeArray( a_default_texture_coord );
    m_default_shader.disableAttributeArray( a_default_barycentric );

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
    m_kernel_shader.setAttributeArray(    a_kernel_texture_coord, m_quad_texture_coordinates.data(), 2 );
    m_kernel_shader.enableAttributeArray( a_kernel_texture_coord );

    // Load vertices for this object
    std::vector<GLfloat> vertices;
    setQuadVertices(vertices, fbo->width(), fbo->height(), DrPointF(0, 0), 0.0f);
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
    m_screen_shader.setUniformValue(        u_screen_matrix, orthoMatrix(lower->width(), lower->height()) );

    // Set Texture Coordinates for Shader
    m_screen_shader.setAttributeArray(      a_screen_texture_coord, m_quad_texture_coordinates.data(), 2 );
    m_screen_shader.enableAttributeArray(   a_screen_texture_coord );

    // Load vertices for this object
    std::vector<GLfloat> vertices;
    setQuadVertices(vertices, lower->width(), lower->height(), DrPointF(0, 0), 0.0f);
    m_screen_shader.setAttributeArray(      a_screen_vertex, vertices.data(), 3 );
    m_screen_shader.enableAttributeArray(   a_screen_vertex );

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



//####################################################################################
//##    Renders Frame Buffer Object to screen buffer as a textured quad
//##        Uses "Simple" shader for speed
//####################################################################################
void DrOpenGL::drawFrameBufferUsingSimpleShader(QOpenGLFramebufferObject *fbo) {

    if (!m_simple_shader.bind()) return;
    if (!fbo) return;

    // Bind offscreen frame buffer object as a texture
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, fbo->texture());

    // Set Matrix for Shader, apply Orthographic Matrix to fill the viewport
    m_simple_shader.setUniformValue( u_simple_matrix, orthoMatrix(fbo->width(), fbo->height()) );

    // Set Texture Coordinates for Shader
    m_simple_shader.setAttributeArray(    a_simple_texture_coord, m_quad_texture_coordinates.data(), 2 );
    m_simple_shader.enableAttributeArray( a_simple_texture_coord );

    // Load vertices for this object
    std::vector<GLfloat> vertices;
    setQuadVertices(vertices, fbo->width(), fbo->height(), DrPointF(0, 0), 0.0f);
    m_simple_shader.setAttributeArray(    a_simple_vertex, vertices.data(), 3 );
    m_simple_shader.enableAttributeArray( a_simple_vertex );

    // Set variables for shader
    m_simple_shader.setUniformValue( u_simple_texture,    0 );                             // Use texture unit "0"
    m_simple_shader.setUniformValue( u_simple_alpha,      1.0f );

    // Draw triangles using shader program
    glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

    // Disable arrays
    m_simple_shader.disableAttributeArray( a_simple_vertex );
    m_simple_shader.disableAttributeArray( a_simple_texture_coord );

    // Release Shader
    m_simple_shader.release();
}










