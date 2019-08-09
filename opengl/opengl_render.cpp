//
//      Created by Stephens Nunnally on 5/3/2019, (c) 2019 Scidian Software, All Rights Reserved
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
#include "engine/engine_thing_light.h"
#include "engine/engine_thing_object.h"
#include "engine/engine_thing_water.h"
#include "engine/engine_texture.h"
#include "engine/engine_world.h"
#include "engine/form_engine.h"
#include "helper.h"
#include "opengl/opengl.h"


//####################################################################################
//##        Render, Paint the Scene (called by update())
//####################################################################################
void DrOpenGL::paintGL() {
    // ***** Find OpenGL Version supported on this system
    ///auto ver = glGetString(GL_VERSION);
    ///m_engine->info = QString::fromUtf8(reinterpret_cast<const char*>(ver));

    // ***** Update Camera / View Matrix
    updateViewMatrix(m_engine->getCurrentWorld()->render_type, c_use_cam_offset);

    // ***** Make sure Things vector is sorted by depth
    EngineThings &things = m_engine->getCurrentWorld()->getThings();
    std::sort(things.begin(), things.end(), [] (const DrEngineThing *a, const DrEngineThing *b) { return a->z_order < b->z_order; });

    // ***** If there are Lights, Render Occluder Map, Calculate Shadow Maps, Draw Glow Lights
    process2DLights();

    // ***** Render Onto Frame Buffer Object
    bindOffscreenBuffer();                                                          // Create / Bind Offscreen Frame Buffer Object
    ///drawCube( QVector3D( 2000, 400, -300) );                                     // Render Background 3D Objects
    drawSpace();                                                                    // Render cpSpace Objects
    ///drawCube( QVector3D(1600, 500, 600) );                                       // Render Foreground 3D Objects
    releaseOffscreenBuffer();                                                       // Release Frame Buffer Object
    QOpenGLFramebufferObject::blitFramebuffer(m_texture_fbo, m_render_fbo);         // Copy fbo to a GL_TEXTURE_2D (non multi-sampled) Frame Buffer Object

    // ***** Bind default Qt FrameBuffer, Clear and Render FBO to screen buffer as a textured quad, with post processing available
    QOpenGLFramebufferObject::bindDefault();
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glDisable(GL_BLEND);
    drawFrameBufferUsingDefaultShader(m_texture_fbo);

    // ***** Draws Debug Shapes / Text Onto Frame Buffer Object
    QOpenGLPaintDevice paint_gl(width() * devicePixelRatio(), height() * devicePixelRatio());
    QPainter painter (&paint_gl);
    drawDebug(painter);
    painter.end();

    // ***** Update FPS
    ++m_form_engine->fps_count_render;
}


//####################################################################################
//##        Returns a square matrix for rendering a flat quad (usually to fill screen)
//####################################################################################
QMatrix4x4 DrOpenGL::orthoMatrix(float width, float height) {
    float left =   0.0f - (width  / 2.0f);
    float right =  0.0f + (width  / 2.0f);
    float top =    0.0f + (height / 2.0f);
    float bottom = 0.0f - (height / 2.0f);
    QMatrix4x4 m;
    m.ortho( left, right, bottom, top, c_near_plane, c_far_plane);
    return m;
}

//####################################################################################
//##        Applies coordinates that represents an entire texture
//####################################################################################
void DrOpenGL::setWholeTextureCoordinates(std::vector<float> &texture_coords) {
    texture_coords.clear();
    texture_coords.resize( 8 );
    texture_coords[0] = 1;    texture_coords[1] = 1;
    texture_coords[2] = 0;    texture_coords[3] = 1;
    texture_coords[4] = 1;    texture_coords[5] = 0;
    texture_coords[6] = 0;    texture_coords[7] = 0;
}


//####################################################################################
//##        Returns list of vertices at z plane 0 from sides passed in
//####################################################################################
void DrOpenGL::setQuadVertices(QVector<GLfloat> &vertices, float width, float height, QPointF center, float z) {
    float left =   static_cast<float>(center.x()) - (width  / 2.0f);
    float right =  static_cast<float>(center.x()) + (width  / 2.0f);
    float top =    static_cast<float>(center.y()) + (height / 2.0f);
    float bottom = static_cast<float>(center.y()) - (height / 2.0f);
    QVector3D top_right = QVector3D( right, top, 0);
    QVector3D top_left =  QVector3D( left,  top, 0);
    QVector3D bot_right = QVector3D( right, bottom, 0);
    QVector3D bot_left =  QVector3D( left,  bottom, 0);

    vertices.clear();
    vertices.resize( 12 );              // in sets of x, y, z
    vertices[ 0] = top_right.x();       vertices[ 1] = top_right.y();       vertices[ 2] = z;           // Top Right
    vertices[ 3] = top_left.x();        vertices[ 4] = top_left.y();        vertices[ 5] = z;           // Top Left
    vertices[ 6] = bot_right.x();       vertices[ 7] = bot_right.y();       vertices[ 8] = z;           // Bottom Right
    vertices[ 9] = bot_left.x();        vertices[10] = bot_left.y();        vertices[11] = z;           // Bottom Left
}


//####################################################################################
//##        Sets shader variables to default normal render
//####################################################################################
void DrOpenGL::setShaderDefaultValues(float texture_width, float texture_height) {
    m_default_shader.setUniformValue( u_default_texture,        0 );                            // Use texture unit "0"
    m_default_shader.setUniformValue( u_default_alpha,          1.0f );
    m_default_shader.setUniformValue( u_default_tint,           0.0f, 0.0f, 0.0f );             // Add 0 to red, green, and blue
    m_default_shader.setUniformValue( u_default_zoom,           m_scale );
    m_default_shader.setUniformValue( u_default_width,          texture_width  );
    m_default_shader.setUniformValue( u_default_height,         texture_height );
    m_default_shader.setUniformValue( u_default_time,           static_cast<float>(QTime::currentTime().msecsSinceStartOfDay() / 1000.0) );
    m_default_shader.setUniformValue( u_default_pre,            false );
    m_default_shader.setUniformValue( u_default_pixel_x,        1.0f );
    m_default_shader.setUniformValue( u_default_pixel_y,        1.0f );
    m_default_shader.setUniformValue( u_default_pixel_offset,   0.0f, 0.0f );
    m_default_shader.setUniformValue( u_default_negative,       false );
    m_default_shader.setUniformValue( u_default_grayscale,      false );
    m_default_shader.setUniformValue( u_default_hue,            0.0f );
    m_default_shader.setUniformValue( u_default_saturation,     0.0f );
    m_default_shader.setUniformValue( u_default_contrast,       0.0f );
    m_default_shader.setUniformValue( u_default_brightness,     0.0f );
    m_default_shader.setUniformValue( u_default_bitrate,        16.0f );
    m_default_shader.setUniformValue( u_default_cartoon,        false );
    m_default_shader.setUniformValue( u_default_wavy,           false );
    m_default_shader.setUniformValue( u_default_fisheye,        false );
}


//####################################################################################
//##        Renders Frame Buffer Object to screen buffer as a textured quad
//##            Post processing available through the fragment shader
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
    std::vector<float> texture_coordinates;
    setWholeTextureCoordinates(texture_coordinates);
    m_default_shader.setAttributeArray( a_default_texture_coord, texture_coordinates.data(), 2 );
    m_default_shader.enableAttributeArray( a_default_texture_coord );

    // Load vertices for this object
    QVector<GLfloat> vertices;
    setQuadVertices(vertices, fbo->width(), fbo->height(), QPointF(0, 0), 0.0f);
    m_default_shader.setAttributeArray( a_default_vertex, vertices.data(), 3 );
    m_default_shader.enableAttributeArray( a_default_vertex );

    // Set variables for shader
    setShaderDefaultValues( fbo->width(), fbo->height() );

    // Set Pixelation Variables for Shader
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

    m_default_shader.setUniformValue( u_default_bitrate,    m_engine->getCurrentWorld()->bitrate );
    m_default_shader.setUniformValue( u_default_cartoon,    m_engine->getCurrentWorld()->cartoon );
    m_default_shader.setUniformValue( u_default_wavy,       m_engine->getCurrentWorld()->wavy );
    m_default_shader.setUniformValue( u_default_fisheye,    m_engine->getCurrentWorld()->fisheye );

    // Draw triangles using shader program
    glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

    // Disable arrays
    m_default_shader.disableAttributeArray( a_default_vertex );
    m_default_shader.disableAttributeArray( a_default_texture_coord );

    // Release Shader
    m_default_shader.release();
}


//####################################################################################
//##        Renders Frame Buffer Object to screen buffer as a textured quad
//##            Uses Kernel filter effect to blur, sharpen, etc
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
    std::vector<float> texture_coordinates;
    setWholeTextureCoordinates(texture_coordinates);
    m_kernel_shader.setAttributeArray( a_kernel_texture_coord, texture_coordinates.data(), 2 );
    m_kernel_shader.enableAttributeArray( a_kernel_texture_coord );

    // Load vertices for this object
    QVector<GLfloat> vertices;
    setQuadVertices(vertices, fbo->width(), fbo->height(), QPointF(0, 0), 0.0f);
    m_kernel_shader.setAttributeArray( a_kernel_vertex, vertices.data(), 3 );
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
//##        Renders Frame Buffer Object to screen buffer as a textured quad
//##            Uses "Screen" shader to multiply glow lights into texture
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
    std::vector<float> texture_coordinates;
    setWholeTextureCoordinates(texture_coordinates);
    m_screen_shader.setAttributeArray(    a_screen_texture_coord, texture_coordinates.data(), 2 );
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





